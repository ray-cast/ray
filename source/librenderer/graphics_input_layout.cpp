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

GraphicsVertexLayout::GraphicsVertexLayout() noexcept
	: _index(0)
	, _count(0)
	, _size(0)
	, _offset(0)
	, _format(GraphicsFormat::GraphicsFormatUndefined)
{
}

GraphicsVertexLayout::GraphicsVertexLayout(const std::string& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint16_t offset, std::uint8_t slot) noexcept
	: _semantic(semantic)
	, _index(semanticIndex)
	, _offset(offset)
	, _format(format)
{
	_count = getVertexCount(format);
	_size = getVertexSize(format);
}

GraphicsVertexLayout::~GraphicsVertexLayout() noexcept
{
}

void
GraphicsVertexLayout::setSemantic(const std::string& semantic) noexcept
{
	_semantic = semantic;
}

void
GraphicsVertexLayout::setVertexFormat(GraphicsFormat format) noexcept
{
	if (_format != format)
	{
		_count = getVertexCount(format);
		_size = getVertexSize(format);

		_format = format;
	}
}

const std::string&
GraphicsVertexLayout::getSemantic() const noexcept
{
	return _semantic;
}

void
GraphicsVertexLayout::setSemanticIndex(std::uint8_t index) noexcept
{
	_index = index;
}

std::uint8_t
GraphicsVertexLayout::getSemanticIndex() const noexcept
{
	return _index;
}

GraphicsFormat
GraphicsVertexLayout::getVertexFormat() const noexcept
{
	return _format;
}

void
GraphicsVertexLayout::setVertexOffset(std::uint16_t offset) noexcept
{
	_offset = offset;
}

std::uint16_t
GraphicsVertexLayout::getVertexOffset() const noexcept
{
	return _offset;
}

std::uint8_t
GraphicsVertexLayout::getVertexCount() const noexcept
{
	return _count;
}

std::uint8_t
GraphicsVertexLayout::getVertexSize() const noexcept
{
	return _size;
}

std::uint8_t
GraphicsVertexLayout::getVertexCount(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormat::GraphicsFormatR8UNorm:
	case GraphicsFormat::GraphicsFormatR8SNorm:
	case GraphicsFormat::GraphicsFormatR8UScaled:
	case GraphicsFormat::GraphicsFormatR8SScaled:
	case GraphicsFormat::GraphicsFormatR8UInt:
	case GraphicsFormat::GraphicsFormatR8SInt:
	case GraphicsFormat::GraphicsFormatR8SRGB:
	case GraphicsFormat::GraphicsFormatR16UNorm:
	case GraphicsFormat::GraphicsFormatR16SNorm:
	case GraphicsFormat::GraphicsFormatR16UScaled:
	case GraphicsFormat::GraphicsFormatR16SScaled:
	case GraphicsFormat::GraphicsFormatR16UInt:
	case GraphicsFormat::GraphicsFormatR16SInt:
	case GraphicsFormat::GraphicsFormatR16SFloat:
	case GraphicsFormat::GraphicsFormatD16UNorm:
	case GraphicsFormat::GraphicsFormatR32UInt:
	case GraphicsFormat::GraphicsFormatR32SInt:
	case GraphicsFormat::GraphicsFormatR32SFloat:
	case GraphicsFormat::GraphicsFormatR64UInt:
	case GraphicsFormat::GraphicsFormatR64SInt:
	case GraphicsFormat::GraphicsFormatR64SFloat:
	case GraphicsFormat::GraphicsFormatS8UInt:
	case GraphicsFormat::GraphicsFormatX8_D24UNormPack32:
	case GraphicsFormat::GraphicsFormatD32_SFLOAT:
		return 1;
	case GraphicsFormat::GraphicsFormatR4G4UNormPack8:
	case GraphicsFormat::GraphicsFormatR8G8SInt:
	case GraphicsFormat::GraphicsFormatR8G8UInt:
	case GraphicsFormat::GraphicsFormatR8G8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8SRGB:
	case GraphicsFormat::GraphicsFormatR16G16SInt:
	case GraphicsFormat::GraphicsFormatR16G16UInt:
	case GraphicsFormat::GraphicsFormatR16G16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16SFloat:
	case GraphicsFormat::GraphicsFormatR32G32SInt:
	case GraphicsFormat::GraphicsFormatR32G32UInt:
	case GraphicsFormat::GraphicsFormatR32G32SFloat:
	case GraphicsFormat::GraphicsFormatR64G64UInt:
	case GraphicsFormat::GraphicsFormatR64G64SInt:
	case GraphicsFormat::GraphicsFormatR64G64SFloat:
	case GraphicsFormat::GraphicsFormatD16UNorm_S8UInt:
	case GraphicsFormat::GraphicsFormatD24UNorm_S8UInt:
	case GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt:
		return 2;
	case GraphicsFormat::GraphicsFormatR5G6B5UNormPack16:
	case GraphicsFormat::GraphicsFormatB5G6R5UNormPack16:
	case GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32:
	case GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32:
	case GraphicsFormat::GraphicsFormatR8G8B8SInt:
	case GraphicsFormat::GraphicsFormatR8G8B8UInt:
	case GraphicsFormat::GraphicsFormatR8G8B8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8SRGB:
	case GraphicsFormat::GraphicsFormatR16G16B16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16UInt:
	case GraphicsFormat::GraphicsFormatR16G16B16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32UInt:
	case GraphicsFormat::GraphicsFormatR32G32B32SFloat:
	case GraphicsFormat::GraphicsFormatB8G8R8UNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8SNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8UScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8SScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8UInt:
	case GraphicsFormat::GraphicsFormatB8G8R8SInt:
	case GraphicsFormat::GraphicsFormatB8G8R8SRGB:
	case GraphicsFormat::GraphicsFormatR64G64B64UInt:
	case GraphicsFormat::GraphicsFormatR64G64B64SInt:
	case GraphicsFormat::GraphicsFormatR64G64B64SFloat:
		return 3;
	case GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16:
	case GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16:
	case GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16:
	case GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16:
	case GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16:
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
	case GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SNormPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10UScaledPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SScaledPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32:
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
	case GraphicsFormat::GraphicsFormatR64G64B64A64UInt:
	case GraphicsFormat::GraphicsFormatR64G64B64A64SInt:
	case GraphicsFormat::GraphicsFormatR64G64B64A64SFloat:
		return 4;
	case GraphicsFormat::GraphicsFormatBC1RGBUNormBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock:
	case GraphicsFormat::GraphicsFormatBC2UNormBlock:
	case GraphicsFormat::GraphicsFormatBC2SRGBBlock:
	case GraphicsFormat::GraphicsFormatBC3UNormBlock:
	case GraphicsFormat::GraphicsFormatBC3SRGBBlock:
	case GraphicsFormat::GraphicsFormatBC4UNormBlock:
	case GraphicsFormat::GraphicsFormatBC4SNormBlock:
	case GraphicsFormat::GraphicsFormatBC5UNormBlock:
	case GraphicsFormat::GraphicsFormatBC5SNormBlock:
	case GraphicsFormat::GraphicsFormatBC6HUFloatBlock:
	case GraphicsFormat::GraphicsFormatBC6HSFloatBlock:
	case GraphicsFormat::GraphicsFormatBC7UNormBlock:
	case GraphicsFormat::GraphicsFormatBC7SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock:
	case GraphicsFormat::GraphicsFormatEACR11UNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11SNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11G11UNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11G11SNormBlock:
	case GraphicsFormat::GraphicsFormatASTC4x4UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC5x4UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC5x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC6x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC6x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x8UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x8UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x10UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC12x10UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC12x12UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock:
	default:
		assert(false);
		return 0;
	}
}

std::uint8_t
GraphicsVertexLayout::getVertexSize(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormat::GraphicsFormatR4G4UNormPack8:
	case GraphicsFormat::GraphicsFormatR8UNorm:
	case GraphicsFormat::GraphicsFormatR8SNorm:
	case GraphicsFormat::GraphicsFormatR8UScaled:
	case GraphicsFormat::GraphicsFormatR8SScaled:
	case GraphicsFormat::GraphicsFormatR8UInt:
	case GraphicsFormat::GraphicsFormatR8SInt:
	case GraphicsFormat::GraphicsFormatR8SRGB:
	case GraphicsFormat::GraphicsFormatS8UInt:
		return 1;
	case GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16:
	case GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16:
	case GraphicsFormat::GraphicsFormatR5G6B5UNormPack16:
	case GraphicsFormat::GraphicsFormatB5G6R5UNormPack16:
	case GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16:
	case GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16:
	case GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16:
	case GraphicsFormat::GraphicsFormatR8G8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8UInt:
	case GraphicsFormat::GraphicsFormatR8G8SInt:
	case GraphicsFormat::GraphicsFormatR8G8SRGB:
	case GraphicsFormat::GraphicsFormatR16UNorm:
	case GraphicsFormat::GraphicsFormatR16SNorm:
	case GraphicsFormat::GraphicsFormatR16UScaled:
	case GraphicsFormat::GraphicsFormatR16SScaled:
	case GraphicsFormat::GraphicsFormatR16UInt:
	case GraphicsFormat::GraphicsFormatR16SInt:
	case GraphicsFormat::GraphicsFormatR16SFloat:
	case GraphicsFormat::GraphicsFormatD16UNorm:
		return 2;
	case GraphicsFormat::GraphicsFormatR8G8B8UNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8SNorm:
	case GraphicsFormat::GraphicsFormatR8G8B8UScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8SScaled:
	case GraphicsFormat::GraphicsFormatR8G8B8UInt:
	case GraphicsFormat::GraphicsFormatR8G8B8SInt:
	case GraphicsFormat::GraphicsFormatR8G8B8SRGB:
	case GraphicsFormat::GraphicsFormatB8G8R8UNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8SNorm:
	case GraphicsFormat::GraphicsFormatB8G8R8UScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8SScaled:
	case GraphicsFormat::GraphicsFormatB8G8R8UInt:
	case GraphicsFormat::GraphicsFormatB8G8R8SInt:
	case GraphicsFormat::GraphicsFormatB8G8R8SRGB:
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
	case GraphicsFormat::GraphicsFormatR16G16SInt:
	case GraphicsFormat::GraphicsFormatR16G16UInt:
	case GraphicsFormat::GraphicsFormatR16G16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16SFloat:
	case GraphicsFormat::GraphicsFormatR32UInt:
	case GraphicsFormat::GraphicsFormatR32SInt:
	case GraphicsFormat::GraphicsFormatR32SFloat:
	case GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SNormPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10UScaledPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SScaledPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32:
	case GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32:
	case GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32:
	case GraphicsFormat::GraphicsFormatX8_D24UNormPack32:
	case GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32:
	case GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32:
	case GraphicsFormat::GraphicsFormatD32_SFLOAT:
	case GraphicsFormat::GraphicsFormatD16UNorm_S8UInt:
	case GraphicsFormat::GraphicsFormatD24UNorm_S8UInt:
	case GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt:
		return 4;
	case GraphicsFormat::GraphicsFormatR16G16B16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16UInt:
	case GraphicsFormat::GraphicsFormatR16G16B16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16SFloat:
		return 6;
	case GraphicsFormat::GraphicsFormatR32G32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32SInt:
	case GraphicsFormat::GraphicsFormatR32G32UInt:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UNorm:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UScaled:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SFloat:
	case GraphicsFormat::GraphicsFormatR16G16B16A16SInt:
	case GraphicsFormat::GraphicsFormatR16G16B16A16UInt:
	case GraphicsFormat::GraphicsFormatR64UInt:
	case GraphicsFormat::GraphicsFormatR64SInt:
	case GraphicsFormat::GraphicsFormatR64SFloat:
		return 8;
	case GraphicsFormat::GraphicsFormatR32G32B32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32UInt:
		return 12;
	case GraphicsFormat::GraphicsFormatR32G32B32A32SFloat:
	case GraphicsFormat::GraphicsFormatR32G32B32A32SInt:
	case GraphicsFormat::GraphicsFormatR32G32B32A32UInt:
	case GraphicsFormat::GraphicsFormatR64G64UInt:
	case GraphicsFormat::GraphicsFormatR64G64SInt:
	case GraphicsFormat::GraphicsFormatR64G64SFloat:
		return 16;
	case GraphicsFormat::GraphicsFormatR64G64B64UInt:
	case GraphicsFormat::GraphicsFormatR64G64B64SInt:
	case GraphicsFormat::GraphicsFormatR64G64B64SFloat:
		return 24;
	case GraphicsFormat::GraphicsFormatR64G64B64A64UInt:
	case GraphicsFormat::GraphicsFormatR64G64B64A64SInt:
	case GraphicsFormat::GraphicsFormatR64G64B64A64SFloat:
		return 32;
	case GraphicsFormat::GraphicsFormatBC1RGBUNormBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock:
	case GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock:
	case GraphicsFormat::GraphicsFormatBC2UNormBlock:
	case GraphicsFormat::GraphicsFormatBC2SRGBBlock:
	case GraphicsFormat::GraphicsFormatBC3UNormBlock:
	case GraphicsFormat::GraphicsFormatBC3SRGBBlock:
	case GraphicsFormat::GraphicsFormatBC4UNormBlock:
	case GraphicsFormat::GraphicsFormatBC4SNormBlock:
	case GraphicsFormat::GraphicsFormatBC5UNormBlock:
	case GraphicsFormat::GraphicsFormatBC5SNormBlock:
	case GraphicsFormat::GraphicsFormatBC6HUFloatBlock:
	case GraphicsFormat::GraphicsFormatBC6HSFloatBlock:
	case GraphicsFormat::GraphicsFormatBC7UNormBlock:
	case GraphicsFormat::GraphicsFormatBC7SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock:
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock:
	case GraphicsFormat::GraphicsFormatEACR11UNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11SNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11G11UNormBlock:
	case GraphicsFormat::GraphicsFormatEACR11G11SNormBlock:
	case GraphicsFormat::GraphicsFormatASTC4x4UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC5x4UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC5x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC6x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC6x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC8x8UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x5UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x6UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x8UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC10x10UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC12x10UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock:
	case GraphicsFormat::GraphicsFormatASTC12x12UNormBlock:
	case GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock:
	default:
		assert(false);
		return 0;
	}
}

GraphicsInputLayoutDesc::GraphicsInputLayoutDesc() noexcept
{
}

GraphicsInputLayoutDesc::~GraphicsInputLayoutDesc() noexcept
{
}

void
GraphicsInputLayoutDesc::setGraphicsVertexLayouts(const GraphicsVertexLayouts& component) noexcept
{
	_components = component;
}

const GraphicsVertexLayouts&
GraphicsInputLayoutDesc::getGraphicsVertexLayouts() const noexcept
{
	return _components;
}

void
GraphicsInputLayoutDesc::addComponent(const GraphicsVertexLayout& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const GraphicsVertexLayout& it) { return it.getSemantic() == compoent.getSemantic();});
	if (it == _components.end())
		_components.push_back(compoent);
}

void
GraphicsInputLayoutDesc::removeComponent(const GraphicsVertexLayout& compoent) noexcept
{
	auto it = std::find_if(_components.begin(), _components.end(), [compoent](const GraphicsVertexLayout& it) { return it.getSemantic() == compoent.getSemantic();});
	if (it != _components.end())
		_components.erase(it);
}

std::uint32_t
GraphicsInputLayoutDesc::getVertexSize() const noexcept
{
	std::uint32_t size = 0;
	for (auto& it : _components)
		size += it.getVertexSize();
	return size;
}

GraphicsInputLayout::GraphicsInputLayout() noexcept
{
}

GraphicsInputLayout::~GraphicsInputLayout() noexcept
{
}

_NAME_END