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
#include "vk_render_pipeline.h"
#include "vk_graphics_data.h"
#include "vk_render_state.h"
#include "vk_descriptor_pool.h"
#include "vk_descriptor_set.h"
#include "vk_descriptor_set_layout.h"
#include "vk_semaphore.h"

_NAME_BEGIN

char* deviceValidationLayers[] = {
	"VK_LAYER_LUNARG_threading",      "VK_LAYER_LUNARG_mem_tracker",
	"VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_draw_state",
	"VK_LAYER_LUNARG_param_checker",  "VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
	"VK_LAYER_GOOGLE_unique_objects",
};

__ImplementSubClass(VulkanDevice, GraphicsDevice, "VulkanDevice")

VulkanDevice::VulkanDevice() noexcept
	: _device(VK_NULL_HANDLE)
	, _physicalDevice(VK_NULL_HANDLE)
{
}

VulkanDevice::~VulkanDevice() noexcept
{
	this->close();
}

bool 
VulkanDevice::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
{
	if (!checkInstance())
		return false;

	if (!checkDeviceLayer())
		return false;

	if (!checkDeviceExtension())
		return false;

	std::uint32_t deviceCount = 0;
	if (vkEnumeratePhysicalDevices(VulkanSystem::instance()->getInstance(), &deviceCount, 0) > 0)
	{
		this->print("vkEnumeratePhysicalDevices fail");
		return false;
	}

	if (deviceCount > 0)
	{
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);

		if (vkEnumeratePhysicalDevices(VulkanSystem::instance()->getInstance(), &deviceCount, &physicalDevices[0]) > 0)
		{
			this->print("vkEnumeratePhysicalDevices fail");
			return false;
		}

		_physicalDevice = physicalDevices.front();
	}

	std::uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, 0);

	std::vector<VkQueueFamilyProperties> props(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, &props[0]);
	
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
	const char* extensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
	info.enabledLayerCount = sizeof(deviceValidationLayers) / sizeof(deviceValidationLayers[0]);
	info.ppEnabledLayerNames = deviceValidationLayers;
	info.enabledExtensionCount = 1;
	info.ppEnabledExtensionNames = &extensionNames;
	info.pEnabledFeatures = 0;

	if (!vkGetPhysicalDeviceWin32PresentationSupportKHR(_physicalDevice, graphicsQueueNodeIndex))
		return false;

	if (vkCreateDevice(_physicalDevice, &info, 0, &_device) > 0)
	{
		this->print("vkCreateDevice fail.\n");
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
		vkDestroyDevice(_device, 0);
		_device = VK_NULL_HANDLE;
	}
}

GraphicsSwapchainPtr 
VulkanDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	auto swapchain = std::make_shared<VulkanSwapchain>();
	swapchain->setDevice(this->downcast<VulkanDevice>());
	if (swapchain->setup(desc))
		return swapchain;
	return nullptr;
}

GraphicsContextPtr 
VulkanDevice::createGraphicsContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<VulkanDeviceContext>();
	context->setDevice(this->downcast<VulkanDevice>());
	if (context->setup(desc))
		return context;
	return nullptr;
}

GraphicsTexturePtr 
VulkanDevice::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<VulkanTexture>();
	texture->setDevice(this->downcast<VulkanDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr 
VulkanDevice::createSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<VulkanSampler>();
	sampler->setDevice(this->downcast<VulkanDevice>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsFramebufferLayoutPtr
VulkanDevice::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	auto pass = std::make_shared<VulkanFramebufferLayout>();
	pass->setDevice(this->downcast<VulkanDevice>());
	if (pass->setup(desc))
		return pass;
	return nullptr;
}

GraphicsFramebufferPtr 
VulkanDevice::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<VulkanFramebuffer>();
	framebuffer->setDevice(this->downcast<VulkanDevice>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsShaderPtr
VulkanDevice::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<VulkanShader>();
	shader->setDevice(this->downcast<VulkanDevice>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
VulkanDevice::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<VulkanShaderObject>();
	program->setDevice(this->downcast<VulkanDevice>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr 
VulkanDevice::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<VulkanRenderPipeline>();
	pipeline->setDevice(this->downcast<VulkanDevice>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDataPtr 
VulkanDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto buffer = std::make_shared<VulkanGraphicsData>();
	buffer->setDevice(this->downcast<VulkanDevice>());
	if (buffer->setup(desc))
		return buffer;
	return nullptr;
}

GraphicsInputLayoutPtr 
VulkanDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<VulkanInputLayout>();
	inputLayout->setDevice(this->downcast<VulkanDevice>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsStatePtr 
VulkanDevice::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto renderState = std::make_shared<VulkanRenderState>();
	renderState->setDevice(this->downcast<VulkanDevice>());
	if (renderState->setup(desc))
		return renderState;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr
VulkanDevice::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast<VulkanDevice>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

GraphicsDescriptorPoolPtr 
VulkanDevice::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	auto descriptorPool = std::make_shared<VulkanDescriptorPool>();
	descriptorPool->setDevice(this->downcast<VulkanDevice>());
	if (descriptorPool->setup(desc))
		return descriptorPool;
	return nullptr;
}

GraphicsDescriptorSetPtr 
VulkanDevice::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<VulkanDescriptorSet>();
	descriptorSet->setDevice(this->downcast<VulkanDevice>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsCommandQueuePtr
VulkanDevice::createGraphicsCommandQueue(const GraphicsCommandQueueDesc& desc) noexcept
{
	auto queue = std::make_shared<VulkanCommandQueue>();
	queue->setDevice(this->downcast<VulkanDevice>());
	if (queue->setup(desc))
		return queue;
	return nullptr;
}

GraphicsCommandPoolPtr
VulkanDevice::createGraphicsCommandPool(const GraphicsCommandPoolDesc& desc) noexcept
{
	auto pool = std::make_shared<VulkanCommandPool>();
	pool->setDevice(this->downcast<VulkanDevice>());
	if (pool->setup(desc))
		return pool;
	return nullptr;
}

GraphicsCommandListPtr
VulkanDevice::createGraphicsCommandList(const GraphicsCommandListDesc& desc) noexcept
{
	auto list = std::make_shared<VulkanCommandList>();
	list->setDevice(this->downcast<VulkanDevice>());
	if (list->setup(desc))
		return list;
	return nullptr;
}

GraphicsSemaphorePtr
VulkanDevice::createSemaphore(const GraphicsSemaphoreDesc& desc) noexcept
{
	auto semaphore = std::make_shared<VulkanSemaphore>();
	semaphore->setDevice(this->downcast<VulkanDevice>());
	if (semaphore->setup(desc))
		return semaphore;
	return nullptr;
}

GraphicsFormat 
VulkanDevice::findCompatibleFormat(GraphicsPixelFormatDesc& desc) noexcept
{
	auto minR = desc.getMinRedBits();
	auto maxR = desc.getMaxRedBits();
	auto minG = desc.getMinGreenBits();
	auto maxG = desc.getMaxGreenBits();
	auto minB = desc.getMinBlueBits();
	auto maxB = desc.getMaxBlueBits();
	auto minA = desc.getMinAlphaBits();
	auto maxA = desc.getMaxAlphaBits();
	auto minDepth = desc.getMinDepthBits();
	auto maxDepth = desc.getMaxDepthBits();
	auto minStencil = desc.getMinStencilBits();
	auto maxStencil = desc.getMaxStencilBits();

	auto type = desc.getFormatType();
	switch (type)
	{
	case GraphicsFormatType::GraphicsFormatTypeRInt:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32SInt;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SInt;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUInt:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32UInt;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UInt;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSRGB:
	{
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSNorm:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SNorm;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUNorm:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UNorm;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSScaled:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SScaled;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUScaled:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UScaled;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRFloat:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32SFloat;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SInt;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UInt;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SNorm;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UNorm;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UNorm;
		if (minR <= 4 && maxR >= 4 && minR <= 4 && maxR >= 4)
			return GraphicsFormat::GraphicsFormatR4G4UNormPack8;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UNorm;
		if (minR <= 5 && maxR >= 5 && minG <= 6 && maxG >= 6 && minB <= 5 && maxB >= 5)
			return GraphicsFormat::GraphicsFormatR5G6B5UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16UInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UNorm;
		if (minR <= 4 && maxR >= 4 && minG <= 4 && maxG >= 4 && minB <= 4 && maxB >= 4 && minA <= 4 && maxA >= 4)
			return GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16;
		if (minR <= 5 && maxR >= 5 && minG <= 5 && maxG >= 5 && minB <= 5 && maxB >= 5 && minA <= 1 && maxA >= 1)
			return GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UNorm;
		if (minR <= 5 && maxR >= 5 && minG <= 6 && maxG >= 6 && minB <= 5 && maxB >= 5)
			return GraphicsFormat::GraphicsFormatB5G6R5UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SRGB;
		if (minR <= 5 && maxR >= 5 && minG <= 5 && maxG >= 5 && minB <= 5 && maxB >= 5 && minA <= 1 && maxA >= 1)
			return GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16;
		if (minR <= 4 && maxR >= 4 && minG <= 4 && maxG >= 4 && minB <= 4 && maxB >= 4 && minA <= 4 && maxA >= 4)
			return GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SInt;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeDepth:
	{
		if (minDepth <= 32 && maxDepth >= 32)
			return GraphicsFormat::GraphicsFormatD32_SFLOAT;
		if (minDepth <= 24 && maxDepth >= 24)
			return GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
		if (minDepth <= 16 && maxDepth >= 16)
			return GraphicsFormat::GraphicsFormatD16UNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeDepthStencil:
	{
		if (minDepth <= 32 && maxDepth >= 32 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt;
		if (minDepth <= 24 && maxDepth >= 24 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD24UNorm_S8UInt;
		if (minDepth <= 16 && maxDepth >= 16 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD16UNorm_S8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRFloat:
	case GraphicsFormatType::GraphicsFormatTypeBGRAFloat:
	case GraphicsFormatType::GraphicsFormatTypeABGRFloat:
	default:
		VK_PLATFORM_ASSERT(false, "Can't support format");
		return GraphicsFormat::GraphicsFormatUndefined;
	}

	return GraphicsFormat::GraphicsFormatUndefined;
}

const GraphicsDeviceDesc& 
VulkanDevice::getGraphicsDeviceDesc() const noexcept
{
	return _deviceDesc;
}

bool
VulkanDevice::getDeviceName(std::string& deviceName) const noexcept
{
	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(_physicalDevice, &prop);
	deviceName = prop.deviceName;
	return true;
}

VkInstance 
VulkanDevice::getInstance() const noexcept
{
	return VulkanSystem::instance()->getInstance();
}

VkDevice
VulkanDevice::getDevice() const noexcept
{
	return _device;
}

VkPhysicalDevice 
VulkanDevice::getPhysicsDevice() const noexcept
{
	return _physicalDevice;
}

bool
VulkanDevice::checkInstance() noexcept
{
	return VulkanSystem::instance()->open();
}

bool
VulkanDevice::checkDeviceLayer() noexcept
{
	std::uint32_t deviceLayerCount = 0;
	std::uint32_t deviceEnabledLayerCount = sizeof(deviceValidationLayers) / sizeof(deviceValidationLayers[0]);

	if (vkEnumerateDeviceLayerProperties(_physicalDevice, &deviceLayerCount, 0) > 0)
	{
		this->print("vkEnumerateDeviceLayerProperties fail.\n");
		return false;
	}

	if (deviceLayerCount > 0) 
	{
		std::vector<VkLayerProperties> deviceLayers(deviceLayerCount);
		if (vkEnumerateDeviceLayerProperties(_physicalDevice, &deviceLayerCount, &deviceLayers[0]) > 0)
		{
			this->print("vkEnumerateDeviceLayerProperties fail.\n");
			return false;
		}

		bool validationFound = true;

		for (std::uint32_t i = 0; i < deviceEnabledLayerCount; i++)
		{
			VkBool32 found = 0;
			for (uint32_t j = 0; j < deviceLayerCount; j++)
			{
				if (!strcmp(deviceValidationLayers[i], deviceLayers[j].layerName))
				{
					found = 1;
					break;
				}
			}

			if (!found)
			{
				this->print(std::string("Cannot find layer: ") + deviceValidationLayers[i] + ".\n");
				validationFound = false;
				break;
			}
		}
	}

	return true;
}

bool
VulkanDevice::checkDeviceExtension() noexcept
{
	std::uint32_t deviceExtensionCount = 0;
	if (vkEnumerateDeviceExtensionProperties(_physicalDevice, 0, &deviceExtensionCount, 0) > 0)
	{
		this->print("vkEnumerateDeviceExtensionProperties fail.\n");
		return false;
	}

	bool swapchainExtFound = false;
	if (deviceExtensionCount > 0) 
	{
		std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);

		if (vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &deviceExtensionCount, &deviceExtensions[0]) > 0)
		{
			this->print("vkEnumerateDeviceExtensionProperties fail.\n");
			return false;
		}		

		for (uint32_t i = 0; i < deviceExtensionCount; i++)
		{
			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName)) 
			{
				swapchainExtFound = true;
			}
		}
	}

	if (!swapchainExtFound) 
	{
		this->print("vkEnumerateDeviceExtensionProperties failed to find "
			"the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n");
		return false;
	}

	return true;
}

void
VulkanDevice::print(const std::string& message) noexcept
{
	VulkanSystem::instance()->print(message);
}

_NAME_END