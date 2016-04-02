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
#include "vk_types.h"

_NAME_BEGIN

VkFormat
VulkanTypes::asGraphicsFormat(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormat::GraphicsFormatUndefined:
		return VK_FORMAT_UNDEFINED;
	case GraphicsFormat::GraphicsFormatR4G4UNormPack8:
		return VK_FORMAT_R4G4_UNORM_PACK8;
	case GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16:
		return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16:
		return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatR5G6B5UNormPack16:
		return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatB5G6R5UNormPack16:
		return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16:
		return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16:
		return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16:
		return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	case GraphicsFormat::GraphicsFormatR8UNorm:
		return VK_FORMAT_R8_UNORM;
	case GraphicsFormat::GraphicsFormatR8SNorm:
		return VK_FORMAT_R8_SNORM;
	case GraphicsFormat::GraphicsFormatR8UScaled:
		return VK_FORMAT_R8_USCALED;
	case GraphicsFormat::GraphicsFormatR8SScaled:
		return VK_FORMAT_R8_SSCALED;
	case GraphicsFormat::GraphicsFormatR8UInt:
		return VK_FORMAT_R8_UINT;
	case GraphicsFormat::GraphicsFormatR8SInt:
		return VK_FORMAT_R8_SINT;
	case GraphicsFormat::GraphicsFormatR8SRGB:
		return VK_FORMAT_R8_SRGB;
	case GraphicsFormat::GraphicsFormatR8G8UNorm:
		return VK_FORMAT_R8G8_UNORM;
	case GraphicsFormat::GraphicsFormatR8G8SNorm:
		return VK_FORMAT_R8G8_SNORM;
	case GraphicsFormat::GraphicsFormatR8G8UScaled:
		return VK_FORMAT_R8G8_USCALED;
	case GraphicsFormat::GraphicsFormatR8G8SScaled:
		return VK_FORMAT_R8G8_SSCALED;
	case GraphicsFormat::GraphicsFormatR8G8UInt:
		return VK_FORMAT_R8G8_UINT;
	case GraphicsFormat::GraphicsFormatR8G8SInt:
		return VK_FORMAT_R8G8_SINT;
	case GraphicsFormat::GraphicsFormatR8G8SRGB:
		return VK_FORMAT_R8G8_SRGB;
	case GraphicsFormat::GraphicsFormatR8G8B8UNorm:
		return VK_FORMAT_R8G8B8_UNORM;
	case GraphicsFormat::GraphicsFormatR8G8B8SNorm:
		return VK_FORMAT_R8G8B8_SNORM;
	case GraphicsFormat::GraphicsFormatR8G8B8UScaled:
		return VK_FORMAT_R8G8B8_USCALED;
	case GraphicsFormat::GraphicsFormatR8G8B8SScaled:
		return VK_FORMAT_R8G8B8_SSCALED;
	case GraphicsFormat::GraphicsFormatR8G8B8UInt:
		return VK_FORMAT_R8G8B8_UINT;
	case GraphicsFormat::GraphicsFormatR8G8B8SInt:
		return VK_FORMAT_R8G8B8_SINT;
	case GraphicsFormat::GraphicsFormatR8G8B8SRGB:
		return VK_FORMAT_R8G8B8_SRGB;
	case GraphicsFormat::GraphicsFormatB8G8R8UNorm:
		return VK_FORMAT_B8G8R8_UNORM;
	case GraphicsFormat::GraphicsFormatB8G8R8SNorm:
		return VK_FORMAT_B8G8R8_SNORM;
	case GraphicsFormat::GraphicsFormatB8G8R8UScaled:
		return VK_FORMAT_B8G8R8_USCALED;
	case GraphicsFormat::GraphicsFormatB8G8R8SScaled:
		return VK_FORMAT_B8G8R8_SSCALED;
	case GraphicsFormat::GraphicsFormatB8G8R8UInt:
		return VK_FORMAT_B8G8R8_UINT;
	case GraphicsFormat::GraphicsFormatB8G8R8SInt:
		return VK_FORMAT_B8G8R8_SINT;
	case GraphicsFormat::GraphicsFormatB8G8R8SRGB:
		return VK_FORMAT_B8G8R8_SRGB;
	case GraphicsFormat::GraphicsFormatR8G8B8A8UNorm:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case GraphicsFormat::GraphicsFormatR8G8B8A8SNorm:
		return VK_FORMAT_R8G8B8A8_SNORM;
	case GraphicsFormat::GraphicsFormatR8G8B8A8UScaled:
		return VK_FORMAT_R8G8B8A8_USCALED;
	case GraphicsFormat::GraphicsFormatR8G8B8A8SScaled:
		return VK_FORMAT_R8G8B8A8_SSCALED;
	case GraphicsFormat::GraphicsFormatR8G8B8A8UInt:
		return VK_FORMAT_R8G8B8A8_UINT;
	case GraphicsFormat::GraphicsFormatR8G8B8A8SInt:
		return VK_FORMAT_R8G8B8A8_SINT;
	case GraphicsFormat::GraphicsFormatR8G8B8A8SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;
	case GraphicsFormat::GraphicsFormatB8G8R8A8UNorm:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case GraphicsFormat::GraphicsFormatB8G8R8A8SNorm:
		return VK_FORMAT_B8G8R8A8_SNORM;
	case GraphicsFormat::GraphicsFormatB8G8R8A8UScaled:
		return VK_FORMAT_B8G8R8A8_USCALED;
	case GraphicsFormat::GraphicsFormatB8G8R8A8SScaled:
		return VK_FORMAT_B8G8R8A8_SSCALED;
	case GraphicsFormat::GraphicsFormatB8G8R8A8UInt:
		return VK_FORMAT_B8G8R8A8_UINT;
	case GraphicsFormat::GraphicsFormatB8G8R8A8SInt:
		return VK_FORMAT_B8G8R8A8_SINT;
	case GraphicsFormat::GraphicsFormatB8G8R8A8SRGB:
		return VK_FORMAT_B8G8R8A8_SRGB;
	case GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32:
		return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32:
		return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32:
		return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32:
		return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32:
		return VK_FORMAT_A8B8G8R8_UINT_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32:
		return VK_FORMAT_A8B8G8R8_SINT_PACK32;
	case GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32:
		return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32:
		return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10SNormPack32:
		return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10UScaledPack32:
		return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10SScaledPack32:
		return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32:
		return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32:
		return VK_FORMAT_A2R10G10B10_SINT_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32:
		return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32:
		return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32:
		return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32:
		return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32:
		return VK_FORMAT_A2B10G10R10_UINT_PACK32;
	case GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32:
		return VK_FORMAT_A2B10G10R10_SINT_PACK32;
	case GraphicsFormat::GraphicsFormatR16UNorm:
		return VK_FORMAT_R16_UNORM;
	case GraphicsFormat::GraphicsFormatR16SNorm:
		return VK_FORMAT_R16_SNORM;
	case GraphicsFormat::GraphicsFormatR16UScaled:
		return VK_FORMAT_R16_USCALED;
	case GraphicsFormat::GraphicsFormatR16SScaled:
		return VK_FORMAT_R16_SSCALED;
	case GraphicsFormat::GraphicsFormatR16UInt:
		return VK_FORMAT_R16_UINT;
	case GraphicsFormat::GraphicsFormatR16SInt:
		return VK_FORMAT_R16_SINT;
	case GraphicsFormat::GraphicsFormatR16SFloat:
		return VK_FORMAT_R16_SFLOAT;
	case GraphicsFormat::GraphicsFormatR16G16UNorm:
		return VK_FORMAT_R16G16_UNORM;
	case GraphicsFormat::GraphicsFormatR16G16SNorm:
		return VK_FORMAT_R16G16_SNORM;
	case GraphicsFormat::GraphicsFormatR16G16UScaled:
		return VK_FORMAT_R16G16_USCALED;
	case GraphicsFormat::GraphicsFormatR16G16SScaled:
		return VK_FORMAT_R16G16_SSCALED;
	case GraphicsFormat::GraphicsFormatR16G16UInt:
		return VK_FORMAT_R16G16_UINT;
	case GraphicsFormat::GraphicsFormatR16G16SInt:
		return VK_FORMAT_R16G16_SINT;
	case GraphicsFormat::GraphicsFormatR16G16SFloat:
		return VK_FORMAT_R16G16_SFLOAT;
	case GraphicsFormat::GraphicsFormatR16G16B16UNorm:
		return VK_FORMAT_R16G16B16_UNORM;
	case GraphicsFormat::GraphicsFormatR16G16B16SNorm:
		return VK_FORMAT_R16G16B16_SNORM;
	case GraphicsFormat::GraphicsFormatR16G16B16UScaled:
		return VK_FORMAT_R16G16B16_USCALED;
	case GraphicsFormat::GraphicsFormatR16G16B16SScaled:
		return VK_FORMAT_R16G16B16_SSCALED;
	case GraphicsFormat::GraphicsFormatR16G16B16UInt:
		return VK_FORMAT_R16G16B16_UINT;
	case GraphicsFormat::GraphicsFormatR16G16B16SInt:
		return VK_FORMAT_R16G16B16_SINT;
	case GraphicsFormat::GraphicsFormatR16G16B16SFloat:
		return VK_FORMAT_R16G16B16_SFLOAT;
	case GraphicsFormat::GraphicsFormatR16G16B16A16UNorm:
		return VK_FORMAT_R16G16B16A16_UNORM;
	case GraphicsFormat::GraphicsFormatR16G16B16A16SNorm:
		return VK_FORMAT_R16G16B16A16_SNORM;
	case GraphicsFormat::GraphicsFormatR16G16B16A16UScaled:
		return VK_FORMAT_R16G16B16A16_USCALED;
	case GraphicsFormat::GraphicsFormatR16G16B16A16SScaled:
		return VK_FORMAT_R16G16B16A16_SSCALED;
	case GraphicsFormat::GraphicsFormatR16G16B16A16UInt:
		return VK_FORMAT_R16G16B16A16_UINT;
	case GraphicsFormat::GraphicsFormatR16G16B16A16SInt:
		return VK_FORMAT_R16G16B16A16_SINT;
	case GraphicsFormat::GraphicsFormatR16G16B16A16SFloat:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case GraphicsFormat::GraphicsFormatR32UInt:
		return VK_FORMAT_R32_UINT;
	case GraphicsFormat::GraphicsFormatR32SInt:
		return VK_FORMAT_R32_SINT;
	case GraphicsFormat::GraphicsFormatR32SFloat:
		return VK_FORMAT_R32_SFLOAT;
	case GraphicsFormat::GraphicsFormatR32G32UInt:
		return VK_FORMAT_R32G32_UINT;
	case GraphicsFormat::GraphicsFormatR32G32SInt:
		return VK_FORMAT_R32G32_SINT;
	case GraphicsFormat::GraphicsFormatR32G32SFloat:
		return VK_FORMAT_R32G32_SFLOAT;
	case GraphicsFormat::GraphicsFormatR32G32B32UInt:
		return VK_FORMAT_R32G32B32_UINT;
	case GraphicsFormat::GraphicsFormatR32G32B32SInt:
		return VK_FORMAT_R32G32B32_SINT;
	case GraphicsFormat::GraphicsFormatR32G32B32SFloat:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case GraphicsFormat::GraphicsFormatR32G32B32A32UInt:
		return VK_FORMAT_R32G32B32A32_UINT;
	case GraphicsFormat::GraphicsFormatR32G32B32A32SInt:
		return VK_FORMAT_R32G32B32A32_SINT;
	case GraphicsFormat::GraphicsFormatR32G32B32A32SFloat:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case GraphicsFormat::GraphicsFormatR64UInt:
		return VK_FORMAT_R64_UINT;
	case GraphicsFormat::GraphicsFormatR64SInt:
		return VK_FORMAT_R64_SINT;
	case GraphicsFormat::GraphicsFormatR64SFloat:
		return VK_FORMAT_R64_SFLOAT;
	case GraphicsFormat::GraphicsFormatR64G64UInt:
		return VK_FORMAT_R64G64_UINT;
	case GraphicsFormat::GraphicsFormatR64G64SInt:
		return VK_FORMAT_R64G64_SINT;
	case GraphicsFormat::GraphicsFormatR64G64SFloat:
		return VK_FORMAT_R64G64_SFLOAT;
	case GraphicsFormat::GraphicsFormatR64G64B64UInt:
		return VK_FORMAT_R64G64B64_UINT;
	case GraphicsFormat::GraphicsFormatR64G64B64SInt:
		return VK_FORMAT_R64G64B64_SINT;
	case GraphicsFormat::GraphicsFormatR64G64B64SFloat:
		return VK_FORMAT_R64G64B64_SFLOAT;
	case GraphicsFormat::GraphicsFormatR64G64B64A64UInt:
		return VK_FORMAT_R64G64B64A64_UINT;
	case GraphicsFormat::GraphicsFormatR64G64B64A64SInt:
		return VK_FORMAT_R64G64B64A64_SINT;
	case GraphicsFormat::GraphicsFormatR64G64B64A64SFloat:
		return VK_FORMAT_R64G64B64A64_SFLOAT;
	case GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32:
		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32:
		return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
	case GraphicsFormat::GraphicsFormatD16UNorm:
		return VK_FORMAT_D16_UNORM;
	case GraphicsFormat::GraphicsFormatX8_D24UNormPack32:
		return VK_FORMAT_X8_D24_UNORM_PACK32;
	case GraphicsFormat::GraphicsFormatD32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;
	case GraphicsFormat::GraphicsFormatS8UInt:
		return VK_FORMAT_S8_UINT;
	case GraphicsFormat::GraphicsFormatD16UNorm_S8UInt:
		return VK_FORMAT_D16_UNORM_S8_UINT;
	case GraphicsFormat::GraphicsFormatD24UNorm_S8UInt:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	case GraphicsFormat::GraphicsFormatBC1RGBUNormBlock:
		return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock:
		return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock:
		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock:
		return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatBC2UNormBlock:
		return VK_FORMAT_BC2_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC2SRGBBlock:
		return VK_FORMAT_BC2_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatBC3UNormBlock:
		return VK_FORMAT_BC3_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC3SRGBBlock:
		return VK_FORMAT_BC3_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatBC4UNormBlock:
		return VK_FORMAT_BC4_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC4SNormBlock:
		return VK_FORMAT_BC4_SNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC5UNormBlock:
		return VK_FORMAT_BC5_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC5SNormBlock:
		return VK_FORMAT_BC5_SNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC6HUFloatBlock:
		return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case GraphicsFormat::GraphicsFormatBC6HSFloatBlock:
		return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case GraphicsFormat::GraphicsFormatBC7UNormBlock:
		return VK_FORMAT_BC7_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatBC7SRGBBlock:
		return VK_FORMAT_BC7_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock:
		return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock:
		return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock:
		return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock:
		return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock:
		return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock:
		return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatEACR11UNormBlock:
		return VK_FORMAT_EAC_R11_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatEACR11SNormBlock:
		return VK_FORMAT_EAC_R11_SNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatEACR11G11UNormBlock:
		return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatEACR11G11SNormBlock:
		return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC4x4UNormBlock:
		return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock:
		return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC5x4UNormBlock:
		return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock:
		return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC5x5UNormBlock:
		return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock:
		return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC6x5UNormBlock:
		return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock:
		return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC6x6UNormBlock:
		return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock:
		return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x5UNormBlock:
		return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock:
		return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x6UNormBlock:
		return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock:
		return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x8UNormBlock:
		return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock:
		return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x5UNormBlock:
		return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock:
		return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x6UNormBlock:
		return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock:
		return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x8UNormBlock:
		return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock:
		return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x10UNormBlock:
		return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock:
		return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC12x10UNormBlock:
		return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock:
		return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC12x12UNormBlock:
		return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
	case GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock:
		return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
	default:
		assert(false);
		return VK_FORMAT_MAX_ENUM;
	}
}

VkImageType
VulkanTypes::asImageType(GraphicsTextureDim type) noexcept
{
	switch (type)
	{
	case GraphicsTextureDim::GraphicsTextureDim2D:
		return VkImageType::VK_IMAGE_TYPE_2D;
	case GraphicsTextureDim::GraphicsTextureDim3D:
		return VkImageType::VK_IMAGE_TYPE_3D;
	case GraphicsTextureDim::GraphicsTextureDim2DArray:
		return VkImageType::VK_IMAGE_TYPE_2D;
	case GraphicsTextureDim::GraphicsTextureDimCube:
		return VkImageType::VK_IMAGE_TYPE_3D;
	case GraphicsTextureDim::GraphicsTextureDimCubeArray:
		return VkImageType::VK_IMAGE_TYPE_3D;
	default:
		assert(false);
		return VkImageType::VK_IMAGE_TYPE_MAX_ENUM;
	}
}

VkImageViewType
VulkanTypes::asImageViewType(GraphicsTextureDim type) noexcept
{
	switch (type)
	{
	case GraphicsTextureDim::GraphicsTextureDim2D:
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	case GraphicsTextureDim::GraphicsTextureDim3D:
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
	case GraphicsTextureDim::GraphicsTextureDim2DArray:
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	case GraphicsTextureDim::GraphicsTextureDimCube:
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
	case GraphicsTextureDim::GraphicsTextureDimCubeArray:
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	default:
		assert(false);
		return VkImageViewType::VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

VkSampleCountFlagBits
VulkanTypes::asTextureSample(GraphicsSampleFlagBits sample) noexcept
{
	switch (sample)
	{
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits1Bit:
		return VK_SAMPLE_COUNT_1_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits2Bit:
		return VK_SAMPLE_COUNT_2_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits4Bit:
		return VK_SAMPLE_COUNT_4_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits8Bit:
		return VK_SAMPLE_COUNT_8_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits16Bit:
		return VK_SAMPLE_COUNT_16_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits32Bit:
		return VK_SAMPLE_COUNT_32_BIT;
	case GraphicsSampleFlagBits::GraphicsSampleFlagBits64Bit:
		return VK_SAMPLE_COUNT_64_BIT;
	default:
		assert(false);
		return (VkSampleCountFlagBits)0;
	}
}

VkSampleCountFlagBits 
VulkanTypes::asTextureSample(GraphicsSamplerAnis anis) noexcept
{
	switch (anis)
	{
	case GraphicsSamplerAnis::GraphicsSamplerAnis1:
		return VK_SAMPLE_COUNT_1_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis2:
		return VK_SAMPLE_COUNT_2_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis4:
		return VK_SAMPLE_COUNT_4_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis8:
		return VK_SAMPLE_COUNT_8_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis16:
		return VK_SAMPLE_COUNT_16_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis32:
		return VK_SAMPLE_COUNT_32_BIT;
	case GraphicsSamplerAnis::GraphicsSamplerAnis64:
		return VK_SAMPLE_COUNT_64_BIT;
	default:
		assert(false);
		return (VkSampleCountFlagBits)0;
	}
}

VkImageUsageFlags
VulkanTypes::asTextureUsage(std::uint32_t usage) noexcept
{
	std::uint32_t flags = 0;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsTransferSrcBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsTransferDstBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsSampledBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsStorageBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsColorAttachmentBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsDepthStencilAttachmentBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsTransientAttachmentBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	if (usage & GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsInputAttachmentBit)
		flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	return flags;
}

VkShaderStageFlagBits
VulkanTypes::asShaderStage(GraphicsShaderStage type) noexcept
{
	switch (type)
	{
	case GraphicsShaderStage::GraphicsShaderStageVertex:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
	case GraphicsShaderStage::GraphicsShaderStageFragment:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
	case GraphicsShaderStage::GraphicsShaderStageGeometry:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
	case GraphicsShaderStage::GraphicsShaderStageCompute:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
	case GraphicsShaderStage::GraphicsShaderStageTessEvaluation:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case GraphicsShaderStage::GraphicsShaderStageTessControl:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	default:
		assert(false);
		return VkShaderStageFlagBits(0);
	}
}

VkCullModeFlags
VulkanTypes::asCullMode(GraphicsCullMode mode) noexcept
{
	switch (mode)
	{
	case GraphicsCullMode::GraphicsCullModeNone:
		return VkCullModeFlagBits::VK_CULL_MODE_NONE;
	case GraphicsCullMode::GraphicsCullModeFront:
		return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
	case GraphicsCullMode::GraphicsCullModeBack:
		return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
	case GraphicsCullMode::GraphicsCullModeFrontBack:
		return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
	default:
		assert(false);
		return VkCullModeFlagBits::VK_CULL_MODE_NONE;
	}
}

VkPolygonMode
VulkanTypes::asPolygonMode(GraphicsPolygonMode mode) noexcept
{
	switch (mode)
	{
	case GraphicsPolygonMode::GraphicsPolygonModePoint:
		return VkPolygonMode::VK_POLYGON_MODE_POINT;
	case GraphicsPolygonMode::GraphicsPolygonModeWireframe:
		return VkPolygonMode::VK_POLYGON_MODE_LINE;
	case GraphicsPolygonMode::GraphicsPolygonModeSolid:
		return VkPolygonMode::VK_POLYGON_MODE_FILL;
	default:
		assert(false);
		return VkPolygonMode::VK_POLYGON_MODE_MAX_ENUM;
	}
}

VkFrontFace
VulkanTypes::asFrontFace(GraphicsFrontFace face) noexcept
{
	switch (face)
	{
	case GraphicsFrontFaceCW:
		return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
	case GraphicsFrontFaceCCW:
		return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		assert(false);
		return VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
	}
}

VkCompareOp
VulkanTypes::asCompareOp(GraphicsCompareFunc compare) noexcept
{
	switch (compare)
	{
	case GraphicsCompareFunc::GraphicsCompareFuncNone:
		return VkCompareOp::VK_COMPARE_OP_NEVER;
	case GraphicsCompareFunc::GraphicsCompareFuncLequal:
		return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncEqual:
		return VkCompareOp::VK_COMPARE_OP_EQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncGreater:
		return VkCompareOp::VK_COMPARE_OP_GREATER;
	case GraphicsCompareFunc::GraphicsCompareFuncLess:
		return VkCompareOp::VK_COMPARE_OP_LESS;
	case GraphicsCompareFunc::GraphicsCompareFuncGequal:
		return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncNotEqual:
		return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncAlways:
		return VkCompareOp::VK_COMPARE_OP_ALWAYS;
	case GraphicsCompareFunc::GraphicsCompareFuncNever:
		return VkCompareOp::VK_COMPARE_OP_NEVER;
	default:
		assert(false);
		return VkCompareOp::VK_COMPARE_OP_MAX_ENUM;
	}
}

VkStencilOp
VulkanTypes::asStencilOp(GraphicsStencilOp stencilop) noexcept
{
	switch (stencilop)
	{
	case GraphicsStencilOp::GraphicsStencilOpKeep:
		return VkStencilOp::VK_STENCIL_OP_KEEP;
	case GraphicsStencilOp::GraphicsStencilOpReplace:
		return VkStencilOp::VK_STENCIL_OP_REPLACE;
	case GraphicsStencilOp::GraphicsStencilOpIncr:
		return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case GraphicsStencilOp::GraphicsStencilOpDecr:
		return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case GraphicsStencilOp::GraphicsStencilOpZero:
		return VkStencilOp::VK_STENCIL_OP_ZERO;
	case GraphicsStencilOp::GraphicsStencilOpIncrWrap:
		return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case GraphicsStencilOp::GraphicsStencilOpDecrWrap:
		return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
	default:
		assert(false);
		return VkStencilOp::VK_STENCIL_OP_MAX_ENUM;
	}
}

VkBufferUsageFlagBits
VulkanTypes::asBufferUsageFlagBits(GraphicsDataType type) noexcept
{
	switch (type)
	{
	case GraphicsDataType::GraphicsDataTypeTransferSrc:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	case GraphicsDataType::GraphicsDataTypeTransferDst:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	case GraphicsDataType::GraphicsDataTypeUniformTexelBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
	case GraphicsDataType::GraphicsDataTypeUniformBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	case GraphicsDataType::GraphicsDataTypeStorageTexelBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
	case GraphicsDataType::GraphicsDataTypeStorageBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	case GraphicsDataType::GraphicsDataTypeStorageVertexBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	case GraphicsDataType::GraphicsDataTypeStorageIndexBuffer:
		return VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	default:
		assert(false);
		return VkBufferUsageFlagBits(0);
	}
}

VkDescriptorType
VulkanTypes::asDescriptorType(GraphicsUniformType type) noexcept
{
	switch (type)
	{
	case GraphicsUniformType::GraphicsUniformTypeSampler:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
	case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	default:
		assert(false);
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkPrimitiveTopology
VulkanTypes::asPrimitiveTopology(GraphicsVertexType type) noexcept
{
	switch (type)
	{
	case GraphicsVertexType::GraphicsVertexTypePointList:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case GraphicsVertexType::GraphicsVertexTypeLineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case GraphicsVertexType::GraphicsVertexTypeLineStrip:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case GraphicsVertexType::GraphicsVertexTypeTriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case GraphicsVertexType::GraphicsVertexTypeTriangleStrip:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case GraphicsVertexType::GraphicsVertexTypeTriangleFan:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	case GraphicsVertexType::GraphicsVertexTypeLineListWithAdjacency:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
	case GraphicsVertexType::GraphicsVertexTypeLineStripWithAdjacency:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
	case GraphicsVertexType::GraphicsVertexTypeTriangleListWithAdjacency:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
	case GraphicsVertexType::GraphicsVertexTypeTriangleStripWithAdjacency:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
	case GraphicsVertexType::GraphicsVertexTypePatchList:
		return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	default:
		assert(false);
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}
}

VkImageLayout
VulkanTypes::asImageLayout(GraphicsViewLayout layout) noexcept
{
	switch (layout)
	{
	case GraphicsViewLayout::GraphicsViewLayoutUndefined:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	case GraphicsViewLayout::GraphicsViewLayoutGeneral:
		return VK_IMAGE_LAYOUT_GENERAL;
	case GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal:
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal:
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal:
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutShaderReadOnlyOptimal:
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutTransferSrcOptimal:
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutTransferDstOptimal:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case GraphicsViewLayout::GraphicsViewLayoutPreinitialized:
		return VK_IMAGE_LAYOUT_PREINITIALIZED;
	case GraphicsViewLayout::GraphicsViewLayoutPresentSrcKhr:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default:
		assert(false);
		return VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}

_NAME_END