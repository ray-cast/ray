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
#include "egl2_layout.h"

_NAME_BEGIN

__ImplementSubClass(EGL2GraphicsLayout, GraphicsLayout, "EGL2GraphicsLayout")

EGL2GraphicsLayout::EGL2GraphicsLayout() noexcept
	: _vertexSize(0)
	, _indexType(GL_NONE)
	, _indexSize(0)
{
}

EGL2GraphicsLayout::~EGL2GraphicsLayout() noexcept
{
}

bool
EGL2GraphicsLayout::open(const GraphicsLayoutDesc& layout) noexcept
{
	_indexType = EGL2Types::asEGL2IndexType(layout.getIndexType());
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

	return true;
}

void
EGL2GraphicsLayout::close() noexcept
{
}

void
EGL2GraphicsLayout::setVertexComponents(const VertexComponents& component) noexcept
{
	_layout.setVertexComponents(component);
}

const VertexComponents&
EGL2GraphicsLayout::getVertexComponents() const noexcept
{
	return _layout.getVertexComponents();
}

void
EGL2GraphicsLayout::addComponent(const VertexComponent& compoent) noexcept
{
	_layout.addComponent(compoent);
}

void
EGL2GraphicsLayout::removeComponent(const VertexComponent& compoent) noexcept
{
	_layout.removeComponent(compoent);
}

void
EGL2GraphicsLayout::setIndexType(GLenum type) noexcept
{
	if (type == GL_UNSIGNED_SHORT)
		_layout.setIndexType(IndexType::Uint16);
	else if (type == GL_UNSIGNED_INT)
		_layout.setIndexType(IndexType::Uint32);
	else
		_layout.setIndexType(IndexType::None);
}

GLenum
EGL2GraphicsLayout::getIndexType() const noexcept
{
	return _indexType;
}

GLsizei
EGL2GraphicsLayout::getIndexSize() const noexcept
{
	return _indexSize;
}

GLsizei
EGL2GraphicsLayout::getVertexSize() const noexcept
{
	return _vertexSize;
}

void
EGL2GraphicsLayout::getGraphicsLayout(GraphicsLayoutDesc& layout) const noexcept
{
	layout = _layout;
}

void
EGL2GraphicsLayout::bindLayout() noexcept
{
}

_NAME_END