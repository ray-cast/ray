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

#include <ray/graphics_device.h>
#include <ray/graphics_device_property.h>
#include <ray/graphics_data.h>
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
	, _dpi_w(0)
	, _dpi_h(0)
	, _planeIndices(0)
	, _sphereIndices(0)
	, _coneIndices(0)
	, _planeIndexType(GraphicsIndexType::GraphicsIndexTypeUInt16)
	, _coneIndexType(GraphicsIndexType::GraphicsIndexTypeUInt16)
	, _sphereIndexType(GraphicsIndexType::GraphicsIndexTypeUInt16)
{
}

RenderPipeline::~RenderPipeline() noexcept
{
	this->close();
}

bool
RenderPipeline::setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h, GraphicsSwapInterval interval) noexcept
{
	assert(pipelineDevice);

	_width = w;
	_height = h;
	_dpi_w = dpi_w;
	_dpi_h = dpi_h;

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
	this->destroyPostProcess();
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

	auto modelView = view * transform;
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
		_width = width;
		_height = height;

		_graphicsSwapchain->setWindowResolution(width, height);

		for (auto& it : _postprocessors)
		{
			if (it->getActive())
				it->onResolutionChange(*this);
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
RenderPipeline::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
{
	if (_dpi_w != w || _dpi_h != h)
	{
		_dpi_w = w;
		_dpi_h = h;

		for (auto& it : _postprocessors)
		{
			if (it->getActive())
				it->onResolutionChangeDPI(*this);
		}
	}
}

void
RenderPipeline::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _dpi_w;
	h = _dpi_h;
}

GraphicsDeviceType
RenderPipeline::getDeviceType() const noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->getDeviceType();
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

	_dataManager = camera->getRenderDataManager();
	_dataManager->assginVisiable(*camera);

	_camera = camera;
}

CameraPtr
RenderPipeline::getCamera() const noexcept
{
	return _camera;
}

void
RenderPipeline::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setViewport(i, view);
}

const Viewport&
RenderPipeline::getViewport(std::uint32_t i) const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getViewport(i);
}

void
RenderPipeline::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setScissor(i, scissor);
}

const Scissor&
RenderPipeline::getScissor(std::uint32_t i) const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getScissor(i);
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
RenderPipeline::readFramebuffer(const GraphicsTexturePtr& texture, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->readFramebuffer(texture, x, y, width, height);
}

void
RenderPipeline::discardFramebuffer(std::uint32_t i) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->discardFramebuffer(i);
}

void
RenderPipeline::setMaterialPass(const MaterialPassPtr& pass) noexcept
{
	pass->update(*_semanticsManager);
	_graphicsContext->setRenderPipeline(pass->getRenderPipeline());
	_graphicsContext->setDescriptorSet(pass->getDescriptorSet());
}

void
RenderPipeline::setVertexBuffer(std::uint32_t i, GraphicsDataPtr vbo, std::intptr_t offset) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setVertexBufferData(i, vbo, offset);
}

void
RenderPipeline::setIndexBuffer(GraphicsDataPtr ibo, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setIndexBufferData(ibo, offset, indexType);
}

void
RenderPipeline::generateMipmap(GraphicsTexturePtr texture)
{
	assert(_graphicsContext);
	_graphicsContext->generateMipmap(texture);
}

void
RenderPipeline::drawSphere(const MaterialTech& tech, std::uint32_t layer) noexcept
{
	this->setVertexBuffer(0, _sphereVbo, 0);
	this->setIndexBuffer(_sphereIbo, 0, _sphereIndexType);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawIndexedLayer(_sphereIndices, 1, 0, 0, 0, layer);
	}
}

void
RenderPipeline::drawCone(const MaterialTech& tech, std::uint32_t layer) noexcept
{
	this->setVertexBuffer(0, _coneVbo, 0);
	this->setIndexBuffer(_coneIbo, 0, _coneIndexType);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawIndexedLayer(_coneIndices, 1, 0, 0, 0, layer);
	}
}

void
RenderPipeline::drawScreenQuad(const MaterialTech& tech, std::uint32_t instanceCount) noexcept
{
	this->setVertexBuffer(0, _planeVbo, 0);
	this->setIndexBuffer(_planeIbo, 0, _planeIndexType);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawIndexed(_planeIndices, instanceCount, 0, 0, 0);
	}
}

void
RenderPipeline::drawScreenQuadLayer(const MaterialTech& tech, std::uint32_t layer, std::uint32_t instanceCount) noexcept
{
	this->setVertexBuffer(0, _planeVbo, 0);
	this->setIndexBuffer(_planeIbo, 0, _planeIndexType);

	auto& passList = tech.getPassList();
	for (auto& pass : passList)
	{
		pass->update(*_semanticsManager);

		this->setMaterialPass(pass);
		this->drawIndexedLayer(_planeIndices, instanceCount, 0, 0, 0, layer);
	}
}

void
RenderPipeline::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	_graphicsContext->draw(numVertices, numInstances, startVertice, startInstances);
}

void
RenderPipeline::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	_graphicsContext->drawIndexed(numIndices, numInstances, startIndice, startVertice, startInstances);
}

void
RenderPipeline::drawLayer(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept
{
	_graphicsContext->setStencilReference(GraphicsStencilFaceFlagBits::GraphicsStencilFaceAllBit, 1 << layer);
	_graphicsContext->draw(numVertices, numInstances, startVertice, startInstances);
}

void
RenderPipeline::drawIndexedLayer(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept
{
	_graphicsContext->setStencilReference(GraphicsStencilFaceFlagBits::GraphicsStencilFaceAllBit, 1 << layer);
	_graphicsContext->drawIndexed(numIndices, numInstances, startIndice, startVertice, startInstances);
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
RenderPipeline::drawPostProcess(RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
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
	return _graphicsContext->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties().isTextureSupport(format);
}

bool
RenderPipeline::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return _graphicsContext->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties().isTextureDimSupport(dimension);
}

bool
RenderPipeline::isVertexSupport(GraphicsFormat format) noexcept
{
	return _graphicsContext->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties().isVertexSupport(format);
}

bool
RenderPipeline::isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept
{
	return _graphicsContext->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties().isShaderSupport(stage);
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
RenderPipeline::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter, GraphicsSamplerWrap wrap) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(w, h, dim, format, filter, wrap);
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
	contextDesc.setSwapchain(_graphicsSwapchain);
	_graphicsContext = _pipelineDevice->createDeviceContext(contextDesc);
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
	this->makePlane(2, 2, 1, 1);
	this->makeSphere(1, 24, 18);
	this->makeCone(1, 1, 16);

	return true;
}

void
RenderPipeline::makePlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
{
	std::vector<float3> _vertices;
	std::vector<float3> _normals;
	std::vector<float2> _texcoord;
	std::vector<std::uint16_t> _indices;

	float widthHalf = width * 0.5f;
	float heightHalf = height * 0.5f;

	std::uint32_t gridX = widthSegments;
	std::uint32_t gridY = heightSegments;

	std::uint32_t gridX1 = gridX + 1;
	std::uint32_t gridY1 = gridY + 1;

	float segmentWidth = width / gridX;
	float segmentHeight = height / gridY;

	const Vector3& normal = Vector3::UnitZ;

	for (std::uint32_t iz = 0; iz < gridY1; iz++)
	{
		for (std::uint32_t ix = 0; ix < gridX1; ix++)
		{
			float x = ix * segmentWidth - widthHalf;
			float z = iz * segmentHeight - heightHalf;

			_normals.push_back(normal);
			_vertices.emplace_back(x, z, 0.0f);
		}
	}

	for (std::uint32_t iy = 0; iy < gridY; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridX; ix++)
		{
			_texcoord.emplace_back((float)ix / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoord.emplace_back((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoord.emplace_back((float)ix / gridX, (float)(1 - iy) / gridY);
			_texcoord.emplace_back((float)(ix + 1) / gridX, (float)(1 - iy) / gridY);

			std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
			std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
			std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
			std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

			_indices.push_back(a);
			_indices.push_back(b);
			_indices.push_back(c);

			_indices.push_back(c);
			_indices.push_back(d);
			_indices.push_back(a);
		}
	}

	GraphicsDataDesc _vb;
	_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	_vb.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_vb.setStream((std::uint8_t*)_vertices.data());
	_vb.setStreamSize(_vertices.size() * sizeof(float3));

	_planeVbo = this->createGraphicsData(_vb);

	GraphicsDataDesc _ib;
	_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	_ib.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_ib.setStream((std::uint8_t*)_indices.data());
	_ib.setStreamSize(_indices.size() * sizeof(std::uint16_t));

	_planeIbo = this->createGraphicsData(_ib);
	_planeIndices = static_cast<std::uint16_t>(_indices.size());
	_planeIndexType = GraphicsIndexType::GraphicsIndexTypeUInt16;
}

void
RenderPipeline::makeSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
{
	std::vector<float3> _vertices;
	std::vector<float3> _normals;
	std::vector<float2> _texcoord;
	std::vector<std::uint16_t> _indices;

	std::vector<std::uint32_t> vertices;

	for (std::uint32_t y = 0; y <= heightSegments; y++)
	{
		for (std::uint32_t x = 0; x <= widthSegments; x++)
		{
			float u = (float)(x) / widthSegments;
			float v = (float)(y) / heightSegments;

			Vector3 vertex;
			vertex.x = -radius * sin(thetaStart + v * thetaLength) * cos(phiStart + u * phiLength);
			vertex.y = radius * cos(thetaStart + v * thetaLength);
			vertex.z = radius * sin(thetaStart + v * thetaLength) * sin(phiStart + u * phiLength);

			_vertices.push_back(vertex);
			_normals.push_back(math::normalize(vertex));
			_texcoord.emplace_back(u, 1 - v);

			vertices.push_back((std::uint32_t)_vertices.size() - 1);
		}
	}

	for (std::uint32_t y = 0; y < heightSegments; y++)
	{
		for (std::uint32_t x = 0; x < widthSegments; x++)
		{
			std::uint32_t v1 = vertices[y * (widthSegments + 1) + x];
			std::uint32_t v2 = vertices[y * (widthSegments + 1) + x + 1];
			std::uint32_t v3 = vertices[(y + 1) * (widthSegments + 1) + x];
			std::uint32_t v4 = vertices[(y + 1) * (widthSegments + 1) + x + 1];

			if (math::abs((_vertices)[v2].y) == radius)
			{
				_indices.push_back(v2);
				_indices.push_back(v3);
				_indices.push_back(v4);
			}
			else if (math::abs((_vertices)[v3].y) == radius)
			{
				_indices.push_back(v2);
				_indices.push_back(v1);
				_indices.push_back(v3);
			}
			else
			{
				_indices.push_back(v2);
				_indices.push_back(v3);
				_indices.push_back(v4);

				_indices.push_back(v2);
				_indices.push_back(v1);
				_indices.push_back(v3);
			}
		}
	}

	GraphicsDataDesc _vb;
	_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	_vb.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_vb.setStream((std::uint8_t*)_vertices.data());
	_vb.setStreamSize(_vertices.size() * sizeof(float3));

	_sphereVbo = this->createGraphicsData(_vb);

	GraphicsDataDesc _ib;
	_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	_ib.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_ib.setStream((std::uint8_t*)_indices.data());
	_ib.setStreamSize(_indices.size() * sizeof(std::uint16_t));

	_sphereIbo = this->createGraphicsData(_ib);
	_sphereIndices = static_cast<std::uint16_t>(_indices.size());
	_sphereIndexType = GraphicsIndexType::GraphicsIndexTypeUInt16;
}

void
RenderPipeline::makeCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
{
	std::vector<float3> _vertices;
	std::vector<float3> _normals;
	std::vector<float2> _texcoord;
	std::vector<std::uint16_t> _indices;

	_vertices.emplace_back(0.0f, 0.0f, 0.0f);
	_vertices.emplace_back(0.0f, 0.0f, -height);

	_normals.emplace_back(0.0f, 0.0f, 0.0f);
	_normals.emplace_back(0.0f, 0.0f, -1.0f);

	_texcoord.emplace_back(0.0f, 0.0f);
	_texcoord.emplace_back(1.0f, 1.0f);

	float segment = thetaLength / segments;

	for (std::uint32_t i = 0; i <= segments; i++)
	{
		float sin;
		float cos;

		math::sinCos(&sin, &cos, thetaStart + i * segment);

		Vector3 v;
		v.x = radius * cos;
		v.y = -radius * sin;
		v.z = 0;

		_vertices.push_back(v);
		_normals.push_back(math::normalize(v));

		_texcoord.emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
	}

	for (std::uint32_t i = 2; i <= segments + 1; i++)
	{
		std::uint32_t v1 = i;
		std::uint32_t v2 = 0;
		std::uint32_t v3 = i + 1;

		_indices.push_back(v1);
		_indices.push_back(v2);
		_indices.push_back(v3);
	}

	for (std::uint32_t i = 2; i <= segments + 1; i++)
	{
		std::uint32_t v1 = i;
		std::uint32_t v2 = 1;
		std::uint32_t v3 = i + 1;

		_indices.push_back(v3);
		_indices.push_back(v2);
		_indices.push_back(v1);
	}

	GraphicsDataDesc _vb;
	_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	_vb.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_vb.setStream((std::uint8_t*)_vertices.data());
	_vb.setStreamSize(_vertices.size() * sizeof(float3));

	_coneVbo = this->createGraphicsData(_vb);

	GraphicsDataDesc _ib;
	_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	_ib.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_ib.setStream((std::uint8_t*)_indices.data());
	_ib.setStreamSize(_indices.size() * sizeof(std::uint16_t));

	_coneIbo = this->createGraphicsData(_ib);
	_coneIndices = static_cast<std::uint16_t>(_indices.size());
	_coneIndexType = GraphicsIndexType::GraphicsIndexTypeUInt16;
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
	_planeVbo.reset();
	_sphereVbo.reset();
	_coneVbo.reset();
	_planeIbo.reset();
	_sphereIbo.reset();
	_coneIbo.reset();
}

void
RenderPipeline::destroyDataManager() noexcept
{
	_dataManager.reset();
}

_NAME_END