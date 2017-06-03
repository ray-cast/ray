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
#include "LightMassAmbientOcclusion.h"

#include <GL/glew.h>

_NAME_BEGIN

const char* OcclusionVS =
R"(#version 150 core
	in vec3 position;
	uniform mat4 mvp;

	void main()
	{
		gl_Position = mvp * vec4(position, 1.0);
	};
)";

const char* OcclusionPS =
R"(#version 150 core
	void main()
	{
		gl_FragColor = vec4(0,0,0, gl_FrontFacing ? 1.0 : 0.0);
	};
)";

const char* OcclusionAttribs[] =
{
	"position",
};

LightBakingAO::GLContext::GLContext() noexcept
	: program(0)
	, vs(0)
	, fs(0)
	, mvp(0)
	, vao(0)
	, vbo(0)
	, ibo(0)
{
}

LightBakingAO::GLContext::~GLContext() noexcept
{
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

LightBakingAO::LightBakingAO() noexcept
{
}

LightBakingAO::LightBakingAO(const LightBakingParams& params)
{
	this->open(params);
}

LightBakingAO::~LightBakingAO() noexcept
{
	this->close();
}

bool
LightBakingAO::open(const LightBakingParams& params) noexcept
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
		cmd.faceType = faceType;

		_drawcalls[i] = cmd;
	}

	if (this->getLightMassListener())
		this->getLightMassListener()->onMessage("Calculated the bounding box of the model.");

	_progress = params.baking.listener;

	return true;
}

void
LightBakingAO::close() noexcept
{
	_glcontext.reset();
}

bool
LightBakingAO::doSampleHemisphere(const Viewportt<int>& vp, const float4x4& mvp)
{
	assert(_glcontext);

	if (_progress)
	{
		if (!_progress(this->getSampleProcess()))
			return false;
	}

	Frustum fru;
	fru.extract(mvp);

	glViewport(vp.left, vp.top, vp.width, vp.height);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(_glcontext->program);
	glUniformMatrix4fv(_glcontext->mvp, 1, GL_FALSE, mvp.ptr());
	glBindVertexArray(_glcontext->vao);

	for (std::size_t i = 0; i < _drawcalls.size(); i++)
	{
		if (!fru.contains(_boundingBoxs[i].aabb()))
			continue;

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

	return true;
}

bool
LightBakingAO::setupContext(const LightModelData& params) noexcept
{
	assert(params.vertices >= 0 && params.indices >= 0);
	assert(params.numVertices > 0 && params.numIndices > 0);
	assert(params.subsets.size() >= 1);
	assert(params.strideVertices < params.sizeofVertices && params.strideTexcoord < params.sizeofVertices);
	assert(params.sizeofVertices > 0);
	assert(params.sizeofIndices == 1 || params.sizeofIndices == 2 || params.sizeofIndices == 4);

	auto glcontext = std::make_unique<GLContext>();

	if (::glewInit() != GLEW_OK)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to init GLEW.");

		return false;
	}

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glGenBuffers(1, &glcontext->vbo);
	if (glcontext->vbo == GL_NONE)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not create a vertex buffer.");

		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBufferData(GL_ARRAY_BUFFER, params.numVertices * params.sizeofVertices, params.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &glcontext->ibo);
	if (glcontext->ibo == GL_NONE)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not create a element buffer.");

		return false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, params.numIndices * params.sizeofIndices, params.indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &glcontext->vao);
	if (glcontext->vao == GL_NONE)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not create a vertex array object.");

		return false;
	}

	glBindVertexArray(glcontext->vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, params.sizeofVertices, (char*)nullptr + params.strideVertices);

	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);

	glBindVertexArray(GL_NONE);

	glcontext->vs = loadShader(GL_VERTEX_SHADER, OcclusionVS);
	if (!glcontext->vs)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to load vs.");

		return false;
	}

	glcontext->fs = loadShader(GL_FRAGMENT_SHADER, OcclusionPS);
	if (!glcontext->fs)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to load ps.");

		return false;
	}

	glcontext->program = loadProgram(glcontext->vs, glcontext->fs, OcclusionAttribs, 1);
	if (!glcontext->program)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to loading shader.");

		return false;
	}

	glcontext->mvp = glGetUniformLocation(glcontext->program, "mvp");
	if (glcontext->mvp == GL_INVALID_INDEX)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Failed to get location.");

		return false;
	}

	_glcontext = std::move(glcontext);

	return true;
}

void
LightBakingAO::computeBoundingBox(const LightModelData& model, ray::BoundingBox& boundingBox, std::uint32_t firstFace, std::size_t faceCount) noexcept
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

_NAME_END