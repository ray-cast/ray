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
#include <ray/render_mesh.h>

#include <ray/graphics_context.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

#include <ray/camera.h>
#include <ray/geometry.h>

#include <ray/render_object.h>
#include <ray/render_object_manager.h>

#include <ray/material.h>
#include <ray/material_manager.h>

_NAME_BEGIN

__ImplementSubClass(RenderPipeline, rtti::Interface, "RenderPipeline")

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
RenderPipeline::setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	assert(pipelineDevice);

	_width = w;
	_height = h;

	_pipelineDevice = pipelineDevice;

	if (!setupDeviceContext(window, w, h))
		return false;

	if (!setupBaseMeshes())
		return false;

	if (!setupMaterialSemantic())
		return false;

	if (!setupDataManager())
		return false;

	return true;
}

void
RenderPipeline::close() noexcept
{
	destroyBaseMeshes();
	destroyMaterialSemantic();
	destroyDataManager();
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
RenderPipeline::addRenderData(RenderQueue queue, RenderObjectPtr object) noexcept
{
	assert(_dataManager);
	_dataManager->addRenderData(queue, object);
}

const RenderObjects&
RenderPipeline::getRenderData(RenderQueue queue) const noexcept
{
	assert(_dataManager);
	return _dataManager->getRenderData(queue);
}

void
RenderPipeline::setTransform(const float4x4& transform) noexcept
{
	assert(_materialMatModel);
	_materialMatModel->assign(transform);
	_materialMatModelView->assign(transform * _materialMatView->getFloat4x4());
	_materialMatModelViewProject->assign(transform * _materialMatViewProject->getFloat4x4());
}

void
RenderPipeline::setTransformInverse(const float4x4& transform) noexcept
{
	assert(_materialMatModelInverse);
	_materialMatModelInverse->assign(transform);
}

void
RenderPipeline::setTransformInverseTranspose(const float4x4& transform) noexcept
{
	assert(_materialMatModelInverseTranspose);
	_materialMatModelInverseTranspose->assign(transform);
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
	assert(_dataManager);

	_materialCameraNear->assign(camera->getNear());
	_materialCameraFar->assign(camera->getFar());
	_materialCameraAperture->assign(camera->getAperture());
	_materialCameraPosition->assign(camera->getTranslate());
	_materialCameraDirection->assign(camera->getForward());
	_materialMatView->assign(camera->getView());
	_materialMatViewInverse->assign(camera->getViewInverse());
	_materialMatViewInverseTranspose->assign(camera->getTransformInverseTranspose());
	_materialMatProject->assign(camera->getProject());
	_materialMatProjectInverse->assign(camera->getProjectInverse());
	_materialMatViewProject->assign(camera->getViewProject());
	_materialMatViewProjectInverse->assign(camera->getViewProjectInverse());

	_dataManager->assginVisiable(camera);

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
RenderPipeline::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_width != width || _height != height)
	{
		auto& drawPostProcess = _postprocessors[RenderQueue::RenderQueuePostprocess];
		for (auto& it : drawPostProcess)
		{
			if (it->getActive())
				it->onResolutionChangeBefore(*this);
		}

		_width = width;
		_height = height;

		for (auto& it : drawPostProcess)
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

bool
RenderPipeline::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->updateBuffer(data, str, cnt);
}

void*
RenderPipeline::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->mapBuffer(data, access);
}

void
RenderPipeline::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->unmapBuffer(data);
}

void
RenderPipeline::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setFramebuffer(target);
}

void
RenderPipeline::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearFramebuffer(flags, color, depth, stencil);
}

void
RenderPipeline::discradRenderTexture() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->discardFramebuffer();
}

void
RenderPipeline::readFramebuffer(GraphicsFramebufferPtr texture, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->readFramebuffer(texture, pfd, w, h, data);
}

void
RenderPipeline::blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->blitFramebuffer(srcTarget, src, destTarget, dest);
}

void
RenderPipeline::drawSphere(MaterialTechPtr tech) noexcept
{
	this->drawMesh(tech, _renderSphere, _renderSphereIndirect);
}

void
RenderPipeline::drawCone(MaterialTechPtr tech) noexcept
{
	this->drawMesh(tech, _renderCone, _renderConeIndirect);
}

void
RenderPipeline::drawScreenQuad(MaterialTechPtr tech) noexcept
{
	this->drawMesh(tech, _renderScreenQuad, _renderScreenQuadIndirect);
}

void
RenderPipeline::drawMesh(MaterialTechPtr tech, RenderMeshPtr mesh, const GraphicsIndirect& renderable) noexcept
{
	_graphicsContext->setVertexBufferData(mesh->getVertexBuffer());
	_graphicsContext->setIndexBufferData(mesh->getIndexBuffer());

	auto& passList = tech->getPassList();
	for (auto& pass : passList)
	{
		auto& semantics = pass->getParameters();
		for (auto& it : semantics)
		{
			auto semantic = it->getSemantic();
			if (semantic)
			{
				auto type = semantic->getType();
				switch (type)
				{
				case GraphicsUniformType::GraphicsUniformTypeBool:
					it->assign(semantic->getBool());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt:
					it->assign(semantic->getInt());
					break;
				case GraphicsUniformType::GraphicsUniformTypeInt2:
					it->assign(semantic->getInt2());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat:
					it->assign(semantic->getFloat());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat2:
					it->assign(semantic->getFloat2());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3:
					it->assign(semantic->getFloat3());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4:
					it->assign(semantic->getFloat4());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
					it->assign(semantic->getFloat3x3());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
					it->assign(semantic->getFloat4x4());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloatArray:
					it->assign(semantic->getFloatArray());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
					it->assign(semantic->getFloat2Array());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
					it->assign(semantic->getFloat3Array());
					break;
				case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
					it->assign(semantic->getFloat4Array());
					break;
				case GraphicsUniformType::GraphicsUniformTypeStorageImage:
					it->assign(semantic->getTexture());
					break;
				default:
					break;
				}
			}
		}

		_graphicsContext->setRenderPipeline(pass->getRenderPipeline());
		_graphicsContext->setDescriptorSet(pass->getDescriptorSet());
		_graphicsContext->drawRenderMesh(renderable);
	}
}

void
RenderPipeline::drawRenderQueue(RenderQueue queue, MaterialTechPtr tech) noexcept
{
	auto& renderable = this->getRenderData(queue);
	for (auto& it : renderable)
		it->render(*this, queue, tech);
}

void
RenderPipeline::addPostProcess(RenderPostProcessPtr postprocess) noexcept
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& drawPostProcess = _postprocessors[renderQueue];

	if (std::find(drawPostProcess.begin(), drawPostProcess.end(), postprocess) == drawPostProcess.end())
	{
		postprocess->_setRenderPipeline(this);
		postprocess->setActive(true);
		drawPostProcess.push_back(postprocess);
	}
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& drawPostProcess = _postprocessors[renderQueue];

	auto it = std::find(drawPostProcess.begin(), drawPostProcess.end(), postprocess);
	if (it != drawPostProcess.end())
	{
		postprocess->setActive(false);
		drawPostProcess.erase(it);
	}
}

void
RenderPipeline::destroyPostProcess() noexcept
{
	for (auto& postprocess : _postprocessors)
	{
		for (auto& it : postprocess)
			it->setActive(false);

		postprocess.clear();
	}
}

void
RenderPipeline::drawPostProcess(RenderQueue queue, GraphicsFramebufferPtr source, GraphicsFramebufferPtr swap, GraphicsFramebufferPtr dest) noexcept
{
	GraphicsFramebufferPtr view = dest;
	GraphicsFramebufferPtr cur;

	auto& drawPostProcess = _postprocessors[queue];
	for (auto& it : drawPostProcess)
	{
		if (!it->getActive())
			continue;

		if (it->onRender(*this, source, view))
		{
			cur = view;
			source = view;

			std::swap(swap, view);
		}
	}

	if (cur != dest && cur)
	{
		std::uint32_t w = source->getGraphicsFramebufferDesc().getWidth();
		std::uint32_t h = source->getGraphicsFramebufferDesc().getHeight();
		Viewport viewport(0, 0, w, h);
		this->blitFramebuffer(cur, viewport, dest, viewport);
	}
}

void
RenderPipeline::present() noexcept
{
	_graphicsContext->present();
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
RenderPipeline::createTexture(const std::string& name, GraphicsTextureDim dim) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(name, dim);
}

GraphicsTexturePtr 
RenderPipeline::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(w, h, dim, format);
}

MaterialPtr
RenderPipeline::createMaterial(const std::string& name) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createMaterial(name);
}

RenderMeshPtr
RenderPipeline::createRenderMesh(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderMesh(vb, ib);
}

RenderMeshPtr
RenderPipeline::createRenderMesh(const MeshProperty& mesh) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderMesh(mesh);
}

RenderMeshPtr
RenderPipeline::createRenderMesh(const MeshPropertys& mesh) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderMesh(mesh);
}

MaterialVariantPtr 
RenderPipeline::createSemantic(const std::string& name, GraphicsUniformType type) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createSemantic(name, type);
}

void 
RenderPipeline::destroySemantic(MaterialVariantPtr semantic) const noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->destroySemantic(semantic);
}

MaterialVariantPtr 
RenderPipeline::getSemantic(const std::string& semantic) const noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->getSemantic(semantic);
}

bool
RenderPipeline::setupDeviceContext(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	GraphicsSwapchainDesc swapchainDesc;
	swapchainDesc.setWindHandle(window);
	swapchainDesc.setWidth(w);
	swapchainDesc.setHeight(h);
	swapchainDesc.setImageNums(2);
	swapchainDesc.setColorFormat(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	swapchainDesc.setDepthStencilFormat(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_graphicsSwapchain = _pipelineDevice->createSwapchain(swapchainDesc);
	if (!_graphicsSwapchain)
		return false;

	GraphicsContextDesc contextDesc;
	contextDesc.setSwapchain(_graphicsSwapchain);
	_graphicsContext = _pipelineDevice->createDeviceContext(contextDesc);
	if (!_graphicsContext)
		return false;

	return true;
}

bool
RenderPipeline::setupMaterialSemantic() noexcept
{
	_materialMatModel = _pipelineDevice->createSemantic("matModel", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatModel)
		return false;

	_materialMatModelInverse = _pipelineDevice->createSemantic("matModelInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatModelInverse)
		return false;

	_materialMatModelInverseTranspose = _pipelineDevice->createSemantic("matModelInverseTranspose", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatModelInverseTranspose)
		return false;

	_materialMatProject = _pipelineDevice->createSemantic("matProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatProject)
		return false;

	_materialMatProjectInverse = _pipelineDevice->createSemantic("matProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatProjectInverse)
		return false;

	_materialMatView = _pipelineDevice->createSemantic("matView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatView)
		return false;

	_materialMatViewInverse = _pipelineDevice->createSemantic("matViewInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatViewInverse)
		return false;

	_materialMatViewInverseTranspose = _pipelineDevice->createSemantic("matViewInverseTranspose", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatViewInverseTranspose)
		return false;

	_materialMatViewProject = _pipelineDevice->createSemantic("matViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatViewProject)
		return false;

	_materialMatViewProjectInverse = _pipelineDevice->createSemantic("matViewProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatViewProjectInverse)
		return false;

	_materialMatModelView = _pipelineDevice->createSemantic("matModelView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatModelView)
		return false;

	_materialMatModelViewProject = _pipelineDevice->createSemantic("matModelViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	if (!_materialMatModelViewProject)
		return false;

	_materialCameraAperture = _pipelineDevice->createSemantic("CameraAperture", GraphicsUniformType::GraphicsUniformTypeFloat);
	if (!_materialCameraAperture)
		return false;

	_materialCameraFar = _pipelineDevice->createSemantic("CameraFar", GraphicsUniformType::GraphicsUniformTypeFloat);
	if (!_materialCameraFar)
		return false;

	_materialCameraNear = _pipelineDevice->createSemantic("CameraNear", GraphicsUniformType::GraphicsUniformTypeFloat);
	if (!_materialCameraNear)
		return false;

	_materialCameraPosition = _pipelineDevice->createSemantic("CameraPosition", GraphicsUniformType::GraphicsUniformTypeFloat3);
	if (!_materialCameraPosition)
		return false;

	_materialCameraDirection = _pipelineDevice->createSemantic("CameraDirection", GraphicsUniformType::GraphicsUniformTypeFloat3);
	if (!_materialCameraDirection)
		return false;

	_materialDepthMap = _pipelineDevice->createSemantic("DepthMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDepthMap)
		return false;

	_materialColorMap = _pipelineDevice->createSemantic("ColorMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialColorMap)
		return false;

	_materialNormalMap = _pipelineDevice->createSemantic("NormalMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialNormalMap)
		return false;

	_materialDeferredDepthMap = _pipelineDevice->createSemantic("DeferredDepthMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredDepthMap)
		return false;

	_materialDeferredDepthLinearMap = _pipelineDevice->createSemantic("DeferredDepthLinearMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredDepthLinearMap)
		return false;

	_materialDeferredGraphicMap = _pipelineDevice->createSemantic("DeferredGraphicMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredGraphicMap)
		return false;

	_materialDeferredNormalMap = _pipelineDevice->createSemantic("DeferredNormalMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredNormalMap)
		return false;

	_materialDeferredLightMap = _pipelineDevice->createSemantic("DeferredLightMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredLightMap)
		return false;

	_materialDeferredShadowMap = _pipelineDevice->createSemantic("DeferredShadowMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	if (!_materialDeferredShadowMap)
		return false;

	return true;
}

bool 
RenderPipeline::setupBaseMeshes() noexcept
{
	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_renderScreenQuad = this->createRenderMesh(mesh);
	if (!_renderScreenQuad)
		return false;

	_renderScreenQuadIndirect.startVertice = 0;
	_renderScreenQuadIndirect.numVertices = mesh.getNumVertices();
	_renderScreenQuadIndirect.startIndice = 0;
	_renderScreenQuadIndirect.numIndices = mesh.getNumIndices();
	_renderScreenQuadIndirect.numInstances = 0;

	mesh.makeSphere(1, 24, 18);

	_renderSphere = this->createRenderMesh(mesh);
	if (!_renderSphere)
		return false;

	_renderSphereIndirect.startVertice = 0;
	_renderSphereIndirect.numVertices = mesh.getNumVertices();
	_renderSphereIndirect.startIndice = 0;
	_renderSphereIndirect.numIndices = mesh.getNumIndices();
	_renderSphereIndirect.numInstances = 0;

	mesh.makeCone(1, 1, 16);

	_renderCone = this->createRenderMesh(mesh);
	if (!_renderCone)
		return false;

	_renderConeIndirect.startVertice = 0;
	_renderConeIndirect.numVertices = mesh.getNumVertices();
	_renderConeIndirect.startIndice = 0;
	_renderConeIndirect.numIndices = mesh.getNumIndices();
	_renderConeIndirect.numInstances = 0;

	return true;
}

bool
RenderPipeline::setupDataManager() noexcept
{
	_dataManager = std::make_shared<DefaultRenderDataManager>();
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
	_pipelineDevice->destroySemantic(_materialMatModel);
	_pipelineDevice->destroySemantic(_materialMatModelInverse);
	_pipelineDevice->destroySemantic(_materialMatModelInverseTranspose);
	_pipelineDevice->destroySemantic(_materialMatProject);
	_pipelineDevice->destroySemantic(_materialMatProjectInverse);
	_pipelineDevice->destroySemantic(_materialMatView);
	_pipelineDevice->destroySemantic(_materialMatViewInverse);
	_pipelineDevice->destroySemantic(_materialMatViewInverseTranspose);
	_pipelineDevice->destroySemantic(_materialMatViewProject);
	_pipelineDevice->destroySemantic(_materialMatViewProjectInverse);
	_pipelineDevice->destroySemantic(_materialCameraAperture);
	_pipelineDevice->destroySemantic(_materialCameraFar);
	_pipelineDevice->destroySemantic(_materialCameraNear);
	_pipelineDevice->destroySemantic(_materialCameraPosition);
	_pipelineDevice->destroySemantic(_materialCameraDirection);
	_pipelineDevice->destroySemantic(_materialDepthMap);
	_pipelineDevice->destroySemantic(_materialColorMap);
	_pipelineDevice->destroySemantic(_materialNormalMap);
	_pipelineDevice->destroySemantic(_materialDeferredDepthMap);
	_pipelineDevice->destroySemantic(_materialDeferredDepthLinearMap);
	_pipelineDevice->destroySemantic(_materialDeferredGraphicMap);
	_pipelineDevice->destroySemantic(_materialDeferredNormalMap);
	_pipelineDevice->destroySemantic(_materialDeferredLightMap);
	_pipelineDevice->destroySemantic(_materialDeferredShadowMap);

	_materialMatModel.reset();
	_materialMatModelInverse.reset();
	_materialMatModelInverseTranspose.reset();
	_materialMatProject.reset();
	_materialMatProjectInverse.reset();
	_materialMatView.reset();
	_materialMatViewInverse.reset();
	_materialMatViewInverseTranspose.reset();
	_materialMatViewProject.reset();
	_materialMatViewProjectInverse.reset();
	_materialMatModelView.reset();
	_materialMatModelViewProject.reset();
	_materialCameraAperture.reset();
	_materialCameraFar.reset();
	_materialCameraNear.reset();
	_materialCameraPosition.reset();
	_materialCameraDirection.reset();
	_materialDepthMap.reset();
	_materialColorMap.reset();
	_materialNormalMap.reset();
	_materialDeferredDepthMap.reset();
	_materialDeferredDepthLinearMap.reset();
	_materialDeferredGraphicMap.reset();
	_materialDeferredNormalMap.reset();
	_materialDeferredLightMap.reset();
	_materialDeferredShadowMap.reset();
}

void
RenderPipeline::destroyBaseMeshes() noexcept
{
	_renderScreenQuad.reset();
	_renderSphere.reset();
	_renderCone.reset();
}

void
RenderPipeline::destroyDataManager() noexcept
{
	_dataManager.reset();
}

_NAME_END