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

	void setViewport(const Viewport& viewport, std::size_t i) noexcept;
	const Viewport& getViewport(std::size_t i) const noexcept;

	void setScissor(const Scissor& viewport, std::size_t i) noexcept;
	const Scissor& getScissor(std::size_t i) const noexcept;

	void setInputLayout(GraphicsInputLayoutPtr data) noexcept;
	GraphicsInputLayoutPtr getInputLayout() const noexcept;

	void setVertexBufferData(GraphicsDataPtr data) noexcept;
	GraphicsDataPtr getVertexBufferData() const noexcept;

	void setIndexBufferData(GraphicsDataPtr data) noexcept;
	GraphicsDataPtr getIndexBufferData() const noexcept;

	void setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept;
	GraphicsPipelinePtr getRenderPipeline() const noexcept;

	void setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept;
	GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

	bool updateBuffer(GraphicsDataPtr& data, void* buf, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	void setRenderTexture(GraphicsFramebufferPtr target) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discardRenderTexture() noexcept;
	void blitRenderTexture(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept;
	void readRenderTexture(GraphicsFramebufferPtr source, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	GraphicsFramebufferPtr getRenderTexture() const noexcept;

	void drawRenderBuffer(const GraphicsIndirect& renderable) noexcept;
	void drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept;

	void present() noexcept;

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

	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif