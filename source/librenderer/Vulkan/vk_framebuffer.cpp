// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include "vk_framebuffer.h"
#include "vk_texture.h"
#include "vk_device.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanFramebufferLayout, GraphicsFramebufferLayout, "VulkanFramebufferLayout")
__ImplementSubClass(VulkanFramebuffer, GraphicsFramebuffer, "VulkanFramebuffer")

VulkanFramebufferLayout::VulkanFramebufferLayout() noexcept
	: _vkFramebufferLayout(VK_NULL_HANDLE)
{
}

VulkanFramebufferLayout::~VulkanFramebufferLayout() noexcept
{
	this->close();
}

bool
VulkanFramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& passDesc) noexcept
{
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkAttachmentReference> colorAttachments;
	std::vector<VkAttachmentReference> depthAttachments;

	for (const auto& component : passDesc.getComponents())
	{
		auto imageLayout = VulkanTypes::asImageLayout(component.getAttachType());
		if (imageLayout == GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal ||
			imageLayout == GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal)
		{
			VkAttachmentReference reference;
			reference.attachment = component.getAttachSlot();
			reference.layout = imageLayout;

			depthAttachments.push_back(reference);
		}
		else
		{
			VkAttachmentReference reference;
			reference.attachment = component.getAttachSlot();
			reference.layout = imageLayout;
			colorAttachments.push_back(reference);
		}

		VkAttachmentDescription attachment;
		attachment.flags = 0;
		attachment.format = VulkanTypes::asGraphicsFormat(component.getAttachFormat());
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.initialLayout = imageLayout;
		attachment.finalLayout = imageLayout;

		attachments.push_back(attachment);
	}

	VkSubpassDescription subpass;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = colorAttachments.size();
	subpass.pColorAttachments = colorAttachments.data();
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = depthAttachments.data();
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo pass;
	pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	pass.flags = 0;
	pass.pNext = nullptr;
	pass.attachmentCount = attachments.size();
	pass.pAttachments = attachments.data();
	pass.subpassCount = 1;
	pass.pSubpasses = &subpass;
	pass.dependencyCount = 0;
	pass.pDependencies = nullptr;

	if (vkCreateRenderPass(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &pass, nullptr, &_vkFramebufferLayout) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateRenderPass() fail.");
		return false;
	}

	_renderPassDesc = passDesc;
	return true;
}

void
VulkanFramebufferLayout::close() noexcept
{
	if (_vkFramebufferLayout != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkFramebufferLayout, nullptr);
		_vkFramebufferLayout = VK_NULL_HANDLE;
	}
}

VkRenderPass
VulkanFramebufferLayout::getRenderPass() const noexcept
{
	return _vkFramebufferLayout;
}

void
VulkanFramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanFramebufferLayout::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsFramebufferLayoutDesc&
VulkanFramebufferLayout::getGraphicsFramebufferLayoutDesc() const noexcept
{
	return _renderPassDesc;
}

VulkanFramebuffer::VulkanFramebuffer() noexcept
	: _vkFramebuffer(VK_NULL_HANDLE)
{
}

VulkanFramebuffer::~VulkanFramebuffer() noexcept
{
	this->close();
}

bool
VulkanFramebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
{
	assert(framebufferDesc.getGraphicsFramebufferLayout());
	assert(framebufferDesc.getGraphicsFramebufferLayout()->isInstanceOf<VulkanFramebufferLayout>());
	assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

	const auto& renderTextures = framebufferDesc.getTextures();
	const auto& framebufferLayoutDesc = framebufferDesc.getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc();
	const auto& framebufferAttachments = framebufferLayoutDesc.getComponents();

	for (auto& attachment : framebufferAttachments)
	{
		auto format = attachment.getAttachFormat();
		if (VulkanTypes::isDepthStencilFormat(format))
		{
			if (!framebufferDesc.getSharedDepthStencilTexture())
			{
				VK_PLATFORM_LOG("Empty depth stencil texture");
				return false;
			}

			if (format != framebufferDesc.getSharedDepthStencilTexture()->getGraphicsTextureDesc().getTexFormat())
			{
				VK_PLATFORM_LOG("Invlid depth stencil texture");
				return false;
			}
		}
		else if (VulkanTypes::isDepthFormat(format))
		{
			if (!framebufferDesc.getSharedDepthStencilTexture())
			{
				VK_PLATFORM_LOG("Empty depth texture");
				return false;
			}

			if (format != framebufferDesc.getSharedDepthStencilTexture()->getGraphicsTextureDesc().getTexFormat())
			{
				VK_PLATFORM_LOG("Invlid depth texture");
				return false;
			}
		}
		else if (VulkanTypes::isStencilFormat(format))
		{
			VK_PLATFORM_LOG("Can't support stencil texture");
			return false;
		}
	}

	std::vector<VkImageView> imageViews(renderTextures.size());
	for (std::size_t i = 0; i < renderTextures.size(); i++)
	{
		const auto texture = renderTextures[i]->downcast<VulkanTexture>();
		const auto& textureDesc = texture->getGraphicsTextureDesc();

		if (textureDesc.getWidth() < framebufferDesc.getWidth() &&
			textureDesc.getHeight() < framebufferDesc.getHeight() &&
			textureDesc.getTexFormat() != framebufferAttachments[i].getAttachFormat())
		{
			return false;
		}

		imageViews[i] = texture->getImageView();
	}

	if (framebufferDesc.getSharedDepthStencilTexture())
	{
		const auto texture = framebufferDesc.getSharedDepthStencilTexture()->downcast<VulkanTexture>();
		imageViews.push_back(texture->getImageView());
	}

	VkFramebufferCreateInfo framebuffer;
	framebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer.flags = 0;
	framebuffer.pNext = nullptr;
	framebuffer.renderPass = framebufferDesc.getGraphicsFramebufferLayout()->downcast<VulkanFramebufferLayout>()->getRenderPass();
	framebuffer.attachmentCount = imageViews.size();
	framebuffer.pAttachments = imageViews.data();
	framebuffer.width = framebufferDesc.getWidth();
	framebuffer.height = framebufferDesc.getHeight();
	framebuffer.layers = 1;

	if (vkCreateFramebuffer(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &framebuffer, nullptr, &_vkFramebuffer) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateFramebuffer() fail.");
		return false;
	}

	_framebufferDesc = framebufferDesc;
	return true;
}

void
VulkanFramebuffer::close() noexcept
{
	if (_vkFramebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkFramebuffer, nullptr);
		_vkFramebuffer = VK_NULL_HANDLE;
	}
}

const GraphicsFramebufferDesc&
VulkanFramebuffer::getGraphicsFramebufferDesc() const noexcept
{
	return _framebufferDesc;
}

VkFramebuffer
VulkanFramebuffer::getFramebuffer() const noexcept
{
	return _vkFramebuffer;
}

void
VulkanFramebuffer::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanFramebuffer::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END