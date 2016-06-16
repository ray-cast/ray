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
	if (pipelineContext->setup(this->downcast_pointer<RenderPipelineDevice>(), window, w, h, interval))
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
RenderPipelineDevice::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter, GraphicsSamplerWrap warp) noexcept
{
	assert(_graphicsDevice);
	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(w);
	textureDesc.setHeight(h);
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	textureDesc.setSamplerFilter(filter);
	textureDesc.setSamplerWrap(warp);
	return _graphicsDevice->createTexture(textureDesc);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter, GraphicsSamplerWrap warp) noexcept
{
	return _materialManager->createTexture(name, dim, filter, warp);
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

GraphicsDataPtr
RenderPipelineDevice::createIndexBuffer(const MeshProperty& mesh) noexcept
{
	std::size_t numIndices = mesh.getNumIndices();
	for (auto& it : mesh.getChildren())
		numIndices += it->getNumIndices();

	if (numIndices == 0)
		return nullptr;

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
	_ib.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_ib.setStream((std::uint8_t*)faces.data());
	_ib.setStreamSize(faces.size());

	return this->createGraphicsData(_ib);
}

GraphicsDataPtr
RenderPipelineDevice::createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	std::size_t numVertex = mesh.getNumVertices();
	for (auto& it : mesh.getChildren())
		numVertex += it->getNumVertices();

	if (numVertex == 0)
		return nullptr;

	std::uint32_t inputSize = 0;
	if (!mesh.getVertexArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!mesh.getTangentQuatArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!mesh.getColorArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	if (!mesh.getTexcoordArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);
	if (!mesh.getWeightArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
	{
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UScaled);
	}		

	auto _buildVertexBuffer = [&](const MeshProperty& mesh, std::size_t& offsetVertices, const std::vector<char>& _vbo)
	{
		auto& vertices = mesh.getVertexArray();
		auto& tangentQuats = mesh.getTangentQuatArray();
		auto& colors = mesh.getColorArray();
		auto& texcoords = mesh.getTexcoordArray();
		auto& weight = mesh.getWeightArray();

		std::size_t offset1 = 0;
		std::uint8_t* mapBuffer = (std::uint8_t*)_vbo.data();

		if (!vertices.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float3);
		}

		if (!tangentQuats.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : tangentQuats)
			{
				float4 unorm = math::snorm2unorm(it);

				data[0] = math::fpToInt8UNORM(unorm.x);
				data[1] = math::fpToInt8UNORM(unorm.y);
				data[2] = math::fpToInt8UNORM(unorm.z);
				data[3] = math::fpToInt8UNORM(unorm.w);
				
				data += inputSize;
			}

			offset1 += 4;
		}

		if (!colors.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float4);
		}

		if (!texcoords.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float2);
		}

		if (!weight.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : weight)
			{
				data[0] = math::fpToInt8UNORM(it.weight1);
				data[1] = math::fpToInt8UNORM(it.weight2);
				data[2] = math::fpToInt8UNORM(it.weight3);
				data[3] = math::fpToInt8UNORM(it.weight4);
				data[4] = it.bone1;
				data[5] = it.bone2;
				data[6] = it.bone3;
				data[7] = it.bone4;
				
				data += inputSize;
			}

			offset1 += 8;
		}

		offsetVertices += mesh.getNumVertices() * inputSize;
	};

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
	_vb.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_vb.setStream((std::uint8_t*)_data.data());
	_vb.setStreamSize(_data.size());

	return this->createGraphicsData(_vb);
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

_NAME_END