// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include "egl3_layout.h"
#include "egl3_vbo.h"
#include "egl3_ibo.h"
#include "egl3_shader.h"

_NAME_BEGIN

__ImplementSubClass(EGL3GraphicsLayout, GraphicsLayout, "EGL3GraphicsLayout")

EGL3GraphicsLayout::EGL3GraphicsLayout() noexcept
	: _vao(GL_NONE)
	, _indexType(GL_NONE)
	, _indexSize(0)
	, _vertexSize(0)
{
}

EGL3GraphicsLayout::~EGL3GraphicsLayout() noexcept
{
}

bool
EGL3GraphicsLayout::open(const GraphicsLayoutDesc& layout) noexcept
{
	assert(!_vao);

	_indexType = EGL3Types::asEGL3IndexType(layout.getIndexType());
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

	_layout = layout;

	glGenVertexArrays(1, &_vao);
	if (_vao == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateVertexArrays() fail");
		return false;
	}

	return true;
}

void
EGL3GraphicsLayout::close() noexcept
{
	if (_vao)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

void
EGL3GraphicsLayout::setVertexComponents(const VertexComponents& component) noexcept
{
	_layout.setVertexComponents(component);
}

const VertexComponents&
EGL3GraphicsLayout::getVertexComponents() const noexcept
{
	return _layout.getVertexComponents();
}

void
EGL3GraphicsLayout::addComponent(const VertexComponent& compoent) noexcept
{
	_layout.addComponent(compoent);
}

void
EGL3GraphicsLayout::removeComponent(const VertexComponent& compoent) noexcept
{
	_layout.removeComponent(compoent);
}

void
EGL3GraphicsLayout::setIndexType(GLenum type) noexcept
{
	if (type == GL_UNSIGNED_SHORT)
		_layout.setIndexType(IndexType::Uint16);
	else if (type == GL_UNSIGNED_INT)
		_layout.setIndexType(IndexType::Uint32);
	else
		_layout.setIndexType(IndexType::None);
}

GLenum
EGL3GraphicsLayout::getIndexType() const noexcept
{
	return _indexType;
}

GLsizei
EGL3GraphicsLayout::getIndexSize() const noexcept
{
	return _indexSize;
}

GLsizei
EGL3GraphicsLayout::getVertexSize() const noexcept
{
	return _vertexSize;
}

const GraphicsLayoutDesc&
EGL3GraphicsLayout::getGraphicsLayout() const noexcept
{
	return _layout;
}

void
EGL3GraphicsLayout::bindLayout(const EGL3ShaderObjectPtr& program) noexcept
{
	glBindVertexArray(_vao);

	if (_program != program)
	{
		GLuint offset = 0;

		auto& components = this->getVertexComponents();
		for (auto& it : components)
		{
			GLuint attribIndex = GL_INVALID_INDEX;
			GLuint bindingIndex = it.getVertexSlot();
			GLenum type = EGL3Types::asEGL3VertexFormat(it.getVertexFormat());

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
EGL3GraphicsLayout::bindVbo(const EGL3VertexBufferPtr& vbo) noexcept
{
	if (_vbo != vbo)
	{
		glBindVertexBuffer(0, vbo->getInstanceID(), 0, _vertexSize);
		_vbo = vbo;
	}
}

void
EGL3GraphicsLayout::bindIbo(const EGL3IndexBufferPtr& ibo) noexcept
{
	if (_ibo != ibo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
		_ibo = ibo;
	}
}

void
EGL3GraphicsLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3GraphicsLayout::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END