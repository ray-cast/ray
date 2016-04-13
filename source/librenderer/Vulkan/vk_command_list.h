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
#ifndef _H_VK_COMMAND_LIST_H_
#define _H_VK_COMMAND_LIST_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanCommandList final : public GraphicsCommandList
{
	__DeclareSubClass(VulkanCommandList, GraphicsCommandList)
public:
	VulkanCommandList() noexcept;
	virtual ~VulkanCommandList() noexcept;

	bool setup(const GraphicsCommandListDesc& desc) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setViewport(const Viewport viewport[], std::uint32_t first, std::uint32_t count) noexcept;
	void setScissor(const Scissor Scissor[], std::uint32_t first, std::uint32_t count) noexcept;

	void clearTexture(GraphicsTexturePtr texture, const ClearValue& value) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr framebuffer) noexcept;
	void clearFramebuffer(ClearValue value[], std::uint32_t first, std::uint32_t count) noexcept;

	void setPipeline(GraphicsPipelinePtr pipeline) noexcept;
	void setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept;

	void setVertexBuffers(GraphicsDataPtr data[], std::uint32_t first, std::uint32_t count) noexcept;
	void setIndexBuffer(GraphicsDataPtr data) noexcept;

	void drawRenderMesh(const GraphicsIndirect& renderable) noexcept;
	void drawRenderMesh(const GraphicsIndirect renderable[], std::size_t count) noexcept;

	void execute(const GraphicsCommandListPtr& command) noexcept;

	VkCommandBuffer getInstance() const noexcept;

	const GraphicsCommandListDesc& getGraphicsCommandListDesc() const noexcept;

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VkCommandBuffer _vkCommandBuffer;

	VkFramebuffer _vkFramebuffer;
	VkRenderPass _vkFramebufferLayout;

	std::vector<VkBuffer> _vkVertexBuffers;
	std::vector<VkDeviceSize> _vkVertexOffsets;

	GraphicsPipelinePtr _pipeline;
	GraphicsFramebufferPtr _framebuffers;

	GraphicsCommandListDesc _commandListDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif