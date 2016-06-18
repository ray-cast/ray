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
#ifndef _H_GRAPHICS_PHYSICAL_DEVICE_H_
#define _H_GRAPHICS_PHYSICAL_DEVICE_H_

#include <ray/graphics_types.h>

_NAME_BEGIN

class GraphicsDeviceProperties final
{
public:
	GraphicsDeviceProperties() noexcept;
	~GraphicsDeviceProperties() noexcept;

	bool isTextureSupport(GraphicsFormat format) const noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) const noexcept;
	bool isVertexSupport(GraphicsFormat format) const noexcept;
	bool isShaderSupport(GraphicsShaderStageFlagBits stage) const noexcept;

	std::uint32_t         maxImageDimension1D;
	std::uint32_t         maxImageDimension2D;
	std::uint32_t         maxImageDimension3D;
	std::uint32_t         maxImageDimensionCube;
	std::uint32_t         maxImageArrayLayers;
	std::uint32_t         maxTexelBufferElements;
	std::uint32_t         maxUniformBufferRange;
	std::uint32_t         maxStorageBufferRange;
	std::uint32_t         maxPushConstantsSize;
	std::uint32_t         maxMemoryAllocationCount;
	std::uint32_t         maxSamplerAllocationCount;
	std::uint64_t         bufferImageGranularity;
	std::uint64_t         sparseAddressSpaceSize;
	std::uint32_t         maxBoundDescriptorSets;
	std::uint32_t         maxPerStageDescriptorSamplers;
	std::uint32_t         maxPerStageDescriptorUniformBuffers;
	std::uint32_t         maxPerStageDescriptorStorageBuffers;
	std::uint32_t         maxPerStageDescriptorSampledImages;
	std::uint32_t         maxPerStageDescriptorStorageImages;
	std::uint32_t         maxPerStageDescriptorInputAttachments;
	std::uint32_t         maxPerStageResources;
	std::uint32_t         maxDescriptorSetSamplers;
	std::uint32_t         maxDescriptorSetUniformBuffers;
	std::uint32_t         maxDescriptorSetUniformBuffersDynamic;
	std::uint32_t         maxDescriptorSetStorageBuffers;
	std::uint32_t         maxDescriptorSetStorageBuffersDynamic;
	std::uint32_t         maxDescriptorSetSampledImages;
	std::uint32_t         maxDescriptorSetStorageImages;
	std::uint32_t         maxDescriptorSetInputAttachments;
	std::uint32_t         maxVertexInputAttributes;
	std::uint32_t         maxVertexInputBindings;
	std::uint32_t         maxVertexInputAttributeOffset;
	std::uint32_t         maxVertexInputBindingStride;
	std::uint32_t         maxVertexOutputComponents;
	std::uint32_t         maxTessellationGenerationLevel;
	std::uint32_t         maxTessellationPatchSize;
	std::uint32_t         maxTessellationControlPerVertexInputComponents;
	std::uint32_t         maxTessellationControlPerVertexOutputComponents;
	std::uint32_t         maxTessellationControlPerPatchOutputComponents;
	std::uint32_t         maxTessellationControlTotalOutputComponents;
	std::uint32_t         maxTessellationEvaluationInputComponents;
	std::uint32_t         maxTessellationEvaluationOutputComponents;
	std::uint32_t         maxGeometryShaderInvocations;
	std::uint32_t         maxGeometryInputComponents;
	std::uint32_t         maxGeometryOutputComponents;
	std::uint32_t         maxGeometryOutputVertices;
	std::uint32_t         maxGeometryTotalOutputComponents;
	std::uint32_t         maxFragmentInputComponents;
	std::uint32_t         maxFragmentOutputAttachments;
	std::uint32_t         maxFragmentDualSrcAttachments;
	std::uint32_t         maxFragmentCombinedOutputResources;
	std::uint32_t         maxComputeSharedMemorySize;
	std::uint32_t         maxComputeWorkGroupCount[3];
	std::uint32_t         maxComputeWorkGroupInvocations;
	std::uint32_t         maxComputeWorkGroupSize[3];
	std::uint32_t         subPixelPrecisionBits;
	std::uint32_t         subTexelPrecisionBits;
	std::uint32_t         mipmapPrecisionBits;
	std::uint32_t         maxDrawIndexedIndexValue;
	std::uint32_t         maxDrawIndirectCount;
	float                 maxSamplerLodBias;
	float                 maxSamplerAnisotropy;
	std::uint32_t         maxViewports;
	std::uint32_t         maxViewportDimensionsW;
	std::uint32_t         maxViewportDimensionsH;
	float                 minViewportBoundsRange;
	float                 maxViewportBoundsRange;
	std::uint32_t         viewportSubPixelBits;
	std::size_t           minMemoryMapAlignment;
	std::uint64_t         minTexelBufferOffsetAlignment;
	std::uint64_t         minUniformBufferOffsetAlignment;
	std::uint64_t         minStorageBufferOffsetAlignment;
	std::int32_t          minTexelOffset;
	std::uint32_t         maxTexelOffset;
	std::int32_t          minTexelGatherOffset;
	std::uint32_t         maxTexelGatherOffset;
	float                 minInterpolationOffset;
	float                 maxInterpolationOffset;
	std::uint32_t         subPixelInterpolationOffsetBits;
	std::uint32_t         maxFramebufferWidth;
	std::uint32_t         maxFramebufferHeight;
	std::uint32_t         maxFramebufferLayers;
	std::uint32_t         maxFramebufferColorAttachments;
	GraphicsSampleFlags   framebufferColorSampleCounts;
	GraphicsSampleFlags   framebufferDepthSampleCounts;
	GraphicsSampleFlags   framebufferStencilSampleCounts;
	GraphicsSampleFlags   framebufferNoAttachmentsSampleCounts;
	GraphicsSampleFlags   sampledImageColorSampleCounts;
	GraphicsSampleFlags   sampledImageIntegerSampleCounts;
	GraphicsSampleFlags   sampledImageDepthSampleCounts;
	GraphicsSampleFlags   sampledImageStencilSampleCounts;
	GraphicsSampleFlags   storageImageSampleCounts;
	std::uint32_t         maxSampleMaskWords;
	std::uint32_t         timestampComputeAndGraphics;
	float                 timestampPeriod;
	std::uint32_t         maxClipDistances;
	std::uint32_t         maxCullDistances;
	std::uint32_t         maxCombinedClipAndCullDistances;
	std::uint32_t         discreteQueuePriorities;
	float                 minPointSizeRange;
	float                 maxPointSizeRange;
	float                 pointSizeGranularity;
	float                 minLineWidthRange;
	float                 maxLineWidthRange;
	float                 lineWidthGranularity;
	std::uint32_t         strictLines;
	std::uint32_t         standardSampleLocations;
	std::uint64_t         optimalBufferCopyOffsetAlignment;
	std::uint64_t         optimalBufferCopyRowPitchAlignment;
	std::uint64_t         nonCoherentAtomSize;
	std::vector<GraphicsFormat> supportTextures;
	std::vector<GraphicsTextureDim> supportTextureDims;
	std::vector<GraphicsFormat> supportAttribute;
	std::vector<GraphicsShaderStageFlagBits> supportShaders;
};

class GraphicsDeviceProperty : public rtti::Interface
{
	__DeclareSubInterface(GraphicsDeviceProperty, rtti::Interface)
public:
	GraphicsDeviceProperty() noexcept;
	virtual ~GraphicsDeviceProperty() noexcept;
	
	virtual const GraphicsDeviceProperties& getGraphicsDeviceProperties() const noexcept = 0;

private:
	GraphicsDeviceProperty(const GraphicsDeviceProperty&) = delete;
	GraphicsDeviceProperty& operator=(const GraphicsDeviceProperty&) = delete;
};

_NAME_END

#endif