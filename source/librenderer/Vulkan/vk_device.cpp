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
#include "vk_device.h"
#include "vk_device_context.h"
#include "vk_device_property.h"
#include "vk_command_list.h"
#include "vk_command_pool.h"
#include "vk_command_queue.h"
#include "vk_swapchain.h"
#include "vk_system.h"
#include "vk_texture.h"
#include "vk_sampler.h"
#include "vk_shader.h"
#include "vk_framebuffer.h"
#include "vk_input_layout.h"
#include "vk_pipeline.h"
#include "vk_graphics_data.h"
#include "vk_render_state.h"
#include "vk_descriptor_set.h"
#include "vk_semaphore.h"

_NAME_BEGIN

__ImplementSubClass(VulkanDevice, GraphicsDevice2, "VulkanDevice")

VulkanDevice::VulkanDevice() noexcept
	: _device(VK_NULL_HANDLE)
{
}

VulkanDevice::~VulkanDevice() noexcept
{
	this->close();
}

bool
VulkanDevice::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
{
	assert(deviceDesc.getDeviceType() == GraphicsDeviceType::GraphicsDeviceTypeVulkan);

	static const char* extensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	_physicalDevice = VulkanSystem::instance()->getPhysicalDevices().front()->downcast_pointer<VulkanDeviceProperty>();

	VkPhysicalDevice physicalDevice = _physicalDevice->downcast<VulkanDeviceProperty>()->getPhysicalDevice();

	std::uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, 0);

	std::vector<VkQueueFamilyProperties> props(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, &props[0]);

	std::uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	for (std::uint32_t i = 0; i < queueCount; i++)
	{
		if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueNodeIndex = i;
			break;
		}
	}

	const float queuePriorities[1] = { 0.0 };

	VkDeviceQueueCreateInfo queue;
	queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue.pNext = 0;
	queue.flags = 0;
	queue.queueFamilyIndex = graphicsQueueNodeIndex;
	queue.queueCount = 1;
	queue.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.flags = 0;
	info.pNext = 0;
	info.queueCreateInfoCount = 1;
	info.pQueueCreateInfos = &queue;
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = nullptr;
	info.enabledExtensionCount = 1;
	info.ppEnabledExtensionNames = extensionNames;
	info.pEnabledFeatures = 0;

#if defined(_BUILD_PLATFORM_WINDOWS)
	if (!vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, graphicsQueueNodeIndex))
	{
		VK_PLATFORM_LOG("vkGetPhysicalDeviceWin32PresentationSupportKHR fail.");
		return false;
	}
#elif defined(_BUILD_PLATFORM_WINDOWS)
	if (!vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, graphicsQueueNodeIndex))
	{
		VK_PLATFORM_LOG("vkGetPhysicalDeviceWin32PresentationSupportKHR fail.");
		return false;
	}
#endif

	if (vkCreateDevice(physicalDevice, &info, 0, &_device) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateDevice fail.\n");
		return false;
	}

	_deviceDesc = deviceDesc;
	return true;
}

void
VulkanDevice::close() noexcept
{
	if (_device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(_device, nullptr);
		_device = VK_NULL_HANDLE;
	}

	VulkanSystem::instance()->stopDebugControl();
}

GraphicsSwapchainPtr
VulkanDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	auto swapchain = std::make_shared<VulkanSwapchain>();
	swapchain->setDevice(this->downcast_pointer<VulkanDevice>());
	if (swapchain->setup(desc))
		return swapchain;
	return nullptr;
}

GraphicsContextPtr
VulkanDevice::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<VulkanDeviceContext>();
	context->setDevice(this->downcast_pointer<VulkanDevice>());
	if (context->setup(desc))
		return context;
	return nullptr;
}

GraphicsTexturePtr
VulkanDevice::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<VulkanTexture>();
	texture->setDevice(this->downcast_pointer<VulkanDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
VulkanDevice::createSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<VulkanSampler>();
	sampler->setDevice(this->downcast_pointer<VulkanDevice>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsFramebufferLayoutPtr
VulkanDevice::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	auto pass = std::make_shared<VulkanFramebufferLayout>();
	pass->setDevice(this->downcast_pointer<VulkanDevice>());
	if (pass->setup(desc))
		return pass;
	return nullptr;
}

GraphicsFramebufferPtr
VulkanDevice::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<VulkanFramebuffer>();
	framebuffer->setDevice(this->downcast_pointer<VulkanDevice>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsShaderPtr
VulkanDevice::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<VulkanShader>();
	shader->setDevice(this->downcast_pointer<VulkanDevice>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
VulkanDevice::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<VulkanProgram>();
	program->setDevice(this->downcast_pointer<VulkanDevice>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr
VulkanDevice::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<VulkanPipeline>();
	pipeline->setDevice(this->downcast_pointer<VulkanDevice>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDataPtr
VulkanDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto buffer = std::make_shared<VulkanGraphicsData>();
	buffer->setDevice(this->downcast_pointer<VulkanDevice>());
	if (buffer->setup(desc))
		return buffer;
	return nullptr;
}

GraphicsInputLayoutPtr
VulkanDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<VulkanInputLayout>();
	inputLayout->setDevice(this->downcast_pointer<VulkanDevice>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsStatePtr
VulkanDevice::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto renderState = std::make_shared<VulkanRenderState>();
	renderState->setDevice(this->downcast_pointer<VulkanDevice>());
	if (renderState->setup(desc))
		return renderState;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr
VulkanDevice::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast_pointer<VulkanDevice>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

GraphicsDescriptorSetPtr
VulkanDevice::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<VulkanDescriptorSet>();
	descriptorSet->setDevice(this->downcast_pointer<VulkanDevice>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsDescriptorPoolPtr
VulkanDevice::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	auto descriptorPool = std::make_shared<VulkanDescriptorPool>();
	descriptorPool->setDevice(this->downcast_pointer<VulkanDevice>());
	if (descriptorPool->setup(desc))
		return descriptorPool;
	return nullptr;
}

GraphicsCommandQueuePtr
VulkanDevice::createCommandQueue(const GraphicsCommandQueueDesc& desc) noexcept
{
	auto queue = std::make_shared<VulkanCommandQueue>();
	queue->setDevice(this->downcast_pointer<VulkanDevice>());
	if (queue->setup(desc))
		return queue;
	return nullptr;
}

GraphicsCommandPoolPtr
VulkanDevice::createCommandPool(const GraphicsCommandPoolDesc& desc) noexcept
{
	auto pool = std::make_shared<VulkanCommandPool>();
	pool->setDevice(this->downcast_pointer<VulkanDevice>());
	if (pool->setup(desc))
		return pool;
	return nullptr;
}

GraphicsCommandListPtr
VulkanDevice::createCommandList(const GraphicsCommandListDesc& desc) noexcept
{
	auto list = std::make_shared<VulkanCommandList>();
	list->setDevice(this->downcast_pointer<VulkanDevice>());
	if (list->setup(desc))
		return list;
	return nullptr;
}

GraphicsSemaphorePtr
VulkanDevice::createSemaphore(const GraphicsSemaphoreDesc& desc) noexcept
{
	auto semaphore = std::make_shared<VulkanSemaphore>();
	semaphore->setDevice(this->downcast_pointer<VulkanDevice>());
	if (semaphore->setup(desc))
		return semaphore;
	return nullptr;
}

void
VulkanDevice::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
{
}

const GraphicsDeviceProperty&
VulkanDevice::getGraphicsDeviceProperty() const noexcept
{
	return *_physicalDevice;
}

const GraphicsDeviceDesc&
VulkanDevice::getGraphicsDeviceDesc() const noexcept
{
	return _deviceDesc;
}

VkDevice
VulkanDevice::getDevice() const noexcept
{
	return _device;
}

VkPhysicalDevice 
VulkanDevice::getPhysicalDevice() const noexcept
{
	return _physicalDevice->getPhysicalDevice();
}

_NAME_END