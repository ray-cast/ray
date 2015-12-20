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
#include "ogl_layout.h"

_NAME_BEGIN

__ImplementSubClass(OGLGraphicsLayout, GraphicsLayout, "OGLGraphicsLayout")

OGLGraphicsLayout::OGLGraphicsLayout() noexcept
	: _vao(GL_NONE)
	, _vertexSize(0)
	, _indexType(GL_NONE)
	, _indexSize(0)
{
}

OGLGraphicsLayout::~OGLGraphicsLayout() noexcept
{
}

bool
OGLGraphicsLayout::open(const GraphicsLayoutDesc& layout) noexcept
{
	assert(!_vao);

	_indexType = OGLTypes::asOGLIndexType(layout.getIndexType());
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

	_layout = layout;

	glGenVertexArrays(1, &_vao);
	if (_vao != GL_NONE)
	{
		glBindVertexArray(_vao);

		GLuint offset = 0;

		auto& components = this->getVertexComponents();
		for (auto& it : components)
		{
			GLenum type = OGLTypes::asOGLVertexFormat(it.getVertexFormat());
			if (type != GL_INVALID_ENUM)
			{
				GLboolean normalize = it.getNormalize() ? GL_TRUE : GL_FALSE;

				glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
				glVertexAttribFormat(it.getVertexAttrib(), it.getVertexCount(), type, normalize, offset);
				glVertexAttribBinding(it.getVertexAttrib(), 0);
			}

			offset += it.getVertexSize();
		}

		glBindVertexArray(GL_NONE);

		return true;
	}

	return false;
}

void
OGLGraphicsLayout::close() noexcept
{
	if (_vao)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

void
OGLGraphicsLayout::setVertexComponents(const VertexComponents& component) noexcept
{
	_layout.setVertexComponents(component);
}

const VertexComponents&
OGLGraphicsLayout::getVertexComponents() const noexcept
{
	return _layout.getVertexComponents();
}

void
OGLGraphicsLayout::addComponent(const VertexComponent& compoent) noexcept
{
	_layout.addComponent(compoent);
}

void
OGLGraphicsLayout::removeComponent(const VertexComponent& compoent) noexcept
{
	_layout.removeComponent(compoent);
}

void
OGLGraphicsLayout::setIndexType(GLenum type) noexcept
{
	if (type == GL_UNSIGNED_SHORT)
		_layout.setIndexType(IndexType::Uint16);
	else if (type == GL_UNSIGNED_INT)
		_layout.setIndexType(IndexType::Uint32);
	else
		_layout.setIndexType(IndexType::None);
}

GLenum
OGLGraphicsLayout::getIndexType() const noexcept
{
	return _indexType;
}

GLsizei
OGLGraphicsLayout::getIndexSize() const noexcept
{
	return _indexSize;
}

GLsizei
OGLGraphicsLayout::getVertexSize() const noexcept
{
	return _vertexSize;
}

const GraphicsLayoutDesc&
OGLGraphicsLayout::getGraphicsLayout() const noexcept
{
	return _layout;
}

void
OGLGraphicsLayout::bindLayout() noexcept
{
	glBindVertexArray(_vao);
}

_NAME_END