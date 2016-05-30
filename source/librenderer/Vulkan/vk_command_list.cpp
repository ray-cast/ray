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
#include "vk_pipeline.h"
#include "vk_graphics_data.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanCommandList, GraphicsCommandList, "VulkanCommandList")

VulkanCommandList::VulkanCommandList() noexcept
	: _commandBuffer(VK_NULL_HANDLE)
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
VulkanCommandList::setup(const GraphicsCommandListDesc& commandListDesc) noexcept
{
	assert(commandListDesc.getGraphicsCommandPool());

	VkCommandBufferAllocateInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = commandListDesc.getGraphicsCommandPool()->downcast<VulkanCommandPool>()->getInstance();
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(_device.lock()->getDevice(), &info, &_commandBuffer) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkAllocateCommandBuffers() fail.");
		return false;
	}

	_commandListDesc = commandListDesc;
	return true;
}

void
VulkanCommandList::close() noexcept
{
	if (_commandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(_device.lock()->getDevice(), _commandListDesc.getGraphicsCommandPool()->downcast<VulkanCommandPool>()->getInstance(), 1, &_commandBuffer);
		_commandBuffer = VK_NULL_HANDLE;
	}
}

void
VulkanCommandList::renderBegin() noexcept
{
	_pipeline = nullptr;

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

	vkBeginCommandBuffer(_commandBuffer, &cmd_buf_info);
}

void
VulkanCommandList::renderEnd() noexcept
{
	if (_vkFramebufferLayout != VK_NULL_HANDLE)
	{
		vkCmdEndRenderPass(_commandBuffer);
		_vkFramebufferLayout = VK_NULL_HANDLE;
	}

	vkEndCommandBuffer(_commandBuffer);
}

void
VulkanCommandList::setViewport(const Viewport viewport[], std::uint32_t first, std::uint32_t count) noexcept
{
	if (_viewports.size() < first + count)
		_viewports.resize(first + count);

	for (std::size_t i = 0; i < count; i++)
	{
		_viewports[i].x = viewport[i].left;
		_viewports[i].y = viewport[i].top;
		_viewports[i].width = viewport[i].width;
		_viewports[i].height = viewport[i].height;
		_viewports[i].minDepth = viewport[i].minDepth;
		_viewports[i].maxDepth = viewport[i].maxDepth;
	}

	if (count > 0)
	{
		vkCmdSetViewport(_commandBuffer, first, count, _viewports.data());
	}
}

void
VulkanCommandList::setScissor(const Scissor scissor[], std::uint32_t first, std::uint32_t count) noexcept
{
	if (_scissors.size() < first + count)
		_scissors.resize(first + count);

	for (std::size_t i = 0; i < count; i++)
	{
		_scissors[i].offset.x = scissor[i].left;
		_scissors[i].offset.y = scissor[i].top;
		_scissors[i].extent.width = scissor[i].width;
		_scissors[i].extent.height = scissor[i].height;
	}

	if (count > 0)
	{
		vkCmdSetScissor(_commandBuffer, first, count, _scissors.data());
	}
}

void 
VulkanCommandList::setStencilCompareMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	VkStencilFaceFlags flags = 0;
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_FRONT_BIT;
		_pipelineState.setStencilFrontReadMask(mask);
	}
		
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_BACK_BIT;
		_pipelineState.setStencilBackReadMask(mask);
	}
		
	vkCmdSetStencilCompareMask(_commandBuffer, flags, mask);
}

std::uint32_t
VulkanCommandList::getStencilCompareMask(GraphicsStencilFace face) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
		return _pipelineState.getStencilFrontReadMask();
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
		return _pipelineState.getStencilBackReadMask();
	return 0;
}

void 
VulkanCommandList::setStencilReference(GraphicsStencilFace face, std::uint32_t reference) noexcept
{
	VkStencilFaceFlags flags = 0;
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_FRONT_BIT;
		_pipelineState.setStencilFrontRef(reference);
	}

	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_BACK_BIT;
		_pipelineState.setStencilBackRef(reference);
	}

	vkCmdSetStencilReference(_commandBuffer, flags, reference);
}

std::uint32_t 
VulkanCommandList::getStencilReference(GraphicsStencilFace face) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
		return _pipelineState.getStencilFrontRef();
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
		return _pipelineState.getStencilBackRef();
	return 0;
}

void 
VulkanCommandList::setStencilFrontWriteMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	VkStencilFaceFlags flags = 0;
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_FRONT_BIT;
		_pipelineState.setStencilFrontWriteMask(mask);
	}

	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		flags |= VkStencilFaceFlagBits::VK_STENCIL_FACE_BACK_BIT;
		_pipelineState.setStencilBackWriteMask(mask);
	}

	vkCmdSetStencilWriteMask(_commandBuffer, flags, mask);
}

std::uint32_t 
VulkanCommandList::getStencilFrontWriteMask(GraphicsStencilFace face) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
		return _pipelineState.getStencilFrontWriteMask();
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
		return _pipelineState.getStencilBackWriteMask();
	return 0;
}

void
VulkanCommandList::clearTexture(GraphicsTexturePtr texture, const ClearValue& value) noexcept
{
	auto _vktexture = std::dynamic_pointer_cast<VulkanTexture>(texture);

	auto format = texture->getGraphicsTextureDesc().getTexFormat();
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

		vkCmdClearDepthStencilImage(_commandBuffer, _vktexture->getImage(), VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &cmd, 1, &range);
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

		vkCmdClearColorImage(_commandBuffer, _vktexture->getImage(), VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &cmd, 1, &range);
	}
}

void
VulkanCommandList::setFramebuffer(GraphicsFramebufferPtr framebuffer) noexcept
{
	assert(framebuffer);
	assert(framebuffer->isInstanceOf<VulkanFramebuffer>());

	if (_vkFramebuffer != VK_NULL_HANDLE)
	{
		vkCmdEndRenderPass(_commandBuffer);
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

	vkCmdBeginRenderPass(_commandBuffer, &cmd, VK_SUBPASS_CONTENTS_INLINE);
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
		vkCmdEndRenderPass(_commandBuffer);
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

	vkCmdBeginRenderPass(_commandBuffer, &cmd, VK_SUBPASS_CONTENTS_INLINE);
}

void
VulkanCommandList::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	VkClearAttachment attachment[VK_MAX_ATTACHMENT];
	VkClearRect rects[VK_MAX_ATTACHMENT];

	std::uint32_t usageCount = 0;
	auto& framebufferDesc = _framebuffer->downcast<VulkanFramebuffer>()->getGraphicsFramebufferDesc();

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
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

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit ||
		flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
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

			if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
				attachment[usageCount].aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;

			attachment[usageCount].clearValue.depthStencil.depth = depth;
			attachment[usageCount].clearValue.depthStencil.stencil = stencil;
			attachment[usageCount].colorAttachment = 0;

			usageCount++;
		}
	}

	vkCmdClearAttachments(_commandBuffer, usageCount, attachment, usageCount, rects);
}

void
VulkanCommandList::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	VkClearAttachment attachment[VK_MAX_ATTACHMENT];
	VkClearRect rects[VK_MAX_ATTACHMENT];

	std::uint32_t usageCount = 0;
	auto& framebufferDesc = _framebuffer->downcast<VulkanFramebuffer>()->getGraphicsFramebufferDesc();

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
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

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit ||
		flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
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

			attachment[usageCount].aspectMask = 0;
			if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
				attachment[usageCount].aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
				attachment[usageCount].aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

			attachment[usageCount].clearValue.depthStencil.depth = depth;
			attachment[usageCount].clearValue.depthStencil.stencil = stencil;
			attachment[usageCount].colorAttachment = 0;

			usageCount++;
		}
	}

	vkCmdClearAttachments(_commandBuffer, usageCount, attachment, usageCount, rects);
}

void
VulkanCommandList::setPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<VulkanRenderPipeline>());

	auto vulkanPipeline = pipeline->downcast<VulkanRenderPipeline>();
	if (_pipeline != vulkanPipeline)
	{
		vkCmdBindPipeline(_commandBuffer, vulkanPipeline->getPipelineBindPoint(), vulkanPipeline->getPipeline());
		_pipeline = vulkanPipeline;
	}
}

void
VulkanCommandList::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(_pipeline);
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<VulkanDescriptorSet>());

	auto vulkanDescripotrSet = descriptorSet->downcast<VulkanDescriptorSet>();
	if (_descripotrSet != vulkanDescripotrSet)
	{
		VkDescriptorSet descriptorSetHandle = vulkanDescripotrSet->getDescriptorSet();
		vkCmdBindDescriptorSets(_commandBuffer, _pipeline->getPipelineBindPoint(), _pipeline->getPipelineLayout(), 0, 1, &descriptorSetHandle, 0, nullptr);
		_descripotrSet = vulkanDescripotrSet;
	}
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

	vkCmdBindVertexBuffers(_commandBuffer, first, count, _vkVertexBuffers.data(), _vkVertexOffsets.data());
}

void
VulkanCommandList::setIndexBuffer(GraphicsDataPtr data) noexcept
{
	VkBuffer buffer = data->downcast<VulkanGraphicsData>()->getBuffer(); 
	auto stride = data->downcast<VulkanGraphicsData>()->getGraphicsDataDesc().getStride();
	if (stride == 16)
		vkCmdBindIndexBuffer(_commandBuffer, buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);
	else if (stride == 32)
		vkCmdBindIndexBuffer(_commandBuffer, buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
}

void
VulkanCommandList::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	if (renderable.numIndices == 0)
		vkCmdDraw(_commandBuffer, renderable.numVertices, renderable.numInstances, renderable.startVertice, renderable.startInstances);
	else
		vkCmdDrawIndexed(_commandBuffer, renderable.numIndices, renderable.numInstances, renderable.startIndice, renderable.startVertice, renderable.startInstances);
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
	vkCmdExecuteCommands(_commandBuffer, 1, &otherCommand);
}

VkCommandBuffer
VulkanCommandList::getInstance() const noexcept
{
	return _commandBuffer;
}

void
VulkanCommandList::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device->downcast_pointer<VulkanDevice>();;
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