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
#include "egl2_types.h"

_NAME_BEGIN

GLboolean EGL2Types::_egl2init = GL_FALSE;
GLboolean EGL2Types::_egl2Features[EGL2_RangeSize];

bool
EGL2Types::setup() noexcept
{
	if (_egl2init)
		return true;

	std::memset(_egl2Features, GL_FALSE, sizeof(_egl2Features));

	const GLubyte* extension = glGetString(GL_EXTENSIONS);
	if (!extension)
		return false;

	std::size_t extensionLength = std::strlen((const char*)extension);
	if (extensionLength == 0)
		return false;

	const char* offset = (char*)extension;
	for (;;)
	{
		const char* pos = std::strstr(offset, " ");
		if (!pos)
		{
			std::size_t length = std::strlen(offset);
			if (length == 0)
				break;

			pos = offset + length;
		}

		std::intptr_t length = pos - offset;
		if (length < 0)
			return false;

		const char* src = pos - length;

		if (strncmp(src, "GL_OES_required_internalformat", length) == 0)		   _egl2Features[EGL2Features::EGL2_OES_required_internalformat] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_read_format_bgra", length) == 0)             _egl2Features[EGL2Features::EGL2_EXT_read_format_bgra] = GL_TRUE;
		else if (strncmp(src, "GL_ARM_rgba8", length) == 0)                        _egl2Features[EGL2Features::EGL2_ARM_rgba8] = GL_TRUE;
		else if (strncmp(src, "GL_OES_rgb8_rgba8", length) == 0)                   _egl2Features[EGL2Features::EGL2_OES_rgb8_rgba8] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_format_BGRA8888", length) == 0)      _egl2Features[EGL2Features::EGL2_EXT_texture_format_BGRA8888] = GL_TRUE;
		else if (strncmp(src, "GL_APPLE_texture_format_BGRA8888", length) == 0)    _egl2Features[EGL2Features::EGL2_APPLE_texture_format_BGRA8888] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_sRGB", length) == 0)                         _egl2Features[EGL2Features::EGL2_EXT_sRGB] = GL_TRUE;
		else if (strncmp(src, "GL_NV_sRGB_formats", length) == 0)                  _egl2Features[EGL2Features::EGL2_NV_sRGB_formats] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_rg", length) == 0)                   _egl2Features[EGL2Features::EGL2_EXT_texture_rg] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_norm16", length) == 0)               _egl2Features[EGL2Features::EGL2_EXT_texture_norm16] = GL_TRUE;
		else if (strncmp(src, "GL_OES_texture_half_float", length) == 0)           _egl2Features[EGL2Features::EGL2_OES_texture_half_float] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_color_buffer_half_float", length) == 0)      _egl2Features[EGL2Features::EGL2_EXT_color_buffer_half_float] = GL_TRUE;
		else if (strncmp(src, "GL_OES_texture_float", length) == 0)                _egl2Features[EGL2Features::EGL2_OES_texture_float] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_color_buffer_float", length) == 0)           _egl2Features[EGL2Features::EGL2_EXT_color_buffer_float] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_type_2_10_10_10_REV", length) == 0)  _egl2Features[EGL2Features::EGL2_EXT_texture_type_2_10_10_10_REV] = GL_TRUE;
		else if (strncmp(src, "GL_APPLE_texture_packed_float", length) == 0)       _egl2Features[EGL2Features::EGL2_APPLE_texture_packed_float] = GL_TRUE;
		else if (strncmp(src, "GL_OES_depth_texture", length) == 0)                _egl2Features[EGL2Features::EGL2_OES_depth_texture] = GL_TRUE;
		else if (strncmp(src, "GL_OES_depth24", length) == 0)                      _egl2Features[EGL2Features::EGL2_OES_depth24] = GL_TRUE;
		else if (strncmp(src, "GL_OES_depth32", length) == 0)                      _egl2Features[EGL2Features::EGL2_OES_depth32] = GL_TRUE;
		else if (strncmp(src, "GL_OES_texture_stencil8", length) == 0)             _egl2Features[EGL2Features::EGL2_OES_texture_stencil8] = GL_TRUE;
		else if (strncmp(src, "GL_OES_packed_depth_stencil", length) == 0)         _egl2Features[EGL2Features::EGL2_OES_packed_depth_stencil] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_compression_dxt1", length) == 0)     _egl2Features[EGL2Features::EGL2_EXT_texture_compression_dxt1] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_compression_s3tc", length) == 0)     _egl2Features[EGL2Features::EGL2_EXT_texture_compression_s3tc] = GL_TRUE;
		else if (strncmp(src, "GL_KHR_texture_compression_astc_ldr", length) == 0) _egl2Features[EGL2Features::EGL2_KHR_texture_compression_astc_ldr] = GL_TRUE;
		else if (strncmp(src, "GL_OES_vertex_type_10_10_10_2", length) == 0)       _egl2Features[EGL2Features::EGL2_OES_vertex_type_10_10_10_2] = GL_TRUE;
		else if (strncmp(src, "GL_OES_vertex_half_float", length) == 0)            _egl2Features[EGL2Features::EGL2_OES_vertex_half_float] = GL_TRUE;
		else if (strncmp(src, "GL_EXT_texture_filter_anisotropic", length) == 0)   _egl2Features[EGL2Features::EGL2_EXT_texture_filter_anisotropic] = GL_TRUE;
		else if (strncmp(src, "GL_KHR_debug", length) == 0)                        _egl2Features[EGL2Features::EGL2_KHR_debug] = GL_TRUE;

		offset = pos + 1;
	}

	_egl2init = true;
	return true;
}

GLenum
EGL2Types::asVertexType(GraphicsVertexType type) noexcept
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
EGL2Types::asVertexFormat(GraphicsFormat format) noexcept
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
	case GraphicsFormatR64UInt:
	case GraphicsFormatR64G64UInt:
	case GraphicsFormatR64G64B64UInt:
	case GraphicsFormatR64G64B64A64UInt:
	case GraphicsFormatR64SFloat:
	case GraphicsFormatR64G64SFloat:
	case GraphicsFormatR64G64B64SFloat:
	case GraphicsFormatR64G64B64A64SFloat:
		GL_PLATFORM_LOG("Can't support format");
		return GL_INVALID_ENUM;
	default:
		return GL_INVALID_ENUM;
		break;
	}
}

GLenum
EGL2Types::asIndexType(GraphicsIndexType type) noexcept
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
EGL2Types::asShaderStage(GraphicsShaderStage stage) noexcept
{
	switch (stage)
	{
	case GraphicsShaderStage::GraphicsShaderStageVertex:         return GL_VERTEX_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageFragment:       return GL_FRAGMENT_SHADER;
	case GraphicsShaderStage::GraphicsShaderStageCompute:
	case GraphicsShaderStage::GraphicsShaderStageGeometry:
	case GraphicsShaderStage::GraphicsShaderStageTessControl:
	case GraphicsShaderStage::GraphicsShaderStageTessEvaluation:
		GL_PLATFORM_LOG("Can't support shader stage");
	default:
		GL_PLATFORM_LOG("Invalid shader type");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asTextureTarget(GraphicsTextureDim target, bool multisampler) noexcept
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
		{
			GL_PLATFORM_LOG("Can't support 3d texture.");
			return GL_INVALID_ENUM;
		}
		else
		{
			GL_PLATFORM_LOG("Can't support 3d texture multisampler.");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == GraphicsTextureDim::GraphicsTextureDim2DArray)
	{
		if (!multisampler)
		{
			GL_PLATFORM_LOG("Can't support 2d array texture.");
			return GL_INVALID_ENUM;
		}
		else
		{
			GL_PLATFORM_LOG("Can't support 2d array texture multisampler.");
			return GL_INVALID_ENUM;
		}
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
			return GL_TEXTURE_CUBE_MAP_ARRAY_EXT;
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
EGL2Types::asTextureFormat(GraphicsFormat format) noexcept
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
		return GL_RED_EXT;
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
		return GL_RG_EXT;
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
		GL_PLATFORM_LOG("Can't support BGR format");
		return GL_INVALID_ENUM;
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
		return GL_BGRA_EXT;
	case GraphicsFormatA8B8G8R8UNormPack32:
	case GraphicsFormatA8B8G8R8SNormPack32:
	case GraphicsFormatA8B8G8R8UScaledPack32:
	case GraphicsFormatA8B8G8R8SScaledPack32:
	case GraphicsFormatA8B8G8R8UIntPack32:
	case GraphicsFormatA8B8G8R8SIntPack32:
		GL_PLATFORM_LOG("Can't support ABGR format");
		return GL_INVALID_ENUM;
	case GraphicsFormatR8SRGB:
	case GraphicsFormatR8G8SRGB:
	case GraphicsFormatR8G8B8SRGB:
	case GraphicsFormatB8G8R8SRGB:
		return GL_SRGB_EXT;
	case GraphicsFormatR8G8B8A8SRGB:
	case GraphicsFormatB8G8R8A8SRGB:
	case GraphicsFormatA8B8G8R8SRGBPack32:
		return GL_SRGB_ALPHA_EXT;
	case GraphicsFormatD16UNorm:
	case GraphicsFormatX8_D24UNormPack32:
	case GraphicsFormatD32_SFLOAT:
		return GL_DEPTH_COMPONENT;
	case GraphicsFormatS8UInt:
		return GL_STENCIL_INDEX8;
	case GraphicsFormatD24UNorm_S8UInt:
	case GraphicsFormatD16UNorm_S8UInt:
	case GraphicsFormatD32_SFLOAT_S8UInt:
		return GL_DEPTH_STENCIL_OES;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid texture format");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asTextureType(GraphicsFormat format) noexcept
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
	case GraphicsFormatA1R5G5B5UNormPack16:      return GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT;
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
	case GraphicsFormatA2R10G10B10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
	case GraphicsFormatA2R10G10B10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
	case GraphicsFormatA2R10G10B10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
	case GraphicsFormatA2B10G10R10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
	case GraphicsFormatA2B10G10R10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
	case GraphicsFormatA2B10G10R10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
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
	case GraphicsFormatD16UNorm:                 return GL_UNSIGNED_SHORT;
	case GraphicsFormatD32_SFLOAT:               return GL_FLOAT;
	case GraphicsFormatS8UInt:                   return GL_UNSIGNED_BYTE;
	case GraphicsFormatX8_D24UNormPack32:        return GL_UNSIGNED_INT;
	case GraphicsFormatD16UNorm_S8UInt:          return GL_INVALID_ENUM;
	case GraphicsFormatD24UNorm_S8UInt:          return GL_UNSIGNED_INT_24_8_OES;
	case GraphicsFormatD32_SFLOAT_S8UInt:        return GL_INVALID_ENUM;
	case GraphicsFormatA2R10G10B10SNormPack32:   return GL_INVALID_ENUM;
	case GraphicsFormatA2R10G10B10SScaledPack32: return GL_INVALID_ENUM;
	case GraphicsFormatA2R10G10B10SIntPack32:    return GL_INVALID_ENUM;
	case GraphicsFormatA2B10G10R10SNormPack32:   return GL_INVALID_ENUM;
	case GraphicsFormatA2B10G10R10SScaledPack32: return GL_INVALID_ENUM;
	case GraphicsFormatA2B10G10R10SIntPack32:    return GL_INVALID_ENUM;
	case GraphicsFormatB10G11R11UFloatPack32:    return GL_INVALID_ENUM;
	case GraphicsFormatE5B9G9R9UFloatPack32:     return GL_INVALID_ENUM;
	case GraphicsFormatR64UInt:
	case GraphicsFormatR64SInt:
	case GraphicsFormatR64SFloat:
	case GraphicsFormatR64G64UInt:
	case GraphicsFormatR64G64SInt:
	case GraphicsFormatR64G64SFloat:
	case GraphicsFormatR64G64B64UInt:
	case GraphicsFormatR64G64B64SInt:
	case GraphicsFormatR64G64B64SFloat:
	case GraphicsFormatR64G64B64A64UInt:
	case GraphicsFormatR64G64B64A64SInt:
	case GraphicsFormatR64G64B64A64SFloat:
		GL_PLATFORM_ASSERT(false, "Can't support 64bit format");
		return GL_INVALID_ENUM;
	default:
		GL_PLATFORM_ASSERT(false, "Invlida texture data type.");
	}

	return GL_INVALID_ENUM;
}

GLenum
EGL2Types::asTextureInternalFormat(GraphicsFormat format) noexcept
{
	GLenum internalFormat = GL_INVALID_ENUM;
	switch (format)
	{
	case GraphicsFormatR4G4UNormPack8:	         internalFormat = GL_LUMINANCE8_ALPHA8_EXT; break;
	case GraphicsFormatR4G4B4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
	case GraphicsFormatB4G4R4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
	case GraphicsFormatR5G6B5UNormPack16:	     internalFormat = GL_RGB565; break;
	case GraphicsFormatB5G6R5UNormPack16:	     internalFormat = GL_RGB565; break;
	case GraphicsFormatR5G5B5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatB5G5R5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatA1R5G5B5UNormPack16:	     internalFormat = GL_RGB5_A1; break;
	case GraphicsFormatR8UNorm:	                 internalFormat = GL_R8_EXT; break;
	case GraphicsFormatR8SNorm:	                 internalFormat = GL_R8_SNORM; break;
	case GraphicsFormatR8UScaled:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8SScaled:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8UInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8SInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8SRGB:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8UNorm:	             internalFormat = GL_RG8_EXT; break;
	case GraphicsFormatR8G8SNorm:	             internalFormat = GL_RG8_SNORM; break;
	case GraphicsFormatR8G8UScaled:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8SScaled:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8SRGB:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8UNorm:	             internalFormat = GL_RGB8_OES; break;
	case GraphicsFormatR8G8B8SNorm:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8UScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8SScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8SRGB:	             internalFormat = GL_SRGB_EXT; break;
	case GraphicsFormatB8G8R8UNorm:	             internalFormat = GL_RGB8_OES; break;
	case GraphicsFormatB8G8R8SNorm:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8UScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8SScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8SRGB:	             internalFormat = GL_SRGB_EXT; break;
	case GraphicsFormatR8G8B8A8UNorm:	         internalFormat = GL_RGBA8_OES; break;
	case GraphicsFormatR8G8B8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatR8G8B8A8UScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8A8SScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8A8UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8A8SInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR8G8B8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8_EXT; break;
	case GraphicsFormatB8G8R8A8UNorm:	         internalFormat = GL_RGB8_OES; break;
	case GraphicsFormatB8G8R8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatB8G8R8A8UScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8A8SScaled:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8A8UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8A8SInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatB8G8R8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8_EXT; break;
	case GraphicsFormatA8B8G8R8UNormPack32:	     internalFormat = GL_RGBA8_OES; break;
	case GraphicsFormatA8B8G8R8SNormPack32:	     internalFormat = GL_RGBA8_SNORM; break;
	case GraphicsFormatA8B8G8R8UScaledPack32:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA8B8G8R8SScaledPack32:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA8B8G8R8UIntPack32:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA8B8G8R8SIntPack32:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA8B8G8R8SRGBPack32:	     internalFormat = GL_SRGB8_ALPHA8_EXT; break;
	case GraphicsFormatA2R10G10B10UNormPack32:	 internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2R10G10B10SNormPack32:	 internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2R10G10B10UScaledPack32: internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA2R10G10B10SScaledPack32: internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2R10G10B10UIntPack32:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA2R10G10B10SIntPack32:	 internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2B10G10R10UNormPack32:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA2B10G10R10SNormPack32:	 internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2B10G10R10UScaledPack32: internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA2B10G10R10SScaledPack32: internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatA2B10G10R10UIntPack32:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatA2B10G10R10SIntPack32:	 internalFormat = GL_RGB10_A2_EXT; break;
	case GraphicsFormatR16UNorm:	             internalFormat = GL_R16_EXT; break;
	case GraphicsFormatR16SNorm:	             internalFormat = GL_R16_SNORM_EXT; break;
	case GraphicsFormatR16UScaled:	             internalFormat = GL_R16_EXT; break;
	case GraphicsFormatR16SScaled:	             internalFormat = GL_R16_EXT; break;
	case GraphicsFormatR16UInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16SInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16SFloat:	             internalFormat = GL_R16F_EXT; break;
	case GraphicsFormatR16G16UNorm:	             internalFormat = GL_RG16_EXT; break;
	case GraphicsFormatR16G16SNorm:	             internalFormat = GL_RG16_SNORM_EXT; break;
	case GraphicsFormatR16G16UScaled:	         internalFormat = GL_RG16_EXT; break;
	case GraphicsFormatR16G16SScaled:	         internalFormat = GL_RG16_EXT; break;
	case GraphicsFormatR16G16UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16SFloat:	         internalFormat = GL_RG16F_EXT; break;
	case GraphicsFormatR16G16B16UNorm:	         internalFormat = GL_RGB16_EXT; break;
	case GraphicsFormatR16G16B16SNorm:	         internalFormat = GL_RGB16_SNORM_EXT; break;
	case GraphicsFormatR16G16B16UScaled:	     internalFormat = GL_RGB16_EXT; break;
	case GraphicsFormatR16G16B16SScaled:	     internalFormat = GL_RGB16_EXT; break;
	case GraphicsFormatR16G16B16UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16B16SInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16B16SFloat:	         internalFormat = GL_RGB16F_EXT; break;
	case GraphicsFormatR16G16B16A16UNorm:	     internalFormat = GL_RGBA16_EXT; break;
	case GraphicsFormatR16G16B16A16SNorm:	     internalFormat = GL_RGBA16_SNORM_EXT; break;
	case GraphicsFormatR16G16B16A16UScaled:	     internalFormat = GL_RGBA16_EXT; break;
	case GraphicsFormatR16G16B16A16SScaled:	     internalFormat = GL_RGBA16_EXT; break;
	case GraphicsFormatR16G16B16A16UInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16B16A16SInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR16G16B16A16SFloat:	     internalFormat = GL_RGBA16F_EXT; break;
	case GraphicsFormatR32UInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32SInt:	                 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32SFloat:	             internalFormat = GL_R32F_EXT; break;
	case GraphicsFormatR32G32UInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32SInt:	             internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32SFloat:	         internalFormat = GL_RG32F_EXT; break;
	case GraphicsFormatR32G32B32UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32B32SInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32B32SFloat:	         internalFormat = GL_RGB32F_EXT; break;
	case GraphicsFormatR32G32B32A32UInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32B32A32SInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatR32G32B32A32SFloat:	     internalFormat = GL_RGBA32F_EXT; break;
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
	case GraphicsFormatB10G11R11UFloatPack32:	 internalFormat = GL_R11F_G11F_B10F_APPLE; break;
	case GraphicsFormatE5B9G9R9UFloatPack32:	 internalFormat = GL_RGB9_E5_APPLE; break;
	case GraphicsFormatD16UNorm:	             internalFormat = GL_DEPTH_COMPONENT16; break;
	case GraphicsFormatX8_D24UNormPack32:	     internalFormat = GL_DEPTH_COMPONENT24_OES; break;
	case GraphicsFormatD32_SFLOAT:	             internalFormat = GL_DEPTH_COMPONENT32_OES; break;
	case GraphicsFormatS8UInt:	                 internalFormat = GL_STENCIL_INDEX8; break;
	case GraphicsFormatD16UNorm_S8UInt:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatD24UNorm_S8UInt:	         internalFormat = GL_DEPTH24_STENCIL8_OES; break;
	case GraphicsFormatD32_SFLOAT_S8UInt:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC1RGBUNormBlock:	     internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC1RGBSRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC1RGBAUNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
	case GraphicsFormatBC1RGBASRGBBlock:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC2UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
	case GraphicsFormatBC2SRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC3UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
	case GraphicsFormatBC3SRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC4UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC4SNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC5UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC5SNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC6HUFloatBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC6HSFloatBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC7UNormBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatBC7SRGBBlock:	         internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8UNormBlock:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8SRGBBlock:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8A1UNormBlock:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8A1SRGBBlock:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8A8UNormBlock:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatETC2R8G8B8A8SRGBBlock:	 internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatEACR11UNormBlock:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatEACR11SNormBlock:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatEACR11G11UNormBlock:	     internalFormat = GL_INVALID_ENUM; break;
	case GraphicsFormatEACR11G11SNormBlock:	     internalFormat = GL_INVALID_ENUM; break;
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
EGL2Types::asCompareFunction(GraphicsCompareFunc func) noexcept
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
EGL2Types::asBlendFactor(GraphicsBlendFactor func) noexcept
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
EGL2Types::asBlendOperation(GraphicsBlendOp blendop) noexcept
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
EGL2Types::asCullMode(GraphicsCullMode mode) noexcept
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
EGL2Types::asFrontFace(GraphicsFrontFace face) noexcept
{
	switch (face)
	{
	case GraphicsFrontFace::GraphicsFrontFaceCW:
		return GL_CW;
	case GraphicsFrontFace::GraphicsFrontFaceCCW:
		return GL_CCW;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid front face");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asFillMode(GraphicsPolygonMode mode) noexcept
{
	switch (mode)
	{
	case GraphicsPolygonMode::GraphicsPolygonModePoint:
	case GraphicsPolygonMode::GraphicsPolygonModeWireframe:
	case GraphicsPolygonMode::GraphicsPolygonModeSolid:
		GL_PLATFORM_ASSERT(false, "Can't support glPolygonMode");
		return GL_INVALID_ENUM;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid fill mode");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asStencilOperation(GraphicsStencilOp stencilop) noexcept
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
EGL2Types::asSamplerWrap(GraphicsSamplerWrap wrap) noexcept
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
EGL2Types::asSamplerMinFilter(GraphicsSamplerFilter filter) noexcept
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

GLenum
EGL2Types::asSamplerMagFilter(GraphicsSamplerFilter filter) noexcept
{
	switch (filter)
	{
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearest:              return GL_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinear:               return GL_LINEAR;
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapLinear:  return GL_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest: return GL_NEAREST;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest:  return GL_LINEAR;
	case GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear:   return GL_LINEAR;
	default:
		GL_PLATFORM_ASSERT(false, "Invalid sampler filter");
		return GL_INVALID_ENUM;
	}
}

GLboolean
EGL2Types::isSupportFeature(EGL2Features features) noexcept
{
	assert(features >= EGL2Features::EGL2_BeginRange && features <= EGL2Features::EGL2_EndRange);
	return _egl2Features[features];
}

GLboolean
EGL2Types::isStencilFormat(GraphicsFormat format) noexcept
{
	if (format == GraphicsFormat::GraphicsFormatS8UInt)
		return GL_TRUE;
	return GL_FALSE;
}

GLboolean
EGL2Types::isDepthFormat(GraphicsFormat format) noexcept
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
EGL2Types::isDepthStencilFormat(GraphicsFormat format) noexcept
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
EGL2Types::isCompressedTexture(GraphicsFormat format) noexcept
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
EGL2Check::checkError() noexcept
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
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			GL_PLATFORM_LOG("FBO:Incomplete dimensions");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			GL_PLATFORM_LOG("FBO:Unsupported");
			break;
		default:
			GL_PLATFORM_LOG("FBO:Unknown");
		}
	}

	return success;
}

void
EGL2Check::debugOutput(const char* message, ...) noexcept
{
	va_list va;
	va_start(va, message);
	vprintf(message, va);
	printf("\n");
	va_end(va);
}

_NAME_END