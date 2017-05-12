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

LightBakingAO::~LightBakingAO() noexcept
{
	this->close();
}

bool
LightBakingAO::open(const LightModelData& params) noexcept
{
	assert(params.vertices >= 0 && params.indices >= 0);
	assert(params.numVertices > 0 && params.numIndices > 0);
	assert(params.subsets.size() >= 1);
	assert(params.strideVertices < params.sizeofVertices && params.strideTexcoord < params.sizeofVertices);
	assert(params.sizeofVertices > 0);
	assert(params.sizeofIndices == 1 || params.sizeofIndices == 2 || params.sizeofIndices == 4);

	auto glcontext = std::make_unique<GLContext>();

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
		return false;

	glcontext->fs = loadShader(GL_FRAGMENT_SHADER, OcclusionPS);
	if (!glcontext->fs)
		return false;

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
LightBakingAO::close() noexcept
{
	_glcontext.reset();
}

void 
LightBakingAO::doSampleHemisphere(const LightBakingParams& params, const Viewportt<int>& vp, const float4x4& mvp)
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
	glUniformMatrix4fv(_glcontext->mvp, 1, GL_FALSE, mvp.ptr());

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

			GLDrawElementsIndirectCommand command;
			command.baseInstance = it.drawcall.baseInstance;
			command.baseVertex = it.drawcall.baseVertex;
			command.count = it.drawcall.count;
			command.firstIndex = it.drawcall.firstIndex;
			command.instanceCount = it.drawcall.instanceCount;

			_drawcalls.push_back(command);
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