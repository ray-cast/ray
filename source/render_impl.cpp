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
RenderImpl::clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil) noexcept
{
    _renderDevice->clear(flags, color, depth, stencil);
}

void
RenderImpl::setViewport(const Viewport& view) noexcept
{
    _renderDevice->setViewport(view);
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

RenderBufferPtr
RenderImpl::createRenderBuffer(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    return _renderDevice->createRenderBuffer(vb, ib);
}

void
RenderImpl::destroyRenderBuffer(RenderBufferPtr buffer) noexcept
{
    _renderDevice->destroyRenderBuffer(buffer);
}

void
RenderImpl::setRenderBuffer(const RenderBufferPtr buffer) noexcept
{
    _renderDevice->setRenderBuffer(buffer);
}

void
RenderImpl::updateRenderBuffer(RenderBufferPtr buffer, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    _renderDevice->updateRenderBuffer(buffer, vb, ib);
}

void
RenderImpl::drawRenderBuffer(const Renderable& renderable) noexcept
{
    _renderDevice->drawRenderBuffer(renderable);
}

FramebufferPtr
RenderImpl::createFramebuffer(const FramebufferDesc& desc) noexcept
{
    return _renderDevice->createFramebuffer(desc);
}

void
RenderImpl::destroyFramebuffer(FramebufferPtr target) noexcept
{
    _renderDevice->destroyFramebuffer(target);
}

void
RenderImpl::setFramebuffer(FramebufferPtr rt) noexcept
{
    _renderDevice->setFramebuffer(rt);
}

void
RenderImpl::copyFramebuffer(FramebufferPtr srcTarget, const Viewport& src, FramebufferPtr destTarget, const Viewport& dest) noexcept
{
    _renderDevice->copyFramebuffer(srcTarget, src, destTarget, dest);
}

void
RenderImpl::readFramebuffer(FramebufferPtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    _renderDevice->readFramebuffer(target, pfd, w, h, data);
}

MultiFramebufferPtr
RenderImpl::createMultiFramebuffer(const MultiFramebufferDesc& desc) noexcept
{
    return _renderDevice->createMultiFramebuffer(desc);
}

void
RenderImpl::destroyMultiFramebuffer(MultiFramebufferPtr target) noexcept
{
    _renderDevice->destroyMultiFramebuffer(target);
}

void
RenderImpl::setMultiFramebuffer(MultiFramebufferPtr mrt) noexcept
{
    _renderDevice->setMultiFramebuffer(mrt);
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

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const int value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const int2& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const int3& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const int4& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const uint value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const uint2& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const uint3& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const uint4& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float2& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float3& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float4& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float2x2& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float3x3& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const float4x4& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const Texture& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

void
RenderImpl::setShaderConstant(ShaderUniformPtr uniform, const std::vector<float>& value) noexcept
{
    _renderDevice->setShaderConstant(uniform, value);
}

_NAME_END