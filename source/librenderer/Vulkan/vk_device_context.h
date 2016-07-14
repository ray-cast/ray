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
#ifndef _H_VK_DEVICE_CONTEXT_H_
#define _H_VK_DEVICE_CONTEXT_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanDeviceContext final : public GraphicsContext
{
	__DeclareSubClass(VulkanDeviceContext, GraphicsContext)
public:
	VulkanDeviceContext() noexcept;
	virtual ~VulkanDeviceContext() noexcept;

	bool setup(const GraphicsContextDesc& desc) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setViewport(std::uint32_t i, const Viewport& viewport) noexcept;
	const Viewport& getViewport(std::uint32_t i) const noexcept;

	void setScissor(std::uint32_t i, const Scissor& scissor) noexcept;
	const Scissor& getScissor(std::uint32_t i) const noexcept;

	void setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
	std::uint32_t getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept;

	void setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept;
	std::uint32_t getStencilReference(GraphicsStencilFaceFlagBits face) noexcept;

	void setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
	std::uint32_t getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept;

	void setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept;
	GraphicsPipelinePtr getRenderPipeline() const noexcept;

	void setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept;
	GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

	void setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept;
	GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept;

	void setIndexBufferData(GraphicsDataPtr data, std::intptr_t offset, GraphicsIndexType indexType) noexcept;
	GraphicsDataPtr getIndexBufferData() const noexcept;

	void generateMipmap(GraphicsTexturePtr texture) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discardFramebuffer(std::uint32_t i) noexcept;
	GraphicsFramebufferPtr getFramebuffer() const noexcept;

	void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
	void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;

	void present() noexcept;

private:
	bool initCommandList() noexcept;

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanDeviceContext(const VulkanDeviceContext&) noexcept = delete;
	VulkanDeviceContext& operator=(const VulkanDeviceContext&) noexcept = delete;

private:

	std::vector<Scissor> _scissor;
	std::vector<Viewport> _viewports;
	std::vector<ClearValue> _clearValues;

	GraphicsSwapchainPtr _swapchain;

	GraphicsCommandListPtr _commandList;
	GraphicsCommandPoolPtr _commandPool;
	GraphicsCommandQueuePtr _commandQueue;

	GraphicsDataPtr _indexBuffer;
	GraphicsDataPtr _vertexBuffer;
	GraphicsPipelinePtr _pipeline;
	GraphicsDescriptorSetPtr _descriptorSet;
	GraphicsFramebufferPtr _framebuffer;
	GraphicsInputLayoutPtr _inputLayout;

	VulkanDeviceWeakPtr _device;
};

_NAME_END

#endif