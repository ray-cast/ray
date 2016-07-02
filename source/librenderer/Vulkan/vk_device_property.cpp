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
#include "vk_device_property.h"
#include "vk_system.h"

_NAME_BEGIN

VulkanDeviceProperty::VulkanDeviceProperty() noexcept
	: _physicalDevice(VK_NULL_HANDLE)
{
}

VulkanDeviceProperty::~VulkanDeviceProperty() noexcept
{
	this->close();
}

bool
VulkanDeviceProperty::setup(VkPhysicalDevice physicalDevice) noexcept
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
	_deviceProperties.maxViewportDimensionsW = prop.limits.maxViewportDimensions[0];
	_deviceProperties.maxViewportDimensionsH = prop.limits.maxViewportDimensions[1];
	_deviceProperties.minViewportBoundsRange = prop.limits.viewportBoundsRange[0];
	_deviceProperties.maxViewportBoundsRange = prop.limits.viewportBoundsRange[1];
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

	this->initTextureSupports();
	this->initVertexSupports();
	this->initTextureDimSupports();
	this->initShaderSupports();

	_physicalDevice = physicalDevice;
	return true;
}

void
VulkanDeviceProperty::close() noexcept
{
}

VkPhysicalDevice
VulkanDeviceProperty::getPhysicalDevice() const noexcept
{
	return _physicalDevice;
}

bool
VulkanDeviceProperty::checkPhysicalDeviceLayer(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledLayerCount, const char* deviceValidationLayerNames[]) noexcept
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
				VK_PLATFORM_LOG("Cannot find layer.");
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
VulkanDeviceProperty::checkPhysicalDeviceExtension(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledExtensitionCount, const char* deviceEnabledExtensitionNames[]) noexcept
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
				VK_PLATFORM_LOG("Cannot find extensition.");
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

bool
VulkanDeviceProperty::initTextureSupports() noexcept
{
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC5UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11SNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11SNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);

	return true;
}

bool
VulkanDeviceProperty::initTextureDimSupports() noexcept
{
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim3D);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	return true;
}

bool
VulkanDeviceProperty::initVertexSupports() noexcept
{
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);

	return true;
}

bool
VulkanDeviceProperty::initShaderSupports() noexcept
{
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageTessControlBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageTessEvaluationBit);
	return true;
}

const GraphicsDeviceProperties&
VulkanDeviceProperty::getGraphicsDeviceProperties() const noexcept
{
	return _deviceProperties;
}

_NAME_END