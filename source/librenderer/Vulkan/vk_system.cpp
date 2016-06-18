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
#include "vk_system.h"
#include "vk_physical_device.h"

#pragma warning (push)
#pragma warning (disable:4458)
#pragma warning (disable:4464)
#pragma warning (disable:4623)
#pragma warning (disable:5026)
#pragma warning (disable:5027)
#include <SPIRV/GlslangToSpv.h>
#pragma warning (pop)

_NAME_BEGIN

__ImplementSingleton(VulkanSystem)

VulkanSystem::VulkanSystem() noexcept
	: _instance(VK_NULL_HANDLE)
	, _debugHandle(VK_NULL_HANDLE)
	, _isOpened(false)
{
}

VulkanSystem::~VulkanSystem() noexcept
{
	this->close();
}

bool
VulkanSystem::open() noexcept
{
	if (!_isOpened)
	{
		if (!ShInitialize())
			return false;

		if (!this->initInstance())
			return false;

		if (!this->initPhysicalDevices())
			return false;

		_isOpened = true;
	}

	return true;
}

void
VulkanSystem::close() noexcept
{
	::ShFinalize();

	this->stopDebugControl();

	for (auto& physicalDevice : _physicalDevices)
	{
		assert(physicalDevice.unique());
		physicalDevice.reset();
	}

	_physicalDevices.clear();

	if (_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(_instance, 0);
		_instance = VK_NULL_HANDLE;
	}
}

void 
VulkanSystem::getInstanceLayerNames(std::vector<char*>& instanceLayerNames) noexcept
{
	for (auto& it : _instanceLayers)
		instanceLayerNames.push_back(it.layerName);
}

void 
VulkanSystem::getInstanceExtensitionNames(std::vector<char*>& instanceExtensitionNames) noexcept
{
	for (auto& it : _instanceExtensions)
		instanceExtensitionNames.push_back(it.extensionName);
}

const GraphicsPhysicalDevices&
VulkanSystem::getPhysicalDevices() const noexcept
{
	return _physicalDevices;
}

void
VulkanSystem::print(const char* message, ...) noexcept
{
	va_list va;
	va_start(va, message);
	vprintf(message, va);
	printf("\n");
	va_end(va);
}

void
VulkanSystem::startDebugControl() noexcept
{
	if (_debugHandle != VK_NULL_HANDLE)
		return;

	PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT = nullptr;
	fpCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
	if (!fpCreateDebugReportCallbackEXT)
	{
		this->print("vkGetInstanceProcAddr('vkCreateDebugReportCallbackEXT') fail.");
		return;
	}

	VkDebugReportCallbackCreateInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	info.pNext = 0;
	info.pfnCallback = dbgFunc;
	info.pUserData = this;
	info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;

	VkResult err = fpCreateDebugReportCallbackEXT(_instance, &info, NULL, &_debugHandle);
	switch (err)
	{
	case VK_SUCCESS:
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		this->print("CreateDebugReportCallback: out of host memory\n CreateDebugReportCallback Failure.");
		return;
	default:
		this->print("CreateDebugReportCallback: unknown failure\n CreateDebugReportCallback Failure.");
		return;
	}
}

void
VulkanSystem::stopDebugControl() noexcept
{
	if (_debugHandle != VK_NULL_HANDLE)
	{
		PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT = nullptr;
		fpDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
		if (!fpDestroyDebugReportCallbackEXT)
		{
			this->print("vkGetInstanceProcAddr('DestroyDebugReportCallbackEXT') fail.");
			return;
		}

		fpDestroyDebugReportCallbackEXT(_instance, _debugHandle, nullptr);

		_debugHandle = VK_NULL_HANDLE;
	}
}

VkInstance
VulkanSystem::getInstance() const noexcept
{
	return _instance;
}

bool
VulkanSystem::checkInstanceLayer(std::size_t instanceEnabledLayerCount, const char* instanceValidationLayerNames[]) noexcept
{
	std::uint32_t instanceLayerCount = 0;
	if (vkEnumerateInstanceLayerProperties(&instanceLayerCount, 0) != VK_SUCCESS)
	{
		this->print("vkEnumerateInstanceLayerProperties fail.");
		return false;
	}

	bool validationFound = instanceEnabledLayerCount <= instanceLayerCount ? true : false;
	if (instanceLayerCount > 0)
	{
		_instanceLayers.resize(instanceLayerCount);

		if (vkEnumerateInstanceLayerProperties(&instanceLayerCount, &_instanceLayers[0]) != VK_SUCCESS)
		{
			this->print("vkEnumerateInstanceLayerProperties fail.");
			return false;
		}

		for (std::uint32_t i = 0; i < instanceEnabledLayerCount; i++)
		{
			VkBool32 found = 0;
			for (uint32_t j = 0; j < instanceLayerCount; j++)
			{
				if (!strcmp(instanceValidationLayerNames[i], _instanceLayers[j].layerName))
				{
					found = 1;
					break;
				}
			}

			if (!found)
			{
				this->print("Cannot find layer: %s", instanceValidationLayerNames[i]);
				validationFound = false;
				break;
			}
		}
	}

	if (!validationFound)
	{
		this->print("vkEnumerateInstanceLayerProperties failed to find"
			"required validation layer.\n\n"
			"Please look at the Getting Started guide for additional"
			"information.");

		return false;
	}

	return true;
}

bool
VulkanSystem::checkInstanceExtenstion(std::size_t instanceEnabledLayerCount, const char* instanceValidationLayerNames[]) noexcept
{
	bool surfaceExtFound = false;
	bool platformSurfaceExtFound = false;
	char* extensionNames[64];

	std::uint32_t enabledExtensionCount = 0;
	std::uint32_t instanceExtensionCount = 0;

	if (vkEnumerateInstanceExtensionProperties(0, &instanceExtensionCount, 0) != VK_SUCCESS)
	{
		this->print("instanceExtensionCount fail.");
		return false;
	}

	if (instanceExtensionCount > 0)
	{
		_instanceExtensions.resize(instanceExtensionCount);

		if (vkEnumerateInstanceExtensionProperties(0, &instanceExtensionCount, _instanceExtensions.data()) != VK_SUCCESS)
		{
			this->print("instanceExtensionCount fail.");
			return false;
		}

		for (std::uint32_t i = 0; i < instanceExtensionCount; i++)
		{
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, _instanceExtensions[i].extensionName))
			{
				surfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}

#ifdef _WIN32
			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, _instanceExtensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#else
			if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, _instanceExtensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
			}
#endif
			if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, _instanceExtensions[i].extensionName))
			{
				extensionNames[enabledExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
			}
		}
	}

	if (!surfaceExtFound)
	{
		this->print("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n");
		return false;
	}

	if (!platformSurfaceExtFound)
	{
#ifdef _WIN32
		this->print("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n");
#else
		this->print("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n");
#endif
		return false;
	}

	return true;
}

bool
VulkanSystem::initInstance() noexcept
{
	static const char* instanceValidationLayerNames[] = { "VK_LAYER_LUNARG_swapchain" };
	static const char* instanceExtensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME };

	if (!this->checkInstanceLayer(0, nullptr))
		return false;

	if (!this->checkInstanceExtenstion(3, instanceExtensionNames))
		return false;

	VkApplicationInfo app;
	app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app.pNext = NULL;
	app.pApplicationName = "Vulkan";
	app.applicationVersion = 0;
	app.pEngineName = "Vulkan";
	app.engineVersion = 0;
	app.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.flags = 0;
	info.pNext = 0;
	info.pApplicationInfo = &app;
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = nullptr;
	info.enabledExtensionCount = sizeof(instanceExtensionNames) / sizeof(instanceExtensionNames[0]);
	info.ppEnabledExtensionNames = instanceExtensionNames;

	VkResult err = vkCreateInstance(&info, nullptr, &_instance);
	if (err == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		this->print("Cannot find a compatible Vulkan installable client driver "
			"(ICD).\n\nPlease look at the Getting Started guide for "
			"additional information.");
		return false;
	}
	else if (err == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		this->print("Cannot find a specified extension library"
			".\nMake sure your layers path is set appropriately.");
		return false;
	}
	else if (err)
	{
		this->print("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
			"installable client driver (ICD) installed?\nPlease look at "
			"the Getting Started guide for additional information.");
		return false;
	}

	return true;
}

bool 
VulkanSystem::initPhysicalDevices() noexcept
{
	std::uint32_t deviceCount = 0;
	if (vkEnumeratePhysicalDevices(VulkanSystem::instance()->getInstance(), &deviceCount, 0) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkEnumeratePhysicalDevices fail");
		return false;
	}

	if (deviceCount > 0)
	{
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);

		if (vkEnumeratePhysicalDevices(VulkanSystem::instance()->getInstance(), &deviceCount, &physicalDevices[0]) != VK_SUCCESS)
		{
			VK_PLATFORM_LOG("vkEnumeratePhysicalDevices fail");
			return false;
		}

		for (auto& it : physicalDevices)
		{
			auto physicalDevice = std::make_shared<VulkanPhysicalDevice>();
			if (physicalDevice->setup(it))
			{
				_physicalDevices.push_back(physicalDevice);
			}
		}
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanSystem::dbgFunc(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void* pUserData)
{
	VulkanSystem* device = (VulkanSystem*)pUserData;
	device->print(pMsg);
	device->print("\n");
	return false;
}

_NAME_END