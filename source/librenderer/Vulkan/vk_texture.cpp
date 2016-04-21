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
#include "vk_texture.h"
#include "vk_device.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanTexture, GraphicsTexture, "VulkanTexture")

VulkanTexture::VulkanTexture() noexcept
	: _vkImage(VK_NULL_HANDLE)
	, _vkImageView(VK_NULL_HANDLE)
	, _swapchainImage(false)
{
}

VulkanTexture::~VulkanTexture() noexcept
{
	this->close();
}

bool 
VulkanTexture::setup(const GraphicsTextureDesc& textureDesc) noexcept
{
	assert(_vkImageView == VK_NULL_HANDLE);

	auto device = this->getDevice()->downcast<VulkanDevice>();

	VkFormat format = VulkanTypes::asGraphicsFormat(textureDesc.getTexFormat());
	if (format == VK_FORMAT_UNDEFINED || format == VK_FORMAT_MAX_ENUM)
		return false;

	if (!_swapchainImage)
	{
		VkImageType type = VulkanTypes::asImageType(textureDesc.getTexDim());
		if (type == VK_IMAGE_TYPE_MAX_ENUM)
			return false;

		VkImageCreateInfo image;
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.pNext = nullptr;
		image.imageType = type;
		image.format = format;
		image.extent.width = textureDesc.getWidth();
		image.extent.height = textureDesc.getHeight();
		image.extent.depth = textureDesc.getDepth();
		image.mipLevels = std::max(textureDesc.getMipLevel(), std::uint32_t(1));
		image.arrayLayers = textureDesc.getLayerNums();
		image.samples = VulkanTypes::asTextureSample(textureDesc.getSamplerAnis());
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		image.usage = VulkanTypes::asTextureUsage(textureDesc.getTexUsage());
		image.flags = 0;
		image.pQueueFamilyIndices = 0;
		image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image.queueFamilyIndexCount = 0;

		if (vkCreateImage(device->getDevice(), &image, nullptr, &_vkImage) > 0)
		{
			VK_PLATFORM_LOG("vkCreateImage() fail");
			return false;
		}

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device->getDevice(), _vkImage, &memReqs);

		if (!_vkMemory.setup(nullptr, memReqs.size, memReqs.memoryTypeBits, 0))
			return false;

		auto stream = textureDesc.getStream();
		auto streamSize = textureDesc.getStreamSize();
		if (stream != nullptr && streamSize > 0)
		{
			void* data = _vkMemory.map(GraphicsAccessFlagsBits::GraphicsAccessFlagsMapReadBit);
			if (data)
			{
				std::memcpy(data, stream, streamSize);
				_vkMemory.unmap();
			}
		}

		if (vkBindImageMemory(device->getDevice(), _vkImage, _vkMemory.getDeviceMemory(), 0) > 0)
		{
			VK_PLATFORM_LOG("vkBindImageMemory() fail");
			return false;
		}			
	}

	VkImageViewType viewType = VulkanTypes::asImageViewType(textureDesc.getTexDim());
	if (viewType == VK_IMAGE_TYPE_MAX_ENUM)
		return false;

	VkImageAspectFlags flags = VK_IMAGE_ASPECT_COLOR_BIT;
	if (VulkanTypes::isStencilFormat(textureDesc.getTexFormat()))
		flags = VK_IMAGE_ASPECT_STENCIL_BIT;
	else if (VulkanTypes::isDepthFormat(textureDesc.getTexFormat()))
		flags = VK_IMAGE_ASPECT_DEPTH_BIT;
	else if (VulkanTypes::isDepthStencilFormat(textureDesc.getTexFormat()))
		flags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

	VkImageViewCreateInfo view;
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.pNext = nullptr;
	view.image = _vkImage;
	view.format = format;
	view.components.r = flags == VK_IMAGE_ASPECT_COLOR_BIT ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.g = flags == VK_IMAGE_ASPECT_COLOR_BIT ? VK_COMPONENT_SWIZZLE_G : VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.b = flags == VK_IMAGE_ASPECT_COLOR_BIT ? VK_COMPONENT_SWIZZLE_B : VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.a = flags == VK_IMAGE_ASPECT_COLOR_BIT ? VK_COMPONENT_SWIZZLE_A : VK_COMPONENT_SWIZZLE_IDENTITY;
	view.subresourceRange.aspectMask = flags;
	view.subresourceRange.baseMipLevel = textureDesc.getMipBase();
	view.subresourceRange.levelCount = std::max(textureDesc.getMipLevel(), std::uint32_t(1));
	view.subresourceRange.baseArrayLayer = textureDesc.getLayerBase();
	view.subresourceRange.layerCount = textureDesc.getLayerNums();
	view.flags = 0;
	view.viewType = viewType;

	if (vkCreateImageView(device->getDevice(), &view, nullptr, &_vkImageView) > 0)
	{
		VK_PLATFORM_LOG("vkCreateImageView() fail");
		return false;
	}

	_textureDesc = textureDesc;
	return true;
}

void 
VulkanTexture::close() noexcept
{
	auto device = this->getDevice()->downcast<VulkanDevice>();

	if (!this->getSwapchainImage())
	{
		if (_vkImage != VK_NULL_HANDLE)
		{
			vkDestroyImage(device->getDevice(), _vkImage, nullptr);
			_vkImage = VK_NULL_HANDLE;
		}
	}

	if (_vkImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device->getDevice(), _vkImageView, nullptr);
		_vkImageView = VK_NULL_HANDLE;
	}
}

void
VulkanTexture::setSwapchainImage(VkImage image) noexcept
{
	_vkImage = image;
	_swapchainImage = true;
}

bool 
VulkanTexture::getSwapchainImage() const noexcept
{
	return _swapchainImage;
}

VkImage
VulkanTexture::getImage() const noexcept
{
	return _vkImage;
}

VkImageView
VulkanTexture::getImageView() const noexcept
{
	return _vkImageView;
}

const GraphicsTextureDesc&
VulkanTexture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
VulkanTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
	_vkMemory.setDevice(device);
}

GraphicsDevicePtr 
VulkanTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END