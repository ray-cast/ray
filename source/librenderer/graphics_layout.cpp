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
#include <ray/graphics_layout.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsLayout, GraphicsChild, "GraphicsLayoutDesc")

GraphicsLayoutDesc::GraphicsLayoutDesc() noexcept
	: _indexType(IndexType::None)
{
}

GraphicsLayoutDesc::~GraphicsLayoutDesc() noexcept
{
}

void
GraphicsLayoutDesc::setVertexComponents(const VertexComponents& component) noexcept
{
	_components = component;
}

const VertexComponents&
GraphicsLayoutDesc::getVertexComponents() const noexcept
{
	return _components;
}

void
GraphicsLayoutDesc::addComponent(const VertexComponent& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getVertexAttrib() == compoent.getVertexAttrib();});
	if (it == _components.end())
		_components.push_back(compoent);
}

void
GraphicsLayoutDesc::removeComponent(const VertexComponent& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getVertexAttrib() == compoent.getVertexAttrib();});
	if (it != _components.end())
		_components.erase(it);
}

void 
GraphicsLayoutDesc::setIndexType(IndexType type) noexcept
{
	_indexType = type;
}

IndexType 
GraphicsLayoutDesc::getIndexType() const noexcept
{
	return _indexType;
}

std::uint32_t
GraphicsLayoutDesc::getVertexSize() const noexcept
{
	std::uint32_t size = 0;
	for (auto& it : _components)
		size += it.getVertexSize();
	return size;
}

std::uint32_t 
GraphicsLayoutDesc::getIndexSize() const noexcept
{
	return _indexType == IndexType::Uint16 ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
}

GraphicsLayout::GraphicsLayout() noexcept
{
}

GraphicsLayout::~GraphicsLayout() noexcept
{
}

_NAME_END