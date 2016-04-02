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
	, _indexType(GL_NONE)
	, _indexSize(0)
	, _vertexSize(0)
{
}

EGL3InputLayout::~EGL3InputLayout() noexcept
{
}

bool
EGL3InputLayout::setup(const GraphicsInputLayoutDesc& layout) noexcept
{
	assert(!_vao);

	_indexType = EGL3Types::asIndexType(layout.getIndexType());
	if (_indexType == GL_INVALID_ENUM)
		return false;

	_indexSize = layout.getIndexSize();
	if (_indexType != GL_NONE)
	{
		if (_indexSize == 0)
			return false;
	}

	_vertexSize = layout.getVertexSize();
	if (_vertexSize == GL_NONE)
		return false;

	auto& component = layout.getVertexComponents();
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

	_inputLayout = layout;

	glGenVertexArrays(1, &_vao);
	if (_vao == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateVertexArrays() fail");
		return false;
	}

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
EGL3InputLayout::bindLayout(const EGL3ShaderObjectPtr& program) noexcept
{
	glBindVertexArray(_vao);

	if (_program != program)
	{
		GLuint offset = 0;

		auto& components = _inputLayout.getVertexComponents();
		for (auto& it : components)
		{
			GLuint attribIndex = GL_INVALID_INDEX;
			GLuint bindingIndex = it.getVertexSlot();
			GLenum type = EGL3Types::asVertexFormat(it.getVertexFormat());

			auto& attributes = program->getActiveAttributes();
			for (auto& attrib : attributes)
			{
				if (attrib->getSemanticIndex() == it.getSemanticIndex() && attrib->getSemantic() == it.getSemantic())
				{
					attribIndex = attrib->downcast<EGL3ShaderAttribute>()->getLocation();
					break;
				}
			}

			if (attribIndex != GL_INVALID_INDEX)
			{
				glEnableVertexAttribArray(attribIndex);
				glVertexAttribBinding(attribIndex, bindingIndex);
				glVertexAttribFormat(attribIndex, it.getVertexCount(), type, GL_FALSE, offset);

				if (it.getVertexDivisor() > 0)
					glVertexBindingDivisor(bindingIndex, it.getVertexDivisor());
			}

			offset += it.getVertexSize();
		}

		_program = program;
	}
}

void
EGL3InputLayout::bindVbo(const EGL3GraphicsDataPtr& vbo) noexcept
{
	if (_vbo != vbo)
	{
		glBindVertexBuffer(0, vbo->getInstanceID(), 0, _vertexSize);
		_vbo = vbo;
	}
}

void
EGL3InputLayout::bindIbo(const EGL3GraphicsDataPtr& ibo) noexcept
{
	if (_ibo != ibo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
		_ibo = ibo;
	}
}

const GraphicsInputLayoutDesc&
EGL3InputLayout::getGraphicsInputLayoutDesc() const noexcept
{
	return _inputLayout;
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