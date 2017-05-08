// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "LightMassBaking.h"

#include <sstream>

#define LIGHTMAPPER_IMPLEMENTATION
#include <gl\glew.h>
#include "lightmapper.h"

_NAME_BEGIN

struct LightMassContextGL
{
	LightMassContextGL() noexcept
		: program(0)
		, u_lightmap(0)
		, u_mvp(0)
		, lightmap(0)
		, vao(0)
		, vbo(0)
		, ibo(0)
	{
	}

	~LightMassContextGL() noexcept
	{
		glDeleteProgram(program);
		glDeleteTextures(1, &lightmap);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	GLuint program;
	GLint u_lightmap;
	GLint u_mvp;

	GLuint lightmap;
	GLuint vao, vbo, ibo;
};

LightMapData::LightMapData() noexcept
	: width(1024)
	, height(1024)
	, channel(1)
	, margin(1)
	, data(nullptr)
{
}

LightModelDrawCall::LightModelDrawCall() noexcept
	: count(0)
	, instanceCount(1)
	, firstIndex(0)
	, baseVertex(0)
	, baseInstance(0)
{
}

LightModelData::LightModelData() noexcept
	: vertices(nullptr)
	, indices(nullptr)
	, sizeofIndices(2)
	, sizeofVertices(0)
	, strideVertices(0)
	, strideTexcoord(0)
	, numVertices(0)
	, numIndices(0)
{
}

LightBakingParams::LightBakingParams() noexcept
	: hemisphereSize(64)
	, hemisphereNear(0.1)
	, hemisphereFar(100)
	, clearColor(float3::One)
	, interpolationPasses(1)
	, interpolationThreshold(1e-4)
{
}

LightMassBaking::LightMassBaking() noexcept
{
}

LightMassBaking::~LightMassBaking() noexcept
{
}

void
LightMassBaking::setLightMassListener(LightMassListenerPtr pointer) noexcept
{
	_lightMassListener = pointer;
}

LightMassListenerPtr
LightMassBaking::getLightMassListener() const noexcept
{
	return _lightMassListener;
}


GLuint loadShader(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0)
	{
		fprintf(stderr, "Could not create shader!\n");
		return 0;
	}

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		fprintf(stderr, "Could not compile shader!\n");
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* infoLog = (char*)malloc(infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			fprintf(stderr, "%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint loadProgram(const char *vp, const char *fp, const char **attributes, int attributeCount)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vp);
	if (!vertexShader)
		return 0;

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fp);
	if (!fragmentShader)
	{
		glDeleteShader(vertexShader);
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program == 0)
	{
		fprintf(stderr, "Could not create program!\n");
		return 0;
	}

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	for (int i = 0; i < attributeCount; i++)
		glBindAttribLocation(program, i, attributes[i]);

	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		fprintf(stderr, "Could not link program!\n");
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			fprintf(stderr, "%s\n", infoLog);
			free(infoLog);
		}

		glDeleteProgram(program);
		return 0;
	}

	return program;
}

bool
LightMassBaking::baking(const LightBakingOptions& params) noexcept
{
	assert(params.lightMap.data);
	assert(params.lightMap.width >= 0 && params.lightMap.height >= 0);
	assert(params.lightMap.channel == 1 || params.lightMap.channel == 2 || params.lightMap.channel == 3 || params.lightMap.channel == 4);

	assert(params.model.vertices >= 0 && params.model.indices >= 0);
	assert(params.model.numVertices >= 0 && params.model.numIndices >= 0);
	assert(params.model.drawcalls.size() >= 1);
	assert(params.model.strideVertices < params.model.sizeofVertices && params.model.strideTexcoord < params.model.sizeofVertices);
	assert(params.model.sizeofVertices > 0);
	assert(params.model.sizeofIndices == 1 || params.model.sizeofIndices == 2 || params.model.sizeofIndices == 3);

	assert(params.model.boundingBoxs.empty() || params.model.boundingBoxs.size() == params.model.drawcalls.size());

	LightMassContextGL context;
	if (!this->initialize(params, context))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not initialize with OpenGL.");

		return false;
	}

	lm_context *ctx = lmCreate(
		params.baking.hemisphereSize,
		params.baking.hemisphereNear,
		params.baking.hemisphereFar,
		params.baking.clearColor.x,
		params.baking.clearColor.y,
		params.baking.clearColor.z,
		params.baking.interpolationPasses,
		params.baking.interpolationThreshold
	);
	if (!ctx)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not initialize with lightmapper.");

		return false;
	}

	try
	{
		if (_lightMassListener)
			_lightMassListener->onBakingStart();

		lm_type faceType = LM_UNSIGNED_INT;
		if (params.model.sizeofIndices == 1)
			faceType = LM_UNSIGNED_BYTE;
		else if (params.model.sizeofIndices == 2)
			faceType = LM_UNSIGNED_SHORT;

		GLenum glType = GL_UNSIGNED_INT;
		if (params.model.sizeofIndices == 1)
			glType = GL_UNSIGNED_BYTE;
		else if (params.model.sizeofIndices == 2)
			glType = GL_UNSIGNED_SHORT;

		lmSetTargetLightmap(ctx, params.lightMap.data, params.lightMap.width, params.lightMap.height, params.lightMap.channel);
		lmSetGeometry(ctx, float4x4::One.data(),
			LM_FLOAT, params.model.vertices + params.model.strideVertices, params.model.sizeofVertices,
			LM_FLOAT, params.model.vertices + params.model.strideTexcoord, params.model.sizeofVertices,
			params.model.numIndices, faceType, params.model.indices);

		float4x4 view, proj;
		Viewportt<int> vp;

		std::size_t baseIndex = -1;

		while (lmBegin(ctx, vp.ptr(), view.data(), proj.data()))
		{
			float4x4 mvp = proj * view;

			glViewport(vp.left, vp.top, vp.width, vp.height);

			glEnable(GL_DEPTH_TEST);

			glUseProgram(context.program);

			glUniform1i(context.u_lightmap, 0);
			glUniformMatrix4fv(context.u_mvp, 1, GL_FALSE, mvp.ptr());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, context.lightmap);
			glBindVertexArray(context.vao);

			if (params.model.boundingBoxs.empty())
			{
				if (glMultiDrawElementsIndirect)
				{
					glMultiDrawElementsIndirect(GL_TRIANGLES, glType, params.model.drawcalls.data(), params.model.drawcalls.size(), 0);
				}
				else
				{
					for (auto& drawcall : params.model.drawcalls)
					{
						if (glDrawElementsInstancedBaseVertexBaseInstance)
						{
							glDrawElementsInstancedBaseVertexBaseInstance(
								GL_TRIANGLES,
								drawcall.count,
								glType,
								(char*)nullptr + drawcall.firstIndex,
								drawcall.instanceCount,
								drawcall.baseVertex,
								drawcall.baseInstance
							);
						}
						else
						{
							glDrawElements(GL_TRIANGLES, drawcall.count, glType, (char*)nullptr + drawcall.firstIndex);
						}
					}
				}
			}
			else
			{
				Frustum fru;
				fru.extract(mvp);

				std::size_t drawCount = params.model.drawcalls.size();

				for (std::size_t i = 0; i < drawCount; i++)
				{
					if (!fru.contains(params.model.boundingBoxs[i].aabb()))
						continue;

					if (glDrawElementsInstancedBaseVertexBaseInstance)
					{
						glDrawElementsInstancedBaseVertexBaseInstance(
							GL_TRIANGLES,
							params.model.drawcalls[i].count,
							glType,
							(char*)nullptr + params.model.drawcalls[i].firstIndex,
							params.model.drawcalls[i].instanceCount,
							params.model.drawcalls[i].baseVertex,
							params.model.drawcalls[i].baseInstance
						);
					}
					else
					{
						glDrawElements(GL_TRIANGLES, params.model.drawcalls[i].count, glType, (char*)nullptr + params.model.drawcalls[i].firstIndex);
					}
				}
			}

			if (baseIndex != ctx->meshPosition.triangle.baseIndex)
			{
				if (_lightMassListener)
					_lightMassListener->onBakingProgressing(lmProgress(ctx));

				baseIndex = ctx->meshPosition.triangle.baseIndex;
			}

			lmEnd(ctx);
		}

		if (params.lightMap.margin > 0)
		{
			std::unique_ptr<float[]> lightmapTemp = std::make_unique<float[]>(params.lightMap.width * params.lightMap.height * params.lightMap.channel);
			std::memset(lightmapTemp.get(), 0, params.lightMap.width * params.lightMap.height * params.lightMap.channel * sizeof(float));

			for (int j = 0; j < params.lightMap.margin; j++)
			{
				lmImageSmooth(params.lightMap.data, lightmapTemp.get(), params.lightMap.width, params.lightMap.height, params.lightMap.channel);
				lmImageDilate(lightmapTemp.get(), params.lightMap.data, params.lightMap.width, params.lightMap.height, params.lightMap.channel);
			}
		}

		if (_lightMassListener)
			_lightMassListener->onBakingEnd();

		lmDestroy(ctx);
	}
	catch (...)
	{
		lmDestroy(ctx);
	}

	return true;
}

bool
LightMassBaking::saveAsTGA(const std::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t c)
{
	return lmImageSaveTGAf(path.c_str(), data, w, h, c);
}

bool 
LightMassBaking::initialize(const LightBakingOptions& params, LightMassContextGL& context) noexcept
{
	if (glewInit() != GLEW_OK)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not initialize with OpenGL.");

		return false;
	}

	glGenBuffers(1, &context.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, context.vbo);
	glBufferData(GL_ARRAY_BUFFER, params.model.numVertices * params.model.sizeofVertices, params.model.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &context.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, params.model.numIndices * params.model.sizeofIndices, params.model.indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &context.vao);
	glBindVertexArray(context.vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, params.model.sizeofVertices, (char*)nullptr + params.model.strideVertices);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, params.model.sizeofVertices, (char*)nullptr + params.model.strideTexcoord);

	glBindBuffer(GL_ARRAY_BUFFER, context.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context.ibo);

	glBindVertexArray(0);

	glGenTextures(1, &context.lightmap);
	glBindTexture(GL_TEXTURE_2D, context.lightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unsigned char emissive[] = { 0, 0, 0, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emissive);

	const char *vp =
		"#version 150 core\n"
		"in vec3 a_position;\n"
		"in vec2 a_texcoord;\n"
		"uniform mat4 u_mvp;\n"
		"out vec2 v_texcoord;\n"

		"void main()\n"
		"{\n"
		"v_texcoord = a_texcoord;\n"
		"gl_Position = u_mvp * vec4(a_position, 1.0);\n"
		"}\n";

	const char *fp =
		"#version 150 core\n"
		"in vec2 v_texcoord;\n"
		"uniform sampler2D u_lightmap;\n"
		"out vec4 o_color;\n"

		"void main()\n"
		"{\n"
		"o_color = vec4(texture(u_lightmap, v_texcoord).rgb, gl_FrontFacing ? 1.0 : 0.0);\n"
		"}\n";

	const char *attribs[] =
	{
		"a_position",
		"a_texcoord"
	};

	context.program = loadProgram(vp, fp, attribs, 2);
	if (!context.program)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to loading shader.");

		return false;
	}

	context.u_mvp = glGetUniformLocation(context.program, "u_mvp");
	context.u_lightmap = glGetUniformLocation(context.program, "u_lightmap");

	return true;
}

_NAME_END