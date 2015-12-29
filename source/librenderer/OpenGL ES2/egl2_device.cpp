// +---------------------------------------------------------------------
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
#include "egl2_device.h"
#include "egl2_device_context.h"
#include "egl2_canvas.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_texture.h"
#include "egl2_framebuffer.h"
#include "egl2_layout.h"
#include "egl2_vbo.h"
#include "egl2_ibo.h"
#include "egl2_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Device, GraphicsDevice, "EGL2Device")

EGL2Device::EGL2Device() noexcept
{
}

EGL2Device::~EGL2Device() noexcept
{
	this->close();
}

bool
EGL2Device::open(WindHandle win) noexcept
{
	_glcontext = std::make_shared<EGL2DeviceContext>();
	_glcontext->setDevice(this->downcast<EGL2Device>());
	return _glcontext->open(win);
}

void
EGL2Device::close() noexcept
{
	_glcontext.reset();
}

GraphicsContextPtr
EGL2Device::createGraphicsContext(WindHandle win) noexcept
{
	auto context = std::make_shared<EGL2DeviceContext>();
	context->setDevice(this->downcast<EGL2Device>());
	context->open(win);
	return context;
}

void
EGL2Device::setGraphicsContext(GraphicsContextPtr context) noexcept
{
	if (_glcontext != context)
	{
		if (_glcontext)
			_glcontext->setActive(false);

		_glcontext = context->downcast<EGL2DeviceContext>();

		if (_glcontext)
			_glcontext->setActive(true);
	}
}

GraphicsContextPtr
EGL2Device::getGraphicsContext() const noexcept
{
	return _glcontext;
}

void
EGL2Device::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->renderBegin();
}

void
EGL2Device::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->renderEnd();
}

void
EGL2Device::setWireframeMode(bool enable) noexcept
{
	assert(_glcontext);
	_glcontext->setWireframeMode(enable);
}

bool
EGL2Device::getWireframeMode() const noexcept
{
	assert(_glcontext);
	return _glcontext->getWireframeMode();
}

void
EGL2Device::setViewport(const Viewport& viewport, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->setViewport(viewport, i);
}

const Viewport&
EGL2Device::getViewport(std::size_t i) const noexcept
{
	assert(_glcontext);
	return _glcontext->getViewport(i);
}

void
EGL2Device::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
EGL2Device::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

GraphicsLayoutPtr
EGL2Device::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<EGL2GraphicsLayout>();
	layout->setDevice(this->downcast<EGL2Device>());
	if (layout->open(desc))
		return layout;
	return nullptr;
}

void
EGL2Device::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsLayout(layout);
}

GraphicsLayoutPtr
EGL2Device::getGraphicsLayout() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsLayout();
}

GraphicsDataPtr
EGL2Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
	{
		auto data = std::make_shared<EGL2VertexBuffer>();
		data->setDevice(this->downcast<EGL2Device>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsStream::IBO)
	{
		auto data = std::make_shared<EGL2IndexBuffer>();
		data->setDevice(this->downcast<EGL2Device>());
		if (data->setup(desc))
			return data;
	}

	return nullptr;
}

bool
EGL2Device::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_glcontext);
	return _glcontext->updateBuffer(data, str, cnt);
}

void*
EGL2Device::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_glcontext);
	return _glcontext->mapBuffer(data, access);
}

void
EGL2Device::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_glcontext);
	_glcontext->unmapBuffer(data);
}

void
EGL2Device::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setIndexBufferData(data);
}

GraphicsDataPtr
EGL2Device::getIndexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getIndexBufferData();
}

void
EGL2Device::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setVertexBufferData(data);
}

GraphicsDataPtr
EGL2Device::getVertexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getVertexBufferData();
}

GraphicsTexturePtr
EGL2Device::createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL2Texture>();
	texture->setDevice(this->downcast<EGL2Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
EGL2Device::setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, slot);
}

void
EGL2Device::setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, first, count);
}

GraphicsSamplerPtr
EGL2Device::createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept
{
	assert(_glcontext);
	auto sampler = std::make_shared<EGL2Sampler>();
	sampler->setDevice(this->downcast<EGL2Device>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

void
EGL2Device::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, slot);
}

void
EGL2Device::setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, first, count);
}

GraphicsRenderTexturePtr
EGL2Device::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL2RenderTexture>();
	texture->setDevice(this->downcast<EGL2Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
EGL2Device::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTexture(target);
}

void
EGL2Device::setRenderTextureLayer(GraphicsRenderTexturePtr target, std::int32_t layer) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTextureLayer(target, layer);
}

void
EGL2Device::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil);
}

void
EGL2Device::discardRenderTexture() noexcept
{
	assert(_glcontext);
	_glcontext->discardRenderTexture();
}

void
EGL2Device::blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(_glcontext);
	_glcontext->blitRenderTexture(src, v1, dest, v2);
}

void
EGL2Device::readRenderTexture(GraphicsRenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_glcontext);
	_glcontext->readRenderTexture(source, pfd, w, h, data);
}

GraphicsRenderTexturePtr
EGL2Device::getRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getRenderTexture();
}

GraphicsMultiRenderTexturePtr
EGL2Device::createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept
{
	return nullptr;
}

void
EGL2Device::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setMultiRenderTexture(target);
}

void
EGL2Device::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil, i);
}

GraphicsMultiRenderTexturePtr
EGL2Device::getMultiRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getMultiRenderTexture();
}

GraphicsStatePtr
EGL2Device::createGraphicsState(const GraphicsStateDesc& desc) noexcept
{
	assert(_glcontext);
	auto state = std::make_shared<EGL2GraphicsState>();
	state->setDevice(this->downcast<EGL2Device>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

void
EGL2Device::setGraphicsState(GraphicsStatePtr state) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsState(state);
}

GraphicsStatePtr
EGL2Device::getGraphicsState() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsState();
}

GraphicsShaderPtr
EGL2Device::createShader(const ShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<EGL2Shader>();
	shader->setDevice(this->downcast<EGL2Device>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
EGL2Device::createShaderProgram(const ShaderObjectDesc& desc) noexcept
{
	auto program = std::make_shared<EGL2ShaderObject>();
	program->setDevice(this->downcast<EGL2Device>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

void
EGL2Device::setGraphicsProgram(GraphicsProgramPtr program) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsProgram(program);
}

GraphicsProgramPtr
EGL2Device::getGraphicsProgram() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsProgram();
}

void
EGL2Device::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable);
}
void
EGL2Device::drawRenderBuffer(const RenderIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable, first, count);
}

_NAME_END