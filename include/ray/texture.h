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
#ifndef _H_TEXTURE_H_
#define _H_TEXTURE_H_

#include <ray/render_types.h>

_NAME_BEGIN

enum Anisotropy
{
    ANISOTROPY_0,
    ANISOTROPY_1,
    ANISOTROPY_2,
    ANISOTROPY_4,
    ANISOTROPY_8,
    ANISOTROPY_16,
};

enum TextureOp
{
    OP_MULTIPLY,    //* T = T1 * T2
    OP_ADD,         //* T = T1 + T2
    OP_SUBTRACT,    //* T = T1 - T2
    OP_DIVIDE,      //* T = T1 / T2
    OP_SMOOTHADD,   //* T = (T1 + T2) - (T1 * T2)
    OP_SIGNEDADD,   //* T = T1 + (T2-0.5)
};

enum PixelFormat
{
    STENCIL8,
    DEPTH_COMPONENT16,
    DEPTH_COMPONENT24,
    DEPTH_COMPONENT32,
    DEPTH24_STENCIL8,
    DEPTH32_STENCIL8,
    LUMINANCE,
    LUMINANCE_ALPHA,
    R8G8B8,
    R8G8B8A8,
    R8G8B8F,
    R16G16B16F,
    R32G32B32F,
    R4G4B4A4F,
    R8G8B8A8F,
    R16G16B16A16F,
    R32G32B32A32F,
    R11G11B10F,
    SRGB,
    SRGBA,
    SR8G8B8,
    SR8G8B8A8,
    R8,
    R16F,
    R32F,
    RG16F,
    RG32F,
    DXT1,
    DXT3,
    DXT5,
};

enum TextureWrap
{
    REPEAT,
    CLAMP_TO_EDGE,
    MODE_MIRROR,
};

enum TextureFilter
{
    GPU_NEAREST,
    GPU_LINEAR,
    GPU_NEAREST_MIPMAP_LINEAR,
    GPU_NEAREST_MIPMAP_NEAREST,
    GPU_LINEAR_MIPMAP_NEAREST,
    GPU_LINEAR_MIPMAP_LINEAR,
};

enum TextureDim
{
    DIM_2D,
    DIM_3D,
    DIM_2D_ARRAY,
    DIM_3D_ARRAY,
    DIM_CUBE,
    DIM_CUBE_ARRAY,
};

class EXPORT Texture
{
public:
    Texture() noexcept;
    virtual ~Texture() noexcept;

    virtual bool setup() except = 0;
    virtual void close() noexcept = 0;

    void setTexMipmap(bool enable) noexcept;
    void setTexFormat(PixelFormat format) noexcept;
    void setTexOp(TextureOp op) noexcept;
    void setTexWrap(TextureWrap wrap) noexcept;
    void setTexFilter(TextureFilter filter) noexcept;
    void setTexDim(TextureDim mapping) noexcept;
    void setLevel(int level) noexcept;
    void setAnisotropy(Anisotropy anis) noexcept;
    void setWidth(int w) noexcept;
    void setHeight(int h) noexcept;
    void setDepth(int d) noexcept;
    void setStream(void* data) noexcept;
    void setSize(int w, int h, int depth = 0) noexcept;

    TextureOp     getTexOp()   const noexcept;
    PixelFormat   getTexFormat()  const noexcept;
    TextureDim    getTexDim() const noexcept;
    TextureWrap   getTexWrap() const noexcept;
    TextureFilter getTexFilter() const noexcept;
    Anisotropy    getTexAnisotropy() const noexcept;

    int getWidth()   const noexcept;
    int getHeight()  const noexcept;
    int getDepth()   const noexcept;
    const int3& getSize()  const noexcept;

    int getLevel() const noexcept;

    void* getStream() const noexcept;

    bool isMipmap() const noexcept;
    bool isMultiSample() const noexcept;
    bool isCompressed() const noexcept;

    void copy(Texture* other) noexcept;
    void copy(TexturePtr other) noexcept;

private:
    Texture(const Texture&) noexcept = delete;
    const Texture& operator=(const Texture&)noexcept = delete;

private:

    bool _mipmap;
    bool _multisample;

    int3 _size;
    int _level;

    PixelFormat _format;
    TextureDim _dim;
    TextureOp _texop;
    TextureFilter _filter;
    TextureWrap _wrap;
    Anisotropy _anis;

    void* _data;
};

_NAME_END

#endif