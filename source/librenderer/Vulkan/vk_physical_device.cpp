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
#include "vk_physical_device.h"
#include "vk_system.h"

_NAME_BEGIN

VulkanPhysicalDevice::VulkanPhysicalDevice() noexcept
	: _physicalDevice(VK_NULL_HANDLE)
{
}

VulkanPhysicalDevice::~VulkanPhysicalDevice() noexcept
{
	this->close();
}

bool
VulkanPhysicalDevice::setup(VkPhysicalDevice physicalDevice)
{
	if (!checkPhysicalDeviceLayer(physicalDevice, 0, nullptr))
		return false;

	if (!checkPhysicalDeviceExtension(physicalDevice, 0, nullptr))
		return false;

	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(physicalDevice, &prop);

	_deviceProperties.maxImageDimension1D = prop.limits.maxImageDimension1D;
	_deviceProperties.maxImageDimension2D = prop.limits.maxImageDimension2D;
	_deviceProperties.maxImageDimension3D = prop.limits.maxImageDimension3D;
	_deviceProperties.maxImageDimensionCube = prop.limits.maxImageDimensionCube;
	_deviceProperties.maxImageArrayLayers = prop.limits.maxImageArrayLayers;
	_deviceProperties.maxTexelBufferElements = prop.limits.maxTexelBufferElements;
	_deviceProperties.maxUniformBufferRange = prop.limits.maxUniformBufferRange;
	_deviceProperties.maxStorageBufferRange = prop.limits.maxStorageBufferRange;
	_deviceProperties.maxPushConstantsSize = prop.limits.maxPushConstantsSize;
	_deviceProperties.maxMemoryAllocationCount = prop.limits.maxMemoryAllocationCount;
	_deviceProperties.maxSamplerAllocationCount = prop.limits.maxSamplerAllocationCount;
	_deviceProperties.bufferImageGranularity = prop.limits.bufferImageGranularity;
	_deviceProperties.sparseAddressSpaceSize = prop.limits.sparseAddressSpaceSize;
	_deviceProperties.maxBoundDescriptorSets = prop.limits.maxBoundDescriptorSets;
	_deviceProperties.maxPerStageDescriptorSamplers = prop.limits.maxPerStageDescriptorSamplers;
	_deviceProperties.maxPerStageDescriptorUniformBuffers = prop.limits.maxPerStageDescriptorUniformBuffers;
	_deviceProperties.maxPerStageDescriptorStorageBuffers = prop.limits.maxPerStageDescriptorStorageBuffers;
	_deviceProperties.maxPerStageDescriptorSampledImages = prop.limits.maxPerStageDescriptorSampledImages;
	_deviceProperties.maxPerStageDescriptorStorageImages = prop.limits.maxPerStageDescriptorStorageImages;
	_deviceProperties.maxPerStageDescriptorInputAttachments = prop.limits.maxPerStageDescriptorInputAttachments;
	_deviceProperties.maxPerStageResources = prop.limits.maxPerStageResources;
	_deviceProperties.maxDescriptorSetSamplers = prop.limits.maxDescriptorSetSamplers;
	_deviceProperties.maxDescriptorSetUniformBuffers = prop.limits.maxDescriptorSetUniformBuffers;
	_deviceProperties.maxDescriptorSetUniformBuffersDynamic = prop.limits.maxDescriptorSetUniformBuffersDynamic;
	_deviceProperties.maxDescriptorSetStorageBuffers = prop.limits.maxDescriptorSetStorageBuffers;
	_deviceProperties.maxDescriptorSetStorageBuffersDynamic = prop.limits.maxDescriptorSetStorageBuffersDynamic;
	_deviceProperties.maxDescriptorSetSampledImages = prop.limits.maxDescriptorSetSampledImages;
	_deviceProperties.maxDescriptorSetStorageImages = prop.limits.maxDescriptorSetStorageImages;
	_deviceProperties.maxDescriptorSetInputAttachments = prop.limits.maxDescriptorSetInputAttachments;
	_deviceProperties.maxVertexInputAttributes = prop.limits.maxVertexInputAttributes;
	_deviceProperties.maxVertexInputBindings = prop.limits.maxVertexInputBindings;
	_deviceProperties.maxVertexInputAttributeOffset = prop.limits.maxVertexInputAttributeOffset;
	_deviceProperties.maxVertexInputBindingStride = prop.limits.maxVertexInputBindingStride;
	_deviceProperties.maxVertexOutputComponents = prop.limits.maxVertexOutputComponents;
	_deviceProperties.maxTessellationGenerationLevel = prop.limits.maxTessellationGenerationLevel;
	_deviceProperties.maxTessellationPatchSize = prop.limits.maxTessellationPatchSize;
	_deviceProperties.maxTessellationControlPerVertexInputComponents = prop.limits.maxTessellationControlPerVertexInputComponents;
	_deviceProperties.maxTessellationControlPerVertexOutputComponents = prop.limits.maxTessellationControlPerVertexOutputComponents;
	_deviceProperties.maxTessellationControlPerPatchOutputComponents = prop.limits.maxTessellationControlPerPatchOutputComponents;
	_deviceProperties.maxTessellationControlTotalOutputComponents = prop.limits.maxTessellationControlTotalOutputComponents;
	_deviceProperties.maxTessellationEvaluationInputComponents = prop.limits.maxTessellationEvaluationInputComponents;
	_deviceProperties.maxTessellationEvaluationOutputComponents = prop.limits.maxTessellationEvaluationOutputComponents;
	_deviceProperties.maxGeometryShaderInvocations = prop.limits.maxGeometryShaderInvocations;
	_deviceProperties.maxGeometryInputComponents = prop.limits.maxGeometryInputComponents;
	_deviceProperties.maxGeometryOutputComponents = prop.limits.maxGeometryOutputComponents;
	_deviceProperties.maxGeometryOutputVertices = prop.limits.maxGeometryOutputVertices;
	_deviceProperties.maxGeometryTotalOutputComponents = prop.limits.maxGeometryTotalOutputComponents;
	_deviceProperties.maxFragmentInputComponents = prop.limits.maxFragmentInputComponents;
	_deviceProperties.maxFragmentOutputAttachments = prop.limits.maxFragmentOutputAttachments;
	_deviceProperties.maxFragmentDualSrcAttachments = prop.limits.maxFragmentDualSrcAttachments;
	_deviceProperties.maxFragmentCombinedOutputResources = prop.limits.maxFragmentCombinedOutputResources;
	_deviceProperties.maxComputeSharedMemorySize = prop.limits.maxComputeSharedMemorySize;
	_deviceProperties.maxComputeWorkGroupCount[0] = prop.limits.maxComputeWorkGroupCount[0];
	_deviceProperties.maxComputeWorkGroupCount[1] = prop.limits.maxComputeWorkGroupCount[1];
	_deviceProperties.maxComputeWorkGroupCount[2] = prop.limits.maxComputeWorkGroupCount[2];
	_deviceProperties.maxComputeWorkGroupInvocations = prop.limits.maxComputeWorkGroupInvocations;
	_deviceProperties.maxComputeWorkGroupSize[0] = prop.limits.maxComputeWorkGroupSize[0];
	_deviceProperties.maxComputeWorkGroupSize[1] = prop.limits.maxComputeWorkGroupSize[1];
	_deviceProperties.maxComputeWorkGroupSize[2] = prop.limits.maxComputeWorkGroupSize[2];
	_deviceProperties.subPixelPrecisionBits = prop.limits.subPixelPrecisionBits;
	_deviceProperties.subTexelPrecisionBits = prop.limits.subTexelPrecisionBits;
	_deviceProperties.mipmapPrecisionBits = prop.limits.mipmapPrecisionBits;
	_deviceProperties.maxDrawIndexedIndexValue = prop.limits.maxDrawIndexedIndexValue;
	_deviceProperties.maxDrawIndirectCount = prop.limits.maxDrawIndirectCount;
	_deviceProperties.maxSamplerLodBias = prop.limits.maxSamplerLodBias;
	_deviceProperties.maxSamplerAnisotropy = prop.limits.maxSamplerAnisotropy;
	_deviceProperties.maxViewports = prop.limits.maxViewports;
	_deviceProperties.maxViewportDimensions[0] = prop.limits.maxViewportDimensions[0];
	_deviceProperties.maxViewportDimensions[1] = prop.limits.maxViewportDimensions[1];
	_deviceProperties.viewportBoundsRange[0] = prop.limits.viewportBoundsRange[0];
	_deviceProperties.viewportBoundsRange[1] = prop.limits.viewportBoundsRange[1];
	_deviceProperties.viewportSubPixelBits = prop.limits.viewportSubPixelBits;
	_deviceProperties.minMemoryMapAlignment = prop.limits.minMemoryMapAlignment;
	_deviceProperties.minTexelBufferOffsetAlignment = prop.limits.minTexelBufferOffsetAlignment;
	_deviceProperties.minUniformBufferOffsetAlignment = prop.limits.minUniformBufferOffsetAlignment;
	_deviceProperties.minStorageBufferOffsetAlignment = prop.limits.minStorageBufferOffsetAlignment;
	_deviceProperties.minTexelOffset = prop.limits.minTexelOffset;
	_deviceProperties.maxTexelOffset = prop.limits.maxTexelOffset;
	_deviceProperties.minTexelGatherOffset = prop.limits.minTexelGatherOffset;
	_deviceProperties.maxTexelGatherOffset = prop.limits.maxTexelGatherOffset;
	_deviceProperties.minInterpolationOffset = prop.limits.minInterpolationOffset;
	_deviceProperties.maxInterpolationOffset = prop.limits.maxInterpolationOffset;
	_deviceProperties.subPixelInterpolationOffsetBits = prop.limits.subPixelInterpolationOffsetBits;
	_deviceProperties.maxFramebufferWidth = prop.limits.maxFramebufferWidth;
	_deviceProperties.maxFramebufferHeight = prop.limits.maxFramebufferHeight;
	_deviceProperties.maxFramebufferLayers = prop.limits.maxFramebufferLayers;
	_deviceProperties.maxFramebufferColorAttachments = prop.limits.maxColorAttachments;
	_deviceProperties.framebufferColorSampleCounts = prop.limits.framebufferColorSampleCounts;
	_deviceProperties.framebufferDepthSampleCounts = prop.limits.framebufferDepthSampleCounts;
	_deviceProperties.framebufferStencilSampleCounts = prop.limits.framebufferStencilSampleCounts;
	_deviceProperties.framebufferNoAttachmentsSampleCounts = prop.limits.framebufferNoAttachmentsSampleCounts;
	_deviceProperties.sampledImageColorSampleCounts = prop.limits.sampledImageColorSampleCounts;
	_deviceProperties.sampledImageIntegerSampleCounts = prop.limits.sampledImageIntegerSampleCounts;
	_deviceProperties.sampledImageDepthSampleCounts = prop.limits.sampledImageDepthSampleCounts;
	_deviceProperties.sampledImageStencilSampleCounts = prop.limits.sampledImageStencilSampleCounts;
	_deviceProperties.storageImageSampleCounts = prop.limits.storageImageSampleCounts;
	_deviceProperties.maxSampleMaskWords = prop.limits.maxSampleMaskWords;
	_deviceProperties.timestampComputeAndGraphics = prop.limits.timestampComputeAndGraphics;
	_deviceProperties.timestampPeriod = prop.limits.timestampPeriod;
	_deviceProperties.maxClipDistances = prop.limits.maxClipDistances;
	_deviceProperties.maxCullDistances = prop.limits.maxCullDistances;
	_deviceProperties.maxCombinedClipAndCullDistances = prop.limits.maxCombinedClipAndCullDistances;
	_deviceProperties.discreteQueuePriorities = prop.limits.discreteQueuePriorities;
	_deviceProperties.minPointSizeRange = prop.limits.pointSizeRange[0];
	_deviceProperties.maxPointSizeRange = prop.limits.pointSizeRange[1];
	_deviceProperties.pointSizeGranularity = prop.limits.pointSizeGranularity;
	_deviceProperties.minLineWidthRange = prop.limits.lineWidthRange[0];
	_deviceProperties.maxLineWidthRange = prop.limits.lineWidthRange[1];
	_deviceProperties.lineWidthGranularity = prop.limits.lineWidthGranularity;
	_deviceProperties.strictLines = prop.limits.strictLines;
	_deviceProperties.standardSampleLocations = prop.limits.standardSampleLocations;
	_deviceProperties.optimalBufferCopyOffsetAlignment = prop.limits.optimalBufferCopyOffsetAlignment;
	_deviceProperties.optimalBufferCopyRowPitchAlignment = prop.limits.optimalBufferCopyRowPitchAlignment;
	_deviceProperties.nonCoherentAtomSize = prop.limits.nonCoherentAtomSize;

	_physicalDevice = physicalDevice;
	return true;
}

void
VulkanPhysicalDevice::close() noexcept
{
}

VkPhysicalDevice
VulkanPhysicalDevice::getPhysicalDevice() const noexcept
{
	return _physicalDevice;
}

bool
VulkanPhysicalDevice::checkPhysicalDeviceLayer(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledLayerCount, const char* deviceValidationLayerNames[]) noexcept
{
	std::uint32_t deviceLayerCount = 0;
	if (vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, 0) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkEnumerateDeviceLayerProperties fail.");
		return false;
	}

	bool validationFound = deviceEnabledLayerCount <= deviceLayerCount ? true : false;
	if (deviceLayerCount > 0)
	{
		std::vector<VkLayerProperties> deviceLayers(deviceLayerCount);
		if (vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, &deviceLayers[0]) != VK_SUCCESS)
		{
			VK_PLATFORM_LOG("vkEnumerateDeviceLayerProperties fail.");
			return false;
		}

		for (std::uint32_t i = 0; i < deviceEnabledLayerCount; i++)
		{
			bool found = false;
			for (uint32_t j = 0; j < deviceLayerCount; j++)
			{
				if (!strcmp(deviceValidationLayerNames[i], deviceLayers[j].layerName))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				VK_PLATFORM_LOG("Cannot find layer: %s.", deviceValidationLayerNames[i]);
				validationFound = false;
				break;
			}
		}
	}

	if (!validationFound)
	{
		VK_PLATFORM_LOG("vkEnumerateInstanceLayerProperties failed to find"
			"required validation layer.\n\n"
			"Please look at the Getting Started guide for additional"
			"information.");

		return false;
	}

	return true;
}

bool
VulkanPhysicalDevice::checkPhysicalDeviceExtension(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledExtensitionCount, const char* deviceEnabledExtensitionNames[]) noexcept
{
	std::uint32_t deviceExtensionCount = 0;
	if (vkEnumerateDeviceExtensionProperties(physicalDevice, 0, &deviceExtensionCount, 0) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkEnumerateDeviceExtensionProperties fail.");
		return false;
	}

	bool validationFound = deviceEnabledExtensitionCount < deviceExtensionCount ? true : false;
	if (deviceExtensionCount > 0)
	{
		std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);

		if (vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, &deviceExtensions[0]) != VK_SUCCESS)
		{
			VK_PLATFORM_LOG("vkEnumerateDeviceExtensionProperties fail.");
			return false;
		}

		for (uint32_t i = 0; i < deviceEnabledExtensitionCount; i++)
		{
			bool found = false;
			for (uint32_t j = 0; j < deviceExtensionCount; j++)
			{
				if (!strcmp(deviceEnabledExtensitionNames[i], deviceExtensions[j].extensionName))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				VK_PLATFORM_LOG("Cannot find extensition: %s.", deviceEnabledExtensitionNames[i]);
				validationFound = false;
				break;
			}
		}
	}

	if (!validationFound)
	{
		VK_PLATFORM_LOG("vkEnumerateDeviceExtensionProperties failed to find "
			"required extension.\n\n"
			"Do you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.");
		return false;
	}

	return true;
}

const GraphicsDeviceProperties&
VulkanPhysicalDevice::getGraphicsDeviceProperties() const noexcept
{
	return _deviceProperties;
}

_NAME_END