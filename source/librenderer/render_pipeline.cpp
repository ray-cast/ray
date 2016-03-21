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
#include <ray/render_object_manager.h>
#include <ray/render_post_process.h>
#include <ray/render_scene.h>
#include <ray/render_object.h>
#include <ray/render_buffer.h>
#include <ray/light.h>
#include <ray/render_mesh.h>
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/material_manager.h>
#include <ray/model.h>
#include <ray/image.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/resource.h>

#include <ray/graphics_system.h>
#include <ray/graphics_device.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_data.h>
#include <ray/graphics_view.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_input_layout.h>

_NAME_BEGIN

RenderPipeline::RenderPipeline() noexcept
{
}

RenderPipeline::~RenderPipeline() noexcept
{
	this->close();
}

bool
RenderPipeline::open(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	_graphicsDevice = GraphicsSystem::instance()->createDevice(GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore);

	GraphicsSwapchainDesc swapchainDesc;
	swapchainDesc.setWindHandle(window);
	swapchainDesc.setWidth(w);
	swapchainDesc.setHeight(h);
	_graphicsSwapchain = _graphicsDevice->createGraphicsSwapchain(swapchainDesc);
	if (!_graphicsSwapchain)
		return false;

	GraphicsContextDesc contextDesc;
	contextDesc.setSwapchain(_graphicsSwapchain);
	_graphicsContext = _graphicsDevice->createGraphicsContext(contextDesc);
	if (!_graphicsContext)
		return false;

	_materialManager = std::make_shared<MaterialManager>();
	_materialManager->open(_graphicsDevice);

	_materialMatModel = _materialManager->createSemantic("matModel", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatModelInverse = _materialManager->createSemantic("matModelInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatModelInverseTranspose = _materialManager->createSemantic("matModelInverseTranspose", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatProject = _materialManager->createSemantic("matProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatProjectInverse = _materialManager->createSemantic("matProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatView = _materialManager->createSemantic("matView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatViewInverse = _materialManager->createSemantic("matViewInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatViewInverseTranspose = _materialManager->createSemantic("matViewInverseTranspose", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatViewProject = _materialManager->createSemantic("matViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialMatViewProjectInverse = _materialManager->createSemantic("matViewProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_materialCameraAperture = _materialManager->createSemantic("CameraAperture", GraphicsUniformType::GraphicsUniformTypeFloat);
	_materialCameraFar = _materialManager->createSemantic("CameraFar", GraphicsUniformType::GraphicsUniformTypeFloat);
	_materialCameraNear = _materialManager->createSemantic("CameraNear", GraphicsUniformType::GraphicsUniformTypeFloat);
	_materialCameraPosition = _materialManager->createSemantic("CameraPosition", GraphicsUniformType::GraphicsUniformTypeFloat3);
	_materialCameraDirection = _materialManager->createSemantic("CameraDirection", GraphicsUniformType::GraphicsUniformTypeFloat3);

	_dataManager = std::make_shared<DefaultRenderDataManager>();

	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_renderScreenQuad = this->createRenderBuffer(mesh);
	_renderScreenQuadIndirect.startVertice = 0;
	_renderScreenQuadIndirect.numVertices = mesh.getNumVertices();
	_renderScreenQuadIndirect.startIndice = 0;
	_renderScreenQuadIndirect.numIndices = mesh.getNumIndices();
	_renderScreenQuadIndirect.numInstances = 0;

	mesh.makeSphere(1, 16, 12);

	_renderSphere = this->createRenderBuffer(mesh);
	_renderSphereIndirect.startVertice = 0;
	_renderSphereIndirect.numVertices = mesh.getNumVertices();
	_renderSphereIndirect.startIndice = 0;
	_renderSphereIndirect.numIndices = mesh.getNumIndices();
	_renderSphereIndirect.numInstances = 0;

	mesh.makeCone(1, 1, 16);

	_renderCone = this->createRenderBuffer(mesh);
	_renderConeIndirect.startVertice = 0;
	_renderConeIndirect.numVertices = mesh.getNumVertices();
	_renderConeIndirect.startIndice = 0;
	_renderConeIndirect.numIndices = mesh.getNumIndices();
	_renderConeIndirect.numInstances = 0;

	this->setWindowResolution(w, h);

	return true;
}

void
RenderPipeline::close() noexcept
{
	for (auto& postprocess : _postprocessors)
	{
		for (auto& it : postprocess)
			it->setActive(false);

		postprocess.clear();
	}

	_materialManager.reset();
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
	_materialCameraAperture.reset();
	_materialCameraFar.reset();
	_materialCameraNear.reset();
	_materialCameraPosition.reset();
	_materialCameraDirection.reset();

	_renderScreenQuad.reset();
	_renderSphere.reset();
	_renderCone.reset();
	_dataManager.reset();
	_graphicsContext.reset();
}

void
RenderPipeline::setRenderDataManager(RenderDataManagerPtr manager) noexcept
{
	assert(manager);
	_dataManager = manager;
}

RenderDataManagerPtr
RenderPipeline::getRenderDataManagerPtr() const noexcept
{
	return _dataManager;
}

void
RenderPipeline::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_graphicsSwapchain);
	_graphicsSwapchain->setSwapInterval(interval);
}

SwapInterval
RenderPipeline::getSwapInterval() const noexcept
{
	assert(_graphicsSwapchain);
	return _graphicsSwapchain->getSwapInterval();
}

void
RenderPipeline::setCamera(CameraPtr camera) noexcept
{
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

MaterialPtr
RenderPipeline::createMaterial(const std::string& name) noexcept
{
	assert(_materialManager);
	return _materialManager->createMaterial(name);
}

GraphicsTexturePtr
RenderPipeline::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsTexture(desc);
}

GraphicsTexturePtr
RenderPipeline::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_graphicsDevice);
	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(w);
	textureDesc.setHeight(h);
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	return _graphicsDevice->createGraphicsTexture(textureDesc);
}

GraphicsTexturePtr
RenderPipeline::createTexture(const std::string& name) noexcept
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, name))
	{
		Image image;
		if (image.load(*stream))
		{
			GraphicsFormat format = GraphicsFormat::GraphicsFormatUndefined;

			if (image.getImageType() == ImageType::ImageTypeBC1RGBU)
				format = GraphicsFormat::GraphicsFormatBC1RGBUNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC1RGBAU)
				format = GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC1RGBSRGB)
				format = GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC1RGBASRGB)
				format = GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC3U)
				format = GraphicsFormat::GraphicsFormatBC3UNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC3SRGB)
				format = GraphicsFormat::GraphicsFormatBC3SRGBBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC4U)
				format = GraphicsFormat::GraphicsFormatBC4UNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC4S)
				format = GraphicsFormat::GraphicsFormatBC4SNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC5U)
				format = GraphicsFormat::GraphicsFormatBC5UNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC5S)
				format = GraphicsFormat::GraphicsFormatBC5SNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC6HUFloat)
				format = GraphicsFormat::GraphicsFormatBC6HUFloatBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC6HSFloat)
				format = GraphicsFormat::GraphicsFormatBC6HSFloatBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC7U)
				format = GraphicsFormat::GraphicsFormatBC7UNormBlock;
			else if (image.getImageType() == ImageType::ImageTypeBC7SRGB)
				format = GraphicsFormat::GraphicsFormatBC7SRGBBlock;
			else
			{
				if (image.bpp() == 24)
					format = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
				else if (image.bpp() == 32)
					format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
				else
				{
					assert(false);
				}
			}

			GraphicsTextureDesc textureDesc;
			textureDesc.setMipLevel(image.getMipLevel());
			textureDesc.setMipSize(image.size());
			textureDesc.setSize(image.width(), image.height());
			textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
			textureDesc.setTexFormat(format);
			textureDesc.setStream(image.data());

			return this->createTexture(textureDesc);
		}
	}

	return nullptr;
}

GraphicsRenderTexturePtr
RenderPipeline::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createRenderTexture(desc);
}

RenderBufferPtr
RenderPipeline::createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept
{
	auto mesh = std::make_shared<RenderBuffer>();
	mesh->setVertexBuffer(vb);
	mesh->setIndexBuffer(ib);
	return mesh;
}

RenderBufferPtr
RenderPipeline::createRenderBuffer(const MeshProperty& mesh) noexcept
{
	auto numVertex = mesh.getNumVertices();
	auto numIndices = mesh.getNumIndices();

	VertexComponents components;

	auto& vertices = mesh.getVertexArray();
	if (!vertices.empty())
	{
		if (vertices.size() == numVertex)
			components.push_back(VertexComponent("POSITION", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	}

	auto& colors = mesh.getColorArray();
	if (!colors.empty())
	{
		if (colors.size() == numVertex)
			components.push_back(VertexComponent("COLOR", 0, GraphicsFormat::GraphicsFormatR32G32B32A32SFloat));
	}

	auto& normals = mesh.getNormalArray();
	if (!normals.empty())
	{
		if (normals.size() == numVertex)
			components.push_back(VertexComponent("NORMAL", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	}

	auto& texcoords = mesh.getTexcoordArray();
	if (!texcoords.empty())
	{
		if (texcoords.size() == numVertex)
			components.push_back(VertexComponent("TEXCOORD", 0, GraphicsFormat::GraphicsFormatR32G32SFloat));
	}

	auto& tangent = mesh.getTangentArray();
	if (!tangent.empty())
	{
		if (tangent.size() == numVertex)
			components.push_back(VertexComponent("TANGENT", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	}

	GraphicsInputLayoutDesc layout;
	layout.setVertexComponents(components);
	layout.setIndexType(GraphicsIndexType::GraphicsIndexTypeUint32);

	GraphicsDataPtr vb;

	if (numVertex)
	{
		std::size_t offset = 0;
		std::size_t stride = layout.getVertexSize();
		std::vector<char> _data(numVertex * stride);

		if (vertices.size() == numVertex)
		{
			char* data = (char*)_data.data();
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (colors.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += stride;
			}

			offset += sizeof(float4);
		}

		if (normals.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (texcoords.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += stride;
			}

			offset += sizeof(float2);
		}

		GraphicsDataDesc _vb;
		_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		_vb.setUsage(UsageFlags::MAP_READ_BIT);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());
		_vb.setStride(stride);

		vb = this->createGraphicsData(_vb);
	}

	GraphicsDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndices > 0)
	{
		GraphicsDataDesc _ib;
		_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		_ib.setUsage(UsageFlags::MAP_READ_BIT);
		_ib.setStride(sizeof(std::uint32_t));
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	auto buffer = this->createRenderBuffer(vb, ib);
	if (buffer)
	{
		buffer->setInputLayout(this->createInputLayout(layout));
		return buffer;
	}

	return nullptr;
}

RenderBufferPtr
RenderPipeline::createRenderBuffer(const MeshPropertys& meshes) noexcept
{
	auto numVertex = 0;
	auto numIndices = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndices += it->getNumIndices();
	}

	GraphicsInputLayoutDesc layout;

	if (!meshes.front()->getVertexArray().empty())
		layout.addComponent(VertexComponent("POSITION", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	if (!meshes.front()->getColorArray().empty())
		layout.addComponent(VertexComponent("COLOR", 0, GraphicsFormat::GraphicsFormatR32G32B32A32SFloat));
	if (!meshes.front()->getNormalArray().empty())
		layout.addComponent(VertexComponent("NORMAL", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	if (!meshes.front()->getTexcoordArray().empty())
		layout.addComponent(VertexComponent("TEXCOORD", 0, GraphicsFormat::GraphicsFormatR32G32SFloat));
	if (!meshes.front()->getTangentArray().empty())
		layout.addComponent(VertexComponent("TANGENT", 0, GraphicsFormat::GraphicsFormatR32G32B32SFloat));
	if (!meshes.front()->getFaceArray().empty())
		layout.setIndexType(GraphicsIndexType::GraphicsIndexTypeUint32);

	GraphicsDataPtr vb;

	if (numVertex)
	{
		std::size_t offsetVertices = 0;
		std::size_t stride = layout.getVertexSize();
		std::vector<char> _data(numVertex * stride);

		const char* mapBuffer = _data.data();

		for (auto& mesh : meshes)
		{
			auto& vertices = mesh->getVertexArray();
			auto& colors = mesh->getColorArray();
			auto& normals = mesh->getNormalArray();
			auto& tangents = mesh->getTangentArray();
			auto& texcoords = mesh->getTexcoordArray();

			std::size_t offset1 = 0;

			if (!vertices.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : vertices)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!colors.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : colors)
				{
					*(float4*)data = it;
					data += stride;
				}

				offset1 += sizeof(float4);
			}

			if (!normals.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : normals)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!texcoords.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : texcoords)
				{
					*(float2*)data = it;
					data += stride;
				}

				offset1 += sizeof(float2);
			}

			if (!tangents.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : tangents)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			offsetVertices += mesh->getNumVertices() * layout.getVertexSize();
		}

		GraphicsDataDesc _vb;
		_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		_vb.setUsage(UsageFlags::MAP_READ_BIT);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());
		_vb.setStride(stride);

		vb = this->createGraphicsData(_vb);
	}

	GraphicsDataPtr ib;

	if (numIndices > 0)
	{
		std::vector<std::uint32_t> faces(numIndices);
		std::size_t offsetIndices = 0;

		for (auto& it : meshes)
		{
			auto& array = it->getFaceArray();
			if (!array.empty())
			{
				std::uint32_t* indices = (std::uint32_t*)faces.data() + offsetIndices;
				for (auto& face : array)
				{
#if !defined(EGLAPI)
					*indices++ = face;
#else
					*indices++ = offsetIndices + face;
#endif
				}

				offsetIndices += array.size();
			}
		}

		GraphicsDataDesc _ib;
		_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		_ib.setUsage(UsageFlags::MAP_READ_BIT);
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStride(sizeof(std::uint32_t));
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	auto buffer = this->createRenderBuffer(vb, ib);
	if (buffer)
	{
		buffer->setInputLayout(this->createInputLayout(layout));
		return buffer;
	}

	return nullptr;
}

GraphicsInputLayoutPtr
RenderPipeline::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createInputLayout(desc);
}

GraphicsDataPtr
RenderPipeline::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsData(desc);
}

void
RenderPipeline::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	assert(_dataManager);
	_dataManager->addRenderData(queue, pass, object);
}

RenderObjects&
RenderPipeline::getRenderData(RenderQueue queue, RenderPass pass) noexcept
{
	assert(_dataManager);
	return _dataManager->getRenderData(queue, pass);
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
	_graphicsContext->present();
}

void
RenderPipeline::setViewport(const Viewport& view, std::size_t i) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setViewport(view, i);
}

const Viewport&
RenderPipeline::getViewport(std::size_t i) const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getViewport(i);
}

void
RenderPipeline::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setRenderTexture(target);
}

void
RenderPipeline::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipeline::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearRenderTexture(flags, color, depth, stencil, i);
}

void
RenderPipeline::discradRenderTexture() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->discardRenderTexture();
}

void
RenderPipeline::readRenderTexture(GraphicsRenderTexturePtr texture, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipeline::blitRenderTexture(GraphicsRenderTexturePtr srcTarget, const Viewport& src, GraphicsRenderTexturePtr destTarget, const Viewport& dest) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->blitRenderTexture(srcTarget, src, destTarget, dest);
}

void
RenderPipeline::setMaterialManager(MaterialManagerPtr manager) noexcept
{
	_materialManager = manager;
}

MaterialManagerPtr
RenderPipeline::getMaterialManager() noexcept
{
	return _materialManager;
}

void
RenderPipeline::setMaterialPass(MaterialPassPtr pass) noexcept
{
	if (_materialPass == pass)
		return;

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
}

MaterialPassPtr
RenderPipeline::getMaterialPass() noexcept
{
	return _materialPass;
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
RenderPipeline::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setInputLayout(buffer->getInputLayout());
	_graphicsContext->setVertexBufferData(buffer->getVertexBuffer());
	_graphicsContext->setIndexBufferData(buffer->getIndexBuffer());
}

void
RenderPipeline::drawRenderBuffer(const GraphicsIndirect& renderable) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->drawRenderBuffer(renderable);
}

void
RenderPipeline::drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->drawRenderBuffer(renderable, first, count);
}

void
RenderPipeline::drawSphere(MaterialPassPtr pass, const float4x4& transform) noexcept
{
	_materialMatModel->assign(transform);
	this->drawMesh(pass, _renderSphere, _renderSphereIndirect);
}

void
RenderPipeline::drawCone(MaterialPassPtr pass, const float4x4& transform) noexcept
{
	_materialMatModel->assign(transform);
	this->drawMesh(pass, _renderCone, _renderConeIndirect);
}

void
RenderPipeline::drawScreenQuad(MaterialPassPtr pass) noexcept
{
	assert(pass);
	this->drawMesh(pass, _renderScreenQuad, _renderScreenQuadIndirect);
}

void
RenderPipeline::drawMesh(MaterialPassPtr pass, RenderBufferPtr buffer, const GraphicsIndirect& renderable) noexcept
{
	this->setMaterialPass(pass);
	this->setRenderBuffer(buffer);
	this->drawRenderBuffer(renderable);
}

void
RenderPipeline::drawRenderQueue(RenderQueue queue, RenderPass pass, MaterialPassPtr material, GraphicsRenderTexturePtr target) noexcept
{
	auto& renderable = this->getRenderData(queue, pass);
	for (auto& it : renderable)
	{
		this->onRenderObjectPre(*it, queue, pass, material);
		this->onRenderObject(*it, queue, pass, material);
		this->onRenderObjectPost(*it, queue, pass, material);
	}
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
RenderPipeline::drawPostProcess(RenderQueue queue, GraphicsTexturePtr source, GraphicsRenderTexturePtr swap, GraphicsRenderTexturePtr dest) noexcept
{
	GraphicsRenderTexturePtr view = dest;
	GraphicsRenderTexturePtr cur;

	auto& drawPostProcess = _postprocessors[queue];
	for (auto& it : drawPostProcess)
	{
		if (it->getActive())
		{
			it->onRenderPre(*this);
			it->onRender(*this, source, view);
			it->onRenderPost(*this);
		}

		cur = view;
		source = view->getGraphicsRenderTextureDesc().getTextures()[0];

		std::swap(swap, view);
	}

	if (cur != dest && cur)
	{
		std::uint32_t w = source->getGraphicsTextureDesc().getWidth();
		std::uint32_t h = source->getGraphicsTextureDesc().getHeight();
		Viewport viewport(0, 0, w, h);
		this->blitRenderTexture(cur, viewport, dest, viewport);
	}
}

void
RenderPipeline::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept
{
}

void
RenderPipeline::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept
{
}

void
RenderPipeline::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) noexcept
{
	auto pass = _pass ? _pass : object.downcast<RenderMesh>()->getMaterial()->getTech(queue)->getPass(passType);

	if (pass)
	{
		_materialMatModel->assign(object.getTransform());
		_materialMatModelInverse->assign(object.getTransformInverse());
		_materialMatModelInverseTranspose->assign(object.getTransformInverseTranspose());

		this->drawMesh(pass, object.downcast<RenderMesh>()->getRenderBuffer(), *object.downcast<RenderMesh>()->getGraphicsIndirect());

		auto listener = object.getOwnerListener();
		if (listener)
			listener->onRenderObject(*this, *this->getCamera());
	}
}

_NAME_END