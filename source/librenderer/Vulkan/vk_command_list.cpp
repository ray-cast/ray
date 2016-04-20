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
#include "vk_command_list.h"
#include "vk_command_pool.h"
#include "vk_device.h"
#include "vk_texture.h"
#include "vk_framebuffer.h"
#include "vk_descriptor_set.h"
#include "vk_render_pipeline.h"
#include "vk_graphics_data.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanCommandList, GraphicsCommandList, "VulkanCommandList")

VulkanCommandList::VulkanCommandList() noexcept
	: _vkCommandBuffer(VK_NULL_HANDLE)
	, _vkFramebuffer(VK_NULL_HANDLE)
	, _vkFramebufferLayout(VK_NULL_HANDLE)
	, _vkVertexBuffers(8)
	, _vkVertexOffsets(8)
{
}

VulkanCommandList::~VulkanCommandList() noexcept
{
	this->close();
}

bool
VulkanCommandList::setup(const GraphicsCommandListDesc& desc) noexcept
{
	assert(desc.getGraphicsCommandPool());

	VkCommandBufferAllocateInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = desc.getGraphicsCommandPool()->downcast<VulkanCommandPool>()->getInstance();
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, &_vkCommandBuffer) > 0)
	{
		VK_PLATFORM_LOG("vkAllocateCommandBuffers() fail.");
		return false;
	}

	return true;
}

void 
VulkanCommandList::close() noexcept
{
	if (_vkCommandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(
			this->getDevice()->downcast<VulkanDevice>()->getDevice(), 
			_commandListDesc.getGraphicsCommandPool()->downcast<VulkanCommandPool>()->getInstance(), 1, &_vkCommandBuffer);
		_vkCommandBuffer = VK_NULL_HANDLE;
	}
}

void
VulkanCommandList::renderBegin() noexcept
{
	VkCommandBufferInheritanceInfo cmd_buf_hinfo;
	cmd_buf_hinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	cmd_buf_hinfo.pNext = NULL;
	cmd_buf_hinfo.renderPass = VK_NULL_HANDLE;
	cmd_buf_hinfo.subpass = 0;
	cmd_buf_hinfo.framebuffer = VK_NULL_HANDLE;
	cmd_buf_hinfo.occlusionQueryEnable = VK_FALSE;
	cmd_buf_hinfo.queryFlags = 0;
	cmd_buf_hinfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo cmd_buf_info;
	cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_buf_info.pNext = NULL;
	cmd_buf_info.flags = 0;
	cmd_buf_info.pInheritanceInfo = &cmd_buf_hinfo;
	
	vkBeginCommandBuffer(_vkCommandBuffer, &cmd_buf_info);	
}

void
VulkanCommandList::renderEnd() noexcept
{
	if (_vkFramebufferLayout != VK_NULL_HANDLE)
	{
		vkCmdEndRenderPass(_vkCommandBuffer);
		_vkFramebufferLayout = VK_NULL_HANDLE;
	}

	vkEndCommandBuffer(_vkCommandBuffer);
}

void
VulkanCommandList::setViewport(const Viewport viewport[], std::uint32_t first, std::uint32_t count) noexcept
{
	std::uint32_t viewportCount = 0;
	VkViewport cmd[VK_MAX_VIEWPORT_ARRAY];

	for (std::size_t i = 0; i < count; i++, viewportCount++)
	{
		cmd[i].x = viewport[i].left;
		cmd[i].y = viewport[i].top;
		cmd[i].width = viewport[i].width;
		cmd[i].height = viewport[i].height;
		cmd[i].minDepth = viewport[i].minDepth;
		cmd[i].maxDepth = viewport[i].maxDepth;

		if (viewportCount == VK_MAX_VIEWPORT_ARRAY)
		{
			vkCmdSetViewport(_vkCommandBuffer, first, VK_MAX_VIEWPORT_ARRAY, cmd);
			first += VK_MAX_VIEWPORT_ARRAY;
			viewportCount = 0;
		}
	}
	
	if (viewportCount > 0)
	{
		vkCmdSetViewport(_vkCommandBuffer, first, viewportCount, cmd);
	}
}

void
VulkanCommandList::setScissor(const Scissor scissor[], std::uint32_t first, std::uint32_t count) noexcept
{
	std::uint32_t scissorCount = 0;
	VkRect2D cmd[VK_MAX_VIEWPORT_ARRAY];

	for (std::size_t i = 0; i < count; i++, scissorCount++)
	{
		cmd[i].offset.x = scissor[i].left;
		cmd[i].offset.y = scissor[i].top;
		cmd[i].extent.width = scissor[i].width;
		cmd[i].extent.height = scissor[i].height;

		if (scissorCount == VK_MAX_VIEWPORT_ARRAY)
		{
			vkCmdSetScissor(_vkCommandBuffer, first, VK_MAX_VIEWPORT_ARRAY, cmd);
			first += VK_MAX_VIEWPORT_ARRAY;
			scissorCount = 0;
		}
	}

	if (scissorCount > 0)
	{
		vkCmdSetScissor(_vkCommandBuffer, first, scissorCount, cmd);
	}
}

void 
VulkanCommandList::clearTexture(GraphicsTexturePtr texture, const ClearValue& value) noexcept
{
	auto _vktexture = std::dynamic_pointer_cast<VulkanTexture>(texture);

	const auto& textureDesc = texture->getGraphicsTextureDesc();
	auto format = textureDesc.getTexFormat();
	if (format == GraphicsFormat::GraphicsFormatD16UNorm || format == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt ||
		format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 || format == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt ||
		format == GraphicsFormat::GraphicsFormatD32_SFLOAT || format == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
	{
		VkClearDepthStencilValue cmd;
		VkImageSubresourceRange range;

		cmd.depth = value.depth;
		cmd.stencil = value.stencil;

		range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		range.baseArrayLayer = value.rect->first;
		range.baseMipLevel = 0;
		range.layerCount = value.rect->count;
		range.levelCount = 1;

		vkCmdClearDepthStencilImage(_vkCommandBuffer, _vktexture->getImage(), VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &cmd, 1, &range);
	}
	else
	{
		VkClearColorValue cmd;
		VkImageSubresourceRange range;

		cmd.float32[0] = value.color.x;
		cmd.float32[1] = value.color.y;
		cmd.float32[2] = value.color.z;
		cmd.float32[3] = value.color.w;

		range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseArrayLayer = value.rect->first;
		range.layerCount = value.rect->count;
		range.baseMipLevel = 0;
		range.levelCount = 1;

		vkCmdClearColorImage(_vkCommandBuffer, _vktexture->getImage(), VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &cmd, 1, &range);
	}
}

void
VulkanCommandList::setFramebuffer(GraphicsFramebufferPtr framebuffer) noexcept
{
	assert(framebuffer);
	assert(framebuffer->isInstanceOf<VulkanFramebuffer>());

	if (_vkFramebuffer != VK_NULL_HANDLE)
	{
		vkCmdEndRenderPass(_vkCommandBuffer);
		_vkFramebuffer = VK_NULL_HANDLE;
	}

	_framebuffer = framebuffer->downcast<VulkanFramebuffer>();

	auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
	auto framebufferLayout = framebufferDesc.getGraphicsFramebufferLayout();

	VkClearValue clear[2];
	clear[0].color.float32[0] = 0.0f;
	clear[0].color.float32[1] = 1.0f;
	clear[0].color.float32[2] = 0.0f;
	clear[0].color.float32[3] = 0.0f;
	clear[1].depthStencil.depth = 1.0f;
	clear[1].depthStencil.stencil = 0;

	VkRenderPassBeginInfo cmd;
	cmd.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	cmd.pNext = 0;
	cmd.renderPass = _vkFramebufferLayout = framebufferLayout->downcast<VulkanFramebufferLayout>()->getRenderPass();
	cmd.framebuffer = _framebuffer->getFramebuffer();
	cmd.renderArea.offset.x = 0;
	cmd.renderArea.offset.y = 0;
	cmd.renderArea.extent.width = framebufferDesc.getWidth();
	cmd.renderArea.extent.height = framebufferDesc.getHeight();
	cmd.clearValueCount = 2;
	cmd.pClearValues = clear;

	vkCmdBeginRenderPass(_vkCommandBuffer, &cmd, VK_SUBPASS_CONTENTS_INLINE);
}

void
VulkanCommandList::setFramebuffer(GraphicsFramebufferPtr framebuffer, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(framebuffer);
	assert(framebuffer->isInstanceOf<VulkanFramebuffer>());

	VkClearValue attachment[VK_MAX_ATTACHMENT];

	_framebuffer = framebuffer->downcast<VulkanFramebuffer>();

	std::uint32_t usageCount = 0;
	auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
	auto framebufferLayout = framebufferDesc.getGraphicsFramebufferLayout()->downcast<VulkanFramebufferLayout>();

	auto txtureCount = framebufferDesc.getTextures().size();
	for (std::uint32_t i = 0; i < txtureCount; i++)
	{
		attachment[usageCount].color.float32[0] = color.x;
		attachment[usageCount].color.float32[1] = color.y;
		attachment[usageCount].color.float32[2] = color.z;
		attachment[usageCount].color.float32[3] = color.w;
		usageCount++;
	}

	auto depthStencil = framebufferDesc.getSharedDepthStencilTexture();
	if (depthStencil)
	{
		attachment[usageCount].depthStencil.depth = depth;
		attachment[usageCount].depthStencil.stencil = stencil;
		usageCount++;
	}

	if (_vkFramebuffer != VK_NULL_HANDLE)
	{
		vkCmdEndRenderPass(_vkCommandBuffer);
		_vkFramebuffer = VK_NULL_HANDLE;
	}

	VkRenderPassBeginInfo cmd;
	cmd.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	cmd.pNext = 0;
	cmd.renderPass = _vkFramebufferLayout = framebufferLayout->downcast<VulkanFramebufferLayout>()->getRenderPass();
	cmd.framebuffer = framebuffer->downcast<VulkanFramebuffer>()->getFramebuffer();
	cmd.renderArea.offset.x = 0;
	cmd.renderArea.offset.y = 0;
	cmd.renderArea.extent.width = framebufferDesc.getWidth();
	cmd.renderArea.extent.height = framebufferDesc.getHeight();
	cmd.clearValueCount = usageCount;
	cmd.pClearValues = attachment;

	vkCmdBeginRenderPass(_vkCommandBuffer, &cmd, VK_SUBPASS_CONTENTS_INLINE);
}

void
VulkanCommandList::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	VkClearAttachment attachment[VK_MAX_ATTACHMENT];
	VkClearRect rects[VK_MAX_ATTACHMENT];

	std::uint32_t usageCount = 0;
	auto& framebufferDesc = _framebuffer->downcast<VulkanFramebuffer>()->getGraphicsFramebufferDesc();

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		auto& textures = framebufferDesc.getTextures();
		for (auto& texture : textures)
		{
			auto& textureDesc = texture->getGraphicsTextureDesc();

			rects[usageCount].baseArrayLayer = textureDesc.getLayerBase();
			rects[usageCount].layerCount = textureDesc.getLayerNums();
			rects[usageCount].rect.offset.x = 0;
			rects[usageCount].rect.offset.y = 0;
			rects[usageCount].rect.extent.width = framebufferDesc.getWidth();
			rects[usageCount].rect.extent.height = framebufferDesc.getHeight();

			attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			attachment[usageCount].colorAttachment = usageCount;
			attachment[usageCount].clearValue.color.float32[0] = color.x;
			attachment[usageCount].clearValue.color.float32[1] = color.y;
			attachment[usageCount].clearValue.color.float32[2] = color.z;
			attachment[usageCount].clearValue.color.float32[3] = color.w;

			usageCount++;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth ||
		flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
	{
		auto depthStencil = framebufferDesc.getSharedDepthStencilTexture();
		if (depthStencil)
		{
			auto& textureDesc = depthStencil->getGraphicsTextureDesc();

			rects[usageCount].baseArrayLayer = textureDesc.getLayerBase();
			rects[usageCount].layerCount = textureDesc.getLayerNums();
			rects[usageCount].rect.offset.x = 0;
			rects[usageCount].rect.offset.y = 0;
			rects[usageCount].rect.extent.width = framebufferDesc.getWidth();
			rects[usageCount].rect.extent.height = framebufferDesc.getHeight();

			if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;

			attachment[usageCount].clearValue.depthStencil.depth = depth;
			attachment[usageCount].clearValue.depthStencil.stencil = stencil;
			attachment[usageCount].colorAttachment = 0;

			usageCount++;
		}
	}

	vkCmdClearAttachments(_vkCommandBuffer, usageCount, attachment, usageCount, rects);
}

void
VulkanCommandList::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	VkClearAttachment attachment[VK_MAX_ATTACHMENT];
	VkClearRect rects[VK_MAX_ATTACHMENT];

	std::uint32_t usageCount = 0;
	auto& framebufferDesc = _framebuffer->downcast<VulkanFramebuffer>()->getGraphicsFramebufferDesc();

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		auto& textures = framebufferDesc.getTextures();
		if (i < textures.size())
		{
			auto texture = textures[i];
			auto& textureDesc = texture->getGraphicsTextureDesc();

			rects[usageCount].baseArrayLayer = textureDesc.getLayerBase();
			rects[usageCount].layerCount = textureDesc.getLayerNums();
			rects[usageCount].rect.offset.x = 0;
			rects[usageCount].rect.offset.y = 0;
			rects[usageCount].rect.extent.width = framebufferDesc.getWidth();
			rects[usageCount].rect.extent.height = framebufferDesc.getHeight();

			attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			attachment[usageCount].colorAttachment = usageCount;
			attachment[usageCount].clearValue.color.float32[0] = color.x;
			attachment[usageCount].clearValue.color.float32[1] = color.y;
			attachment[usageCount].clearValue.color.float32[2] = color.z;
			attachment[usageCount].clearValue.color.float32[3] = color.w;

			usageCount++;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth ||
		flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
	{
		auto depthStencil = framebufferDesc.getSharedDepthStencilTexture();
		if (depthStencil)
		{
			auto& textureDesc = depthStencil->getGraphicsTextureDesc();

			rects[usageCount].baseArrayLayer = textureDesc.getLayerBase();
			rects[usageCount].layerCount = textureDesc.getLayerNums();
			rects[usageCount].rect.offset.x = 0;
			rects[usageCount].rect.offset.y = 0;
			rects[usageCount].rect.extent.width = framebufferDesc.getWidth();
			rects[usageCount].rect.extent.height = framebufferDesc.getHeight();

			if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;

			attachment[usageCount].clearValue.depthStencil.depth = depth;
			attachment[usageCount].clearValue.depthStencil.stencil = stencil;
			attachment[usageCount].colorAttachment = 0;

			usageCount++;
		}
	}

	vkCmdClearAttachments(_vkCommandBuffer, usageCount, attachment, usageCount, rects);
}

void 
VulkanCommandList::setPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<VulkanRenderPipeline>());

	_pipeline = pipeline->downcast<VulkanRenderPipeline>();
	vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->getPipeline());
}

void 
VulkanCommandList::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<VulkanDescriptorSet>());

	VkPipelineLayout pipelineLayout = _pipeline->downcast<VulkanRenderPipeline>()->getPipelineLayout();
	VkDescriptorSet descriptorSetHandle = descriptorSet->downcast<VulkanDescriptorSet>()->getDescriptorSet();
	vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSetHandle, 0, nullptr);
}

void
VulkanCommandList::setVertexBuffers(GraphicsDataPtr data[], std::uint32_t first, std::uint32_t count) noexcept
{
	assert(data);

	for (std::uint32_t i = 0; i < count; i++)
	{
		_vkVertexBuffers[i] = data[i]->downcast<VulkanGraphicsData>()->getBuffer();
		_vkVertexOffsets[i] = 0;
	}
	
	vkCmdBindVertexBuffers(_vkCommandBuffer, first, count, _vkVertexBuffers.data(), _vkVertexOffsets.data());
}

void
VulkanCommandList::setIndexBuffer(GraphicsDataPtr data) noexcept
{
	VkBuffer buffer = data->downcast<VulkanGraphicsData>()->getBuffer();
	vkCmdBindIndexBuffer(_vkCommandBuffer, buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
}

void 
VulkanCommandList::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	if (renderable.numIndices > 0)
		vkCmdDrawIndexed(_vkCommandBuffer, renderable.numIndices, renderable.numInstances, renderable.startIndice, renderable.startVertice, renderable.startInstances);
	else
		vkCmdDraw(_vkCommandBuffer, renderable.numVertices, renderable.numInstances, renderable.startVertice, renderable.startInstances);
}

void 
VulkanCommandList::drawRenderMesh(const GraphicsIndirect renderable[], std::size_t count) noexcept
{
	for (std::size_t i = 0; i < count; i++)
		this->drawRenderMesh(renderable[i]);
}

void
VulkanCommandList::execute(const GraphicsCommandListPtr& command) noexcept
{
	VkCommandBuffer otherCommand = command->downcast<VulkanCommandList>()->getInstance();
	vkCmdExecuteCommands(_vkCommandBuffer, 1, &otherCommand);
}

VkCommandBuffer
VulkanCommandList::getInstance() const noexcept
{
	return _vkCommandBuffer;
}

void
VulkanCommandList::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanCommandList::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsCommandListDesc& 
VulkanCommandList::getGraphicsCommandListDesc() const noexcept
{
	return _commandListDesc;
}

_NAME_END