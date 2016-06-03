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
#include "vk_physical_device.h"
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
	auto physicalDevice = device->getGraphicsDeviceDesc().getPhysicalDevice()->downcast<VulkanPhysicalDevice>()->getPhysicalDevice();

	VkFormat format = VulkanTypes::asGraphicsFormat(textureDesc.getTexFormat());
	if (format == VK_FORMAT_UNDEFINED || format == VK_FORMAT_MAX_ENUM)
	{
		VK_PLATFORM_LOG("Invlid texture format");
		return false;
	}

	VkImageType type = VulkanTypes::asImageType(textureDesc.getTexDim());
	if (type == VK_IMAGE_TYPE_MAX_ENUM)
	{
		VK_PLATFORM_LOG("Invlid texture dimension");
		return false;
	}

	VkImageViewType viewType = VulkanTypes::asImageViewType(textureDesc.getTexDim());
	if (viewType == VK_IMAGE_TYPE_MAX_ENUM)
	{
		VK_PLATFORM_LOG("Invlid texture dimension");
		return false;
	}

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
	
	if (!_swapchainImage)
	{
		VkImageCreateInfo image;
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.pNext = nullptr;
		image.imageType = type;
		image.format = format;
		image.extent.width = textureDesc.getWidth();
		image.extent.height = textureDesc.getHeight();
		image.extent.depth = textureDesc.getDepth();
		image.mipLevels = textureDesc.getMipLevel();
		image.arrayLayers = textureDesc.getLayerNums();
		image.samples = VulkanTypes::asTextureSample(textureDesc.getSamplerAnis());
		image.tiling = VulkanTypes::asTextureTiling(textureDesc.getTexTiling());
		image.usage = VulkanTypes::asTextureUsage(textureDesc.getTexUsage());
		image.flags = 0;
		image.pQueueFamilyIndices = 0;
		image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image.queueFamilyIndexCount = 0;

		if (vkCreateImage(device->getDevice(), &image, nullptr, &_vkImage) != VK_SUCCESS)
		{
			VK_PLATFORM_LOG("vkCreateImage() fail");
			return false;
		}

		if (_vkImage == VK_NULL_HANDLE)
		{
			VK_PLATFORM_LOG("vkCreateImage() fail.");
			return false;
		}

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device->getDevice(), _vkImage, &memReqs);

		std::uint32_t mask = image.tiling == VK_IMAGE_TILING_LINEAR ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0;
		if (!_vkMemory.setup(memReqs.size, memReqs.memoryTypeBits, mask))
			return false;

		if (vkBindImageMemory(device->getDevice(), _vkImage, _vkMemory.getDeviceMemory(), 0) != VK_SUCCESS)
		{
			VK_PLATFORM_LOG("vkBindImageMemory() fail.");
			return false;
		}

		char* stream = (char*)textureDesc.getStream();
		if (stream)
		{
			VkImageSubresource subres;
			subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subres.mipLevel = 0;
			subres.arrayLayer = 0;

			VkSubresourceLayout layout;
			vkGetImageSubresourceLayout(device->getDevice(), _vkImage, &subres, &layout);

			void* data = nullptr;
			if (_vkMemory.map(0, memReqs.size, GraphicsAccessFlagBits::GraphicsAccessFlagMapWriteBit, &data))
			{
				memcpy(data, stream, memReqs.size);
				_vkMemory.unmap();
			}
		}
	}

	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	if (VulkanTypes::isStencilFormat(textureDesc.getTexFormat()))
		aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
	else if (VulkanTypes::isDepthFormat(textureDesc.getTexFormat()))
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	else if (VulkanTypes::isDepthStencilFormat(textureDesc.getTexFormat()))
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

	VkImageViewCreateInfo view;
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.pNext = nullptr;
	view.flags = 0;
	view.image = _vkImage;
	view.format = format;
	view.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view.subresourceRange.aspectMask = aspectMask;
	view.subresourceRange.baseMipLevel = textureDesc.getMipBase();
	view.subresourceRange.levelCount = textureDesc.getMipLevel();
	view.subresourceRange.baseArrayLayer = textureDesc.getLayerBase();
	view.subresourceRange.layerCount = textureDesc.getLayerNums();
	view.viewType = viewType;

	if (vkCreateImageView(device->getDevice(), &view, nullptr, &_vkImageView) != VK_SUCCESS)
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

bool
VulkanTexture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, void** data) noexcept
{
	VkImageSubresource subres;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.mipLevel = 0;
	subres.arrayLayer = 0;

	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkImage, &subres, &layout);

	std::ptrdiff_t offset = layout.offset + y * layout.rowPitch + x;
	std::ptrdiff_t count = layout.rowPitch * h + w;
	return _vkMemory.map(offset, count, GraphicsAccessFlagBits::GraphicsAccessFlagMapWriteBit, data);
}

void
VulkanTexture::unmap() noexcept
{
	_vkMemory.unmap();
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
	_vkMemory.setDevice(device);
}

GraphicsDevicePtr
VulkanTexture::getDevice() noexcept
{
	return _vkMemory.getDevice();
}

_NAME_END