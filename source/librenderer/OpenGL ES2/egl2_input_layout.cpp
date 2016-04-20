// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include "egl2_input_layout.h"
#include "egl2_shader.h"
#include "egl2_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL2InputLayout, GraphicsInputLayout, "EGL2InputLayout")

EGL2InputLayout::EGL2InputLayout() noexcept
	: _vao(GL_NONE)
	, _indexType(GL_NONE)
	, _indexSize(0)
{
}

EGL2InputLayout::~EGL2InputLayout() noexcept
{
	this->close();
}

bool
EGL2InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
{
#ifndef __AMD__
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_array_object))
	{
		glGenVertexArraysOES(1, &_vao);
		if (_vao == GL_NONE)
		{
			GL_PLATFORM_LOG("glCreateVertexArrays() fail");
			return false;
		}
	}
#endif

	_inputLayoutDesc = inputLayoutDesc;
	return true;
}

void
EGL2InputLayout::close() noexcept
{
}

void
EGL2InputLayout::bindLayout(const EGL2ProgramPtr& program) noexcept
{
#ifndef __AMD__
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_array_object))
	{
		glBindVertexArrayOES(_vao);
	}
#endif

	if (_program != program)
	{
		GLuint offset = 0;

		auto& components = _inputLayoutDesc.getGraphicsVertexLayouts();
		for (auto& it : components)
		{
			GLuint attribIndex = GL_INVALID_ENUM;
			GLenum type = EGL2Types::asVertexFormat(it.getVertexFormat());

			auto& attributes = program->getActiveAttributes();
			for (auto& attrib : attributes)
			{
				if (attrib->getSemanticIndex() == it.getSemanticIndex() && attrib->getSemantic() == it.getSemantic())
				{
					attribIndex = attrib->downcast<EGL2GraphicsAttribute>()->getBindingPoint();
					break;
				}
			}

			if (attribIndex != GL_INVALID_ENUM)
			{
				glEnableVertexAttribArray(attribIndex);
				glVertexAttribPointer(attribIndex, it.getVertexCount(), type, GL_FALSE, _vertexSize, (GLchar*)nullptr + offset);
			}

			offset += it.getVertexSize();
		}

		_program = program;
	}
}

void
EGL2InputLayout::bindVbo(const EGL2GraphicsDataPtr& vbo) noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getInstanceID());
}

void
EGL2InputLayout::bindIbo(const EGL2GraphicsDataPtr& ibo) noexcept
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
}

const GraphicsInputLayoutDesc&
EGL2InputLayout::getGraphicsInputLayoutDesc() const noexcept
{
	return _inputLayoutDesc;
}

void
EGL2InputLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2InputLayout::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END