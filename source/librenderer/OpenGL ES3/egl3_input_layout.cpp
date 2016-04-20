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
#include "egl3_input_layout.h"
#include "egl3_shader.h"
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3InputLayout, GraphicsInputLayout, "EGL3InputLayout")

EGL3InputLayout::EGL3InputLayout() noexcept
	: _vao(GL_NONE)
{
}

EGL3InputLayout::~EGL3InputLayout() noexcept
{
	this->close();
}

bool
EGL3InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
{
	assert(!_vao);

	auto& component = inputLayoutDesc.getGraphicsVertexLayouts();
	for (auto& it : component)
	{
		auto& semantic = it.getSemantic();
		if (semantic.empty())
		{
			GL_PLATFORM_LOG("Empty semantic");
			return false;
		}

		for (auto& ch : semantic)
		{
			if (ch < 'a' && ch > 'z')
			{
				GL_PLATFORM_LOG("Error semantic describe : %s", semantic);
				return false;
			}
		}
	}

	glGenVertexArrays(1, &_vao);
	if (_vao == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateVertexArrays() fail");
		return false;
	}

	_inputLayoutDesc = inputLayoutDesc;
	return true;
}

void
EGL3InputLayout::close() noexcept
{
	if (_vao)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

void
EGL3InputLayout::bindLayout(const EGL3ProgramPtr& program) noexcept
{
	glBindVertexArray(_vao);

	if (_program != program)
	{
		GLuint offset = 0;

		auto& components = _inputLayoutDesc.getGraphicsVertexLayouts();
		for (auto& it : components)
		{
			GLuint attribIndex = GL_INVALID_INDEX;
			GLenum type = EGL3Types::asVertexFormat(it.getVertexFormat());

			auto& attributes = program->getActiveAttributes();
			for (auto& attrib : attributes)
			{
				if (attrib->getSemanticIndex() == it.getSemanticIndex() && attrib->getSemantic() == it.getSemantic())
				{
					attribIndex = attrib->downcast<EGL3GraphicsAttribute>()->getBindingPoint();
					break;
				}
			}

			if (attribIndex != GL_INVALID_INDEX)
			{
				glEnableVertexAttribArray(attribIndex);
				glVertexAttribBinding(attribIndex, 0);
				glVertexAttribFormat(attribIndex, it.getVertexCount(), type, GL_FALSE, offset);
			}

			offset += it.getVertexSize();
		}

		_program = program;
	}
}

void
EGL3InputLayout::bindVbo(const EGL3GraphicsDataPtr& vbo, GLuint bindingPoint) noexcept
{
	GLuint stride = vbo->getGraphicsDataDesc().getStride();
	glBindVertexBuffer(bindingPoint, vbo->getInstanceID(), 0, stride);
}

void
EGL3InputLayout::bindIbo(const EGL3GraphicsDataPtr& ibo) noexcept
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
}

const GraphicsInputLayoutDesc&
EGL3InputLayout::getGraphicsInputLayoutDesc() const noexcept
{
	return _inputLayoutDesc;
}

void
EGL3InputLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3InputLayout::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END