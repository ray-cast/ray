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
#include "egl3_device.h"
#include "egl3_device_context.h"
#include "egl3_canvas.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_framebuffer.h"
#include "egl3_layout.h"
#include "egl3_vbo.h"
#include "egl3_ibo.h"
#include "egl3_dibo.h"
#include "egl3_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Device, GraphicsDevice, "EGL3Device")

EGL3Device::EGL3Device() noexcept
{
}

EGL3Device::~EGL3Device() noexcept
{
	this->close();
}

bool 
EGL3Device::open(WindHandle win) noexcept
{
	_glcontext = std::make_shared<EGL3DeviceContext>();
	_glcontext->setDevice(this->downcast<EGL3Device>());
	return _glcontext->open(win);
}

void 
EGL3Device::close() noexcept
{
	_glcontext.reset();
}

GraphicsContextPtr
EGL3Device::createGraphicsContext(WindHandle win) noexcept
{
	auto context = std::make_shared<EGL3DeviceContext>();
	context->setDevice(this->downcast<EGL3Device>());
	context->open(win);
	return context;
}

void
EGL3Device::setGraphicsContext(GraphicsContextPtr context) noexcept
{
	if (_glcontext != context)
	{
		if (_glcontext)
			_glcontext->setActive(false);

		_glcontext = context->downcast<EGL3DeviceContext>();

		if (_glcontext)
			_glcontext->setActive(true);
	}
}

GraphicsContextPtr
EGL3Device::getGraphicsContext() const noexcept
{
	return _glcontext;
}

void 
EGL3Device::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->renderBegin();
}

void 
EGL3Device::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->renderEnd();
}

void
EGL3Device::setWireframeMode(bool enable) noexcept
{
	assert(_glcontext);
	_glcontext->setWireframeMode(enable);
}

bool
EGL3Device::getWireframeMode() const noexcept
{
	assert(_glcontext);
	return _glcontext->getWireframeMode();
}

void
EGL3Device::setViewport(const Viewport& viewport, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->setViewport(viewport, i);
}

const Viewport&
EGL3Device::getViewport(std::size_t i) const noexcept
{
	assert(_glcontext);
	return _glcontext->getViewport(i);
}

void
EGL3Device::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
EGL3Device::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

GraphicsLayoutPtr
EGL3Device::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<EGL3GraphicsLayout>();
	layout->setDevice(this->downcast<EGL3Device>());
	if (layout->open(desc))
		return layout;
	return nullptr;
}

void
EGL3Device::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsLayout(layout);
}

GraphicsLayoutPtr
EGL3Device::getGraphicsLayout() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsLayout();
}

GraphicsDataPtr
EGL3Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
	{
		auto data = std::make_shared<EGL3VertexBuffer>();
		data->setDevice(this->downcast<EGL3Device>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsStream::IBO)
	{
		auto data = std::make_shared<EGL3IndexBuffer>();
		data->setDevice(this->downcast<EGL3Device>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsStream::DIBO)
	{
		auto data = std::make_shared<EGL3DrawIndirectBuffer>();
		data->setDevice(this->downcast<EGL3Device>());
		if (data->setup(desc))
			return data;
	}

	return nullptr;
}

bool
EGL3Device::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_glcontext);
	return _glcontext->updateBuffer(data, str, cnt);
}

void*
EGL3Device::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_glcontext);
	return _glcontext->mapBuffer(data, access);
}

void
EGL3Device::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_glcontext);
	_glcontext->unmapBuffer(data);
}

void
EGL3Device::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setIndexBufferData(data);
}

GraphicsDataPtr
EGL3Device::getIndexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getIndexBufferData();
}

void
EGL3Device::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setVertexBufferData(data);
}

GraphicsDataPtr
EGL3Device::getVertexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getVertexBufferData();
}

GraphicsTexturePtr
EGL3Device::createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3Texture>();
	texture->setDevice(this->downcast<EGL3Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
EGL3Device::setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, slot);
}

void
EGL3Device::setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, first, count);
}

GraphicsSamplerPtr
EGL3Device::createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept
{
	assert(_glcontext);
	auto sampler = std::make_shared<EGL3Sampler>();
	sampler->setDevice(this->downcast<EGL3Device>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

void
EGL3Device::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, slot);
}

void
EGL3Device::setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, first, count);
}

GraphicsRenderTexturePtr
EGL3Device::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3RenderTexture>();
	texture->setDevice(this->downcast<EGL3Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
EGL3Device::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTexture(target);
}

void
EGL3Device::setRenderTextureLayer(GraphicsRenderTexturePtr target, std::int32_t layer) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTextureLayer(target, layer);
}

void
EGL3Device::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil);
}

void
EGL3Device::discardRenderTexture() noexcept
{
	assert(_glcontext);
	_glcontext->discardRenderTexture();
}

void
EGL3Device::blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(_glcontext);
	_glcontext->blitRenderTexture(src, v1, dest, v2);
}

void
EGL3Device::readRenderTexture(GraphicsRenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_glcontext);
	_glcontext->readRenderTexture(source, pfd, w, h, data);
}

GraphicsRenderTexturePtr
EGL3Device::getRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getRenderTexture();
}

GraphicsMultiRenderTexturePtr
EGL3Device::createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3MultiRenderTexture>();
	texture->setDevice(this->downcast<EGL3Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
EGL3Device::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setMultiRenderTexture(target);
}

void
EGL3Device::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil, i);
}

GraphicsMultiRenderTexturePtr
EGL3Device::getMultiRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getMultiRenderTexture();
}

GraphicsStatePtr
EGL3Device::createGraphicsState(const GraphicsStateDesc& desc) noexcept
{
	assert(_glcontext);
	auto state = std::make_shared<EGL3GraphicsState>();
	state->setDevice(this->downcast<EGL3Device>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

void
EGL3Device::setGraphicsState(GraphicsStatePtr state) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsState(state);
}

GraphicsStatePtr
EGL3Device::getGraphicsState() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsState();
}

GraphicsShaderPtr
EGL3Device::createShader(const ShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<EGL3Shader>();
	shader->setDevice(this->downcast<EGL3Device>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
EGL3Device::createShaderProgram(const ShaderObjectDesc& desc) noexcept
{
	auto program = std::make_shared<EGL3ShaderObject>();
	program->setDevice(this->downcast<EGL3Device>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

void
EGL3Device::setGraphicsProgram(GraphicsProgramPtr program) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsProgram(program);
}

GraphicsProgramPtr
EGL3Device::getGraphicsProgram() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsProgram();
}

void
EGL3Device::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable);
}
void
EGL3Device::drawRenderBuffer(const RenderIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable, first, count);
}

_NAME_END