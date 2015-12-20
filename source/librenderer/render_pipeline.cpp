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
#include <ray/render_data_manager.h>
#include <ray/render_post_process.h>
#include <ray/render_scene.h>
#include <ray/graphics_texture.h>
#include <ray/render_object.h>
#include <ray/render_buffer.h>
#include <ray/material.h>
#include <ray/material_manager.h>
#include <ray/model.h>
#include <ray/image.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/resource.h>

#if defined(_BUILD_OPENGL_CORE)
#	include "OpenGL Core/ogl_device.h"
#	define RenderDevice OGLDevice
#elif defined(_BUILD_OPENGL_ES3)
#	include "OpenGL ES3/egl3_device.h"
#	define RenderDevice EGL3Device
#elif defined(_BUILD_OPENGL_ES2)
#	include "OpenGL ES2/egl2_device.h"
#	define RenderDevice EGL2Device
#endif

_NAME_BEGIN

RenderPipeline::RenderPipeline() noexcept
{
}

RenderPipeline::~RenderPipeline() noexcept
{
	this->close();
}

void
RenderPipeline::open(WindHandle window, std::uint32_t w, std::uint32_t h) except
{
	_graphicsDevice = std::make_shared<RenderDevice>();
	_graphicsContext = _graphicsDevice->createGraphicsContext(window);

	_materialManager = std::make_shared<MaterialManager>();
	_materialManager->open(_graphicsDevice);

	_materialMatModel = _materialManager->createSemantic("matModel", ShaderVariantType::Float4x4);
	_materialMatModelInverse = _materialManager->createSemantic("matModelInverse", ShaderVariantType::Float4x4);
	_materialMatModelInverseTranspose = _materialManager->createSemantic("matModelInverseTranspose", ShaderVariantType::Float4x4);
	_materialMatProject = _materialManager->createSemantic("matProject", ShaderVariantType::Float4x4);
	_materialMatProjectInverse = _materialManager->createSemantic("matProjectInverse", ShaderVariantType::Float4x4);
	_materialMatView = _materialManager->createSemantic("matView", ShaderVariantType::Float4x4);
	_materialMatViewInverse = _materialManager->createSemantic("matViewInverse", ShaderVariantType::Float4x4);
	_materialMatViewInverseTranspose = _materialManager->createSemantic("matViewInverseTranspose", ShaderVariantType::Float4x4);
	_materialMatViewProject = _materialManager->createSemantic("matViewProject", ShaderVariantType::Float4x4);
	_materialMatViewProjectInverse = _materialManager->createSemantic("matViewProjectInverse", ShaderVariantType::Float4x4);
	_materialCameraAperture = _materialManager->createSemantic("CameraAperture", ShaderVariantType::Float);
	_materialCameraFar = _materialManager->createSemantic("CameraFar", ShaderVariantType::Float);
	_materialCameraNear = _materialManager->createSemantic("CameraNear", ShaderVariantType::Float);
	_materialCameraView = _materialManager->createSemantic("CameraView", ShaderVariantType::Float3);
	_materialCameraPosition = _materialManager->createSemantic("CameraPosition", ShaderVariantType::Float3);
	_materialCameraDirection = _materialManager->createSemantic("CameraDirection", ShaderVariantType::Float3);

	_dataManager = std::make_shared<DefaultRenderDataManager>();

	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_renderSceneQuad = this->createRenderBuffer(mesh);
	_renderSceneQuadIndirect.startVertice = 0;
	_renderSceneQuadIndirect.numVertices = mesh.getNumVertices();
	_renderSceneQuadIndirect.startIndice = 0;
	_renderSceneQuadIndirect.numIndices = mesh.getNumIndices();
	_renderSceneQuadIndirect.numInstances = 0;

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
	_materialCameraView.reset();
	_materialCameraPosition.reset();
	_materialCameraDirection.reset();

	_renderSceneQuad.reset();
	_renderSphere.reset();
	_renderCone.reset();
	_dataManager.reset();
	_graphicsDevice.reset();
}

void
RenderPipeline::setDefaultGraphicsContext(GraphicsContextPtr context) noexcept
{
	_graphicsContext = context;
}

GraphicsContextPtr
RenderPipeline::getDefaultGraphicsContext() const noexcept
{
	return _graphicsContext;
}

void
RenderPipeline::setRenderDataManager(RenderDataManagerPtr manager) noexcept
{
	_dataManager = manager;
}

RenderDataManagerPtr
RenderPipeline::getRenderDataManagerPtr() const noexcept
{
	return _dataManager;
}

void
RenderPipeline::setWireframeMode(bool enable) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setWireframeMode(enable);
}

bool
RenderPipeline::getWireframeMode() const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getWireframeMode();
}

void
RenderPipeline::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setSwapInterval(interval);
}

SwapInterval
RenderPipeline::getSwapInterval() const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getSwapInterval();
}

void
RenderPipeline::setCamera(CameraPtr camera) noexcept
{
	if (camera->getCameraOrder() != CameraOrder::CO_SHADOW)
	{
		if (camera->getCameraType() == CameraType::CT_PERSPECTIVE)
		{
			float ratio;
			float aperture;
			float znear;
			float zfar;
			camera->getPerspective(aperture, ratio, znear, zfar);

			std::uint32_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makePerspective(aperture, znear, zfar, ratio);
			}
		}
		else
		{
			float left;
			float right;
			float top;
			float bottom;
			float ratio;
			float znear;
			float zfar;
			camera->getOrtho(left, right, top, bottom, ratio, znear, zfar);

			std::uint32_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makeOrtho(left, right, top, bottom, znear, zfar, ratio);
			}
		}
	}

	float ratio;
	float aperture;
	float znear;
	float zfar;
	camera->getPerspective(aperture, ratio, znear, zfar);

	_materialCameraNear->assign(znear);
	_materialCameraFar->assign(zfar);
	_materialCameraAperture->assign(aperture);
	_materialCameraView->assign(camera->getLookAt());
	_materialCameraPosition->assign(camera->getTranslate());
	_materialCameraDirection->assign(camera->getLookAt() - camera->getTranslate());
	_materialMatView->assign(camera->getView());
	_materialMatViewInverse->assign(camera->getViewInverse());
	_materialMatViewInverseTranspose->assign(camera->getViewInverseTranspose());
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
RenderPipeline::setModelMatrix(const float4x4& m) noexcept
{
	_materialMatModel->assign(m);
}

const float4x4&
RenderPipeline::getModelMatrix() const noexcept
{
	return _materialMatModel->getFloat4x4();
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
RenderPipeline::drawMesh(MaterialPassPtr pass, RenderBufferPtr buffer, const RenderIndirect& renderable) noexcept
{
	this->setMaterialPass(pass);
	this->setRenderBuffer(buffer);
	this->drawRenderBuffer(renderable);
}

GraphicsRenderTexturePtr
RenderPipeline::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createRenderTexture(desc);
}

GraphicsRenderTexturePtr 
RenderPipeline::createRenderTexture(std::uint32_t w, std::uint32_t h, TextureDim dim, TextureFormat format) noexcept
{
	assert(_graphicsDevice);
	auto texture = this->createTexture(w, h, dim, format);
	if (texture)
	{
		GraphicsRenderTextureDesc framebufferDesc;
		framebufferDesc.setGraphicsTexture(texture);
		return _graphicsDevice->createRenderTexture(framebufferDesc);
	}

	return nullptr;
}

GraphicsMultiRenderTexturePtr
RenderPipeline::createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createMultiRenderTexture(desc);
}

void
RenderPipeline::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setRenderTexture(target);
}

void
RenderPipeline::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setMultiRenderTexture(target);
}

void
RenderPipeline::setRenderTextureLayer(GraphicsRenderTexturePtr target, int layer) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setRenderTextureLayer(target, layer);
}

void
RenderPipeline::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipeline::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
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
RenderPipeline::readRenderTexture(GraphicsRenderTexturePtr texture, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
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

GraphicsTexturePtr
RenderPipeline::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsTexture(desc);
}

GraphicsTexturePtr 
RenderPipeline::createTexture(std::uint32_t w, std::uint32_t h, TextureDim dim, TextureFormat format) noexcept
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
RenderPipeline::createTexture(const std::string& name) except
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, name))
	{
		Image image;
		if (image.load(*stream))
		{
			TextureFormat format = TextureFormat::R8G8B8A8;

			if (image.getImageType() == ImageType::dds1)
				format = TextureFormat::RGBA_DXT1;
			else if (image.getImageType() == ImageType::dds3)
				format = TextureFormat::RGBA_DXT3;
			else if (image.getImageType() == ImageType::dds5)
				format = TextureFormat::RGBA_DXT5;
			else if (image.getImageType() == ImageType::ati2)
				format = TextureFormat::RG_ATI2;
			else
			{
				if (image.bpp() == 24)
					format = TextureFormat::R8G8B8;
				else if (image.bpp() == 32)
					format = TextureFormat::R8G8B8A8;
				else
				{
					assert(false);
				}
			}

			GraphicsTextureDesc textureDesc;
			textureDesc.setMipLevel(image.getMipLevel());
			textureDesc.setMipSize(image.size());
			textureDesc.setSize(image.width(), image.height());
			textureDesc.setTexDim(TextureDim::DIM_2D);
			textureDesc.setTexFormat(format);
			textureDesc.setStream(image.data());

			return this->createTexture(textureDesc);
		}
	}

	return nullptr;
}

MaterialPtr
RenderPipeline::createMaterial(const std::string& name) except
{
	return _materialManager->createMaterial(name);
}

void
RenderPipeline::setMaterialPass(MaterialPassPtr pass) noexcept
{
	_materialManager->setMaterialPass(pass);

	GraphicsTexturePtr bindTextures[MAX_TEXTURE_UNIT];
	GraphicsSamplerPtr bindSamplers[MAX_SAMPLER_UNIT];

	auto& textures = pass->getTextures();
	auto  textureCount = std::min((std::size_t)MAX_TEXTURE_UNIT, textures.size());
	for (std::size_t i = 0; i < textureCount; i++)
	{
		auto& uniforms = textures[i]->getShaderUniform();
		auto texture = textures[i]->getTexture();

		for (auto& it : uniforms)
			it->assign((int)i);

		bindTextures[i] = textures[i]->getTexture();
		bindSamplers[i] = 0;
	}

	_graphicsContext->setGraphicsTexture(bindTextures, 0, textureCount);
	_graphicsContext->setGraphicsSampler(bindSamplers, 0, textureCount);
	_graphicsContext->setGraphicsState(pass->getGraphicsState());
	_graphicsContext->setShaderObject(pass->getShaderObject());
}

void
RenderPipeline::setMaterialManager(MaterialManagerPtr manager) noexcept
{
	_materialManager = manager;
}

MaterialPassPtr
RenderPipeline::getMaterialPass() noexcept
{
	return _materialManager->getMaterialPass();
}

MaterialManagerPtr
RenderPipeline::getMaterialManager() noexcept
{
	return _materialManager;
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
RenderPipeline::createRenderBuffer(const MeshProperty& mesh) except
{
	auto numVertex = mesh.getNumVertices();
	auto numIndices = mesh.getNumIndices();

	VertexComponents components;

	auto& vertices = mesh.getVertexArray();
	if (!vertices.empty())
	{
		if (vertices.size() == numVertex)
			components.push_back(VertexComponent(VertexFormat::Float3, VertexAttrib::Position));
	}

	auto& colors = mesh.getColorArray();
	if (!colors.empty())
	{
		if (colors.size() == numVertex)
			components.push_back(VertexComponent(VertexFormat::Float4, VertexAttrib::Diffuse));
	}

	auto& normals = mesh.getNormalArray();
	if (!normals.empty())
	{
		if (normals.size() == numVertex)
			components.push_back(VertexComponent(VertexFormat::Float3, VertexAttrib::Normal));
	}

	auto& texcoords = mesh.getTexcoordArray();
	if (!texcoords.empty())
	{
		if (texcoords.size() == numVertex)
			components.push_back(VertexComponent(VertexFormat::Float2, VertexAttrib::Texcoord));
	}

	auto& tangent = mesh.getTangentArray();
	if (!tangent.empty())
	{
		if (tangent.size() == numVertex)
			components.push_back(VertexComponent(VertexFormat::Float3, VertexAttrib::Tangent));
	}

	GraphicsLayoutDesc layout;
	layout.setVertexComponents(components);
	layout.setIndexType(IndexType::Uint32);

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
		_vb.setUsage(UsageFlags::MAP_READ_BIT);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());

		vb = this->createGraphicsData(_vb);
	}

	GraphicsDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndices > 0)
	{
		GraphicsDataDesc _ib;
		_ib.setType(GraphicsStream::IBO);
		_ib.setUsage(UsageFlags::MAP_READ_BIT);
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	auto buffer = this->createRenderBuffer(vb, ib);
	if (buffer)
	{
		buffer->setGraphicsLayout(this->createGraphicsLayout(layout));
		return buffer;
	}

	return nullptr;
}

RenderBufferPtr
RenderPipeline::createRenderBuffer(const MeshPropertys& meshes) except
{
	auto numVertex = 0;
	auto numIndices = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndices += it->getNumIndices();
	}

	GraphicsLayoutDesc layout;

	if (!meshes.front()->getVertexArray().empty())
		layout.addComponent(VertexComponent(VertexFormat::Float3, VertexAttrib::Position));
	if (!meshes.front()->getColorArray().empty())
		layout.addComponent(VertexComponent(VertexFormat::Float4, VertexAttrib::Diffuse));
	if (!meshes.front()->getNormalArray().empty())
		layout.addComponent(VertexComponent(VertexFormat::Float3, VertexAttrib::Normal));
	if (!meshes.front()->getTexcoordArray().empty())
		layout.addComponent(VertexComponent(VertexFormat::Float2, VertexAttrib::Texcoord));
	if (!meshes.front()->getTangentArray().empty())
		layout.addComponent(VertexComponent(VertexFormat::Float3, VertexAttrib::Tangent));
	if (!meshes.front()->getFaceArray().empty())
		layout.setIndexType(IndexType::Uint32);

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
		_vb.setUsage(UsageFlags::MAP_READ_BIT);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());

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
		_ib.setType(GraphicsStream::IBO);
		_ib.setUsage(UsageFlags::MAP_READ_BIT);
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	auto buffer = this->createRenderBuffer(vb, ib);
	if (buffer)
	{
		buffer->setGraphicsLayout(this->createGraphicsLayout(layout));
		return buffer;
	}

	return nullptr;
}

GraphicsLayoutPtr
RenderPipeline::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsLayout(desc);
}

GraphicsDataPtr
RenderPipeline::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsData(desc);
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
RenderPipeline::setRenderBuffer(RenderBufferPtr buffer) except
{
	assert(_graphicsDevice);
	_graphicsContext->setGraphicsLayout(buffer->getGraphicsLayout());
	_graphicsContext->setVertexBufferData(buffer->getVertexBuffer());
	_graphicsContext->setIndexBufferData(buffer->getIndexBuffer());
}

void
RenderPipeline::drawRenderBuffer(const RenderIndirect& renderable) except
{
	assert(_graphicsContext);
	_graphicsContext->drawRenderBuffer(renderable);
}

void
RenderPipeline::drawRenderBuffer(const RenderIndirects& renderable) except
{
	assert(_graphicsDevice);
	_graphicsContext->drawRenderBuffer(renderable);
}

void
RenderPipeline::drawSceneQuad(MaterialPassPtr pass) noexcept
{
	assert(pass);
	this->drawMesh(pass, _renderSceneQuad, _renderSceneQuadIndirect);
}

void
RenderPipeline::drawSphere(MaterialPassPtr pass) noexcept
{
	this->drawMesh(pass, _renderSphere, _renderSphereIndirect);
}

void
RenderPipeline::drawCone(MaterialPassPtr pass) noexcept
{
	this->drawMesh(pass, _renderCone, _renderConeIndirect);
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

	if (pass == RenderPass::RP_POSTPROCESS)
	{
		auto postprocess = _postprocessors[queue];
		for (auto& it : postprocess)
		{
			it->onRenderPre(*this);
			it->onRender(*this, target);
			it->onRenderPost(*this);
		}
	}
}

void
RenderPipeline::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_width != width || _height != height)
	{
		auto& renderPostProcess = _postprocessors[RenderQueue::RQ_POSTPROCESS];
		for (auto& it : renderPostProcess)
		{
			if (it->getActive())
				it->onResolutionChangeBefore(*this);
		}

		_width = width;
		_height = height;

		for (auto& it : renderPostProcess)
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
RenderPipeline::addPostProcess(RenderPostProcessPtr postprocess) except
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& renderPostProcess = _postprocessors[renderQueue];

	if (std::find(renderPostProcess.begin(), renderPostProcess.end(), postprocess) == renderPostProcess.end())
	{
		postprocess->_setRenderPipeline(this);
		postprocess->setActive(true);
		renderPostProcess.push_back(postprocess);
	}
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& renderPostProcess = _postprocessors[renderQueue];

	auto it = std::find(renderPostProcess.begin(), renderPostProcess.end(), postprocess);
	if (it != renderPostProcess.end())
	{
		postprocess->setActive(false);
		renderPostProcess.erase(it);
	}
}

void
RenderPipeline::renderPostProcess(GraphicsRenderTexturePtr renderTexture) except
{
	auto& renderPostProcess = _postprocessors[RenderQueue::RQ_POSTPROCESS];

	for (auto& it : renderPostProcess)
	{
		if (it->getActive())
		{
			it->onRenderPre(*this);
			it->onRender(*this, renderTexture);
			it->onRenderPost(*this);
		}
	}
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
	_graphicsContext->present();
	_graphicsContext->renderEnd();
}

void
RenderPipeline::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) except
{
	auto pass = _pass ? _pass : object.getMaterial()->getTech(queue)->getPass(passType);

	if (pass)
	{
		_materialMatModel->assign(object.getTransform());
		_materialMatModelInverse->assign(object.getTransformInverse());
		_materialMatModelInverseTranspose->assign(object.getTransformInverseTranspose());

		this->drawMesh(pass, object.getRenderBuffer(), *object.getRenderIndirect());

		auto listener = object.getOwnerListener();
		if (listener)
			listener->onRenderObject(*this->getCamera());
	}
}

_NAME_END