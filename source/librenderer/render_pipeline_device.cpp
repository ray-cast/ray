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
#include <ray/render_pipeline_device.h>
#include <ray/render_pipeline.h>

#include <ray/render_object_manager.h>
#include <ray/render_post_process.h>
#include <ray/render_scene.h>
#include <ray/render_mesh.h>
#include <ray/light.h>
#include <ray/geometry.h>
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
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_input_layout.h>
#include <ray/graphics_shader.h>

_NAME_BEGIN

__ImplementSubClass(RenderPipelineDevice, rtti::Interface, "RenderPipelineDevice")

RenderPipelineDevice::RenderPipelineDevice() noexcept
{
}	

RenderPipelineDevice::~RenderPipelineDevice() noexcept
{
	this->close();
}

bool
RenderPipelineDevice::open(GraphicsDeviceType type) noexcept
{
	GraphicsDeviceDesc deviceDesc;
	deviceDesc.setDeviceType(type);

	_graphicsDevice = GraphicsSystem::instance()->createDevice(deviceDesc);
	if (!_graphicsDevice)
		return false;

	_materialManager = std::make_shared<MaterialManager>();
	if (!_materialManager->setup(_graphicsDevice))
		return false;

	return true;
}

void
RenderPipelineDevice::close() noexcept
{
	_materialManager.reset();
	_graphicsDevice.reset();
}

GraphicsDeviceType 
RenderPipelineDevice::getDeviceType() const noexcept
{
	return _graphicsDevice->getGraphicsDeviceDesc().getDeviceType();
}

RenderPipelinePtr
RenderPipelineDevice::createRenderPipeline(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	auto pipelineContext = std::make_shared<RenderPipeline>();
	if (pipelineContext->setup(this->downcast<RenderPipelineDevice>(), window, w, h))
		return pipelineContext;
	return nullptr;
}

MaterialPtr
RenderPipelineDevice::createMaterial(const std::string& name) noexcept
{
	assert(_materialManager);
	return _materialManager->createMaterial(name);
}

void
RenderPipelineDevice::destroyMaterial(MaterialPtr material) noexcept
{
	assert(_materialManager);
	return _materialManager->destroyMaterial(material);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createTexture(desc);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_graphicsDevice);
	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(w);
	textureDesc.setHeight(h);
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	return _graphicsDevice->createTexture(textureDesc);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(const std::string& name, GraphicsTextureDim dim) noexcept
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
				auto imageFormat = image.getImageFormat();
				if (imageFormat != ImageFormat::ImageFormatUnknow)
				{
					if (imageFormat == ImageFormat::ImageFormatR8G8B8)
						format = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
					else if (imageFormat == ImageFormat::ImageFormatR8G8B8A8)
						format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
					else if (imageFormat == ImageFormat::ImageFormatB8G8R8)
						format = GraphicsFormat::GraphicsFormatB8G8R8UNorm;
					else if (imageFormat == ImageFormat::ImageFormatB8G8R8A8)
						format = GraphicsFormat::GraphicsFormatB8G8R8A8UNorm;
					else if (imageFormat == ImageFormat::ImageFormatL8)
						format = GraphicsFormat::GraphicsFormatR8UNorm;
					else if (imageFormat == ImageFormat::ImageFormatL8A8)
						format = GraphicsFormat::GraphicsFormatR8G8UNorm;
					else
					{
						assert(false);
						return nullptr;
					}
				}
				else
				{
					if (image.bpp() == 24)
						format = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
					else if (image.bpp() == 32)
						format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
					else
					{
						assert(false);
						return nullptr;
					}
				}
			}

			GraphicsTextureDesc textureDesc;
			textureDesc.setSize(image.width(), image.height(), image.depth());
			textureDesc.setTexDim(dim);
			textureDesc.setTexFormat(format);
			textureDesc.setStream(image.data());
			textureDesc.setStreamSize(image.size());
			textureDesc.setMipLevel(image.getMipLevel());

			return this->createTexture(textureDesc);
		}
	}

	return nullptr;
}

GraphicsSwapchainPtr
RenderPipelineDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createSwapchain(desc);
}

GraphicsContextPtr
RenderPipelineDevice::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createDeviceContext(desc);
}

GraphicsFramebufferLayoutPtr
RenderPipelineDevice::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createFramebufferLayout(desc);
}

GraphicsFramebufferPtr
RenderPipelineDevice::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createFramebuffer(desc);
}

RenderMeshPtr
RenderPipelineDevice::createRenderMesh(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept
{
	auto mesh = std::make_shared<RenderMesh>();
	mesh->setVertexBuffer(vb);
	mesh->setIndexBuffer(ib);
	return mesh;
}

RenderMeshPtr
RenderPipelineDevice::createRenderMesh(const MeshProperty& mesh) noexcept
{
	auto numVertex = mesh.getNumVertices();
	auto numIndices = mesh.getNumIndices();

	const auto& vertices = mesh.getVertexArray();
	const auto& colors = mesh.getColorArray();
	const auto& normals = mesh.getNormalArray();
	const auto& tangents = mesh.getTangentArray();
	const auto& texcoords = mesh.getTexcoordArray();

	std::uint32_t inputSize = 0;
	if (!vertices.empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!colors.empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	if (!normals.empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!tangents.empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!texcoords.empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);

	GraphicsDataPtr vb;

	if (numVertex)
	{
		std::size_t offset = 0;
		std::vector<char> _data(numVertex * inputSize);

		if (vertices.size() == numVertex)
		{
			char* data = (char*)_data.data();
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset += sizeof(float3);
		}

		if (colors.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += inputSize;
			}

			offset += sizeof(float4);
		}

		if (normals.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset += sizeof(float3);
		}

		if (tangents.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : tangents)
			{
				*(float3*)data = it;
				data += inputSize;
			}
			
			offset += sizeof(float3);
		}

		if (texcoords.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += inputSize;
			}

			offset += sizeof(float2);
		}

		GraphicsDataDesc _vb;
		_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		_vb.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());
		_vb.setStride(inputSize);

		vb = this->createGraphicsData(_vb);
	}

	GraphicsDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndices > 0)
	{
		GraphicsDataDesc _ib;
		_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		_ib.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
		_ib.setStride(sizeof(std::uint32_t));
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	return this->createRenderMesh(vb, ib);
}

RenderMeshPtr
RenderPipelineDevice::createRenderMesh(const MeshPropertys& meshes) noexcept
{
	auto numVertex = 0;
	auto numIndices = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndices += it->getNumIndices();
	}

	std::uint32_t inputSize = 0;
	if (!meshes.front()->getVertexArray().empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!meshes.front()->getColorArray().empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	if (!meshes.front()->getNormalArray().empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!meshes.front()->getTangentArray().empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!meshes.front()->getTexcoordArray().empty())
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);

	GraphicsDataPtr vb;

	if (numVertex)
	{
		std::size_t offsetVertices = 0;
		std::size_t stride = inputSize;
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

			offsetVertices += mesh->getNumVertices() * inputSize;
		}

		GraphicsDataDesc _vb;
		_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		_vb.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
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
					*indices++ = face;
				}

				offsetIndices += array.size();
			}
		}

		GraphicsDataDesc _ib;
		_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		_ib.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStride(sizeof(std::uint32_t));
		_ib.setStreamSize(faces.size() * sizeof(std::uint32_t));

		ib = this->createGraphicsData(_ib);
	}

	return this->createRenderMesh(vb, ib);
}

GraphicsInputLayoutPtr
RenderPipelineDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createInputLayout(desc);
}

GraphicsDataPtr
RenderPipelineDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsData(desc);
}

MaterialVariantPtr
RenderPipelineDevice::createSemantic(const std::string& name, GraphicsUniformType type) noexcept
{
	assert(_materialManager);
	return _materialManager->createSemantic(name, type);
}

void
RenderPipelineDevice::destroySemantic(MaterialVariantPtr semantic) const noexcept
{
	assert(_materialManager);
	return _materialManager->destroySemantic(semantic);
}

MaterialVariantPtr
RenderPipelineDevice::getSemantic(const std::string& semantic) const noexcept
{
	assert(_materialManager);
	return _materialManager->getSemantic(semantic);
}

_NAME_END