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
#include <ray/graphics_input_layout.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsInputLayout, GraphicsChild, "GraphicsInputLayoutDesc")

VertexComponent::VertexComponent() noexcept
	: _index(0)
	, _slot(0)
	, _count(0)
	, _size(0)
	, _divisor(0)
	, _format(GraphicsFormat::GraphicsFormatUndefined)
{
}

VertexComponent::VertexComponent(const std::string& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint8_t slot, std::uint8_t divisor) noexcept
	: _semantic(semantic)
	, _index(semanticIndex)
	, _slot(slot)
	, _divisor(divisor)
	, _format(format)
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
VertexComponent::setVertexFormat(GraphicsFormat format) noexcept
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

GraphicsFormat
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
VertexComponent::getVertexCount(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormat::GraphicsFormatR8G8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8SRGB:
	case GraphicsFormat::GraphicsFormatR8G8SInt:
	case GraphicsFormat::GraphicsFormatR8G8UInt:
	case GraphicsFormat::GraphicsFormatR16G16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16SInt:
	case GraphicsFormat::GraphicsFormatR16G16UInt:
	case GraphicsFormat::GraphicsFormatR32G32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32SInt:
	case GraphicsFormat::GraphicsFormatR32G32UInt:
		return 2;
	case GraphicsFormat::GraphicsFormatR16G16B16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16B16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16UInt:
	case GraphicsFormat::GraphicsFormatR32G32B32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32UInt:
		return 3;
	case GraphicsFormat::GraphicsFormatR8G8B8A8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8A8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8A8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8A8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8A8UInt:
	case GraphicsFormat::GraphicsFormatR8G8B8A8SInt:
	case GraphicsFormat::GraphicsFormatR8G8B8A8SRGB:
	case GraphicsFormat::GraphicsFormatB8G8R8A8UNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8A8SNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8A8UScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8A8SScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8A8UInt:
	case GraphicsFormat::GraphicsFormatB8G8R8A8SInt:
	case GraphicsFormat::GraphicsFormatB8G8R8A8SRGB:
	case GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32:
	case GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UInt:
	case GraphicsFormat::GraphicsFormatR32G32B32A32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32A32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32A32UInt:
		return 4;
	default:
		assert(false);
		return 0;
	}
}

std::uint8_t
VertexComponent::getVertexSize(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormat::GraphicsFormatR8G8B8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8SNorm:
		return 3;
	case GraphicsFormat::GraphicsFormatR8G8B8A8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8A8SNorm:
	case GraphicsFormat::GraphicsFormatR16G16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16SInt:
	case GraphicsFormat::GraphicsFormatR16G16UInt:
		return 4;
	case GraphicsFormat::GraphicsFormatR16G16B16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16B16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16UInt:
		return 6;
	case GraphicsFormat::GraphicsFormatR32G32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32SInt:
	case GraphicsFormat::GraphicsFormatR32G32UInt:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UInt:
		return 8;
	case GraphicsFormat::GraphicsFormatR32G32B32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32UInt:
		return 12;
	case GraphicsFormat::GraphicsFormatR32G32B32A32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32A32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32A32UInt:
		return 16;
	default:
		assert(false);
		return 0;
	}
}

GraphicsInputLayoutDesc::GraphicsInputLayoutDesc() noexcept
	: _indexType(GraphicsIndexType::GraphicsIndexTypeNone)
{
}

GraphicsInputLayoutDesc::~GraphicsInputLayoutDesc() noexcept
{
}

void
GraphicsInputLayoutDesc::setVertexComponents(const VertexComponents& component) noexcept
{
	_components = component;
}

const VertexComponents&
GraphicsInputLayoutDesc::getVertexComponents() const noexcept
{
	return _components;
}

void
GraphicsInputLayoutDesc::addComponent(const VertexComponent& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getSemantic() == compoent.getSemantic();});
	if (it == _components.end())
		_components.push_back(compoent);
}

void
GraphicsInputLayoutDesc::removeComponent(const VertexComponent& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const VertexComponent& it) { return it.getSemantic() == compoent.getSemantic();});
	if (it != _components.end())
		_components.erase(it);
}

void
GraphicsInputLayoutDesc::setIndexType(GraphicsIndexType type) noexcept
{
	_indexType = type;
}

GraphicsIndexType
GraphicsInputLayoutDesc::getIndexType() const noexcept
{
	return _indexType;
}

std::uint32_t
GraphicsInputLayoutDesc::getVertexSize() const noexcept
{
	std::uint32_t size = 0;
	for (auto& it : _components)
		size += it.getVertexSize();
	return size;
}

std::uint32_t
GraphicsInputLayoutDesc::getIndexSize() const noexcept
{
	if (_indexType == GraphicsIndexType::GraphicsIndexTypeNone)
		return 0;
	else if (_indexType == GraphicsIndexType::GraphicsIndexTypeUint16)
		return sizeof(std::uint16_t);
	else if (_indexType == GraphicsIndexType::GraphicsIndexTypeUint32)
		return sizeof(std::uint32_t);
	else
	{
		assert(false);
		return 0;
	}
}

GraphicsInputLayout::GraphicsInputLayout() noexcept
{
}

GraphicsInputLayout::~GraphicsInputLayout() noexcept
{
}

_NAME_END