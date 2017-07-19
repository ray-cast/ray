// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/render_system.h>
#include <ray/render_scene.h>
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_device.h>
#include <ray/render_pipeline_manager.h>

_NAME_BEGIN

__ImplementSingleton(RenderSystem)

RenderSystem::RenderSystem() noexcept
{
}

RenderSystem::~RenderSystem() noexcept
{
	this->close();
}

bool
RenderSystem::setup(const RenderSetting& setting) noexcept
{
	assert(!_pipelineManager);

	try
	{
		_pipelineManager = std::make_shared<RenderPipelineManager>();
		_pipelineManager->setup(setting);

		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

void
RenderSystem::close() noexcept
{
	_pipelineManager.reset();
}

bool
RenderSystem::setRenderSetting(const RenderSetting& setting) noexcept
{
	assert(_pipelineManager);

	try
	{
		return _pipelineManager->setRenderSetting(setting);
	}
	catch (const std::exception&)
	{
		assert(false);
		return false;
	}
}

const RenderSetting&
RenderSystem::getRenderSetting() const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderSetting();
}

bool
RenderSystem::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(w > 0 && h > 0);
	assert(_pipelineManager);

	try
	{
		_pipelineManager->setWindowResolution(w, h);
		return true;
	}
	catch (const std::exception&)
	{
		assert(false);
		return false;
	}
}

void
RenderSystem::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getWindowResolution(w, h);
}

bool
RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(w > 0 && h > 0);
	assert(_pipelineManager);

	try
	{
		_pipelineManager->setFramebufferSize(w, h);
		return true;
	}
	catch (const std::exception&)
	{
		assert(false);
		return false;
	}
}

void
RenderSystem::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getFramebufferSize(w, h);
}

void
RenderSystem::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setViewport(i, view);
}

const Viewport&
RenderSystem::getViewport(std::uint32_t i) const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->getViewport(i);
}

void
RenderSystem::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setScissor(i, scissor);
}

const Scissor&
RenderSystem::getScissor(std::uint32_t i) const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->getScissor(i);
}

void
RenderSystem::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setFramebuffer(target);
}

void
RenderSystem::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->clearFramebuffer(i, flags, color, depth, stencil);
}

void
RenderSystem::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->readFramebuffer(i, texture, x, y, width, height);
}

void
RenderSystem::discardFramebuffer(std::uint32_t i) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->discardFramebuffer(i);
}

void
RenderSystem::setMaterialPass(const MaterialPassPtr& pass) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setMaterialPass(pass);
}

void
RenderSystem::setVertexBuffer(std::uint32_t i, const GraphicsDataPtr& vbo, std::intptr_t offset) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setVertexBuffer(i, vbo, offset);
}

void
RenderSystem::setIndexBuffer(const GraphicsDataPtr& ibo, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->setIndexBuffer(ibo, offset, indexType);
}

void
RenderSystem::drawCone(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawCone(tech);
}

void
RenderSystem::drawSphere(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawSphere(tech);
}

void
RenderSystem::drawScreenQuad(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawScreenQuad(tech);
}

void
RenderSystem::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->draw(numVertices, numInstances, startVertice, startInstances);
}

void
RenderSystem::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawIndexed(numIndices, numInstances, startIndice, startVertice, startInstances);
}

void
RenderSystem::drawLayer(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawLayer(numVertices, numInstances, startVertice, startInstances, layer);
}

void
RenderSystem::drawIndexedLayer(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getRenderPipeline()->drawIndexedLayer(numIndices, numInstances, startIndice, startVertice, startInstances, layer);
}

bool
RenderSystem::isTextureSupport(GraphicsFormat format) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->isTextureSupport(format);
}

bool
RenderSystem::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->isTextureDimSupport(dimension);
}

bool
RenderSystem::isVertexSupport(GraphicsFormat format) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->isVertexSupport(format);
}

bool
RenderSystem::isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipeline()->isShaderSupport(stage);
}

GraphicsTexturePtr
RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createTexture(desc);
}

GraphicsTexturePtr
RenderSystem::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter, GraphicsSamplerWrap wrap) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createTexture(w, h, dim, format, filter, wrap);
}

MaterialPtr
RenderSystem::createMaterial(const std::string& name) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createMaterial(name);
}

GraphicsFramebufferPtr
RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createFramebuffer(desc);
}

GraphicsFramebufferLayoutPtr
RenderSystem::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createFramebufferLayout(desc);
}

GraphicsPipelinePtr
RenderSystem::createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createGraphicsPipeline(desc);
}

GraphicsDataPtr
RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createGraphicsData(desc);
}

GraphicsInputLayoutPtr
RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderPipelineDevice()->createInputLayout(desc);
}

void
RenderSystem::renderBegin() noexcept
{
	assert(_pipelineManager);
	_pipelineManager->renderBegin();
}

void
RenderSystem::renderEnd() noexcept
{
	assert(_pipelineManager);
	_pipelineManager->renderEnd();
}

void
RenderSystem::render() noexcept
{
	assert(_pipelineManager);

	for (auto& scene : RenderScene::getSceneAll())
	{
		if (!scene->getVisible())
			continue;
		_pipelineManager->render(*scene);
	}
}

_NAME_END