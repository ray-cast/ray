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
#include "vk_swapchain.h"
#include "vk_device.h"
#include "vk_texture.h"
#include "vk_framebuffer.h"

_NAME_BEGIN

__ImplementSubClass(VulkanSwapchain, GraphicsSwapchain, "VulkanSwapchain")

VulkanSwapchain::VulkanSwapchain() noexcept
	: _colorSpace(VK_COLORSPACE_SRGB_NONLINEAR_KHR)
	, _surface(VK_NULL_HANDLE)
	, _vkSwapchain(VK_NULL_HANDLE)
	, _swapImageIndex(0)
{
}

VulkanSwapchain::~VulkanSwapchain() noexcept
{
	this->close();
}

bool
VulkanSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	_swapchainDesc = swapchainDesc;

	if (!initSurface())
		return false;

	if (!initSemaphore())
		return false;

	if (!initSwapchain())
		return false;

	if (!initSwapchainColorImageView())
		return false;
	
	if (!initSwapchainDepthView())
		return false;

	if (!initFramebuffer())
		return false;

	return true;
}

void 
VulkanSwapchain::close() noexcept
{
	auto device = this->getDevice()->downcast<VulkanDevice>();

	_swapchainFramebuffers.clear();

	_swapchainDepthImageView.reset();
	_swapchainImageViews.clear();
	_swapchainSemaphore.reset();

	if (_vkSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device->getDevice(), _vkSwapchain, 0);
		_vkSwapchain = VK_NULL_HANDLE;
	}

	if (_surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(device->getInstance(), _surface, 0);
		_surface = VK_NULL_HANDLE;
	}
}

void 
VulkanSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	_swapchainDesc.setSwapInterval(interval);
}

GraphicsSwapInterval 
VulkanSwapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

bool 
VulkanSwapchain::initSurface() noexcept
{
	VkWin32SurfaceCreateInfoKHR info;
	info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	info.pNext = 0;
	info.flags = 0;
	info.hinstance = nullptr;
	info.hwnd = (HWND)_swapchainDesc.getWindHandle();

	if (vkCreateWin32SurfaceKHR(this->getDevice()->downcast<VulkanDevice>()->getInstance(), &info, 0, &_surface) > 0)
	{
		VK_PLATFORM_LOG("vkCreateWin32SurfaceKHR() fail");
		return false;
	}

	return true;
}

bool 
VulkanSwapchain::initSemaphore() noexcept
{
	GraphicsSemaphoreDesc semaphoreDesc;
	_swapchainSemaphore = this->getDevice()->downcast<VulkanDevice>()->createSemaphore(semaphoreDesc);
	return _swapchainSemaphore ? true : false;
}

bool 
VulkanSwapchain::initSwapchain() noexcept
{
	std::uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), _surface, &formatCount, NULL);

	std::vector<VkSurfaceFormatKHR> surfFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), _surface, &formatCount, &surfFormats[0]);

	bool formatFound = false;
	for (auto& format : surfFormats)
	{
		if (format.colorSpace == _colorSpace && VulkanTypes::asGraphicsFormat(_swapchainDesc.getColorFormat()) == format.format)
		{
			formatFound = true;
			break;
		}
	}

	if (!formatFound)
	{
		VK_PLATFORM_LOG("Can't support color format.");
		return false;
	}

	if (!VulkanTypes::isDepthFormat(_swapchainDesc.getDepthFormat()) &&
		!VulkanTypes::isDepthStencilFormat(_swapchainDesc.getDepthFormat()))
	{
		VK_PLATFORM_LOG("Invalid depth format");
		return false;
	}

	VkSurfaceCapabilitiesKHR surfCapabilities;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), _surface, &surfCapabilities) > 0)
		return false;

	VkSurfaceTransformFlagBitsKHR preTransform;
	if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = surfCapabilities.currentTransform;

	if (_swapchainDesc.getImageNums() < surfCapabilities.minImageCount &&
		_swapchainDesc.getImageNums() > surfCapabilities.maxImageCount)
	{
		VK_PLATFORM_LOG("Invlida image count, min: %d, max: %d", surfCapabilities.minImageCount, surfCapabilities.maxImageCount);
		return false;
	}

	std::uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), &queueCount, 0);

	std::uint32_t queueFamilyIndexCount = 0;
	for (std::uint32_t i = 0; i < queueCount; i++)
	{
		VkBool32 supportsPresent;
		vkGetPhysicalDeviceSurfaceSupportKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), i, _surface, &supportsPresent);
		if (supportsPresent)
		{
			queueFamilyIndexCount = i;
			break;
		}
	}

	std::uint32_t presentModeCount = 0;
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), _surface, &presentModeCount, nullptr) > 0)
		return false;

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), _surface, &presentModeCount, &presentModes[0]) > 0)
		return false;

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (std::size_t i = 0; i < presentModeCount; i++)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}

		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
		{
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	VkSwapchainCreateInfoKHR swapchain;
	swapchain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain.flags = 0;
	swapchain.pNext = nullptr;
	swapchain.surface = _surface;
	swapchain.minImageCount = _swapchainDesc.getImageNums();
	swapchain.imageFormat = VulkanTypes::asGraphicsFormat(_swapchainDesc.getColorFormat());
	swapchain.imageColorSpace = _colorSpace;
	swapchain.imageExtent.width = _swapchainDesc.getWidth();
	swapchain.imageExtent.height = _swapchainDesc.getHeight();
	swapchain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain.preTransform = preTransform;
	swapchain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain.imageArrayLayers = 1;
	swapchain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain.queueFamilyIndexCount = 0;
	swapchain.pQueueFamilyIndices = nullptr;
	swapchain.presentMode = swapchainPresentMode;
	swapchain.oldSwapchain = 0;
	swapchain.clipped = true;

	if (vkCreateSwapchainKHR(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &swapchain, nullptr, &_vkSwapchain) != VK_SUCCESS)
		return false;

	return _vkSwapchain != VK_NULL_HANDLE;
}

bool 
VulkanSwapchain::initSwapchainColorImageView() noexcept
{
	std::uint32_t swapchainImageCount = 0;
	if (vkGetSwapchainImagesKHR(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkSwapchain, &swapchainImageCount, nullptr) > 0)
		return false;

	std::vector<VkImage> swapchainImages(swapchainImageCount);
	if (vkGetSwapchainImagesKHR(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkSwapchain, &swapchainImageCount, &swapchainImages[0]) > 0)
		return false;

	for (std::uint32_t i = 0; i < swapchainImages.size(); i++)
	{
		GraphicsTextureDesc textureDesc;
		textureDesc.setTexFormat(_swapchainDesc.getColorFormat());
		textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
		textureDesc.setWidth(_swapchainDesc.getWidth());
		textureDesc.setHeight(_swapchainDesc.getHeight());
		textureDesc.setDepth(1);

		auto texture = std::make_shared<VulkanTexture>();
		texture->setDevice(this->getDevice());
		texture->setSwapchainImage(swapchainImages[i]);
		if (!texture->setup(textureDesc))
			return false;

		_swapchainImageViews.push_back(texture);
	}

	return true;
}

bool 
VulkanSwapchain::initSwapchainDepthView() noexcept
{
	GraphicsTextureDesc textureDesc;
	textureDesc.setTexFormat(_swapchainDesc.getDepthFormat());
	textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	textureDesc.setWidth(_swapchainDesc.getWidth());
	textureDesc.setHeight(_swapchainDesc.getHeight());
	textureDesc.setDepth(1);
	textureDesc.setTexUsage(GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsDepthStencilAttachmentBit);

	_swapchainDepthImageView = this->getDevice()->createTexture(textureDesc);
	return true;
}

bool 
VulkanSwapchain::initFramebuffer() noexcept
{
	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _swapchainDesc.getColorFormat(), 0));
	framebufferLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutShaderReadOnlyOptimal, _swapchainDesc.getDepthFormat(), 1));
	_swapchainFramebufferLayout = this->getDevice()->downcast<VulkanDevice>()->createFramebufferLayout(framebufferLayoutDesc);

	for (auto& swapchainImageView : _swapchainImageViews)
	{
		GraphicsFramebufferDesc framebufferDesc;
		framebufferDesc.setWidth(_swapchainDesc.getWidth());
		framebufferDesc.setHeight(_swapchainDesc.getHeight());
		framebufferDesc.attach(swapchainImageView);
		framebufferDesc.attach(_swapchainDepthImageView);
		framebufferDesc.setGraphicsFramebufferLayout(_swapchainFramebufferLayout);

		auto framebuffer = this->getDevice()->createFramebuffer(framebufferDesc);
		_swapchainFramebuffers.push_back(framebuffer);
	}

	return true;
}

VkSwapchainKHR
VulkanSwapchain::getSwapchain() const noexcept
{
	return _vkSwapchain;
}

GraphicsSemaphorePtr
VulkanSwapchain::getSemaphore() const noexcept
{
	return _swapchainSemaphore;
}

void
VulkanSwapchain::acquireNextImage() noexcept
{
	vkAcquireNextImageKHR(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkSwapchain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &_swapImageIndex);
}

std::uint32_t 
VulkanSwapchain::getSwapchainImageIndex() noexcept
{
	return _swapImageIndex;
}

const GraphicsTextures&
VulkanSwapchain::getSwapchainImages() const noexcept
{
	return _swapchainImageViews;
}

const GraphicsFramebuffers& 
VulkanSwapchain::getSwapchainFramebuffers() const noexcept
{
	return _swapchainFramebuffers;
}

const GraphicsSwapchainDesc&
VulkanSwapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
VulkanSwapchain::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr 
VulkanSwapchain::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END