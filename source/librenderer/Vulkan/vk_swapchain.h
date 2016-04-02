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
#ifndef _H_VK_SWAPCHAIN_H_
#define _H_VK_SWAPCHAIN_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanSwapchain final : public GraphicsSwapchain
{
	__DeclareSubClass(VulkanSwapchain, GraphicsSwapchain)
public:
	VulkanSwapchain() noexcept;
	~VulkanSwapchain() noexcept;

	bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
	void close() noexcept;

	void setSwapInterval(GraphicsSwapInterval interval) noexcept;
	GraphicsSwapInterval getSwapInterval() const noexcept;

	VkSwapchainKHR getSwapchain() const noexcept;
	VkSemaphore getSemaphore() const noexcept;

	void acquireNextImage() noexcept;
	std::uint32_t getSwapchainImageIndex() noexcept;

	const GraphicsTextures& getSwapchainImages() const noexcept;
	const GraphicsFramebuffers&  getSwapchainFramebuffers() const noexcept;

	const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept;

private:
	bool initSurface() noexcept;
	bool initSemaphore() noexcept;
	bool initSwapchain() noexcept;
	bool initSwapchainColorImageView() noexcept;
	bool initSwapchainDepthView() noexcept;
	bool initFramebuffer() noexcept;

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanSwapchain(const VulkanSwapchain&) noexcept = delete;
	VulkanSwapchain& operator=(const VulkanSwapchain&) noexcept = delete;

private:

	VkSurfaceKHR _surface;
	VkSemaphore _vkSemaphore;
	VkColorSpaceKHR _colorSpace;
	VkSwapchainKHR _vkSwapchain;

	std::uint32_t _swapImageIndex;

	GraphicsTextures _swapchainImageViews;
	GraphicsTexturePtr _swapchainDepthImageView;

	GraphicsFramebuffers _swapchainFramebuffers;
	GraphicsFramebufferLayoutPtr _swapchainFramebufferLayout;

	GraphicsSwapchainDesc _swapchainDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif