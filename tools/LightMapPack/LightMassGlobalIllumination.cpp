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
#include "LightMassGlobalIllumination.h"

#include <GL/glew.h>

_NAME_BEGIN

LightBakingGI::LightMassContextGL::LightMassContextGL() noexcept
	: program(0)
	, vs(0)
	, fs(0)
	, u_lightmap(0)
	, u_mvp(0)
	, lightmap(0)
	, vao(0)
	, vbo(0)
	, ibo(0)
{
}

LightBakingGI::LightMassContextGL::~LightMassContextGL() noexcept
{
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
	glDeleteTextures(1, &lightmap);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

LightBakingGI::LightBakingGI() noexcept
{
}

LightBakingGI::~LightBakingGI() noexcept
{
	this->close();
}

bool
LightBakingGI::open(const LightModelData& params) noexcept
{
	assert(params.vertices >= 0 && params.indices >= 0);
	assert(params.numVertices > 0 && params.numIndices > 0);
	assert(params.subsets.size() >= 1);
	assert(params.strideVertices < params.sizeofVertices && params.strideTexcoord < params.sizeofVertices);
	assert(params.sizeofVertices > 0);
	assert(params.sizeofIndices == 1 || params.sizeofIndices == 2 || params.sizeofIndices == 3);

	auto glcontext = std::make_unique<LightMassContextGL>();

	glGenBuffers(1, &glcontext->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBufferData(GL_ARRAY_BUFFER, params.numVertices * params.sizeofVertices, params.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &glcontext->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, params.numIndices * params.sizeofIndices, params.indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &glcontext->vao);
	glBindVertexArray(glcontext->vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, params.sizeofVertices, (char*)nullptr + params.strideVertices);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, params.sizeofVertices, (char*)nullptr + params.strideTexcoord);

	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);

	glBindVertexArray(0);

	std::uint8_t emissive[] = { 0, 0, 0, 255 };

	glGenTextures(1, &glcontext->lightmap);
	glBindTexture(GL_TEXTURE_2D, glcontext->lightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

	glcontext->vs = loadShader(GL_VERTEX_SHADER, vp);
	if (!glcontext->vs)
		return false;

	glcontext->fs = loadShader(GL_FRAGMENT_SHADER, fp);
	if (!glcontext->fs)
		return false;

	glcontext->program = loadProgram(glcontext->vs, glcontext->fs, attribs, 2);
	if (!glcontext->program)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to loading shader.");

		return false;
	}

	glcontext->u_mvp = glGetUniformLocation(glcontext->program, "u_mvp");
	glcontext->u_lightmap = glGetUniformLocation(glcontext->program, "u_lightmap");

	_glcontext = std::move(glcontext);

	return true;
}

void
LightBakingGI::close() noexcept
{
	_glcontext.reset();
}

void
LightBakingGI::doSampleHemisphere(const LightBakingOptions& params, const Viewportt<int>& vp, const float4x4& mvp)
{
	assert(_glcontext);

	GLenum faceType = GL_UNSIGNED_INT;
	if (params.model.sizeofIndices == 1)
		faceType = GL_UNSIGNED_BYTE;
	else if (params.model.sizeofIndices == 2)
		faceType = GL_UNSIGNED_SHORT;

	glViewport(vp.left, vp.top, vp.width, vp.height);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(_glcontext->program);
	glUniformMatrix4fv(_glcontext->u_mvp, 1, GL_FALSE, mvp.ptr());

	glBindVertexArray(_glcontext->vao);

	Frustum fru;
	fru.extract(mvp);

	if (glMultiDrawElementsIndirect)
	{
		_drawcalls.clear();

		for (auto& it : params.model.subsets)
		{
			if (!fru.contains(it.boundingBox.aabb()))
				continue;

			LightModelDrawCall drawcall;
			drawcall.baseInstance = it.drawcall.baseInstance;
			drawcall.baseVertex = it.drawcall.baseVertex;
			drawcall.count = it.drawcall.count;
			drawcall.firstIndex = it.drawcall.firstIndex;
			drawcall.instanceCount = it.drawcall.instanceCount;

			_drawcalls.push_back(drawcall);
		}

		glMultiDrawElementsIndirect(GL_TRIANGLES, faceType, _drawcalls.data(), _drawcalls.size(), 0);
	}
	else
	{
		for (auto& it : params.model.subsets)
		{
			if (!fru.contains(it.boundingBox.aabb()))
				continue;

			if (glDrawElementsInstancedBaseVertexBaseInstance)
			{
				glDrawElementsInstancedBaseVertexBaseInstance(
					GL_TRIANGLES,
					it.drawcall.count,
					faceType,
					(char*)nullptr + it.drawcall.firstIndex * params.model.sizeofIndices,
					it.drawcall.instanceCount,
					it.drawcall.baseVertex,
					it.drawcall.baseInstance
				);
			}
			else
			{
				glDrawElements(GL_TRIANGLES, it.drawcall.count, faceType, (char*)nullptr + it.drawcall.firstIndex * params.model.sizeofIndices);
			}
		}
	}
}

_NAME_END