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

const char* GlobalIlluminationVS =
R"(#version 150 core
	in vec3 position;
	in vec2 texcoord;

	uniform mat4 mvp;

	out vec2 oTexcoord;

	void main()
	{
		oTexcoord = texcoord;
		gl_Position = mvp * vec4(position, 1.0);
	};
)";

const char* GlobalIlluminationPS =
R"(#version 150 core
	in vec2 oTexcoord;
	uniform vec3 emissive;
	uniform sampler2D lightmap;
	void main()
	{
		gl_FragColor = vec4(texture(lightmap, oTexcoord).rgb * emissive, gl_FrontFacing ? 1.0 : 0.0);
	};
)";

const char* GlobalIlluminationAttribs[] =
{
	"position",
	"texcoord"
};

LightBakingGI::GLContext::GLContext() noexcept
	: program(0)
	, vs(0)
	, fs(0)
	, mvp(0)
	, lightsamp(0)
	, lightmap(0)
	, vao(0)
	, vbo(0)
	, ibo(0)
{
}

LightBakingGI::GLContext::~GLContext() noexcept
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

LightBakingGI::LightBakingGI(const LightBakingParams& params) noexcept
{
	this->open(params);
}

LightBakingGI::~LightBakingGI() noexcept
{
	this->close();
}

bool
LightBakingGI::open(const LightBakingParams& params) noexcept
{
	assert(params.lightMap);
	assert(params.lightMap->data);
	assert(params.lightMap->width >= 0 && params.lightMap->height >= 0);
	assert(params.lightMap->channel == 1 || params.lightMap->channel == 2 || params.lightMap->channel == 3 || params.lightMap->channel == 4);

	if (!this->setupContext(params.model))
		return false;

	if (!this->setup(params.baking))
	{
		if (this->getLightMassListener())
			this->getLightMassListener()->onMessage("Could not initialize with BakeTools.");

		return false;
	}

	GLenum faceType = GL_UNSIGNED_INT;
	if (params.model.sizeofIndices == 1)
		faceType = GL_UNSIGNED_BYTE;
	else if (params.model.sizeofIndices == 2)
		faceType = GL_UNSIGNED_SHORT;

	this->setRenderTarget(params.lightMap->data.get(), params.lightMap->width, params.lightMap->height, params.lightMap->channel);
	this->setGeometry(
		GL_FLOAT, params.model.vertices + params.model.strideVertices, params.model.sizeofVertices,
		GL_FLOAT, params.model.vertices + params.model.strideTexcoord, params.model.sizeofVertices,
		params.model.numIndices, faceType, params.model.indices);

	if (this->getLightMassListener())
		this->getLightMassListener()->onMessage("Calculating the bounding box of the model.");

	_boundingBoxs.resize(params.model.subsets.size());
	_drawcalls.resize(params.model.subsets.size());

	for (std::uint32_t i = 0; i < params.model.subsets.size(); i++)
	{
		ray::BoundingBox bound;
		this->computeBoundingBox(params.model, bound, params.model.subsets[i].drawcall.firstIndex * params.model.sizeofIndices, params.model.subsets[i].drawcall.count);
		_boundingBoxs[i] = bound;

		GLDrawElementsIndirectCommand cmd;
		cmd.baseInstance = params.model.subsets[i].drawcall.baseInstance;
		cmd.baseVertex = params.model.subsets[i].drawcall.baseVertex;
		cmd.count = params.model.subsets[i].drawcall.count;
		cmd.firstIndex = params.model.subsets[i].drawcall.firstIndex * params.model.sizeofIndices;
		cmd.instanceCount = params.model.subsets[i].drawcall.instanceCount;
		cmd.emissive = params.model.subsets[i].emissive;

		_drawcalls[i] = cmd;
	}

	if (this->getLightMassListener())
		this->getLightMassListener()->onMessage("Calculated the bounding box of the model.");

	_progress = params.baking.listener;

	return true;
}

void
LightBakingGI::close() noexcept
{
	_glcontext.reset();
}

bool
LightBakingGI::doSampleHemisphere(const Viewportt<int>& vp, const float4x4& mvp)
{
	assert(_glcontext);

	if (_progress)
	{
		if (!_progress(this->getSampleProcess()))
			return false;
	}

	glViewport(vp.left, vp.top, vp.width, vp.height);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(_glcontext->program);

	glUniform1i(_glcontext->lightsamp, 0);
	glUniformMatrix4fv(_glcontext->mvp, 1, GL_FALSE, mvp.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _glcontext->lightmap);

	glBindVertexArray(_glcontext->vao);

	Frustum fru;
	fru.extract(mvp);

	for (std::size_t i = 0; i < _drawcalls.size(); i++)
	{
		if (!fru.contains(_boundingBoxs[i].aabb()))
			continue;

		glUniform3f(_glcontext->emissive, _drawcalls[i].emissive.x, _drawcalls[i].emissive.y, _drawcalls[i].emissive.z);

		if (glDrawElementsInstancedBaseVertexBaseInstance)
		{
			glDrawElementsInstancedBaseVertexBaseInstance(
				GL_TRIANGLES,
				_drawcalls[i].count,
				_drawcalls[i].faceType,
				(char*)nullptr + _drawcalls[i].firstIndex,
				_drawcalls[i].instanceCount,
				_drawcalls[i].baseVertex,
				_drawcalls[i].baseInstance
			);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, _drawcalls[i].count, _drawcalls[i].faceType, (char*)nullptr + _drawcalls[i].firstIndex);
		}
	}

	return false;
}

void
LightBakingGI::computeBoundingBox(const LightModelData& model, ray::BoundingBox& boundingBox, std::uint32_t firstFace, std::size_t faceCount) noexcept
{
	boundingBox.reset();

	auto data = model.vertices + model.strideVertices;

	auto getFace = [](const LightModelData& model, std::size_t n, std::uint32_t firstIndex) -> std::uint32_t
	{
		std::uint8_t* data = (std::uint8_t*)model.indices + firstIndex;

		if (model.sizeofIndices == 1)
			return *(std::uint8_t*)(data + n * model.sizeofIndices);
		else if (model.sizeofIndices == 2)
			return *(std::uint16_t*)(data + n * model.sizeofIndices);
		else if (model.sizeofIndices == 4)
			return *(std::uint32_t*)(data + n * model.sizeofIndices);
		else
			return false;
	};

	for (std::size_t i = 0; i < faceCount; i++)
	{
		std::uint32_t face = getFace(model, i, firstFace);
		boundingBox.encapsulate(*(float3*)(data + face * model.sizeofVertices));
	}
}

bool
LightBakingGI::setupContext(const LightModelData& params) noexcept
{
	assert(params.vertices >= 0 && params.indices >= 0);
	assert(params.numVertices > 0 && params.numIndices > 0);
	assert(params.subsets.size() >= 1);
	assert(params.strideVertices < params.sizeofVertices && params.strideTexcoord < params.sizeofVertices);
	assert(params.sizeofVertices > 0);
	assert(params.sizeofIndices == 1 || params.sizeofIndices == 2 || params.sizeofIndices == 4);

	if (::glewInit() != GLEW_OK)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to init GLEW.");
	}

	auto glcontext = std::make_unique<GLContext>();

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

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

	std::uint8_t emissive[] = { 255, 255, 255, 255 };

	glGenTextures(1, &glcontext->lightmap);
	glBindTexture(GL_TEXTURE_2D, glcontext->lightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emissive);

	glcontext->vs = loadShader(GL_VERTEX_SHADER, GlobalIlluminationVS);
	if (!glcontext->vs)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to load vs.");

		return false;
	}

	glcontext->fs = loadShader(GL_FRAGMENT_SHADER, GlobalIlluminationPS);
	if (!glcontext->fs)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to load ps.");

		return false;
	}

	glcontext->program = loadProgram(glcontext->vs, glcontext->fs, GlobalIlluminationAttribs, 2);
	if (!glcontext->program)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to load shader.");

		return false;
	}

	glcontext->mvp = glGetUniformLocation(glcontext->program, "mvp");
	glcontext->lightsamp = glGetUniformLocation(glcontext->program, "lightmap");
	glcontext->emissive = glGetUniformLocation(glcontext->program, "emissive");

	_glcontext = std::move(glcontext);

	return true;
}

_NAME_END