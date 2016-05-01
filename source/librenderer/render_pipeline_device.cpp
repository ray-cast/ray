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
RenderPipelineDevice::createRenderPipeline(WindHandle window, std::uint32_t w, std::uint32_t h, GraphicsSwapInterval interval) noexcept
{
	auto pipelineContext = std::make_shared<RenderPipeline>();
	if (pipelineContext->setup(this->downcast<RenderPipelineDevice>(), window, w, h, interval))
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
RenderPipelineDevice::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter) noexcept
{
	return _materialManager->createTexture(name, dim, filter);
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
RenderPipelineDevice::createRenderMesh(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	std::uint32_t inputSize = 0;
	if (!mesh.getVertexArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!mesh.getColorArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	if (!mesh.getNormalArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitNormal)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!mesh.getTangentArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!mesh.getTexcoordArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);
	if (!mesh.getTexcoordArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
	{
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	}
		

	auto _buildVertexBuffer = [&](const MeshProperty& mesh, std::size_t& offsetVertices, const std::vector<char>& _vbo)
	{
		auto& vertices = mesh.getVertexArray();
		auto& colors = mesh.getColorArray();
		auto& normals = mesh.getNormalArray();
		auto& tangents = mesh.getTangentArray();
		auto& texcoords = mesh.getTexcoordArray();
		auto& weight = mesh.getWeightArray();

		std::size_t offset1 = 0;
		char* mapBuffer = (char*)_vbo.data();

		if (!vertices.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float3);
		}

		if (!colors.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float4);
		}

		if (!normals.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitNormal)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float3);
		}

		if (!tangents.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : tangents)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float3);
		}

		if (!texcoords.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float2);
		}

		if (!weight.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
		{
			char* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : weight)
			{
				*(VertexWeight*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float4);
		}

		offsetVertices += mesh.getNumVertices() * inputSize;
	};

	auto _buildIndiceBuffer = [&](const MeshProperty& mesh, std::size_t& offsetIndices, const std::vector<char>& _ibo)
	{
		auto& array = mesh.getFaceArray();
		if (!array.empty())
		{
			char* indices = (char*)_ibo.data() + offsetIndices;
			std::memcpy(indices, array.data(), array.size() * sizeof(std::uint32_t));
			offsetIndices += mesh.getFaceArray().size() * sizeof(std::uint32_t);
		}
	};

	auto numVertex = mesh.getNumVertices();
	auto numIndices = mesh.getNumIndices();

	for (auto& it : mesh.getChildren())
	{
		numVertex += it->getNumVertices();
		numIndices += it->getNumIndices();
	}

	GraphicsDataPtr vb;
	GraphicsDataPtr ib;

	if (numVertex)
	{
		std::vector<char> _data(numVertex * inputSize);
		std::size_t offsetVertices = 0;

		_buildVertexBuffer(mesh, offsetVertices, _data);

		auto subMeshCount = mesh.getChildCount();
		for (std::size_t i = 0; i < subMeshCount; i++)
		{
			auto submesh = mesh.getChildren()[i];
			_buildVertexBuffer(*submesh, offsetVertices, _data);
		}

		GraphicsDataDesc _vb;
		_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		_vb.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
		_vb.setStream((std::uint8_t*)_data.data());
		_vb.setStreamSize(_data.size());
		_vb.setStride(inputSize);

		vb = this->createGraphicsData(_vb);
	}

	if (numIndices > 0)
	{
		std::vector<char> faces(numIndices * sizeof(std::uint32_t));
		std::size_t offsetIndices = 0;

		_buildIndiceBuffer(mesh, offsetIndices, faces);

		auto subMeshCount = mesh.getChildCount();
		for (std::size_t i = 0; i < subMeshCount; i++)
		{
			auto submesh = mesh.getChildren()[i];
			_buildIndiceBuffer(*submesh, offsetIndices, faces);
		}

		GraphicsDataDesc _ib;
		_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		_ib.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit);
		_ib.setStream((std::uint8_t*)faces.data());
		_ib.setStride(sizeof(std::uint32_t));
		_ib.setStreamSize(faces.size());

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

GraphicsPipelinePtr
RenderPipelineDevice::createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createRenderPipeline(desc);
}

GraphicsDataPtr
RenderPipelineDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsData(desc);
}

MaterialParamPtr
RenderPipelineDevice::createSemantic(const std::string& name, GraphicsUniformType type) noexcept
{
	assert(_materialManager);
	return _materialManager->createSemantic(name, type);
}

void
RenderPipelineDevice::destroySemantic(MaterialParamPtr semantic) const noexcept
{
	assert(_materialManager);
	return _materialManager->destroySemantic(semantic);
}

MaterialParamPtr
RenderPipelineDevice::getSemantic(const std::string& semantic) const noexcept
{
	assert(_materialManager);
	return _materialManager->getSemantic(semantic);
}

_NAME_END