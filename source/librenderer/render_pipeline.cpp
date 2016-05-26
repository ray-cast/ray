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
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_device.h>

#include <ray/render_post_process.h>

#include <ray/graphics_context.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

#include <ray/camera.h>
#include <ray/geometry.h>

#include <ray/render_object.h>
#include <ray/render_object_manager.h>

#include <ray/material.h>
#include <ray/material_semantic.h>
#include <ray/material_manager.h>

_NAME_BEGIN

__ImplementSubClass(RenderPipeline, rtti::Interface, "RenderPipeline")

static float4x4 adjustProject = (float4x4().makeScale(1.0, 1.0, 2.0).setTranslate(0, 0, -1));

RenderPipeline::RenderPipeline() noexcept
	: _width(0)
	, _height(0)
{
}

RenderPipeline::~RenderPipeline() noexcept
{
	this->close();
}

bool
RenderPipeline::setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h, GraphicsSwapInterval interval) noexcept
{
	assert(pipelineDevice);

	_width = w;
	_height = h;

	_pipelineDevice = pipelineDevice;

	if (!this->setupDeviceContext(window, w, h, interval))
		return false;

	if (!this->setupBaseMeshes())
		return false;

	if (!this->setupMaterialSemantic())
		return false;

	return true;
}

void
RenderPipeline::close() noexcept
{
	this->destroyBaseMeshes();
	this->destroyMaterialSemantic();
	this->destroyDataManager();
}

void
RenderPipeline::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	assert(_graphicsSwapchain);
	_graphicsSwapchain->setSwapInterval(interval);
}

GraphicsSwapInterval
RenderPipeline::getSwapInterval() const noexcept
{
	assert(_graphicsSwapchain);
	return _graphicsSwapchain->getSwapInterval();
}

void
RenderPipeline::setTransform(const float4x4& transform) noexcept
{
	assert(_semanticsManager);

	auto& view = _semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeView)->getFloat4x4();
	auto& viewProject = _semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeViewProject)->getFloat4x4();

	auto modelView = math::transformMultiply(view, transform);
	auto modelViewInverse = math::transformInverse(modelView);

	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeModel)->uniform4fmat(transform);
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeModelView)->uniform4fmat(modelView);
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeModelViewProject)->uniform4fmat(viewProject * transform);
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeModelViewInverse)->uniform4fmat(modelViewInverse);
}

void
RenderPipeline::setTransformInverse(const float4x4& transform) noexcept
{
	assert(_semanticsManager);
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeModelInverse)->uniform4fmat(transform);
}

const MaterialSemanticPtr&
RenderPipeline::getSemanticParam(GlobalSemanticType type) const noexcept
{
	assert(_semanticsManager);
	return _semanticsManager->getSemantic(type);
}

void
RenderPipeline::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_width != width || _height != height)
	{
		for (auto& it : _postprocessors)
		{
			if (it->getActive())
				it->onResolutionChangeBefore(*this);
		}

		_width = width;
		_height = height;

		for (auto& it : _postprocessors)
		{
			if (it->getActive())
				it->onResolutionChangeAfter(*this);
		}
	}
}

void
RenderPipeline::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _width;
	h = _height;
}

void
RenderPipeline::renderBegin() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->renderBegin();
}

void
RenderPipeline::renderEnd() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->renderEnd();
}

void
RenderPipeline::setCamera(CameraPtr camera) noexcept
{
	assert(camera);
	assert(camera->getRenderDataManager());

	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeCameraNear)->uniform1f(camera->getNear());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeCameraFar)->uniform1f(camera->getFar());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeCameraAperture)->uniform1f(camera->getAperture());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeCameraPosition)->uniform3f(camera->getTranslate());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeCameraDirection)->uniform3f(camera->getForward());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeView)->uniform4fmat(camera->getView());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeViewInverse)->uniform4fmat(camera->getViewInverse());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeProject)->uniform4fmat(camera->getProject());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeProjectInverse)->uniform4fmat(camera->getProjectInverse());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeViewProject)->uniform4fmat(adjustProject * camera->getViewProject());
	_semanticsManager->getSemantic(GlobalSemanticType::GlobalSemanticTypeViewProjectInverse)->uniform4fmat(camera->getViewProjectInverse());

	camera->assignVisiable();
	_dataManager = camera->getRenderDataManager();

	_camera = camera;
}

CameraPtr
RenderPipeline::getCamera() const noexcept
{
	return _camera;
}

void
RenderPipeline::setViewport(const Viewport& view) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setViewport(view);
}

const Viewport&
RenderPipeline::getViewport() const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getViewport();
}

void
RenderPipeline::setScissor(const Scissor& scissor) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setScissor(scissor);
}

const Scissor&
RenderPipeline::getScissor() const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getScissor();
}

void
RenderPipeline::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setFramebuffer(target);
}

void 
RenderPipeline::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearFramebuffer(i, flags, color, depth, stencil);
}

void
RenderPipeline::discradRenderTexture(GraphicsAttachmentType attachments[], std::size_t numAttachment) noexcept
{
}

void
RenderPipeline::setMaterialPass(const MaterialPassPtr& pass) noexcept
{
	pass->update(*_semanticsManager);
	_graphicsContext->setRenderPipeline(pass->getRenderPipeline());
	_graphicsContext->setDescriptorSet(pass->getDescriptorSet());
}

void 
RenderPipeline::setVertexBuffer(GraphicsDataPtr vbo) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setVertexBufferData(vbo);
}

void 
RenderPipeline::setIndexBuffer(GraphicsDataPtr ibo) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setIndexBufferData(ibo);
}

void
RenderPipeline::drawSphere(const MaterialTech& tech, std::uint32_t layer) noexcept
{
	this->setVertexBuffer(_sphereVbo);
	this->setIndexBuffer(_sphereIbo);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawMesh(_sphereIndirect);
	}
}

void
RenderPipeline::drawCone(const MaterialTech& tech, std::uint32_t layer) noexcept
{
	this->setVertexBuffer(_coneVbo);
	this->setIndexBuffer(_coneIbo);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawMeshLayer(_sphereIndirect, layer);
	}
}

void
RenderPipeline::drawScreenQuad(const MaterialTech& tech) noexcept
{
	this->setVertexBuffer(_screenQuadVbo);
	this->setIndexBuffer(_screenQuadIbo);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawMesh(_screenQuadIndirect);
	}
}

void
RenderPipeline::drawScreenQuadLayer(const MaterialTech& tech, std::uint32_t layer) noexcept
{
	this->setVertexBuffer(_screenQuadVbo);
	this->setIndexBuffer(_screenQuadIbo);
	
	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawMeshLayer(_screenQuadIndirect, layer);
	}
}

void
RenderPipeline::drawMesh(const GraphicsIndirect& renderable) noexcept
{
	_graphicsContext->drawRenderMesh(renderable);
}

void
RenderPipeline::drawMeshLayer(const GraphicsIndirect& renderable, std::uint32_t layer) noexcept
{
	_graphicsContext->setStencilReference(GraphicsStencilFace::GraphicsStencilFaceFrontBack, 1 << layer);
	_graphicsContext->drawRenderMesh(renderable);
}

void 
RenderPipeline::drawRenderQueue(RenderQueue queue) noexcept
{
	assert(_camera);

	auto& renderable = _camera->getRenderDataManager()->getRenderData(queue);
	for (auto& it : renderable)
		it->onRenderObject(*this, queue, nullptr);
}

void
RenderPipeline::drawRenderQueue(RenderQueue queue, const MaterialTechPtr& tech) noexcept
{
	assert(_camera);

	auto& renderable = _camera->getRenderDataManager()->getRenderData(queue);
	for (auto& it : renderable)
		it->onRenderObject(*this, queue, tech.get());
}

void
RenderPipeline::addPostProcess(RenderPostProcessPtr& postprocess) noexcept
{
	if (std::find(_postprocessors.begin(), _postprocessors.end(), postprocess) == _postprocessors.end())
	{
		postprocess->_setRenderPipeline(this);
		postprocess->setActive(true);
		_postprocessors.push_back(postprocess);
	}
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr& postprocess) noexcept
{
	auto it = std::find(_postprocessors.begin(), _postprocessors.end(), postprocess);
	if (it != _postprocessors.end())
	{
		postprocess->setActive(false);
		_postprocessors.erase(it);
	}
}

void
RenderPipeline::destroyPostProcess() noexcept
{
	for (auto& it : _postprocessors)
		it->setActive(false);

	_postprocessors.clear();
}

bool
RenderPipeline::drawPostProcess(RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	GraphicsFramebufferPtr view = swap;
	GraphicsFramebufferPtr cur = source;

	for (auto& it : _postprocessors)
	{
		if (!it->getActive())
			continue;

		if (it->onRender(*this, queue, cur, view))
		{
			std::swap(view, cur);
		}
	}

	return cur != source;
}

void
RenderPipeline::present() noexcept
{
	_graphicsContext->present();
}

bool
RenderPipeline::isTextureSupport(GraphicsFormat format) noexcept
{
	return _graphicsContext->isTextureSupport(format);
}

bool
RenderPipeline::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return _graphicsContext->isTextureDimSupport(dimension);
}

bool
RenderPipeline::isVertexSupport(GraphicsFormat format) noexcept
{
	return _graphicsContext->isVertexSupport(format);
}

bool
RenderPipeline::isShaderSupport(GraphicsShaderStage stage) noexcept
{
	return _graphicsContext->isShaderSupport(stage);
}

GraphicsDataPtr
RenderPipeline::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createGraphicsData(desc);
}

GraphicsInputLayoutPtr
RenderPipeline::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createInputLayout(desc);
}

GraphicsFramebufferPtr
RenderPipeline::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createFramebuffer(desc);
}

GraphicsFramebufferLayoutPtr
RenderPipeline::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createFramebufferLayout(desc);
}

GraphicsTexturePtr
RenderPipeline::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(desc);
}

GraphicsTexturePtr
RenderPipeline::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(name, dim, filter);
}

GraphicsTexturePtr
RenderPipeline::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(w, h, dim, format, filter);
}

MaterialPtr
RenderPipeline::createMaterial(const std::string& name) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createMaterial(name);
}

void
RenderPipeline::destroyMaterial(MaterialPtr material) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->destroyMaterial(material);
}

GraphicsDataPtr
RenderPipeline::createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createVertexBuffer(mesh, flags);
}

GraphicsDataPtr
RenderPipeline::createIndexBuffer(const MeshProperty& mesh) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createIndexBuffer(mesh);
}

bool
RenderPipeline::setupDeviceContext(WindHandle window, std::uint32_t w, std::uint32_t h, GraphicsSwapInterval interval) noexcept
{
	GraphicsSwapchainDesc swapchainDesc;
	swapchainDesc.setWindHandle(window);
	swapchainDesc.setWidth(w);
	swapchainDesc.setHeight(h);
	swapchainDesc.setSwapInterval(interval);
	swapchainDesc.setImageNums(2);
	swapchainDesc.setColorFormat(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	swapchainDesc.setDepthStencilFormat(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_graphicsSwapchain = _pipelineDevice->createSwapchain(swapchainDesc);
	if (!_graphicsSwapchain)
		return false;

	GraphicsContextDesc contextDesc;
	contextDesc.setDebugMode(true);
	contextDesc.setSwapchain(_graphicsSwapchain);
	_graphicsContext = _pipelineDevice->createDeviceContext(contextDesc)->downcast_pointer<GraphicsContext>();
	if (!_graphicsContext)
		return false;

	return true;
}

bool
RenderPipeline::setupMaterialSemantic() noexcept
{
	_semanticsManager = std::make_shared<MaterialSemanticManager>();
	_semanticsManager->setup();
	return true;
}

bool
RenderPipeline::setupBaseMeshes() noexcept
{
	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_screenQuadVbo = this->createVertexBuffer(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_screenQuadVbo)
		return false;

	_screenQuadIbo = this->createIndexBuffer(mesh);
	if (!_screenQuadIbo)
		return false;

	_screenQuadIndirect.startVertice = 0;
	_screenQuadIndirect.numVertices = mesh.getNumVertices();
	_screenQuadIndirect.startIndice = 0;
	_screenQuadIndirect.numIndices = mesh.getNumIndices();
	_screenQuadIndirect.numInstances = 0;

	mesh.makeSphere(1, 24, 18);

	_sphereVbo = this->createVertexBuffer(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_sphereVbo)
		return false;

	_sphereIbo = this->createIndexBuffer(mesh);
	if (!_sphereIbo)
		return false;

	_sphereIndirect.startVertice = 0;
	_sphereIndirect.numVertices = mesh.getNumVertices();
	_sphereIndirect.startIndice = 0;
	_sphereIndirect.numIndices = mesh.getNumIndices();
	_sphereIndirect.numInstances = 0;

	mesh.makeCone(1, 1, 16);

	_coneVbo = this->createVertexBuffer(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_coneVbo)
		return false;

	_coneIbo = this->createIndexBuffer(mesh);
	if (!_coneIbo)
		return false;

	_coneIndirect.startVertice = 0;
	_coneIndirect.numVertices = mesh.getNumVertices();
	_coneIndirect.startIndice = 0;
	_coneIndirect.numIndices = mesh.getNumIndices();
	_coneIndirect.numInstances = 0;

	return true;
}

void
RenderPipeline::destroyDeviceContext() noexcept
{
	_graphicsSwapchain.reset();
	_graphicsContext.reset();
}

void
RenderPipeline::destroyMaterialSemantic() noexcept
{
	_semanticsManager.reset();
}

void
RenderPipeline::destroyBaseMeshes() noexcept
{
	_screenQuadVbo.reset();
	_sphereVbo.reset();
	_coneVbo.reset();
	_screenQuadIbo.reset();
	_sphereIbo.reset();
	_coneIbo.reset();
}

void
RenderPipeline::destroyDataManager() noexcept
{
	_dataManager.reset();
}

_NAME_END