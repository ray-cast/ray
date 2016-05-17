// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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

	_pipelineManager = std::make_shared<RenderPipelineManager>();
	if (!_pipelineManager->setup(setting))
	{
		this->close();
		return false;
	}

	return true;
}

void
RenderSystem::close() noexcept
{
	assert(_sceneList.empty());
	_pipelineManager.reset();
}

RenderScenePtr
RenderSystem::createRenderScene() noexcept
{
	auto scene = std::make_shared<RenderScene>();
	_sceneList.push_back(scene);
	return scene;
}

void
RenderSystem::removeRenderScene(RenderScenePtr scene) noexcept
{
	auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
	if (it != _sceneList.end())
		_sceneList.erase(it);
}

void
RenderSystem::setRenderSetting(const RenderSetting& setting) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setRenderSetting(setting);
}

const RenderSetting&
RenderSystem::getRenderSetting() const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getRenderSetting();
}

void
RenderSystem::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setWindowResolution(w, h);
}

void
RenderSystem::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	assert(_pipelineManager);
	_pipelineManager->getWindowResolution(w, h);
}

void
RenderSystem::setViewport(const Viewport& view) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setViewport(view);
}

const Viewport&
RenderSystem::getViewport() const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getViewport();
}

void 
RenderSystem::setScissor(const Scissor& scissor) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setScissor(scissor);
}

const Scissor&
RenderSystem::getScissor() const noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->getScissor();
}

void
RenderSystem::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setFramebuffer(target);
}

void
RenderSystem::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->clearFramebuffer(flags, color, depth, stencil);
}

void
RenderSystem::discradRenderTexture() noexcept
{
	assert(_pipelineManager);
	_pipelineManager->discradRenderTexture();
}

void
RenderSystem::blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->blitFramebuffer(srcTarget, src, destTarget, dest);
}

void 
RenderSystem::setMaterialPass(const MaterialPassPtr& pass) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setMaterialPass(pass);
}

void
RenderSystem::setVertexBuffer(GraphicsDataPtr vbo) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setVertexBuffer(vbo);
}

void
RenderSystem::setIndexBuffer(GraphicsDataPtr ibo) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->setIndexBuffer(ibo);
}

void
RenderSystem::drawCone(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->drawCone(tech);
}

void
RenderSystem::drawSphere(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->drawSphere(tech);
}

void
RenderSystem::drawScreenQuad(const MaterialTech& tech) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->drawScreenQuad(tech);
}

void
RenderSystem::drawMesh(const GraphicsIndirect& renderable) noexcept
{
	assert(_pipelineManager);
	_pipelineManager->drawMesh(renderable);
}

bool
RenderSystem::isTextureSupport(GraphicsFormat format) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->isTextureSupport(format);
}

bool
RenderSystem::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->isTextureDimSupport(dimension);
}

bool
RenderSystem::isVertexSupport(GraphicsFormat format) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->isVertexSupport(format);
}

bool
RenderSystem::isShaderSupport(GraphicsShaderStage stage) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->isShaderSupport(stage);
}

GraphicsTexturePtr
RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createTexture(desc);
}

GraphicsTexturePtr
RenderSystem::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createTexture(w, h, dim, format);
}

GraphicsTexturePtr
RenderSystem::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createTexture(name, dim, filter);
}

MaterialPtr
RenderSystem::createMaterial(const std::string& name) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createMaterial(name);
}

GraphicsFramebufferPtr
RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createFramebuffer(desc);
}

GraphicsFramebufferLayoutPtr
RenderSystem::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createFramebufferLayout(desc);
}

GraphicsPipelinePtr
RenderSystem::createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createGraphicsPipeline(desc);
}

GraphicsDataPtr
RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createGraphicsData(desc);
}

GraphicsDataPtr
RenderSystem::createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createVertexBuffer(mesh, flags);
}

GraphicsDataPtr
RenderSystem::createIndexBuffer(const MeshProperty& mesh) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createIndexBuffer(mesh);
}

GraphicsInputLayoutPtr
RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_pipelineManager);
	return _pipelineManager->createInputLayout(desc);
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

	for (auto& scene : _sceneList)
		_pipelineManager->render(*scene);
}

_NAME_END