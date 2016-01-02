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
#include "ogl_device.h"
#include "ogl_device_context.h"
#include "ogl_canvas.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_layout.h"
#include "ogl_vbo.h"
#include "ogl_ibo.h"
#include "ogl_dibo.h"
#include "ogl_sampler.h"

_NAME_BEGIN

__ImplementSubClass(OGLDevice, GraphicsDevice, "OGLDevice")

OGLDevice::OGLDevice() noexcept
{
}

OGLDevice::~OGLDevice() noexcept
{
	this->close();
}

bool
OGLDevice::open(WindHandle win) noexcept
{
	auto context = createGraphicsContext(win);
	if (context)
	{
		_glcontext = context->downcast<OGLDeviceContext>();
		_glcontext->setSwapInterval(SwapInterval::Vsync);
		return true;
	}

	return false;
}

void
OGLDevice::close() noexcept
{
	_glcontext.reset();
}

GraphicsContextPtr
OGLDevice::createGraphicsContext(WindHandle win) noexcept
{
	auto context = std::make_shared<OGLDeviceContext>();
	context->setDevice(this->downcast<OGLDevice>());
	if (context->open(win))
		return context;
	return false;
}

void
OGLDevice::setGraphicsContext(GraphicsContextPtr context) noexcept
{
	if (_glcontext != context)
	{
		if (_glcontext)
			_glcontext->setActive(false);

		_glcontext = context->downcast<OGLDeviceContext>();

		if (_glcontext)
			_glcontext->setActive(true);
	}
}

GraphicsContextPtr
OGLDevice::getGraphicsContext() const noexcept
{
	return _glcontext;
}

void
OGLDevice::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->renderBegin();
}

void
OGLDevice::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->renderEnd();
}

void
OGLDevice::setWireframeMode(bool enable) noexcept
{
	assert(_glcontext);
	_glcontext->setWireframeMode(enable);
}

bool
OGLDevice::getWireframeMode() const noexcept
{
	assert(_glcontext);
	return _glcontext->getWireframeMode();
}

void
OGLDevice::setViewport(const Viewport& viewport, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->setViewport(viewport, i);
}

const Viewport&
OGLDevice::getViewport(std::size_t i) const noexcept
{
	assert(_glcontext);
	return _glcontext->getViewport(i);
}

void
OGLDevice::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
OGLDevice::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

GraphicsLayoutPtr
OGLDevice::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<OGLGraphicsLayout>();
	layout->setDevice(this->downcast<OGLDevice>());
	if (layout->open(desc))
		return layout;
	return nullptr;
}

void
OGLDevice::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsLayout(layout);
}

GraphicsLayoutPtr
OGLDevice::getGraphicsLayout() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsLayout();
}

GraphicsDataPtr
OGLDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
	{
		auto data = std::make_shared<OGLVertexBuffer>();
		data->setDevice(this->downcast<OGLDevice>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsStream::IBO)
	{
		auto data = std::make_shared<OGLIndexBuffer>();
		data->setDevice(this->downcast<OGLDevice>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsStream::DIBO)
	{
		auto data = std::make_shared<OGLDrawIndirectBuffer>();
		data->setDevice(this->downcast<OGLDevice>());
		if (data->setup(desc))
			return data;
	}

	return nullptr;
}

bool
OGLDevice::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_glcontext);
	return _glcontext->updateBuffer(data, str, cnt);
}

void*
OGLDevice::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_glcontext);
	return _glcontext->mapBuffer(data, access);
}

void
OGLDevice::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_glcontext);
	_glcontext->unmapBuffer(data);
}

void
OGLDevice::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setIndexBufferData(data);
}

GraphicsDataPtr
OGLDevice::getIndexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getIndexBufferData();
}

void
OGLDevice::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(_glcontext);
	_glcontext->setVertexBufferData(data);
}

GraphicsDataPtr
OGLDevice::getVertexBufferData() const noexcept
{
	assert(_glcontext);
	return _glcontext->getVertexBufferData();
}

GraphicsTexturePtr
OGLDevice::createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<OGLTexture>();
	texture->setDevice(this->downcast<OGLDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
OGLDevice::setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, slot);
}

void
OGLDevice::setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsTexture(texture, first, count);
}

GraphicsSamplerPtr
OGLDevice::createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept
{
	assert(_glcontext);
	auto sampler = std::make_shared<OGLSampler>();
	sampler->setDevice(this->downcast<OGLDevice>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

void
OGLDevice::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, slot);
}

void
OGLDevice::setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsSampler(sampler, first, count);
}

GraphicsRenderTexturePtr
OGLDevice::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<OGLRenderTexture>();
	texture->setDevice(this->downcast<OGLDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
OGLDevice::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTexture(target);
}

void
OGLDevice::setRenderTextureLayer(GraphicsRenderTexturePtr target, std::int32_t layer) noexcept
{
	assert(_glcontext);
	_glcontext->setRenderTextureLayer(target, layer);
}

void
OGLDevice::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil);
}

void
OGLDevice::discardRenderTexture() noexcept
{
	assert(_glcontext);
	_glcontext->discardRenderTexture();
}

void
OGLDevice::blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(_glcontext);
	_glcontext->blitRenderTexture(src, v1, dest, v2);
}

void
OGLDevice::readRenderTexture(GraphicsRenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_glcontext);
	_glcontext->readRenderTexture(source, pfd, w, h, data);
}

GraphicsRenderTexturePtr
OGLDevice::getRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getRenderTexture();
}

GraphicsMultiRenderTexturePtr
OGLDevice::createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<OGLMultiRenderTexture>();
	texture->setDevice(this->downcast<OGLDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

void
OGLDevice::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	assert(_glcontext);
	_glcontext->setMultiRenderTexture(target);
}

void
OGLDevice::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_glcontext);
	_glcontext->clearRenderTexture(flags, color, depth, stencil, i);
}

GraphicsMultiRenderTexturePtr
OGLDevice::getMultiRenderTexture() const noexcept
{
	assert(_glcontext);
	return _glcontext->getMultiRenderTexture();
}

GraphicsStatePtr
OGLDevice::createGraphicsState(const GraphicsStateDesc& desc) noexcept
{
	assert(_glcontext);
	auto state = std::make_shared<OGLGraphicsState>();
	state->setDevice(this->downcast<OGLDevice>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

void
OGLDevice::setGraphicsState(GraphicsStatePtr state) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsState(state);
}

GraphicsStatePtr
OGLDevice::getGraphicsState() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsState();
}

GraphicsShaderPtr
OGLDevice::createShader(const ShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<OGLShader>();
	shader->setDevice(this->downcast<OGLDevice>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
OGLDevice::createShaderProgram(const ShaderObjectDesc& desc) noexcept
{
	auto program = std::make_shared<OGLShaderObject>();
	program->setDevice(this->downcast<OGLDevice>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

void
OGLDevice::setGraphicsProgram(GraphicsProgramPtr program) noexcept
{
	assert(_glcontext);
	_glcontext->setGraphicsProgram(program);
}

GraphicsProgramPtr
OGLDevice::getGraphicsProgram() const noexcept
{
	assert(_glcontext);
	return _glcontext->getGraphicsProgram();
}

void
OGLDevice::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable);
}

void
OGLDevice::drawRenderBuffer(const RenderIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_glcontext);
	_glcontext->drawRenderBuffer(renderable, first, count);
}

_NAME_END