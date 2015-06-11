// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderDevice software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderDevice list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderDevice list of conditions and the
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
#include <ray/render_impl.h>

_NAME_BEGIN

__ImplementSingleton(RenderImpl)

RenderImpl::RenderImpl() noexcept
    : _renderDevice(nullptr)
{
}

RenderImpl::~RenderImpl() noexcept
{
    this->close();
}

bool
RenderImpl::open(RenderDevicePtr renderDevice, const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) except
{
    assert(renderDevice);
    assert(!_renderDevice);

    if (!renderDevice->open(fbconfig, ctxconfig))
        return false;

    _renderDevice = renderDevice;

    return true;
}

void
RenderImpl::close() noexcept
{
    if (_renderDevice)
    {
        _renderDevice.reset();
        _renderDevice = nullptr;
    }
}

void
RenderImpl::setSwapInterval(SwapInterval interval) noexcept
{
    _renderDevice->setSwapInterval(interval);
}

SwapInterval
RenderImpl::getSwapInterval() const noexcept
{
    return _renderDevice->getSwapInterval();
}

void
RenderImpl::setBlendState(const RenderBlendState& state) noexcept
{
    _renderDevice->setBlendState(state);
}

void
RenderImpl::setRasterState(const RenderRasterState& state) noexcept
{
    _renderDevice->setRasterState(state);
}

void
RenderImpl::setDepthState(const RenderDepthState& state) noexcept
{
    _renderDevice->setDepthState(state);
}

void
RenderImpl::setStencilState(const RenderStencilState& state) noexcept
{
    _renderDevice->setStencilState(state);
}

void
RenderImpl::setRenderState(RenderStatePtr state) noexcept
{
    _renderDevice->setRenderState(state);
}

void
RenderImpl::renderBegin() noexcept
{
    _renderDevice->renderBegin();
}

void
RenderImpl::renderEnd() noexcept
{
    _renderDevice->renderEnd();
}

void
RenderImpl::present(RenderCanvasPtr canvas) noexcept
{
    _renderDevice->present(canvas);
}

RenderCanvasPtr
RenderImpl::createRenderCanvas(WindHandle hwnd) noexcept
{
    return _renderDevice->createRenderCanvas(hwnd);
}

void
RenderImpl::destroyRenderCanvas(RenderCanvasPtr canvas) noexcept
{
    _renderDevice->destroyRenderCanvas(canvas);
}

void
RenderImpl::setRenderCanvas(const RenderCanvasPtr canvas) noexcept
{
    _renderDevice->setRenderCanvas(canvas);
}

bool
RenderImpl::createConstantBuffer(ShaderConstantBuffer& buffer) noexcept
{
    return _renderDevice->createConstantBuffer(buffer);
}

void
RenderImpl::setShaderConstantBuffer(ShaderConstantBufferPtr buffer) noexcept
{
    _renderDevice->setShaderConstantBuffer(buffer);
}

void
RenderImpl::destroyConstantBuffer(ShaderConstantBuffer& buffer) noexcept
{
    _renderDevice->destroyConstantBuffer(buffer);
}

bool
RenderImpl::createRenderBuffer(RenderBuffer& buffer) noexcept
{
    return _renderDevice->createRenderBuffer(buffer);
}

void
RenderImpl::destroyRenderBuffer(RenderBuffer& buffer) noexcept
{
    _renderDevice->destroyRenderBuffer(buffer);
}

void
RenderImpl::setRenderBuffer(const RenderBufferPtr buffer) noexcept
{
    _renderDevice->setRenderBuffer(buffer);
}

void
RenderImpl::updateRenderBuffer(RenderBufferPtr buffer) noexcept
{
    _renderDevice->updateRenderBuffer(buffer);
}

void
RenderImpl::drawRenderBuffer(const Renderable& renderable) noexcept
{
    _renderDevice->drawRenderBuffer(renderable);
}

bool
RenderImpl::createRenderTexture(RenderTexture& target) noexcept
{
    return _renderDevice->createRenderTexture(target);
}

void
RenderImpl::destroyRenderTexture(RenderTexture& target) noexcept
{
    _renderDevice->destroyRenderTexture(target);
}

void
RenderImpl::setRenderTexture(RenderTexturePtr rt) noexcept
{
    _renderDevice->setRenderTexture(rt);
}

void
RenderImpl::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
    _renderDevice->copyRenderTexture(srcTarget, src, destTarget, dest);
}

void
RenderImpl::readRenderTexture(RenderTexturePtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    _renderDevice->readRenderTexture(target, pfd, w, h, data);
}

bool
RenderImpl::createMultiRenderTexture(MultiRenderTexture& desc) noexcept
{
    return _renderDevice->createMultiRenderTexture(desc);
}

void
RenderImpl::destroyMultiRenderTexture(MultiRenderTexture& target) noexcept
{
    _renderDevice->destroyMultiRenderTexture(target);
}

void
RenderImpl::setMultiRenderTexture(MultiRenderTexturePtr mrt) noexcept
{
    _renderDevice->setMultiRenderTexture(mrt);
}

bool
RenderImpl::createTexture(Texture& texture) noexcept
{
    return _renderDevice->createTexture(texture);
}

void
RenderImpl::destroyTexture(Texture& texture) noexcept
{
    _renderDevice->destroyTexture(texture);
}

ShaderProgramPtr
RenderImpl::createShaderProgram(std::vector<ShaderPtr>& shaders) noexcept
{
    return _renderDevice->createShaderProgram(shaders);
}

void
RenderImpl::destroyShaderProgram(ShaderProgramPtr shader) noexcept
{
    _renderDevice->destroyShaderProgram(shader);
}

void
RenderImpl::setShaderProgram(ShaderProgramPtr program) noexcept
{
    _renderDevice->setShaderProgram(program);
}

_NAME_END