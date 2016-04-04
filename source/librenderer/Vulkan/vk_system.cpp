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
#include "vk_device.h"

#include <SPIRV/GlslangToSpv.h>

_NAME_BEGIN

char* instanceValidationLayers[] = {
	"VK_LAYER_LUNARG_threading",      "VK_LAYER_LUNARG_mem_tracker",
	"VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_draw_state",
	"VK_LAYER_LUNARG_param_checker",  "VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
	"VK_LAYER_GOOGLE_unique_objects",
};

const char* g_extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME };

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
		if (!glslang::InitializeProcess())
			return false;

		if (!this->initInstance())
			return false;

		_isOpened = true;
	}

	return true;
}

void
VulkanSystem::close() noexcept
{
	this->stopDebugControl();

	glslang::FinalizeProcess();

	if (_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(_instance, 0);
		_instance = VK_NULL_HANDLE;
	}
}

void
VulkanSystem::print(const std::string& message) noexcept
{
	std::cerr << message;
}

bool
VulkanSystem::startDebugControl() noexcept
{
	if (_debugHandle != VK_NULL_HANDLE)
		return true;

	PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT = nullptr;
	fpCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
	if (!fpCreateDebugReportCallbackEXT)
	{
		this->print("vkGetInstanceProcAddr('vkCreateDebugReportCallbackEXT') fail.");
		return false;
	}

	VkDebugReportCallbackCreateInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	info.pNext = 0;
	info.pfnCallback = dbgFunc;
	info.pUserData = this;
	info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

	VkResult err = fpCreateDebugReportCallbackEXT(_instance, &info, NULL, &_debugHandle);
	switch (err)
	{
	case VK_SUCCESS:
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		this->print("CreateDebugReportCallback: out of host memory\n CreateDebugReportCallback Failure.");
		return false;
	default:
		this->print("CreateDebugReportCallback: unknown failure\n CreateDebugReportCallback Failure.");
		return false;
	}

	return true;
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
VulkanSystem::checkInstanceLayer() noexcept
{
	std::uint32_t instanceLayerCount = 0;
	std::uint32_t instanceEnabledLayerCount = sizeof(instanceValidationLayers) / sizeof(instanceValidationLayers[0]);

	if (vkEnumerateInstanceLayerProperties(&instanceLayerCount, 0) > 0)
	{
		this->print("vkEnumerateInstanceLayerProperties fail.");
		return false;
	}

	if (instanceLayerCount > 0)
	{
		std::vector<VkLayerProperties> instanceLayers(instanceLayerCount);

		if (vkEnumerateInstanceLayerProperties(&instanceLayerCount, &instanceLayers[0]) > 0)
		{
			this->print("vkEnumerateInstanceLayerProperties fail.");
			return false;
		}

		bool validationFound = true;

		for (std::uint32_t i = 0; i < instanceEnabledLayerCount; i++)
		{
			VkBool32 found = 0;
			for (uint32_t j = 0; j < instanceLayerCount; j++)
			{
				if (!strcmp(instanceValidationLayers[i], instanceLayers[j].layerName))
				{
					found = 1;
					break;
				}
			}

			if (!found)
			{
				this->print(std::string("Cannot find layer: ") + instanceValidationLayers[i]);
				validationFound = false;
				break;
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
	}

	return true;
}

bool
VulkanSystem::checkInstanceExtenstion() noexcept
{
	bool surfaceExtFound = false;
	bool platformSurfaceExtFound = false;
	char* extensionNames[64];

	std::uint32_t enabledExtensionCount = 0;
	std::uint32_t instanceExtensionCount = 0;

	if (vkEnumerateInstanceExtensionProperties(0, &instanceExtensionCount, 0) > 0)
	{
		this->print("instanceExtensionCount fail.");
		return false;
	}

	if (instanceExtensionCount > 0)
	{
		std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);

		if (vkEnumerateInstanceExtensionProperties(0, &instanceExtensionCount, instanceExtensions.data()) > 0)
		{
			this->print("instanceExtensionCount fail.");
			return false;
		}

		for (std::uint32_t i = 0; i < instanceExtensionCount; i++)
		{
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
			{
				surfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}

#ifdef _WIN32
			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#else
			if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
			}
#endif
			if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName))
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
	if (!this->checkInstanceLayer())
		return false;

	if (!this->checkInstanceExtenstion())
		return false;

	VkApplicationInfo app;
	app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app.pNext = NULL;
	app.pApplicationName = "Vulkan";
	app.applicationVersion = 0;
	app.pEngineName = "Vulkan";
	app.engineVersion = 0;
	app.apiVersion = VK_API_VERSION;

	VkInstanceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.flags = 0;
	info.pNext = 0;
	info.pApplicationInfo = &app;
	info.enabledLayerCount = sizeof(instanceValidationLayers) / sizeof(instanceValidationLayers[0]);
	info.ppEnabledLayerNames = instanceValidationLayers;
	info.enabledExtensionCount = sizeof(g_extensionNames) / sizeof(g_extensionNames[0]);
	info.ppEnabledExtensionNames = g_extensionNames;

	VkResult err = vkCreateInstance(&info, nullptr, &_instance);
	if (err == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		this->print("Cannot find a compatible Vulkan installable client driver "
			"(ICD).\n\nPlease look at the Getting Started guide for "
			"additional information.\n");
		return false;
	}
	else if (err == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		this->print("Cannot find a specified extension library"
			".\nMake sure your layers path is set appropriately.\n");
		return false;
	}
	else if (err)
	{
		this->print("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
			"installable client driver (ICD) installed?\nPlease look at "
			"the Getting Started guide for additional information.\n");
		return false;
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