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
#include <ray/ogl_types.h>

_NAME_BEGIN

GLenum
OGLTypes::asOGLVertexType(VertexType type) noexcept
{
    switch (type)
    {
    case ray::GPU_POINT:
        return GL_POINTS;
    case ray::GPU_LINE:
        return GL_LINES;
    case ray::GPU_TRIANGLE:
        return GL_TRIANGLES;
    case ray::GPU_FAN:
        return GL_TRIANGLE_FAN;
    default:
        assert(false);
        return GL_TRIANGLES;
    }
}

GLenum
OGLTypes::asOGLVertexUsage(VertexUsage usage) noexcept
{
    switch (usage)
    {
    case ray::GPU_USAGE_STATIC:
        return GL_STATIC_DRAW;
    case ray::GPU_USAGE_DYNAMIC:
        return GL_DYNAMIC_DRAW;
    default:
        assert(false);
        return GL_STATIC_DRAW;
    }
}

GLenum
OGLTypes::asOGLVertexDataType(VertexDataType type) noexcept
{
    switch (type)
    {
    case ray::GPU_DATATYPE_BYTE:
        return GL_BYTE;
    case ray::GPU_DATATYPE_SHORT:
        return GL_SHORT;
    case ray::GPU_DATATYPE_INT:
        return GL_INT;
    case ray::GPU_DATATYPE_UNSIGNED_BYTE:
        return GL_UNSIGNED_BYTE;
    case ray::GPU_DATATYPE_UNSIGNED_SHORT:
        return GL_UNSIGNED_SHORT;
    case ray::GPU_DATATYPE_UNSIGNED_INT:
        return GL_UNSIGNED_INT;
    case ray::GPU_DATATYPE_UINT32:
        return GL_UNSIGNED_INT;
    case ray::GPU_DATATYPE_FLOAT:
        return GL_FLOAT;
    default:
        assert(false);
        return GL_INVALID_ENUM;
    }
}

GLenum
OGLTypes::asOGLIndexType(IndexType type) noexcept
{
    switch (type)
    {
    case ray::GPU_UINT16:
        return GL_UNSIGNED_SHORT;
    case ray::GPU_UINT32:
        return GL_UNSIGNED_INT;
    default:
        assert(false);
        return GL_UNSIGNED_SHORT;
    }
}

GLenum
OGLTypes::asOGLShaderType(ShaderType type) noexcept
{
    switch (type)
    {
    case ShaderType::ST_VERTEX:
        return GL_VERTEX_SHADER;
    case ShaderType::ST_FRAGMENT:
        return GL_FRAGMENT_SHADER;
    case ShaderType::ST_COMPUTE:
        return GL_COMPUTE_SHADER;
#if !defined(EGLAPI)
    case ShaderType::ST_GEOMETRY:
        return GL_GEOMETRY_SHADER;
    case ShaderType::ST_TESS_CONTROL:
        return GL_TESS_CONTROL_SHADER;
    case ShaderType::ST_TESS_EVALUATION:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderType::ST_CG_VERTEX:
        return GL_CG_VERTEX_SHADER_EXT;
    case ShaderType::ST_CG_FRAGMENT:
        return GL_CG_FRAGMENT_SHADER_EXT;
#endif
    default:
        assert(false);
        return GL_VERTEX_SHADER;
    }
}

GLenum
OGLTypes::asOGLAttachment(Attachment location) noexcept
{
    switch (location)
    {
    case Attachment::COLOR0:
        return GL_COLOR_ATTACHMENT0;
    case Attachment::COLOR1:
        return GL_COLOR_ATTACHMENT1;
    case Attachment::COLOR2:
        return GL_COLOR_ATTACHMENT2;
    case Attachment::COLOR3:
        return GL_COLOR_ATTACHMENT3;
    case Attachment::COLOR4:
        return GL_COLOR_ATTACHMENT4;
    case Attachment::COLOR5:
        return GL_COLOR_ATTACHMENT5;
    case Attachment::COLOR6:
        return GL_COLOR_ATTACHMENT6;
    case Attachment::COLOR7:
        return GL_COLOR_ATTACHMENT7;
    case Attachment::COLOR8:
        return GL_COLOR_ATTACHMENT8;
    case Attachment::COLOR9:
        return GL_COLOR_ATTACHMENT9;
    case Attachment::COLOR10:
        return GL_COLOR_ATTACHMENT10;
    case Attachment::COLOR11:
        return GL_COLOR_ATTACHMENT11;
    case Attachment::COLOR12:
        return GL_COLOR_ATTACHMENT12;
    case Attachment::COLOR13:
        return GL_COLOR_ATTACHMENT13;
    case Attachment::COLOR14:
        return GL_COLOR_ATTACHMENT14;
    case Attachment::COLOR15:
        return GL_COLOR_ATTACHMENT15;
    case Attachment::DEPTH:
        return GL_DEPTH_ATTACHMENT;
    case Attachment::STENCIL:
        return GL_STENCIL_ATTACHMENT;
    case Attachment::DEPTH_STENCIL:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    default:
        assert(false);
        return GL_COLOR_ATTACHMENT0;
    }
}

GLenum
OGLTypes::asOGLTarget(TextureDim target) noexcept
{
    switch (target)
    {
    case TextureDim::DIM_2D:
        return GL_TEXTURE_2D;
        break;
    case TextureDim::DIM_3D:
        return GL_TEXTURE_3D;
        break;
    case TextureDim::DIM_2D_ARRAY:
        return GL_TEXTURE_2D_ARRAY;
        break;
    case TextureDim::DIM_3D_ARRAY:
        return GL_TEXTURE_2D_ARRAY;
        break;
    case TextureDim::DIM_CUBE:
        return GL_TEXTURE_CUBE_MAP;
        break;
    default:
        break;
    }

    assert(false);
    return (GLenum)GL_INVALID_ENUM;
}

GLenum
OGLTypes::asOGLFormat(PixelFormat format) noexcept
{
    if (format == PixelFormat::LUMINANCE)
        return GL_LUMINANCE;

    if (format == PixelFormat::LUMINANCE_ALPHA)
        return GL_LUMINANCE_ALPHA;

    if (format == PixelFormat::DEPTH_COMPONENT16 || format == PixelFormat::DEPTH_COMPONENT24 || format == PixelFormat::DEPTH_COMPONENT32)
        return GL_DEPTH_COMPONENT;

    if (format == PixelFormat::DEPTH24_STENCIL8 || format == PixelFormat::DEPTH32_STENCIL8)
        return GL_DEPTH_STENCIL;

    if (format == PixelFormat::STENCIL8)
        return GL_STENCIL_INDEX;

    if (format == PixelFormat::R8G8B8 || format == PixelFormat::R16G16B16F || format == PixelFormat::R32G32B32F)
        return GL_RGB;

    if (format == PixelFormat::R8G8B8A8 || format == PixelFormat::R4G4B4A4F || format == PixelFormat::R16G16B16A16F || format == PixelFormat::R32G32B32A32F)
        return GL_RGBA;

    if (format == PixelFormat::SR8G8B8 || format == PixelFormat::SRGB)
        return GL_RGB;

    if (format == PixelFormat::SR8G8B8A8 || format == PixelFormat::SRGBA)
        return GL_RGBA;

    if (format == PixelFormat::R8 || format == PixelFormat::R16F || format == PixelFormat::R32F)
        return GL_RED;

    if (format == PixelFormat::RG16F || format == PixelFormat::RG32F)
        return GL_RG;

    if (format == PixelFormat::DXT1)
        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

    if (format == PixelFormat::DXT1)
        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;

    if (format == PixelFormat::DXT1)
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    assert(false);
    return GL_INVALID_ENUM;
}

GLenum
OGLTypes::asOGLType(PixelFormat format) noexcept
{
    if (format == PixelFormat::LUMINANCE || format == PixelFormat::LUMINANCE_ALPHA)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::DEPTH24_STENCIL8)
        return GL_UNSIGNED_INT_24_8;

    if (format == PixelFormat::DEPTH32_STENCIL8)
        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

    if (format == PixelFormat::STENCIL8)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::R8G8B8F)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::R16G16B16F || format == PixelFormat::R32G32B32F)
        return GL_FLOAT;

    if (format == PixelFormat::R16G16B16A16F || format == PixelFormat::R32G32B32A32F)
        return GL_FLOAT;

    if (format == PixelFormat::DEPTH_COMPONENT16 || format == PixelFormat::DEPTH_COMPONENT24 || format == PixelFormat::DEPTH_COMPONENT32)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::R8G8B8 || format == PixelFormat::R8G8B8A8)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::SR8G8B8 || format == PixelFormat::SRGB)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::SR8G8B8A8 || format == PixelFormat::SRGBA)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::R8)
        return GL_UNSIGNED_BYTE;

    if (format == PixelFormat::R16F || format == PixelFormat::R32F)
        return GL_FLOAT;

    if (format == PixelFormat::RG16F || format == PixelFormat::RG32F)
        return GL_FLOAT;

    assert(false);
    return GL_INVALID_ENUM;
}

GLint
OGLTypes::asOGLInternalformat(PixelFormat format) noexcept
{
    switch (format)
    {
    case PixelFormat::STENCIL8:
        return GL_STENCIL_INDEX8;
    case PixelFormat::DEPTH_COMPONENT16:
        return GL_DEPTH_COMPONENT16;
    case PixelFormat::DEPTH_COMPONENT24:
        return GL_DEPTH_COMPONENT24;
    case PixelFormat::DEPTH_COMPONENT32:
        return GL_DEPTH_COMPONENT32F;
    case PixelFormat::DEPTH24_STENCIL8:
        return GL_DEPTH24_STENCIL8;
    case PixelFormat::DEPTH32_STENCIL8:
        return GL_DEPTH32F_STENCIL8;
    case PixelFormat::LUMINANCE:
        return GL_LUMINANCE;
    case PixelFormat::LUMINANCE_ALPHA:
        return GL_LUMINANCE_ALPHA;
    case PixelFormat::R8G8B8:
        return GL_RGB;
    case PixelFormat::R8G8B8A8:
        return GL_RGBA;
    case PixelFormat::R8G8B8F:
        return GL_RGB8;
    case PixelFormat::R16G16B16F:
        return GL_RGB16F;
    case PixelFormat::R32G32B32F:
        return GL_RGB32F;
    case PixelFormat::R4G4B4A4F:
        return GL_RGBA4;
    case PixelFormat::R8G8B8A8F:
        return GL_RGBA8;
    case PixelFormat::R16G16B16A16F:
        return GL_RGBA16F;
    case PixelFormat::R32G32B32A32F:
        return GL_RGBA32F;
    case PixelFormat::SR8G8B8:
        return GL_SRGB8;
    case PixelFormat::SR8G8B8A8:
        return GL_SRGB8_ALPHA8;
    case PixelFormat::SRGB:
        return GL_SRGB;
    case PixelFormat::SRGBA:
        return GL_SRGB_ALPHA;
    case PixelFormat::R8:
        return GL_RED;
    case PixelFormat::R16F:
        return GL_R16F;
    case PixelFormat::R32F:
        return GL_R32F;
    case PixelFormat::RG16F:
        return GL_RG16F;
    case PixelFormat::RG32F:
        return GL_RG32F;
    default:
        assert(false);
        return GL_RGBA;
    }
}

GLenum
OGLTypes::asCompareFunction(CompareFunction func) noexcept
{
    switch (func)
    {
    case ray::GPU_NONE:
        return GL_NONE;
    case ray::GPU_LEQUAL:
        return GL_LEQUAL;
    case ray::GPU_EQUAL:
        return GL_EQUAL;
    case ray::GPU_GREATER:
        return GL_GREATER;
    case ray::GPU_LESS:
        return GL_LESS;
    case ray::GPU_GEQUAL:
        return GL_GEQUAL;
    case ray::GPU_NOTEQUAL:
        return GL_NOTEQUAL;
    case ray::GPU_ALWAYS:
        return GL_ALWAYS;
    case ray::GPU_NEVER:
        return GL_NEVER;
    default:
        assert(false);
        return GL_ALWAYS;
    }
}

GLenum
OGLTypes::asBlendFactor(BlendFactor func) noexcept
{
    switch (func)
    {
    case ray::GPU_ZERO:
        return GL_ZERO;
    case ray::GPU_ONE:
        return GL_ONE;
    case ray::GPU_DSTCOL:
        return GL_DST_COLOR;
    case ray::GPU_SRCCOLOR:
        return GL_SRC_COLOR;
    case ray::GPU_SRCALPHA:
        return GL_SRC_ALPHA;
    case ray::GPU_DSTALPHA:
        return GL_DST_ALPHA;
    case ray::GPU_ONEMINUSSRCCOL:
        return GL_ONE_MINUS_SRC_COLOR;
    case ray::GPU_ONEMINUSDSTCOL:
        return GL_ONE_MINUS_DST_COLOR;
    case ray::GPU_ONEMINUSSRCALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;
    case ray::GPU_ONEMINUSDSTALPHA:
        return GL_ONE_MINUS_DST_ALPHA;
    default:
        assert(false);
        return GL_ZERO;
    }
}

GLenum
OGLTypes::asBlendOperation(BlendOperation blendop) noexcept
{
#if !defined(EGLAPI)
    switch (blendop)
    {
    case ray::GPU_ADD:
        return GL_ADD;
    case ray::GPU_SUBSTRACT:
        return GL_SUBTRACT;
    case ray::GPU_REVSUBTRACT:
        return GL_FUNC_REVERSE_SUBTRACT;
    default:
        assert(false);
        return GL_ADD;
    }
#else
    assert(false);
    return GL_INVALID_ENUM;
#endif
}

GLenum
OGLTypes::asCullMode(CullMode mode) noexcept
{
    switch (mode)
    {
    case ray::GPU_CULL_NONE:
        return GL_NONE;
    case ray::GPU_CULL_FRONT:
        return GL_FRONT;
    case ray::GPU_CULL_BACK:
        return GL_BACK;
    case ray::GPU_CULL_FRONT_BACK:
        return GL_FRONT_AND_BACK;
    default:
        assert(false);
        return GL_NONE;
    }
}

GLenum
OGLTypes::asFillMode(FillMode mode) noexcept
{
#if !defined(EGLAPI)
    switch (mode)
    {
    case ray::GPU_POINT_MODE:
        return GL_POINT;
    case ray::GPU_WIREFRAME_MODE:
        return GL_LINE;
    case ray::GPU_SOLID_MODE:
        return GL_FILL;
    default:
        assert(false);
        return GL_FILL;
    }
#else
    assert(false);
    return GL_INVALID_ENUM;
#endif
}

GLenum
OGLTypes::asStencilOperation(StencilOperation stencilop) noexcept
{
    switch (stencilop)
    {
    case ray::STENCILOP_KEEP:
        return GL_KEEP;
    case ray::STENCILOP_REPLACE:
        return GL_REPLACE;
    case ray::STENCILOP_INCR:
        return GL_INCR;
    case ray::STENCILOP_DECR:
        return GL_DECR;
    case ray::STENCILOP_ZERO:
        return GL_ZERO;
    case ray::STENCILOP_INCR_WRAP:
        return GL_INCR_WRAP;
    case ray::STENCILOP_DECR_WRAP:
        return GL_DECR_WRAP;
    default:
        assert(false);
        return GL_KEEP;
    }
}

_NAME_END