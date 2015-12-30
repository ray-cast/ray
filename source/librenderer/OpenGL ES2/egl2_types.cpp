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
#include "egl2_types.h"

_NAME_BEGIN

GLenum
EGL2Types::asVertexType(VertexType type) noexcept
{
	switch (type)
	{
	case VertexType::Point:
		return GL_POINTS;
	case VertexType::Line:
		return GL_LINES;
	case VertexType::Triangle:
		return GL_TRIANGLES;
	case VertexType::Fan:
		return GL_TRIANGLE_FAN;
	case VertexType::PointOrLine:
		return GL_POINTS;
	case VertexType::TriangleOrLine:
		return GL_TRIANGLES;
	case VertexType::FanOrLine:
		return GL_TRIANGLE_FAN;
	default:
		GL_PLATFORM_LOG("Invalid vertex type");
		return false;
	}
}

GLenum
EGL2Types::asVertexFormat(VertexFormat format) noexcept
{
	switch (format)
	{
	case VertexFormat::Char:
	case VertexFormat::Char2:
	case VertexFormat::Char3:
	case VertexFormat::Char4:
		return GL_BYTE;
	case VertexFormat::Short:
	case VertexFormat::Short2:
	case VertexFormat::Short3:
	case VertexFormat::Short4:
		return GL_SHORT;
	case VertexFormat::Int:
	case VertexFormat::Int2:
	case VertexFormat::Int3:
	case VertexFormat::Int4:
		return GL_INT;
	case VertexFormat::Uchar:
	case VertexFormat::Uchar2:
	case VertexFormat::Uchar3:
	case VertexFormat::Uchar4:
		return GL_UNSIGNED_BYTE;
	case VertexFormat::Ushort:
	case VertexFormat::Ushort2:
	case VertexFormat::Ushort3:
	case VertexFormat::Ushort4:
		return GL_UNSIGNED_SHORT;
	case VertexFormat::Uint:
	case VertexFormat::Uint2:
	case VertexFormat::Uint3:
	case VertexFormat::Uint4:
		return GL_UNSIGNED_INT;
	case VertexFormat::Float:
	case VertexFormat::Float2:
	case VertexFormat::Float3:
	case VertexFormat::Float4:
		return GL_FLOAT;
	default:
		GL_PLATFORM_LOG("Invalid vertex format");
		return false;
	}
}

GLenum
EGL2Types::asIndexType(IndexType type) noexcept
{
	switch (type)
	{
	case IndexType::None:
		return GL_NONE;
	case IndexType::Uint16:
		return GL_UNSIGNED_SHORT;
	case IndexType::Uint32:
		return GL_UNSIGNED_INT;
	default:
		GL_PLATFORM_LOG("Invalid index type");
		return false;
	}
}

GLenum
EGL2Types::asShaderType(ShaderType type) noexcept
{
	switch (type)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	case ShaderType::Compute:
		GL_PLATFORM_LOG("Can't support compute shader");
		return GL_INVALID_ENUM;
	case ShaderType::TessControl:
		GL_PLATFORM_LOG("Can't support tess control");
		return GL_INVALID_ENUM;
	case ShaderType::TessEvaluation:
		GL_PLATFORM_LOG("Can't support tess evaluation");
		return GL_INVALID_ENUM;
	default:
		GL_PLATFORM_LOG("Invalid shader type");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asTarget(TextureDim target, bool multisampler) noexcept
{
	if (target == TextureDim::DIM_2D)
	{
		if (multisampler)
			return GL_TEXTURE_2D_MULTISAMPLE;
		else
			return GL_TEXTURE_2D;
	}
	else if (target == TextureDim::DIM_2D_ARRAY)
	{
		if (multisampler)
			return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		else
		{
			GL_PLATFORM_LOG("Can't support 2d texture array");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == TextureDim::DIM_3D)
	{
		if (!multisampler)
		{
			GL_PLATFORM_LOG("Can't support 3d texture");
			return GL_INVALID_ENUM;
		}
		else
		{
			GL_PLATFORM_LOG("Can't support 3d texture multisampler");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == TextureDim::DIM_3D_ARRAY)
	{
		if (multisampler)
			return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		else
			return GL_TEXTURE_2D;
	}
	else if (target == TextureDim::DIM_CUBE)
	{
		if (!multisampler)
			return GL_TEXTURE_CUBE_MAP;
		else
		{
			GL_PLATFORM_LOG("Can't support cube multisampler");
			return GL_INVALID_ENUM;
		}
	}
	else if (target == TextureDim::DIM_CUBE_ARRAY)
	{
		if (!multisampler)
			GL_PLATFORM_LOG("Can't support cube array");
		else
			GL_PLATFORM_LOG("Can't support cube array multisampler");

		return GL_INVALID_ENUM;
	}

	GL_PLATFORM_LOG("Invalid texture target");
	return GL_INVALID_ENUM;
}

GLenum
EGL2Types::asFormat(TextureFormat format) noexcept
{
	switch (format)
	{
	case TextureFormat::STENCIL8:
		return GL_STENCIL_INDEX_OES;
	case TextureFormat::DEPTH_COMPONENT16:
	case TextureFormat::DEPTH_COMPONENT24:
	case TextureFormat::DEPTH_COMPONENT32:
		return GL_DEPTH_COMPONENT;
	case TextureFormat::DEPTH24_STENCIL8:
	case TextureFormat::DEPTH32_STENCIL8:
		return GL_DEPTH_STENCIL_OES;
	case TextureFormat::R5G6B5:
	case TextureFormat::R8G8B8:
	case TextureFormat::R16G16B16:
	case TextureFormat::R11G11B10F:
	case TextureFormat::R16G16B16F:
	case TextureFormat::R32G32B32F:
	case TextureFormat::R8G8B8_SNORM:
	case TextureFormat::R16G16B16_SNORM:
		return GL_RGB;
	case TextureFormat::R4G4B4A4:
	case TextureFormat::R5G5B5A1:
	case TextureFormat::R8G8B8A8:
	case TextureFormat::R10G10B10A2:
	case TextureFormat::R16G16B16A16:
	case TextureFormat::R16G16B16A16F:
	case TextureFormat::R32G32B32A32F:
	case TextureFormat::R8G8B8A8_SNORM:
	case TextureFormat::R16G16B16A16_SNORM:
		return GL_RGBA;
	case TextureFormat::SR8G8B8:
		return GL_SRGB_EXT;
	case TextureFormat::SR8G8B8A8:
		return GL_SRGB_ALPHA_EXT;
	case TextureFormat::R8:
	case TextureFormat::R16F:
	case TextureFormat::R32F:
		return GL_RED_EXT;
	case TextureFormat::RG16F:
	case TextureFormat::RG32F:
		return GL_RG_EXT;
	case TextureFormat::RGB_DXT1:
	case TextureFormat::RGBA_DXT1:
	case TextureFormat::RGBA_DXT3:
	case TextureFormat::RGBA_DXT5:
	case TextureFormat::RG_ATI2:
	default:
		GL_PLATFORM_LOG("Invalid texture format");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asType(TextureFormat format) noexcept
{
	switch (format)
	{
	case TextureFormat::STENCIL8:
		return GL_STENCIL_INDEX8;
	case TextureFormat::DEPTH_COMPONENT16:
	case TextureFormat::DEPTH_COMPONENT24:
	case TextureFormat::DEPTH_COMPONENT32:
		return GL_UNSIGNED_INT;
	case TextureFormat::DEPTH24_STENCIL8:
		return GL_UNSIGNED_INT_24_8_OES;
	case TextureFormat::DEPTH32_STENCIL8:
		GL_PLATFORM_LOG("Can't support DEPTH32_STENCIL8");
	case TextureFormat::R4G4B4A4:
		return GL_UNSIGNED_SHORT_4_4_4_4;
	case TextureFormat::R5G6B5:
		return GL_UNSIGNED_SHORT_5_6_5;
	case TextureFormat::R5G5B5A1:
		return GL_UNSIGNED_SHORT_5_6_5;
	case TextureFormat::R8:
	case TextureFormat::R8G8B8:
	case TextureFormat::R8G8B8A8:
	case TextureFormat::SR8G8B8:
	case TextureFormat::SR8G8B8A8:
	case TextureFormat::R10G10B10A2:
		return GL_UNSIGNED_BYTE;
	case TextureFormat::R16G16B16:
	case TextureFormat::R16G16B16A16:
		return GL_UNSIGNED_SHORT;
	case TextureFormat::R8G8B8_SNORM:
	case TextureFormat::R8G8B8A8_SNORM:
	case TextureFormat::R16G16B16_SNORM:
	case TextureFormat::R16G16B16A16_SNORM:
	case TextureFormat::R16G16B16F:
	case TextureFormat::R16G16B16A16F:
	case TextureFormat::R32G32B32F:
	case TextureFormat::R32G32B32A32F:
	case TextureFormat::R11G11B10F:
	case TextureFormat::R16F:
	case TextureFormat::R32F:
	case TextureFormat::RG16F:
	case TextureFormat::RG32F:
		return GL_FLOAT;
	case TextureFormat::RGB_DXT1:
	case TextureFormat::RGBA_DXT1:
	case TextureFormat::RGBA_DXT3:
	case TextureFormat::RGBA_DXT5:
	case TextureFormat::RG_ATI2:
	default:
		GL_PLATFORM_LOG("Invalid texture type");
		return GL_INVALID_ENUM;
	}
}

GLint
EGL2Types::asInternalformat(TextureFormat format) noexcept
{
	switch (format)
	{
	case TextureFormat::STENCIL8:
		return GL_STENCIL_INDEX8;
	case TextureFormat::DEPTH_COMPONENT16:
		return GL_DEPTH_COMPONENT16;
	case TextureFormat::DEPTH_COMPONENT24:
		return GL_DEPTH_COMPONENT24_OES;
	case TextureFormat::DEPTH_COMPONENT32:
		return GL_DEPTH_COMPONENT32_OES;
	case TextureFormat::DEPTH24_STENCIL8:
		return GL_DEPTH24_STENCIL8_OES;
	case TextureFormat::DEPTH32_STENCIL8:
		GL_PLATFORM_LOG("Can't support DEPTH32_STENCIL8");
		return GL_INVALID_ENUM;
	case TextureFormat::R4G4B4A4:
		return GL_RGBA4;
	case TextureFormat::R5G6B5:
		return GL_RGB565;
	case TextureFormat::R5G5B5A1:
		return GL_RGB5_A1;
	case TextureFormat::R10G10B10A2:
		return GL_RGB10_A2_EXT;
	case TextureFormat::R8G8B8_SNORM:
		GL_PLATFORM_LOG("Can't support R8G8B8_SNORM");
		return GL_INVALID_ENUM;
	case TextureFormat::R8G8B8A8_SNORM:
		return GL_RGBA8_SNORM;
	case TextureFormat::R8G8B8:
		return GL_RGB8_OES;
	case TextureFormat::R8G8B8A8:
		return GL_RGBA8_OES;
	case TextureFormat::R16G16B16:
		return GL_RGB16_EXT;
	case TextureFormat::R16G16B16A16:
		return GL_RGBA16_EXT;
	case TextureFormat::R16G16B16F:
		return GL_RGB16F_EXT;
	case TextureFormat::R32G32B32F:
		return GL_RGB32F_EXT;
	case TextureFormat::R16G16B16A16F:
		return GL_RGBA16F_EXT;
	case TextureFormat::R32G32B32A32F:
		return GL_RGBA32F_EXT;
	case TextureFormat::SR8G8B8:
		return GL_SRGB8_NV;
	case TextureFormat::SR8G8B8A8:
		return GL_SRGB8_ALPHA8_EXT;
	case TextureFormat::R8:
		return GL_R8_EXT;
	case TextureFormat::R16F:
		return GL_R16F_EXT;
	case TextureFormat::R32F:
		return GL_R32F_EXT;
	case TextureFormat::RG16F:
		return GL_RG16F_EXT;
	case TextureFormat::RG32F:
		return GL_RG32F_EXT;
	case TextureFormat::R11G11B10F:
		return GL_R11F_G11F_B10F_APPLE;		
	case TextureFormat::R16G16B16_SNORM:
		return GL_RGB16_SNORM_EXT;
	case TextureFormat::R16G16B16A16_SNORM:
		return GL_RGBA16_SNORM_EXT;
	case TextureFormat::RGB_DXT1:
		return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	case TextureFormat::RGBA_DXT1:
		return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case TextureFormat::RGBA_DXT3:
		return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case TextureFormat::RGBA_DXT5:
		return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case TextureFormat::RG_ATI2:
		GL_PLATFORM_LOG("Can't support RG_ATI2");
		return GL_INVALID_ENUM;
	default:
		GL_PLATFORM_LOG("Invalid texture internal format");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asCompareFunction(CompareFunction func) noexcept
{
	switch (func)
	{
	case CompareFunction::None:
		return GL_NONE;
	case CompareFunction::Lequal:
		return GL_LEQUAL;
	case CompareFunction::Equal:
		return GL_EQUAL;
	case CompareFunction::Greater:
		return GL_GREATER;
	case CompareFunction::Less:
		return GL_LESS;
	case CompareFunction::Gequal:
		return GL_GEQUAL;
	case CompareFunction::NotEqual:
		return GL_NOTEQUAL;
	case CompareFunction::Always:
		return GL_ALWAYS;
	case CompareFunction::Never:
		return GL_NEVER;
	default:
		GL_PLATFORM_LOG("Invalid compare function");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asBlendFactor(BlendFactor func) noexcept
{
	switch (func)
	{
	case BlendFactor::Zero:
		return GL_ZERO;
	case BlendFactor::One:
		return GL_ONE;
	case BlendFactor::DstCol:
		return GL_DST_COLOR;
	case BlendFactor::SrcColor:
		return GL_SRC_COLOR;
	case BlendFactor::SrcAlpha:
		return GL_SRC_ALPHA;
	case BlendFactor::DstAlpha:
		return GL_DST_ALPHA;
	case BlendFactor::OneMinusSrcCol:
		return GL_ONE_MINUS_SRC_COLOR;
	case BlendFactor::OneMinusDstCol:
		return GL_ONE_MINUS_DST_COLOR;
	case BlendFactor::OneMinusSrcAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::OneMinusDstAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case BlendFactor::ConstantColor:
		return GL_CONSTANT_COLOR;
	case BlendFactor::ConstantAlpha:
		return GL_CONSTANT_ALPHA;
	case BlendFactor::OneMinusConstantColor:
		return GL_CONSTANT_ALPHA;
	case BlendFactor::OneMinusConstantAlpha:
		return GL_CONSTANT_ALPHA;
	case BlendFactor::SrcAlphaSaturate:
		return GL_SRC_ALPHA_SATURATE;
	default:
		GL_PLATFORM_LOG("Invalid blend factor");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asBlendOperation(BlendOperation blendop) noexcept
{
	switch (blendop)
	{
	case BlendOperation::Add:
		return GL_FUNC_ADD;
	case BlendOperation::Subtract:
		return GL_FUNC_SUBTRACT;
	case BlendOperation::RevSubtract:
		return GL_FUNC_REVERSE_SUBTRACT;
	default:
		GL_PLATFORM_LOG("Invalid blend operation");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asCullMode(CullMode mode) noexcept
{
	switch (mode)
	{
	case CullMode::None:
		return GL_NONE;
	case CullMode::Front:
		return GL_FRONT;
	case CullMode::Back:
		return GL_BACK;
	case CullMode::FrontBack:
		return GL_FRONT_AND_BACK;
	default:
		GL_PLATFORM_LOG("Invalid cull mode");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asFillMode(FillMode mode) noexcept
{
	GL_PLATFORM_LOG("Invalid fill mode");
	return GL_INVALID_ENUM;
}

GLenum
EGL2Types::asStencilOperation(StencilOperation stencilop) noexcept
{
	switch (stencilop)
	{
	case StencilOperation::Keep:
		return GL_KEEP;
	case StencilOperation::Replace:
		return GL_REPLACE;
	case StencilOperation::Incr:
		return GL_INCR;
	case StencilOperation::Decr:
		return GL_DECR;
	case StencilOperation::Zero:
		return GL_ZERO;
	case StencilOperation::IncrWrap:
		return GL_INCR_WRAP;
	case StencilOperation::DecrWrap:
		return GL_DECR_WRAP;
	default:
		GL_PLATFORM_LOG("Invalid stencil operation");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asSamplerWrap(SamplerWrap wrap) noexcept
{
	switch (wrap)
	{
	case SamplerWrap::Repeat:
		return GL_REPEAT;
	case SamplerWrap::Mirror:
		return GL_MIRRORED_REPEAT;
	case SamplerWrap::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	default:
		GL_PLATFORM_LOG("Invalid SamplerWrap");
		return GL_INVALID_ENUM;
	}
}

GLenum
EGL2Types::asSamplerFilter(SamplerFilter filter) noexcept
{
	switch (filter)
	{
	case SamplerFilter::Nearest:
		return GL_NEAREST;
	case SamplerFilter::Linear:
		return GL_LINEAR;
	case SamplerFilter::NearestMipmapLinear:
		return GL_NEAREST_MIPMAP_LINEAR;
	case SamplerFilter::NearestMipmapNearest:
		return GL_NEAREST_MIPMAP_NEAREST;
	case SamplerFilter::LinearMipmapNearest:
		return GL_LINEAR_MIPMAP_NEAREST;
	case SamplerFilter::LinearMipmapLinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	default:
		GL_PLATFORM_LOG("Invalid SamplerFilter");
		return GL_INVALID_ENUM;
	}
}

void
EGL2Check::checkError() noexcept
{
	GLenum result = ::glGetError();
	if (GL_NO_ERROR != result)
	{
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
}

void
EGL2Check::debugOutput(const std::string& message, ...) noexcept
{
	va_list va;
	va_start(va, &message);
	vprintf((message + "\n").c_str(), va);
	va_end(va);
}

_NAME_END