// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_RENDER_TEXTURE_H_
#define _H_RENDER_TEXTURE_H_

#include <ray/texture.h>
#include <ray/render_state.h>

_NAME_BEGIN

enum Attachment
{
    COLOR0,
    COLOR1,
    COLOR2,
    COLOR3,
    COLOR4,
    COLOR5,
    COLOR6,
    COLOR7,
    COLOR8,
    COLOR9,
    COLOR10,
    COLOR11,
    COLOR12,
    COLOR13,
    COLOR14,
    COLOR15,
    DEPTH,
    STENCIL,
    DEPTH_STENCIL,
};

struct RenderTargetBinding
{
    Attachment location;
    RenderTargetPtr texture;
};

typedef std::vector<RenderTargetBinding> RenderTargetBindings;

class EXPORT RenderTarget
{
public:
    RenderTarget() noexcept;
    virtual ~RenderTarget() noexcept;

    void setup(std::size_t w, std::size_t h, TextureDim dim, PixelFormat format, ClearFlags flags = ClearFlags::CLEAR_ALL, const Color4& color = Color4::Black, float depth = 1.0, int stencil = 0) noexcept;
    virtual bool setup() except = 0;
    virtual void close() noexcept = 0;

    void setActive(bool active) except;
    bool getActive() const noexcept;

    void setTexMipmap(bool enable) noexcept;
    void setTexFormat(PixelFormat format) noexcept;
    void setTexFilter(TextureFilter filter) noexcept;
    void setTexWrap(TextureWrap wrap) noexcept;
    void setTexOp(TextureOp op) noexcept;
    void setTexDim(TextureDim map) noexcept;
    void setMultiSample(bool multisample) noexcept;

    TextureOp     getTexOp()   const noexcept;
    TextureFilter getTexFilter() const noexcept;
    TextureWrap   getTexWrap() const noexcept;
    TextureDim    getTexDim() const noexcept;
    PixelFormat getTexFormat()  const noexcept;

    void setWidth(std::size_t w) noexcept;
    void setHeight(std::size_t h) noexcept;
    void setDepth(std::size_t d) noexcept;
    void setSize(std::size_t w, std::size_t h, std::size_t depth = 0) noexcept;

    void setClearFlags(ClearFlags flags) noexcept;
    void setClearColor(const Color4& color) noexcept;
    void setClearDepth(float depth) noexcept;
    void setClearStencil(int stencil) noexcept;

    ClearFlags getClearFlags() const noexcept;
    Color4 getClearColor() const noexcept;
    float getClearDepth() const noexcept;
    int getClearStencil() const noexcept;

    std::size_t getWidth()   const noexcept;
    std::size_t getHeight()  const noexcept;
    std::size_t getDepth()   const noexcept;

    void setViewport(const Viewport& view) noexcept;
    const Viewport& getViewport() const noexcept;

    bool isMipmap() const noexcept;
    bool isMultiSample() const noexcept;

    TexturePtr getResolveTexture() const noexcept;
    TexturePtr getResolveDepthTexture() const noexcept;

    void setSharedDepthTexture(RenderTargetPtr target) noexcept;
    void setSharedStencilTexture(RenderTargetPtr target) noexcept;

    RenderTargetPtr getSharedDepthTexture() const noexcept;
    RenderTargetPtr getSharedStencilTexture() const noexcept;

private:
    RenderTarget(RenderTarget&) noexcept = delete;
    RenderTarget& operator=(const RenderTarget&)noexcept = delete;

private:

    std::size_t _width;
    std::size_t _height;
    std::size_t _depth;

    int        _clearStencil;
    float      _clearDepth;
    Color4     _clearColor;
    ClearFlags _clearFlags;

    Viewport _viewport;

    bool _isMipmap;
    bool _isMultiSample;

    TextureOp     _resolveTexOp;
    PixelFormat   _resolveFormat;
    TextureFilter _resolveFilter;
    TextureDim    _resolveDim;
    TextureWrap   _resolveWrap;

    TexturePtr _resolveTexture;

    Attachment _attachment;

    RenderTargetPtr _sharedDepthTexture;
    RenderTargetPtr _sharedStencilTexture;
};

class EXPORT MultiRenderTarget : public Instance<MultiRenderTarget>
{
public:
    MultiRenderTarget() noexcept;
    virtual ~MultiRenderTarget() noexcept;

    virtual bool setup() noexcept = 0;
    virtual void close() noexcept = 0;

    void attach(RenderTargetPtr texture, Attachment location) noexcept;
    void detach(RenderTargetPtr texture) noexcept;

    void setActive(bool active) except;
    bool getActive() const noexcept;

    RenderTargetBindings& getRenderTargets() noexcept;
    const RenderTargetBindings& getRenderTargets() const noexcept;

protected:
    virtual void onActivate() noexcept = 0;
    virtual void onDeactivate() noexcept = 0;

private:

    bool _active;

    RenderTargetBindings _textures;
};

_NAME_END

#endif