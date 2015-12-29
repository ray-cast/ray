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

VertexComponent::VertexComponent() noexcept
	: _format(VertexFormat::Float)
	, _index(0)
	, _slot(0)
	, _count(0)
	, _size(0)
	, _divisor(0)
{
}

VertexComponent::VertexComponent(const std::string& semantic, std::uint8_t semanticIndex, VertexFormat format, std::uint8_t slot, std::uint8_t divisor) noexcept
	: _format(format)
	, _semantic(semantic)
	, _index(semanticIndex)
	, _slot(slot)
	, _divisor(divisor)
{
	_count = getVertexCount(format);
	_size = getVertexSize(format);
}

VertexComponent::~VertexComponent() noexcept
{
}

void
VertexComponent::setSemantic(const std::string& semantic) noexcept
{
	_semantic = semantic;
}

void
VertexComponent::setVertexFormat(VertexFormat format) noexcept
{
	if (_format != format)
	{
		_count = getVertexCount(format);
		_size = getVertexSize(format);

		_format = format;
	}
}

const std::string&
VertexComponent::getSemantic() const noexcept
{
	return _semantic;
}

void 
VertexComponent::setSemanticIndex(std::uint8_t index) noexcept
{
	_index = index;
}

std::uint8_t 
VertexComponent::getSemanticIndex() const noexcept
{
	return _index;
}

VertexFormat
VertexComponent::getVertexFormat() const noexcept
{
	return _format;
}

void 
VertexComponent::setVertexSlot(std::uint8_t slot) noexcept
{
	_slot = slot;
}

std::uint8_t 
VertexComponent::getVertexSlot() const noexcept
{
	return _slot;
}

void
VertexComponent::setVertexDivisor(std::uint8_t divisor) noexcept
{
	_divisor = divisor;
}

std::uint8_t
VertexComponent::getVertexDivisor() const noexcept
{
	return _divisor;
}

std::uint8_t
VertexComponent::getVertexCount() const noexcept
{
	return _count;
}

std::uint8_t
VertexComponent::getVertexSize() const noexcept
{
	return _size;
}

std::uint8_t
VertexComponent::getVertexCount(VertexFormat format) noexcept
{
	if (format == VertexFormat::Char || format == VertexFormat::Uchar ||
		format == VertexFormat::Short || format == VertexFormat::Ushort ||
		format == VertexFormat::Int || format == VertexFormat::Uint ||
		format == VertexFormat::Float)
	{
		return 1;
	}
	else if (format == VertexFormat::Char2 || format == VertexFormat::Uchar2 ||
		format == VertexFormat::Short2 || format == VertexFormat::Ushort2 ||
		format == VertexFormat::Int2 || format == VertexFormat::Uint2 ||
		format == VertexFormat::Float2)
	{
		return 2;
	}
	else if (format == VertexFormat::Char3 || format == VertexFormat::Uchar3 ||
		format == VertexFormat::Short3 || format == VertexFormat::Short3 ||
		format == VertexFormat::Int3 || format == VertexFormat::Uint3 ||
		format == VertexFormat::Float3)
	{
		return 3;
	}
	else if (format == VertexFormat::Char4 || format == VertexFormat::Uchar4 ||
		format == VertexFormat::Short4 || format == VertexFormat::Ushort4 ||
		format == VertexFormat::Int4 || format == VertexFormat::Uint4 ||
		format == VertexFormat::Float4)
	{
		return 4;
	}
	else if (format == VertexFormat::Float3x3)
	{
		return 12;
	}
	else if (format == VertexFormat::Float4x4)
	{
		return 16;
	}
	else
	{
		assert(false);
		return 0;
	}
}

std::uint8_t
VertexComponent::getVertexSize(VertexFormat format) noexcept
{
	switch (format)
	{
	case VertexFormat::Char:
	case VertexFormat::Uchar:
		return 1;
	case VertexFormat::Char2:
	case VertexFormat::Uchar2:
	case VertexFormat::Short:
	case VertexFormat::Ushort:
		return 2;
	case VertexFormat::Char3:
	case VertexFormat::Uchar3:
		return 3;
	case VertexFormat::Char4:
	case VertexFormat::Uchar4:
	case VertexFormat::Short2:
	case VertexFormat::Ushort2:
	case VertexFormat::Int:
	case VertexFormat::Uint:
	case VertexFormat::Float:
		return 4;
	case VertexFormat::Short3:
	case VertexFormat::Ushort3:
		return 6;
	case VertexFormat::Int2:
	case VertexFormat::Uint2:
	case VertexFormat::Short4:
	case VertexFormat::Ushort4:
	case VertexFormat::Float2:
		return 8;
	case VertexFormat::Int3:
	case VertexFormat::Uint3:
	case VertexFormat::Float3:
		return 12;
	case VertexFormat::Float4:
	case VertexFormat::Int4:
	case VertexFormat::Uint4:
		return 16;
	case VertexFormat::Float3x3:
		return 48;
	case VertexFormat::Float4x4:
		return 56;
	default:
		assert(false);
	}

	return 0;
}

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
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getSemantic() == compoent.getSemantic();});
	if (it == _components.end())
		_components.push_back(compoent);
}

void
GraphicsLayoutDesc::removeComponent(const VertexComponent& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getSemantic() == compoent.getSemantic();});
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