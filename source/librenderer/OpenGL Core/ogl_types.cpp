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
#include "ogl_types.h"

_NAME_BEGIN

#ifdef _BUILD_OPENGL
#	ifdef GLEW_MX
GLEWContext _glewctx;
#	endif
#endif

GLenum
OGLTypes::asVertexType(GraphicsVertexType type) noexcept
{
	switch (type)
	{
	case GraphicsVertexType::GraphicsVertexTypePointList:     return GL_POINTS;
	case GraphicsVertexType::GraphicsVertexTypeLineList:      return GL_LINES;
	case GraphicsVertexType::GraphicsVertexTypeLineStrip:     return GL_LINE_STRIP;
	case GraphicsVertexType::GraphicsVertexTypeTriangleList:  return GL_TRIANGLES;
	case GraphicsVertexType::GraphicsVertexTypeTriangleStrip: return GL_TRIANGLE_STRIP;
	case GraphicsVertexType::GraphicsVertexTypeTriangleFan:   return GL_TRIANGLE_FAN;
	default:
		GL_PLATFORM_LOG("Invalid vertex type");
		return false;
	}
}

GLenum
OGLTypes::asVertexFormat(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormatR8SNorm:
	case GraphicsFormatR8SScaled:
	case GraphicsFormatR8SInt:
	case GraphicsFormatR8SRGB:
	case GraphicsFormatR8G8SNorm:
	case GraphicsFormatR8G8SScaled:
	case GraphicsFormatR8G8SInt:
	case GraphicsFormatR8G8SRGB:
	case GraphicsFormatR8G8B8SNorm:
	case GraphicsFormatR8G8B8SScaled:
	case GraphicsFormatR8G8B8SInt:
	case GraphicsFormatR8G8B8SRGB:
	case GraphicsFormatB8G8R8SNorm:
	case GraphicsFormatB8G8R8SScaled:
	case GraphicsFormatB8G8R8SInt:
	case GraphicsFormatB8G8R8SRGB:
	case GraphicsFormatR8G8B8A8SNorm:
	case GraphicsFormatR8G8B8A8SScaled:
	case GraphicsFormatR8G8B8A8SInt:
	case GraphicsFormatR8G8B8A8SRGB:
	case GraphicsFormatB8G8R8A8SNorm:
	case GraphicsFormatB8G8R8A8SScaled:
	case GraphicsFormatB8G8R8A8SInt:
	case GraphicsFormatB8G8R8A8SRGB:
	case GraphicsFormatA8B8G8R8SNormPack32:
	case GraphicsFormatA8B8G8R8SScaledPack32:
	case GraphicsFormatA8B8G8R8SIntPack32:
	case GraphicsFormatA8B8G8R8SRGBPack32:
		return GL_BYTE;
	case GraphicsFormatR8UNorm:
	case GraphicsFormatR8UScaled:
	case GraphicsFormatR8UInt:
	case GraphicsFormatR8G8UNorm:
	case GraphicsFormatR8G8UScaled:
	case GraphicsFormatR8G8UInt:
	case GraphicsFormatR8G8B8UNorm:
	case GraphicsFormatR8G8B8UScaled:
	case GraphicsFormatR8G8B8UInt:
	case GraphicsFormatB8G8R8UNorm:
	case GraphicsFormatB8G8R8UScaled:
	case GraphicsFormatB8G8R8UInt:
	case GraphicsFormatR8G8B8A8UNorm:
	case GraphicsFormatR8G8B8A8UScaled:
	case GraphicsFormatR8G8B8A8UInt:
	case GraphicsFormatB8G8R8A8UNorm:
	case GraphicsFormatB8G8R8A8UScaled:
	case GraphicsFormatB8G8R8A8UInt:
	case GraphicsFormatA8B8G8R8UNormPack32:
	case GraphicsFormatA8B8G8R8UScaledPack32:
	case GraphicsFormatA8B8G8R8UIntPack32:
		return GL_UNSIGNED_BYTE;
	case GraphicsFormatR16SNorm:
	case GraphicsFormatR16SScaled:
	case GraphicsFormatR16SInt:
	case GraphicsFormatR16SFloat:
	case GraphicsFormatR16G16SNorm:
	case GraphicsFormatR16G16SScaled:
	case GraphicsFormatR16G16SInt:
	case GraphicsFormatR16G16SFloat:
	case GraphicsFormatR16G16B16SNorm:
	case GraphicsFormatR16G16B16SScaled:
	case GraphicsFormatR16G16B16SInt:
	case GraphicsFormatR16G16B16SFloat:
	case GraphicsFormatR16G16B16A16SNorm:
	case GraphicsFormatR16G16B16A16SScaled:
	case GraphicsFormatR16G16B16A16SInt:
	case GraphicsFormatR16G16B16A16SFloat:
		return GL_SHORT;
	case GraphicsFormatR16UNorm:
	case GraphicsFormatR16UScaled:
	case GraphicsFormatR16UInt:
	case GraphicsFormatR16G16UNorm:
	case GraphicsFormatR16G16UScaled:
	case GraphicsFormatR16G16UInt:
	case GraphicsFormatR16G16B16UNorm:
	case GraphicsFormatR16G16B16UScaled:
	case GraphicsFormatR16G16B16UInt:
	case GraphicsFormatR16G16B16A16UNorm:
	case GraphicsFormatR16G16B16A16UScaled:
	case GraphicsFormatR16G16B16A16UInt:
		return GL_UNSIGNED_SHORT;
	case GraphicsFormatR32SInt:
	case GraphicsFormatR32G32SInt:
	case GraphicsFormatR32G32B32SInt:
	case GraphicsFormatR32G32B32A32SInt:
		return GL_INT;
	case GraphicsFormatR32UInt:
	case GraphicsFormatR32G32UInt:
	case GraphicsFormatR32G32B32UInt:
	case GraphicsFormatR32G32B32A32UInt:
		return GL_UNSIGNED_INT;
	case GraphicsFormatR32SFloat:
	case GraphicsFormatR32G32SFloat:
	case GraphicsFormatR32G32B32SFloat:
	case GraphicsFormatR32G32B32A32SFloat:
		return GL_FLOAT;
	case GraphicsFormatR64SInt:
	case GraphicsFormatR64G64SInt:
	case GraphicsFormatR64G64B64SInt:
	case GraphicsFormatR64G64B64A64SInt:
		return GL_INT64_ARB;
	case GraphicsFormatR64UInt:
	case GraphicsFormatR64G64UInt:
	case GraphicsFormatR64G64B64UInt:
	case GraphicsFormatR64G64B64A64UInt:
		return GL_UNSIGNED_INT64_ARB;
	case GraphicsFormatR64SFloat:
	case GraphicsFormatR64G64SFloat:
	case GraphicsFormatR64G64B64SFloat:
	case GraphicsFormatR64G64B64A64SFloat:
		return GL_DOUBLE;
	default:
		return GL_INVALID_ENUM;
		break;
	}
}

GLenum
OGLTypes::asIndexType(GraphicsIndexType type) noexcept
{
	switch (type)
	{
	case GraphicsIndexType::GraphicsIndexTypeNone:   return GL_NONE;
	case GraphicsIndexType::GraphicsIndexTypeUInt16: return GL_UNSIGNED_SHORT;
	case GraphicsIndexType::GraphicsIndexTypeUInt32: return GL_UNSIGNED_INT;
	default:
		GL_PLATFORM_LOG("Invalid index type");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asShaderStage(GraphicsShaderStage stage) noexcept
{
	switch (stage)
	{
	case GraphicsShaderStage::GraphicsShaderStageVertex:         return GL_VERTEX_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageFragment:       return GL_FRAGMENT_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageCompute:        return GL_COMPUTE_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageGeometry:       return GL_GEOMETRY_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageTessControl:    return GL_TESS_CONTROL_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageTessEvaluation: return GL_TESS_EVALUATION_SHADER;
	default:
		GL_PLATFORM_LOG("Invalid shader type");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asTextureTarget(GraphicsTextureDim target, bool multisampler) noexcept
{
	if (target == GraphicsTextureDim::GraphicsTextureDim2D)
	{
		if (multisampler)
			return GL_TEXTURE_2D_MULTISAMPLE;
		else
			return GL_TEXTURE_2D;
	}
	else if (target == GraphicsTextureDim::GraphicsTextureDim3D)
	{
		if (!multisampler)
			return GL_TEXTURE_3D;
		else
		{
			GL_PLATFORM_LOG("Can't support 3d texture multisampler");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == GraphicsTextureDim::GraphicsTextureDim2DArray)
	{
		if (multisampler)
			return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		else
			return GL_TEXTURE_2D_ARRAY;
	}
	else if (target == GraphicsTextureDim::GraphicsTextureDimCube)
	{
		if (!multisampler)
			return GL_TEXTURE_CUBE_MAP;
		else
		{
			GL_PLATFORM_LOG("Can't support cube multisampler");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == GraphicsTextureDim::GraphicsTextureDimCubeArray)
	{
		if (!multisampler)
			return GL_TEXTURE_CUBE_MAP_ARRAY;
		else
		{
			GL_PLATFORM_LOG("Can't support cube array multisampler");
			return GL_INVALID_ENUM;
		}
	}

	GL_PLATFORM_LOG("Invalid texture target");
	return GL_INVALID_ENUM;
}

GLenum
OGLTypes::asTextureFormat(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormatR8UNorm:
	case GraphicsFormatR8SNorm:
	case GraphicsFormatR8UScaled:
	case GraphicsFormatR8SScaled:
	case GraphicsFormatR8UInt:
	case GraphicsFormatR8SInt:
	case GraphicsFormatR16UNorm:
	case GraphicsFormatR16SNorm:
	case GraphicsFormatR16UScaled:
	case GraphicsFormatR16SScaled:
	case GraphicsFormatR16UInt:
	case GraphicsFormatR16SInt:
	case GraphicsFormatR16SFloat:
	case GraphicsFormatR32UInt:
	case GraphicsFormatR32SInt:
	case GraphicsFormatR32SFloat:
	case GraphicsFormatR64UInt:
	case GraphicsFormatR64SInt:
	case GraphicsFormatR64SFloat:
		return GL_RED;
	case GraphicsFormatR4G4UNormPack8:
	case GraphicsFormatR8G8UNorm:
	case GraphicsFormatR8G8SNorm:
	case GraphicsFormatR8G8UScaled:
	case GraphicsFormatR8G8SScaled:
	case GraphicsFormatR8G8UInt:
	case GraphicsFormatR8G8SInt:
	case GraphicsFormatR16G16UNorm:
	case GraphicsFormatR16G16SNorm:
	case GraphicsFormatR16G16UScaled:
	case GraphicsFormatR16G16SScaled:
	case GraphicsFormatR16G16UInt:
	case GraphicsFormatR16G16SInt:
	case GraphicsFormatR16G16SFloat:
	case GraphicsFormatR32G32UInt:
	case GraphicsFormatR32G32SInt:
	case GraphicsFormatR32G32SFloat:
	case GraphicsFormatR64G64UInt:
	case GraphicsFormatR64G64SInt:
	case GraphicsFormatR64G64SFloat:
		return GL_RG;
	case GraphicsFormatR5G6B5UNormPack16:
	case GraphicsFormatR8G8B8UNorm:
	case GraphicsFormatR8G8B8SNorm:
	case GraphicsFormatR8G8B8UScaled:
	case GraphicsFormatR8G8B8SScaled:
	case GraphicsFormatR8G8B8UInt:
	case GraphicsFormatR8G8B8SInt:
	case GraphicsFormatR16G16B16UNorm:
	case GraphicsFormatR16G16B16SNorm:
	case GraphicsFormatR16G16B16UScaled:
	case GraphicsFormatR16G16B16SScaled:
	case GraphicsFormatR16G16B16UInt:
	case GraphicsFormatR16G16B16SInt:
	case GraphicsFormatR16G16B16SFloat:
	case GraphicsFormatR32G32B32UInt:
	case GraphicsFormatR32G32B32SInt:
	case GraphicsFormatR32G32B32SFloat:
	case GraphicsFormatR64G64B64UInt:
	case GraphicsFormatR64G64B64SInt:
	case GraphicsFormatR64G64B64SFloat:
		return GL_RGB;
	case GraphicsFormatB5G6R5UNormPack16:
	case GraphicsFormatB8G8R8UNorm:
	case GraphicsFormatB8G8R8SNorm:
	case GraphicsFormatB8G8R8UScaled:
	case GraphicsFormatB8G8R8SScaled:
	case GraphicsFormatB8G8R8UInt:
	case GraphicsFormatB8G8R8SInt:
	case GraphicsFormatB10G11R11UFloatPack32:
		return GL_BGR;
	case GraphicsFormatR4G4B4A4UNormPack16:
	case GraphicsFormatR5G5B5A1UNormPack16:
	case GraphicsFormatA1R5G5B5UNormPack16:
	case GraphicsFormatR8G8B8A8UNorm:
	case GraphicsFormatR8G8B8A8SNorm:
	case GraphicsFormatR8G8B8A8UScaled:
	case GraphicsFormatR8G8B8A8SScaled:
	case GraphicsFormatR8G8B8A8UInt:
	case GraphicsFormatR8G8B8A8SInt:
	case GraphicsFormatA2R10G10B10UNormPack32:
	case GraphicsFormatA2R10G10B10SNormPack32:
	case GraphicsFormatA2R10G10B10UScaledPack32:
	case GraphicsFormatA2R10G10B10SScaledPack32:
	case GraphicsFormatA2R10G10B10UIntPack32:
	case GraphicsFormatA2R10G10B10SIntPack32:
	case GraphicsFormatR16G16B16A16UNorm:
	case GraphicsFormatR16G16B16A16SNorm:
	case GraphicsFormatR16G16B16A16UScaled:
	case GraphicsFormatR16G16B16A16SScaled:
	case GraphicsFormatR16G16B16A16UInt:
	case GraphicsFormatR16G16B16A16SInt:
	case GraphicsFormatR16G16B16A16SFloat:
	case GraphicsFormatR32G32B32A32UInt:
	case GraphicsFormatR32G32B32A32SInt:
	case GraphicsFormatR32G32B32A32SFloat:
	case GraphicsFormatR64G64B64A64UInt:
	case GraphicsFormatR64G64B64A64SInt:
	case GraphicsFormatR64G64B64A64SFloat:
		return GL_RGBA;
	case GraphicsFormatB4G4R4A4UNormPack16:
	case GraphicsFormatB5G5R5A1UNormPack16:
	case GraphicsFormatB8G8R8A8UNorm:
	case GraphicsFormatB8G8R8A8SNorm:
	case GraphicsFormatB8G8R8A8UScaled:
	case GraphicsFormatB8G8R8A8SScaled:
	case GraphicsFormatB8G8R8A8UInt:
	case GraphicsFormatB8G8R8A8SInt:
	case GraphicsFormatA2B10G10R10UNormPack32:
	case GraphicsFormatA2B10G10R10SNormPack32:
	case GraphicsFormatA2B10G10R10UScaledPack32:
	case GraphicsFormatA2B10G10R10SScaledPack32:
	case GraphicsFormatA2B10G10R10UIntPack32:
	case GraphicsFormatA2B10G10R10SIntPack32:
		return GL_BGRA;
	case GraphicsFormatA8B8G8R8UNormPack32:
	case GraphicsFormatA8B8G8R8SNormPack32:
	case GraphicsFormatA8B8G8R8UScaledPack32:
	case GraphicsFormatA8B8G8R8SScaledPack32:
	case GraphicsFormatA8B8G8R8UIntPack32:
	case GraphicsFormatA8B8G8R8SIntPack32:
		return GL_ABGR_EXT;
	case GraphicsFormatR8SRGB:
	case GraphicsFormatR8G8SRGB:
	case GraphicsFormatR8G8B8SRGB:
	case GraphicsFormatB8G8R8SRGB:
		return GL_SRGB;
	case GraphicsFormatR8G8B8A8SRGB:
	case GraphicsFormatB8G8R8A8SRGB:
	case GraphicsFormatA8B8G8R8SRGBPack32:
		return GL_SRGB_ALPHA;
	case GraphicsFormatD16UNorm:
	case GraphicsFormatX8_D24UNormPack32:
	case GraphicsFormatD32_SFLOAT:
		return GL_DEPTH_COMPONENT;
	case GraphicsFormatS8UInt:
		return GL_STENCIL_INDEX;
	case GraphicsFormatD16UNorm_S8UInt:
	case GraphicsFormatD24UNorm_S8UInt:
	case GraphicsFormatD32_SFLOAT_S8UInt:
		return GL_DEPTH_STENCIL;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid texture format");
	}

	return GL_INVALID_ENUM;
}

GLenum
OGLTypes::asTextureType(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormatR4G4UNormPack8:           return GL_UNSIGNED_BYTE;
	case GraphicsFormatR4G4B4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
	case GraphicsFormatB4G4R4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
	case GraphicsFormatR5G6B5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
	case GraphicsFormatB5G6R5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
	case GraphicsFormatR5G5B5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
	case GraphicsFormatB5G5R5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
	case GraphicsFormatA1R5G5B5UNormPack16:      return GL_UNSIGNED_SHORT_1_5_5_5_REV;
	case GraphicsFormatR8UNorm:                  return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8SNorm:                  return GL_BYTE;
	case GraphicsFormatR8UScaled:                return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8SScaled:                return GL_BYTE;
	case GraphicsFormatR8UInt:                   return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8SInt:                   return GL_BYTE;
	case GraphicsFormatR8SRGB:                   return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8UNorm:                return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8SNorm:                return GL_BYTE;
	case GraphicsFormatR8G8UScaled:              return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8SScaled:              return GL_BYTE;
	case GraphicsFormatR8G8UInt:                 return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8SInt:                 return GL_BYTE;
	case GraphicsFormatR8G8SRGB:                 return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8UNorm:              return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8SNorm:              return GL_BYTE;
	case GraphicsFormatR8G8B8UScaled:            return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8SScaled:            return GL_BYTE;
	case GraphicsFormatR8G8B8UInt:               return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8SInt:               return GL_BYTE;
	case GraphicsFormatR8G8B8SRGB:               return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8UNorm:              return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8SNorm:              return GL_BYTE;
	case GraphicsFormatB8G8R8UScaled:            return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8SScaled:            return GL_BYTE;
	case GraphicsFormatB8G8R8UInt:               return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8SInt:               return GL_BYTE;
	case GraphicsFormatB8G8R8SRGB:               return GL_BYTE;
	case GraphicsFormatR8G8B8A8UNorm:            return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8A8SNorm:            return GL_BYTE;
	case GraphicsFormatR8G8B8A8UScaled:          return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8A8SScaled:          return GL_BYTE;
	case GraphicsFormatR8G8B8A8UInt:             return GL_UNSIGNED_BYTE;
	case GraphicsFormatR8G8B8A8SInt:             return GL_BYTE;
	case GraphicsFormatR8G8B8A8SRGB:             return GL_BYTE;
	case GraphicsFormatB8G8R8A8UNorm:            return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8A8SNorm:            return GL_BYTE;
	case GraphicsFormatB8G8R8A8UScaled:          return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8A8SScaled:          return GL_BYTE;
	case GraphicsFormatB8G8R8A8UInt:             return GL_UNSIGNED_BYTE;
	case GraphicsFormatB8G8R8A8SInt:             return GL_BYTE;
	case GraphicsFormatB8G8R8A8SRGB:             return GL_BYTE;
	case GraphicsFormatA8B8G8R8UNormPack32:      return GL_UNSIGNED_BYTE;
	case GraphicsFormatA8B8G8R8SNormPack32:      return GL_BYTE;
	case GraphicsFormatA8B8G8R8UScaledPack32:    return GL_UNSIGNED_BYTE;
	case GraphicsFormatA8B8G8R8SScaledPack32:    return GL_BYTE;
	case GraphicsFormatA8B8G8R8UIntPack32:       return GL_UNSIGNED_BYTE;
	case GraphicsFormatA8B8G8R8SIntPack32:       return GL_BYTE;
	case GraphicsFormatA8B8G8R8SRGBPack32:       return GL_UNSIGNED_BYTE;
	case GraphicsFormatA2R10G10B10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2R10G10B10SNormPack32:   return GL_INT_2_10_10_10_REV;
	case GraphicsFormatA2R10G10B10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2R10G10B10SScaledPack32: return GL_INT_2_10_10_10_REV;
	case GraphicsFormatA2R10G10B10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2R10G10B10SIntPack32:    return GL_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10SNormPack32:   return GL_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10SScaledPack32: return GL_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV;
	case GraphicsFormatA2B10G10R10SIntPack32:    return GL_INT_2_10_10_10_REV;
	case GraphicsFormatR16UNorm:                 return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16SNorm:                 return GL_SHORT;
	case GraphicsFormatR16UScaled:               return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16SScaled:               return GL_SHORT;
	case GraphicsFormatR16UInt:                  return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16SInt:                  return GL_SHORT;
	case GraphicsFormatR16SFloat:                return GL_FLOAT;
	case GraphicsFormatR16G16UNorm:              return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16SNorm:              return GL_SHORT;
	case GraphicsFormatR16G16UScaled:            return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16SScaled:            return GL_SHORT;
	case GraphicsFormatR16G16UInt:               return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16SInt:               return GL_SHORT;
	case GraphicsFormatR16G16SFloat:             return GL_FLOAT;
	case GraphicsFormatR16G16B16UNorm:           return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16SNorm:           return GL_SHORT;
	case GraphicsFormatR16G16B16UScaled:         return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16SScaled:         return GL_SHORT;
	case GraphicsFormatR16G16B16UInt:            return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16SInt:            return GL_SHORT;
	case GraphicsFormatR16G16B16SFloat:          return GL_FLOAT;
	case GraphicsFormatR16G16B16A16UNorm:        return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16A16SNorm:        return GL_SHORT;
	case GraphicsFormatR16G16B16A16UScaled:      return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16A16SScaled:      return GL_SHORT;
	case GraphicsFormatR16G16B16A16UInt:         return GL_UNSIGNED_SHORT;
	case GraphicsFormatR16G16B16A16SInt:         return GL_SHORT;
	case GraphicsFormatR16G16B16A16SFloat:       return GL_FLOAT;
	case GraphicsFormatR32UInt:                  return GL_UNSIGNED_INT;
	case GraphicsFormatR32SInt:                  return GL_INT;
	case GraphicsFormatR32SFloat:                return GL_FLOAT;
	case GraphicsFormatR32G32UInt:               return GL_UNSIGNED_INT;
	case GraphicsFormatR32G32SInt:               return GL_INT;
	case GraphicsFormatR32G32SFloat:             return GL_FLOAT;
	case GraphicsFormatR32G32B32UInt:            return GL_UNSIGNED_INT;
	case GraphicsFormatR32G32B32SInt:            return GL_INT;
	case GraphicsFormatR32G32B32SFloat:          return GL_FLOAT;
	case GraphicsFormatR32G32B32A32UInt:         return GL_UNSIGNED_INT;
	case GraphicsFormatR32G32B32A32SInt:         return GL_INT;
	case GraphicsFormatR32G32B32A32SFloat:       return GL_FLOAT;
	case GraphicsFormatR64UInt:                  return GL_UNSIGNED_INT64_ARB;
	case GraphicsFormatR64SInt:                  return GL_INT64_ARB;
	case GraphicsFormatR64SFloat:                return GL_DOUBLE;
	case GraphicsFormatR64G64UInt:               return GL_UNSIGNED_INT64_ARB;
	case GraphicsFormatR64G64SInt:               return GL_INT64_ARB;
	case GraphicsFormatR64G64SFloat:             return GL_DOUBLE;
	case GraphicsFormatR64G64B64UInt:            return GL_UNSIGNED_INT64_ARB;
	case GraphicsFormatR64G64B64SInt:            return GL_INT64_ARB;
	case GraphicsFormatR64G64B64SFloat:          return GL_DOUBLE;
	case GraphicsFormatR64G64B64A64UInt:         return GL_UNSIGNED_INT64_ARB;
	case GraphicsFormatR64G64B64A64SInt:         return GL_INT64_ARB;
	case GraphicsFormatR64G64B64A64SFloat:       return GL_DOUBLE;
	case GraphicsFormatB10G11R11UFloatPack32:    return GL_UNSIGNED_INT_10F_11F_11F_REV;
	case GraphicsFormatE5B9G9R9UFloatPack32:     return GL_UNSIGNED_INT_5_9_9_9_REV;
	case GraphicsFormatD16UNorm:                 return GL_UNSIGNED_SHORT;
	case GraphicsFormatX8_D24UNormPack32:        return GL_UNSIGNED_INT_24_8;
	case GraphicsFormatD32_SFLOAT:               return GL_FLOAT;
	case GraphicsFormatS8UInt:                   return GL_UNSIGNED_BYTE;
	case GraphicsFormatD16UNorm_S8UInt:          return GL_INVALID_ENUM;
	case GraphicsFormatD24UNorm_S8UInt:          return GL_UNSIGNED_INT_24_8;
	case GraphicsFormatD32_SFLOAT_S8UInt:        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
	default:
		GL_PLATFORM_ASSERT(false, "Invlida texture data type.");
	}

	return GL_INVALID_ENUM;
}

GLenum
OGLTypes::asTextureInternalFormat(GraphicsFormat format) noexcept
{
	GLenum internalFormat = GL_INVALID_ENUM;
	switch (format)
	{
	case GraphicsFormatR4G4UNormPack8:	         internalFormat = GL_LUMINANCE4_ALPHA4; break;
	case GraphicsFormatR4G4B4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
	case GraphicsFormatB4G4R4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
	case GraphicsFormatR5G6B5UNormPack16:	     internalFormat = GL_RGB565; break;
	case GraphicsFormatB5G6R5UNormPack16:	     internalFormat = GL_RGB565; break;
	case GraphicsFormatR5G5B5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatB5G5R5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatA1R5G5B5UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatR8UNorm:	                 internalFormat = GL_R8; break;
	case GraphicsFormatR8SNorm:	                 internalFormat = GL_R8_SNORM; break;
	case GraphicsFormatR8UScaled:	             internalFormat = GL_R8UI; break;
	case GraphicsFormatR8SScaled:	             internalFormat = GL_R8I; break;
	case GraphicsFormatR8UInt:	                 internalFormat = GL_R8UI; break;
	case GraphicsFormatR8SInt:	                 internalFormat = GL_R8I; break;
	case GraphicsFormatR8SRGB:	                 internalFormat = GL_R8; break;
	case GraphicsFormatR8G8UNorm:	             internalFormat = GL_RG8; break;
	case GraphicsFormatR8G8SNorm:	             internalFormat = GL_RG8_SNORM; break;
	case GraphicsFormatR8G8UScaled:	             internalFormat = GL_RG8UI; break;
	case GraphicsFormatR8G8SScaled:	             internalFormat = GL_RG8I; break;
	case GraphicsFormatR8G8UInt:	             internalFormat = GL_RG8UI; break;
	case GraphicsFormatR8G8SInt:	             internalFormat = GL_RG8I; break;
	case GraphicsFormatR8G8SRGB:	             internalFormat = GL_RG8; break;
	case GraphicsFormatR8G8B8UNorm:	             internalFormat = GL_RGB8; break;
	case GraphicsFormatR8G8B8SNorm:	             internalFormat = GL_RGB8_SNORM; break;
	case GraphicsFormatR8G8B8UScaled:	         internalFormat = GL_RGB8UI; break;
	case GraphicsFormatR8G8B8SScaled:	         internalFormat = GL_RGB8I; break;
	case GraphicsFormatR8G8B8UInt:	             internalFormat = GL_RGB8UI; break;
	case GraphicsFormatR8G8B8SInt:	             internalFormat = GL_RGB8I; break;
	case GraphicsFormatR8G8B8SRGB:	             internalFormat = GL_SRGB8; break;
	case GraphicsFormatB8G8R8UNorm:	             internalFormat = GL_RGB8; break;
	case GraphicsFormatB8G8R8SNorm:	             internalFormat = GL_RGB8_SNORM; break;
	case GraphicsFormatB8G8R8UScaled:	         internalFormat = GL_RGB8UI; break;
	case GraphicsFormatB8G8R8SScaled:	         internalFormat = GL_RGB8I; break;
	case GraphicsFormatB8G8R8UInt:	             internalFormat = GL_RGB8I; break;
	case GraphicsFormatB8G8R8SInt:	             internalFormat = GL_RGB8UI; break;
	case GraphicsFormatB8G8R8SRGB:	             internalFormat = GL_SRGB8; break;
	case GraphicsFormatR8G8B8A8UNorm:	         internalFormat = GL_RGBA8; break;
	case GraphicsFormatR8G8B8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatR8G8B8A8UScaled:	         internalFormat = GL_RGBA8; break;
	case GraphicsFormatR8G8B8A8SScaled:	         internalFormat = GL_RGBA8; break;
	case GraphicsFormatR8G8B8A8UInt:	         internalFormat = GL_RGBA8UI; break;
	case GraphicsFormatR8G8B8A8SInt:	         internalFormat = GL_RGBA8I; break;
	case GraphicsFormatR8G8B8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8; break;
	case GraphicsFormatB8G8R8A8UNorm:	         internalFormat = GL_RGBA8; break;
	case GraphicsFormatB8G8R8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatB8G8R8A8UScaled:	         internalFormat = GL_RGBA8UI; break;
	case GraphicsFormatB8G8R8A8SScaled:	         internalFormat = GL_RGBA8I; break;
	case GraphicsFormatB8G8R8A8UInt:	         internalFormat = GL_RGBA8UI; break;
	case GraphicsFormatB8G8R8A8SInt:	         internalFormat = GL_RGBA8I; break;
	case GraphicsFormatB8G8R8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8; break;
	case GraphicsFormatA8B8G8R8UNormPack32:	     internalFormat = GL_RGBA8; break;
	case GraphicsFormatA8B8G8R8SNormPack32:	     internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatA8B8G8R8UScaledPack32:	 internalFormat = GL_RGBA8UI; break;
	case GraphicsFormatA8B8G8R8SScaledPack32:	 internalFormat = GL_RGBA8I; break;
	case GraphicsFormatA8B8G8R8UIntPack32:	     internalFormat = GL_RGBA8UI; break;
	case GraphicsFormatA8B8G8R8SIntPack32:	     internalFormat = GL_RGBA8I; break;
	case GraphicsFormatA8B8G8R8SRGBPack32:	     internalFormat = GL_SRGB8_ALPHA8; break;
	case GraphicsFormatA2R10G10B10UNormPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2R10G10B10SNormPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2R10G10B10UScaledPack32: internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2R10G10B10SScaledPack32: internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2R10G10B10UIntPack32:	 internalFormat = GL_RGB10_A2UI; break;
	case GraphicsFormatA2R10G10B10SIntPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2B10G10R10UNormPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2B10G10R10SNormPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2B10G10R10UScaledPack32: internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2B10G10R10SScaledPack32: internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatA2B10G10R10UIntPack32:	 internalFormat = GL_RGB10_A2UI; break;
	case GraphicsFormatA2B10G10R10SIntPack32:	 internalFormat = GL_RGB10_A2; break;
	case GraphicsFormatR16UNorm:	             internalFormat = GL_R16; break;
	case GraphicsFormatR16SNorm:	             internalFormat = GL_R16_SNORM; break;
	case GraphicsFormatR16UScaled:	             internalFormat = GL_R16; break;
	case GraphicsFormatR16SScaled:	             internalFormat = GL_R16; break;
	case GraphicsFormatR16UInt:	                 internalFormat = GL_R16UI; break;
	case GraphicsFormatR16SInt:	                 internalFormat = GL_R16I; break;
	case GraphicsFormatR16SFloat:	             internalFormat = GL_R16F; break;
	case GraphicsFormatR16G16UNorm:	             internalFormat = GL_RG16; break;
	case GraphicsFormatR16G16SNorm:	             internalFormat = GL_RG16_SNORM; break;
	case GraphicsFormatR16G16UScaled:	         internalFormat = GL_RG16; break;
	case GraphicsFormatR16G16SScaled:	         internalFormat = GL_RG16; break;
	case GraphicsFormatR16G16UInt:	             internalFormat = GL_RG16UI; break;
	case GraphicsFormatR16G16SInt:	             internalFormat = GL_RG16I; break;
	case GraphicsFormatR16G16SFloat:	         internalFormat = GL_RG16F; break;
	case GraphicsFormatR16G16B16UNorm:	         internalFormat = GL_RGB16; break;
	case GraphicsFormatR16G16B16SNorm:	         internalFormat = GL_RGB16_SNORM; break;
	case GraphicsFormatR16G16B16UScaled:	     internalFormat = GL_RGB16; break;
	case GraphicsFormatR16G16B16SScaled:	     internalFormat = GL_RGB16; break;
	case GraphicsFormatR16G16B16UInt:	         internalFormat = GL_RGB16UI; break;
	case GraphicsFormatR16G16B16SInt:	         internalFormat = GL_RGB16I; break;
	case GraphicsFormatR16G16B16SFloat:	         internalFormat = GL_RGB16F; break;
	case GraphicsFormatR16G16B16A16UNorm:	     internalFormat = GL_RGBA16; break;
	case GraphicsFormatR16G16B16A16SNorm:	     internalFormat = GL_RGBA16_SNORM; break;
	case GraphicsFormatR16G16B16A16UScaled:	     internalFormat = GL_RGBA16; break;
	case GraphicsFormatR16G16B16A16SScaled:	     internalFormat = GL_RGBA16; break;
	case GraphicsFormatR16G16B16A16UInt:	     internalFormat = GL_RGBA16UI; break;
	case GraphicsFormatR16G16B16A16SInt:	     internalFormat = GL_RGBA16I; break;
	case GraphicsFormatR16G16B16A16SFloat:	     internalFormat = GL_RGBA16F; break;
	case GraphicsFormatR32UInt:	                 internalFormat = GL_R32UI; break;
	case GraphicsFormatR32SInt:	                 internalFormat = GL_R32I; break;
	case GraphicsFormatR32SFloat:	             internalFormat = GL_R32F; break;
	case GraphicsFormatR32G32UInt:	             internalFormat = GL_RG32UI; break;
	case GraphicsFormatR32G32SInt:	             internalFormat = GL_RG32I; break;
	case GraphicsFormatR32G32SFloat:	         internalFormat = GL_RG32F; break;
	case GraphicsFormatR32G32B32UInt:	         internalFormat = GL_RGB32UI; break;
	case GraphicsFormatR32G32B32SInt:	         internalFormat = GL_RGB32I; break;
	case GraphicsFormatR32G32B32SFloat:	         internalFormat = GL_RGB32F; break;
	case GraphicsFormatR32G32B32A32UInt:	     internalFormat = GL_RGBA32UI; break;
	case GraphicsFormatR32G32B32A32SInt:	     internalFormat = GL_RGBA32I; break;
	case GraphicsFormatR32G32B32A32SFloat:	     internalFormat = GL_RGBA32F; break;
	case GraphicsFormatR64UInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64SInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64SFloat:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64SFloat:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64SInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64SFloat:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64A64UInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64A64SInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR64G64B64A64SFloat:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB10G11R11UFloatPack32:	 internalFormat = GL_R11F_G11F_B10F; break;
	case GraphicsFormatE5B9G9R9UFloatPack32:	 internalFormat = GL_RGB9_E5; break;
	case GraphicsFormatD16UNorm:	             internalFormat = GL_DEPTH_COMPONENT16; break;
	case GraphicsFormatX8_D24UNormPack32:	     internalFormat = GL_DEPTH_COMPONENT24; break;
	case GraphicsFormatD32_SFLOAT:	             internalFormat = GL_DEPTH_COMPONENT32; break;
	case GraphicsFormatS8UInt:	                 internalFormat = GL_STENCIL_INDEX8; break;
	case GraphicsFormatD16UNorm_S8UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatD24UNorm_S8UInt:	         internalFormat = GL_DEPTH24_STENCIL8; break;
	case GraphicsFormatD32_SFLOAT_S8UInt:	     internalFormat = GL_DEPTH32F_STENCIL8; break;
	case GraphicsFormatBC1RGBUNormBlock:	     internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC1RGBSRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC1RGBAUNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC1RGBASRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC2UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC2SRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC3UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
	case GraphicsFormatBC3SRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT; break;
	case GraphicsFormatBC4UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC4SNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC5UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
	case GraphicsFormatBC5SNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC6HUFloatBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC6HSFloatBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC7UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC7SRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8UNormBlock:	 internalFormat = GL_COMPRESSED_RGB8_ETC2; break;
	case GraphicsFormatETC2R8G8B8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ETC2; break;
	case GraphicsFormatETC2R8G8B8A1UNormBlock:	 internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2; break;
	case GraphicsFormatETC2R8G8B8A1SRGBBlock:	 internalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2; break;
	case GraphicsFormatETC2R8G8B8A8UNormBlock:	 internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC; break;
	case GraphicsFormatETC2R8G8B8A8SRGBBlock:	 internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC; break;
	case GraphicsFormatEACR11UNormBlock:	     internalFormat = GL_COMPRESSED_R11_EAC; break;
	case GraphicsFormatEACR11SNormBlock:	     internalFormat = GL_COMPRESSED_SIGNED_R11_EAC; break;
	case GraphicsFormatEACR11G11UNormBlock:	     internalFormat = GL_COMPRESSED_RG11_EAC; break;
	case GraphicsFormatEACR11G11SNormBlock:	     internalFormat = GL_COMPRESSED_SIGNED_RG11_EAC; break;
	case GraphicsFormatASTC4x4UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;
	case GraphicsFormatASTC4x4SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR; break;
	case GraphicsFormatASTC5x4UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;
	case GraphicsFormatASTC5x4SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR; break;
	case GraphicsFormatASTC5x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR; break;
	case GraphicsFormatASTC5x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR; break;
	case GraphicsFormatASTC6x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR; break;
	case GraphicsFormatASTC6x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR; break;
	case GraphicsFormatASTC6x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR; break;
	case GraphicsFormatASTC6x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR; break;
	case GraphicsFormatASTC8x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR; break;
	case GraphicsFormatASTC8x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR; break;
	case GraphicsFormatASTC8x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR; break;
	case GraphicsFormatASTC8x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR; break;
	case GraphicsFormatASTC8x8UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;
	case GraphicsFormatASTC8x8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR; break;
	case GraphicsFormatASTC10x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR; break;
	case GraphicsFormatASTC10x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR; break;
	case GraphicsFormatASTC10x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR; break;
	case GraphicsFormatASTC10x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR; break;
	case GraphicsFormatASTC10x8UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR; break;
	case GraphicsFormatASTC10x8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR; break;
	case GraphicsFormatASTC10x10UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR; break;
	case GraphicsFormatASTC10x10SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR; break;
	case GraphicsFormatASTC12x10UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR; break;
	case GraphicsFormatASTC12x10SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR; break;
	case GraphicsFormatASTC12x12UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR; break;
	case GraphicsFormatASTC12x12SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR; break;
	default:
		assert(false);
		return GL_INVALID_ENUM;
	}

	GL_PLATFORM_ASSERT(internalFormat != GL_INVALID_ENUM, "Invalid texture internal format.")
		return internalFormat;
}

GLenum
OGLTypes::asCompareFunction(GraphicsCompareFunc func) noexcept
{
	switch (func)
	{
	case GraphicsCompareFunc::GraphicsCompareFuncNone:     return GL_NONE;
	case GraphicsCompareFunc::GraphicsCompareFuncLequal:   return GL_LEQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncEqual:    return GL_EQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncGreater:  return GL_GREATER;
	case GraphicsCompareFunc::GraphicsCompareFuncLess:     return GL_LESS;
	case GraphicsCompareFunc::GraphicsCompareFuncGequal:   return GL_GEQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncNotEqual: return GL_NOTEQUAL;
	case GraphicsCompareFunc::GraphicsCompareFuncAlways:   return GL_ALWAYS;
	case GraphicsCompareFunc::GraphicsCompareFuncNever:    return GL_NEVER;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid compare function");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asBlendFactor(GraphicsBlendFactor func) noexcept
{
	switch (func)
	{
	case GraphicsBlendFactor::GraphicsBlendFactorZero:                  return GL_ZERO;
	case GraphicsBlendFactor::GraphicsBlendFactorOne:                   return GL_ONE;
	case GraphicsBlendFactor::GraphicsBlendFactorDstCol:                return GL_DST_COLOR;
	case GraphicsBlendFactor::GraphicsBlendFactorSrcColor:              return GL_SRC_COLOR;
	case GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha:              return GL_SRC_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorDstAlpha:              return GL_DST_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcCol:        return GL_ONE_MINUS_SRC_COLOR;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstCol:        return GL_ONE_MINUS_DST_COLOR;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorConstantColor:         return GL_CONSTANT_COLOR;
	case GraphicsBlendFactor::GraphicsBlendFactorConstantAlpha:         return GL_CONSTANT_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusConstantColor: return GL_CONSTANT_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorOneMinusConstantAlpha: return GL_CONSTANT_ALPHA;
	case GraphicsBlendFactor::GraphicsBlendFactorSrcAlphaSaturate:      return GL_SRC_ALPHA_SATURATE;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid blend factor");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asBlendOperation(GraphicsBlendOp blendop) noexcept
{
	switch (blendop)
	{
	case GraphicsBlendOp::GraphicsBlendOpAdd:        return GL_FUNC_ADD;
	case GraphicsBlendOp::GraphicsBlendOpSubtract:   return GL_FUNC_SUBTRACT;
	case GraphicsBlendOp::GraphicsBlendOpRevSubtract:return GL_FUNC_REVERSE_SUBTRACT;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid blend operation");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asCullMode(GraphicsCullMode mode) noexcept
{
	switch (mode)
	{
	case GraphicsCullMode::GraphicsCullModeNone:      return GL_NONE;
	case GraphicsCullMode::GraphicsCullModeFront:     return GL_FRONT;
	case GraphicsCullMode::GraphicsCullModeBack:      return GL_BACK;
	case GraphicsCullMode::GraphicsCullModeFrontBack: return GL_FRONT_AND_BACK;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid cull mode");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asFillMode(GraphicsPolygonMode mode) noexcept
{
	switch (mode)
	{
	case GraphicsPolygonMode::GraphicsPolygonModePoint:     return GL_POINT;
	case GraphicsPolygonMode::GraphicsPolygonModeWireframe: return GL_LINE;
	case GraphicsPolygonMode::GraphicsPolygonModeSolid:     return GL_FILL;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid fill mode");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asStencilOperation(GraphicsStencilOp stencilop) noexcept
{
	switch (stencilop)
	{
	case GraphicsStencilOp::GraphicsStencilOpKeep:     return GL_KEEP;
	case GraphicsStencilOp::GraphicsStencilOpReplace:  return GL_REPLACE;
	case GraphicsStencilOp::GraphicsStencilOpIncr:     return GL_INCR;
	case GraphicsStencilOp::GraphicsStencilOpDecr:     return GL_DECR;
	case GraphicsStencilOp::GraphicsStencilOpZero:     return GL_ZERO;
	case GraphicsStencilOp::GraphicsStencilOpIncrWrap: return GL_INCR_WRAP;
	case GraphicsStencilOp::GraphicsStencilOpDecrWrap: return GL_DECR_WRAP;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid stencil operation");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asSamplerWrap(GraphicsSamplerWrap wrap) noexcept
{
	switch (wrap)
	{
	case GraphicsSamplerWrap::GraphicsSamplerWrapRepeat:      return GL_REPEAT;
	case GraphicsSamplerWrap::GraphicsSamplerWrapMirror:      return GL_MIRRORED_REPEAT;
	case GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge: return GL_CLAMP_TO_EDGE;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid sampler wrap");
		return GL_INVALID_ENUM;
	}
}

GLenum
OGLTypes::asSamplerFilter(GraphicsSamplerFilter filter) noexcept
{
	switch (filter)
	{
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearest:              return GL_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinear:               return GL_LINEAR;
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid sampler filter");
		return GL_INVALID_ENUM;
	}
}

GLboolean
OGLTypes::isStencilFormat(GraphicsFormat format) noexcept
{
	if (format == GraphicsFormat::GraphicsFormatS8UInt)
		return GL_TRUE;
	return GL_FALSE;
}

GLboolean
OGLTypes::isDepthFormat(GraphicsFormat format) noexcept
{
	if (format == GraphicsFormat::GraphicsFormatD16UNorm ||
		format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 ||
		format == GraphicsFormat::GraphicsFormatD32_SFLOAT)
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean
OGLTypes::isDepthStencilFormat(GraphicsFormat format) noexcept
{
	if (format == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt ||
		format == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt ||
		format == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean
OGLTypes::isCompressedTexture(GraphicsFormat format) noexcept
{
	switch (format)
	{
	case GraphicsFormatBC1RGBUNormBlock:
	case GraphicsFormatBC1RGBSRGBBlock:
	case GraphicsFormatBC1RGBAUNormBlock:
	case GraphicsFormatBC1RGBASRGBBlock:
	case GraphicsFormatBC2UNormBlock:
	case GraphicsFormatBC2SRGBBlock:
	case GraphicsFormatBC3UNormBlock:
	case GraphicsFormatBC3SRGBBlock:
	case GraphicsFormatBC4UNormBlock:
	case GraphicsFormatBC4SNormBlock:
	case GraphicsFormatBC5UNormBlock:
	case GraphicsFormatBC5SNormBlock:
	case GraphicsFormatBC6HUFloatBlock:
	case GraphicsFormatBC6HSFloatBlock:
	case GraphicsFormatBC7UNormBlock:
	case GraphicsFormatBC7SRGBBlock:
	case GraphicsFormatETC2R8G8B8UNormBlock:
	case GraphicsFormatETC2R8G8B8SRGBBlock:
	case GraphicsFormatETC2R8G8B8A1UNormBlock:
	case GraphicsFormatETC2R8G8B8A1SRGBBlock:
	case GraphicsFormatETC2R8G8B8A8UNormBlock:
	case GraphicsFormatETC2R8G8B8A8SRGBBlock:
	case GraphicsFormatEACR11UNormBlock:
	case GraphicsFormatEACR11SNormBlock:
	case GraphicsFormatEACR11G11UNormBlock:
	case GraphicsFormatEACR11G11SNormBlock:
	case GraphicsFormatASTC4x4UNormBlock:
	case GraphicsFormatASTC4x4SRGBBlock:
	case GraphicsFormatASTC5x4UNormBlock:
	case GraphicsFormatASTC5x4SRGBBlock:
	case GraphicsFormatASTC5x5UNormBlock:
	case GraphicsFormatASTC5x5SRGBBlock:
	case GraphicsFormatASTC6x5UNormBlock:
	case GraphicsFormatASTC6x5SRGBBlock:
	case GraphicsFormatASTC6x6UNormBlock:
	case GraphicsFormatASTC6x6SRGBBlock:
	case GraphicsFormatASTC8x5UNormBlock:
	case GraphicsFormatASTC8x5SRGBBlock:
	case GraphicsFormatASTC8x6UNormBlock:
	case GraphicsFormatASTC8x6SRGBBlock:
	case GraphicsFormatASTC8x8UNormBlock:
	case GraphicsFormatASTC8x8SRGBBlock:
	case GraphicsFormatASTC10x5UNormBlock:
	case GraphicsFormatASTC10x5SRGBBlock:
	case GraphicsFormatASTC10x6UNormBlock:
	case GraphicsFormatASTC10x6SRGBBlock:
	case GraphicsFormatASTC10x8UNormBlock:
	case GraphicsFormatASTC10x8SRGBBlock:
	case GraphicsFormatASTC10x10UNormBlock:
	case GraphicsFormatASTC10x10SRGBBlock:
	case GraphicsFormatASTC12x10UNormBlock:
	case GraphicsFormatASTC12x10SRGBBlock:
	case GraphicsFormatASTC12x12UNormBlock:
	case GraphicsFormatASTC12x12SRGBBlock:
		return GL_TRUE;
	default:
		return GL_FALSE;
	}
}

bool
OGLCheck::checkError() noexcept
{
	bool success = true;

	GLenum result = ::glGetError();
	if (GL_NO_ERROR != result)
	{
		success = false;

		switch (result)
		{
		case GL_INVALID_ENUM:
			GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_OPERATION");
			break;
		case GL_STACK_OVERFLOW:
			GL_PLATFORM_LOG("glGetError() fail : GL_STACK_OVERFLOW");
			break;
		case GL_STACK_UNDERFLOW:
			GL_PLATFORM_LOG("glGetError() fail : GL_STACK_UNDERFLOW");
			break;
		case GL_OUT_OF_MEMORY:
			GL_PLATFORM_LOG("glGetError() fail : GL_OUT_OF_MEMORY");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		default:
			GL_PLATFORM_LOG("glGetError() fail : Unknown");
		};
	}

	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != result)
	{
		success = false;

		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			GL_PLATFORM_LOG("FBO:Incompleteattachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			GL_PLATFORM_LOG("FBO:Incomplete missingattachment");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			GL_PLATFORM_LOG("FBO:Unsupported");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			GL_PLATFORM_LOG("FBO:Incomplete drawbuffer");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			GL_PLATFORM_LOG("FBO:Incomplete readbuffer");
			break;
		default:
			GL_PLATFORM_LOG("FBO:Unknown");
		}
	}

	return success;
}

void
OGLCheck::debugOutput(const char* message, ...) noexcept
{
	va_list va;
	va_start(va, message);
	vprintf(message, va);
	printf("\n");
	va_end(va);
}

_NAME_END