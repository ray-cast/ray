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
#include <ray/renderer.h>
#include <ray/render_impl.h>

_NAME_BEGIN

__ImplementClass(Renderer)

Renderer::Renderer() noexcept
    : _enableWireframe(false)
{
    _fbconfig.redSize = 8;
    _fbconfig.greenSize = 8;
    _fbconfig.blueSize = 8;
    _fbconfig.alphaSize = 8;
    _fbconfig.bufferSize = 32;
    _fbconfig.depthSize = 24;
    _fbconfig.stencilSize = 8;
    _fbconfig.accumRedSize = 8;
    _fbconfig.accumGreenSize = 8;
    _fbconfig.accumBlueSize = 8;
    _fbconfig.accumAlphaSize = 8;
    _fbconfig.stereo = 0;
    _fbconfig.samples = 1;
    _fbconfig.doubleBuffer = 1;
    _fbconfig.sRGB = 0;

    _ctxconfig.major = 3;
    _ctxconfig.minor = 3;
    _ctxconfig.release = 0;
    _ctxconfig.robustness = 0;
    _ctxconfig.share = nullptr;
    _ctxconfig.profile = GPU_GL_CORE_PROFILE;
    _ctxconfig.forward = 0;
    _ctxconfig.multithread = false;
    _ctxconfig.hwnd = nullptr;

#if defined(__DEBUG__)
    _ctxconfig.debug = true;
#else
    _ctxconfig.debug = false;
#endif
}

Renderer::~Renderer() noexcept
{
    this->close();
}

bool
Renderer::open(RenderDevicePtr renderDevice) except
{
    return RenderImpl::instance()->open(renderDevice, _fbconfig, _ctxconfig);
}

void
Renderer::close() noexcept
{
    RenderImpl::instance()->close();
}

void
Renderer::renderBegin() noexcept
{
    RenderImpl::instance()->renderBegin();
}

void
Renderer::renderEnd() noexcept
{
    RenderImpl::instance()->renderEnd();
}

void
Renderer::setSwapInterval(SwapInterval interval) noexcept
{
    RenderImpl::instance()->setSwapInterval(interval);
}

SwapInterval
Renderer::getSwapInterval() const noexcept
{
    return RenderImpl::instance()->getSwapInterval();
}

void
Renderer::drawMesh(RenderBufferPtr buffer, const Renderable& renderable) noexcept
{
    RenderImpl::instance()->setRenderBuffer(buffer);
    if (_enableWireframe)
    {
        Renderable setting = renderable;
        setting.type = VertexType::GPU_LINE;
        RenderImpl::instance()->drawRenderBuffer(setting);
    }
    else
        RenderImpl::instance()->drawRenderBuffer(renderable);
}

void
Renderer::updateMesh(RenderBufferPtr buffer, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    RenderImpl::instance()->updateRenderBuffer(buffer);
}

void
Renderer::setWireframeMode(bool enable) noexcept
{
    _enableWireframe = enable;
}

bool
Renderer::getWireframeMode() const noexcept
{
    return _enableWireframe;
}

void
Renderer::setRenderCanvas(RenderCanvasPtr canvas) noexcept
{
    if (canvas)
        RenderImpl::instance()->setRenderCanvas(canvas);
    else
        RenderImpl::instance()->setRenderCanvas(0);
}

void
Renderer::setRenderTexture(RenderTexturePtr texture) noexcept
{
    if (texture)
        RenderImpl::instance()->setFramebuffer(texture->getFramebuffer());
    else
        RenderImpl::instance()->setFramebuffer(0);
}

void
Renderer::setRenderTexture(MultiRenderTexturePtr texture) noexcept
{
    if (texture)
        RenderImpl::instance()->setMultiFramebuffer(texture->getFramebuffer());
    else
        RenderImpl::instance()->setMultiFramebuffer(0);
}

void
Renderer::readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    assert(texture && texture->getResolveTexture());
    RenderImpl::instance()->readFramebuffer(texture->getFramebuffer(), pfd, w, h, data);
}

void
Renderer::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
    if (srcTarget)
    {
        if (destTarget)
            RenderImpl::instance()->copyFramebuffer(srcTarget->getFramebuffer(), src, destTarget->getFramebuffer(), dest);
        else
            RenderImpl::instance()->copyFramebuffer(srcTarget->getFramebuffer(), src, 0, dest);
    }
}

void
Renderer::setRenderState(RenderStatePtr state) noexcept
{
    RenderImpl::instance()->setRenderState(state);
}

void
Renderer::setBlendState(const RenderBlendState& state) noexcept
{
    RenderImpl::instance()->setBlendState(state);
}

void
Renderer::setRasterState(const RenderRasterState& state) noexcept
{
    RenderImpl::instance()->setRasterState(state);
}

void
Renderer::setDepthState(const RenderDepthState& state) noexcept
{
    RenderImpl::instance()->setDepthState(state);
}

void
Renderer::setStencilState(const RenderStencilState& state) noexcept
{
    RenderImpl::instance()->setStencilState(state);
}

void
Renderer::setShaderObject(ShaderObjectPtr shader) noexcept
{
    RenderImpl::instance()->setShaderProgram(shader->getShaderProgram());
}

void
Renderer::setShaderConstantBuffer(ShaderConstantBufferPtr args) noexcept
{
    RenderImpl::instance()->setShaderConstantBuffer(args);
}

void
Renderer::present(RenderCanvasPtr canvas) noexcept
{
    assert(canvas);
    RenderImpl::instance()->present(canvas);
}

_NAME_END