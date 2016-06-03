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
#ifndef _H_VK_TYPES_H_
#define _H_VK_TYPES_H_

#include <ray/graphics_device.h>
#include <ray/graphics_physical_device.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_context.h>
#include <ray/graphics_data.h>
#include <ray/graphics_state.h>
#include <ray/graphics_sampler.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_shader.h>
#include <ray/graphics_pipeline.h>
#include <ray/graphics_descriptor.h>
#include <ray/graphics_input_layout.h>
#include <ray/graphics_system.h>
#include <ray/graphics_command.h>
#include <ray/graphics_semaphore.h>
#include <ray/graphics_variant.h>

#if defined(__WINDOWS__)
#	define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <vulkan/vulkan.h>

#include <GL/glew.h>

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#pragma warning (push)
#pragma warning (disable:4458)
#pragma warning (disable:4464)
#pragma warning (disable:4623)
#pragma warning (disable:5026)
#pragma warning (disable:5027)
#include <SPIRV/GlslangToSpv.h>
#pragma warning (pop)

#define VK_MAX_SUBMIT_BUFFER 64
#define VK_MAX_VIEWPORT_ARRAY 8
#define VK_MAX_ATTACHMENT 8
#define VK_MAX_PRESENT 8

_NAME_BEGIN

#if _DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define VK_PLATFORM_LOG(format, ...) VulkanSystem::instance()->print(format, __VA_ARGS__)
#else
#	define VK_PLATFORM_LOG(format, ...)
#endif

#if _DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define VK_PLATFORM_ASSERT(expr, format) if (!(expr)) { assert(expr); }
#else
#	define VK_PLATFORM_ASSERT(expr, format)
#endif

typedef std::shared_ptr<class VulkanDevice> VulkanDevicePtr;
typedef std::shared_ptr<class VulkanPhysicalDevice> VulkanPhysicalDevicePtr;
typedef std::shared_ptr<class VulkanSurface> VulkanSurfacePtr;
typedef std::shared_ptr<class VulkanSwapchain> VulkanSwapchainPtr;
typedef std::shared_ptr<class VulkanDeviceContext> VulkanDeviceContextPtr;
typedef std::shared_ptr<class VulkanFramebufferLayout> VulkanFramebufferLayoutPtr;
typedef std::shared_ptr<class VulkanFramebuffer> VulkanFramebufferPtr;
typedef std::shared_ptr<class VulkanShader> VulkanShaderPtr;
typedef std::shared_ptr<class VulkanProgram> VulkanProgramPtr;
typedef std::shared_ptr<class VulkanGraphicsData> VulkanGraphicsDataPtr;
typedef std::shared_ptr<class VulkanInputLayout> VulkanInputLayoutPtr;
typedef std::shared_ptr<class VulkanRenderState> VulkanRenderStatePtr;
typedef std::shared_ptr<class VulkanTexture> VulkanTexturePtr;
typedef std::shared_ptr<class VulkanSampler> VulkanSamplerPtr;
typedef std::shared_ptr<class VulkanPipeline> VulkanPipelinePtr;
typedef std::shared_ptr<class VulkanDescriptorSetPool> VulkanDescriptorSetPoolPtr;
typedef std::shared_ptr<class VulkanDescriptorSet> VulkanDescriptorSetPtr;
typedef std::shared_ptr<class VulkanDescriptorSetLayout> VulkanDescriptorSetLayoutPtr;
typedef std::shared_ptr<class VulkanGraphicsAttribute> VulkanGraphicsAttributePtr;
typedef std::shared_ptr<class VulkanGraphicsUniform> VulkanGraphicsUniformPtr;
typedef std::shared_ptr<class VulkanGraphicsUniformBlock> VulkanGraphicsUniformBlockPtr;

typedef std::weak_ptr<class VulkanDevice> VulkanDeviceWeakPtr;
typedef std::weak_ptr<class VulkanPhysicalDevice> VulkanPhysicalDeviceWeakPtr;
typedef std::weak_ptr<class VulkanSurface> VulkanSurfaceWeakPtr;
typedef std::weak_ptr<class VulkanSwapchain> VulkanSwapchainWeakPtr;
typedef std::weak_ptr<class VulkanDeviceContext> VulkanDeviceContextWeakPtr;
typedef std::weak_ptr<class VulkanFramebufferLayout> VulkanFramebufferLayoutWeakPtr;
typedef std::weak_ptr<class VulkanFramebuffer> VulkanFramebufferWeakPtr;
typedef std::weak_ptr<class VulkanShader> VulkanShaderWeakPtr;
typedef std::weak_ptr<class VulkanProgram> VulkanProgramWeakPtr;
typedef std::weak_ptr<class VulkanGraphicsData> VulkanGraphicsDataWeakPtr;
typedef std::weak_ptr<class VulkanInputLayout> VulkanInputLayoutWeakPtr;
typedef std::weak_ptr<class VulkanRenderState> VulkanRenderStateWeakPtr;
typedef std::weak_ptr<class VulkanTexture> VulkanTextureWeakPtr;
typedef std::weak_ptr<class VulkanSampler> VulkanSamplerWeakPtr;
typedef std::weak_ptr<class VulkanPipeline> VulkanPipelineWeakPtr;
typedef std::weak_ptr<class VulkanDescriptorSetPool> VulkanDescriptorSetPoolWeakPtr;
typedef std::weak_ptr<class VulkanDescriptorSet> VulkanDescriptorSetWeakPtr;
typedef std::weak_ptr<class VulkanDescriptorSetLayout> VulkanDescriptorSetLayoutWeakPtr;
typedef std::weak_ptr<class VulkanGraphicsAttribute> VulkanGraphicsAttributeWeakPtr;
typedef std::weak_ptr<class VulkanGraphicsUniform> VulkanGraphicsUniformWeakPtr;
typedef std::weak_ptr<class VulkanGraphicsUniformBlock> VulkanGraphicsUniformBlockWeakPtr;

class VulkanTypes
{
public:
	static VkFormat asGraphicsFormat(GraphicsFormat format) noexcept;
	static VkImageType asImageType(GraphicsTextureDim type) noexcept;
	static VkImageViewType asImageViewType(GraphicsTextureDim type) noexcept;
	static VkSampleCountFlagBits asTextureSample(GraphicsSampleFlagBits sample) noexcept;
	static VkSampleCountFlagBits asTextureSample(GraphicsSamplerAnis anis) noexcept;
	static VkImageTiling asTextureTiling(GraphicsImageTiling tiling) noexcept;
	static VkImageUsageFlags asTextureUsage(GraphicsViewUsageFlags usage) noexcept;
	static VkShaderStageFlagBits asShaderStage(GraphicsShaderStageFlagBits type) noexcept;
	static VkShaderStageFlags asShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
	static VkCullModeFlags asCullMode(GraphicsCullMode mode) noexcept;
	static VkPolygonMode asPolygonMode(GraphicsPolygonMode mode) noexcept;
	static VkFrontFace asFrontFace(GraphicsFrontFace face) noexcept;
	static VkCompareOp asCompareOp(GraphicsCompareFunc compare) noexcept;
	static VkStencilOp asStencilOp(GraphicsStencilOp stencilop) noexcept;
	static VkBufferUsageFlagBits asBufferUsageFlagBits(GraphicsDataType type) noexcept;
	static VkDescriptorType asDescriptorType(GraphicsUniformType type) noexcept;
	static VkPrimitiveTopology asPrimitiveTopology(GraphicsVertexType type) noexcept;
	static VkImageLayout asImageLayout(GraphicsImageLayout layout) noexcept;

	static bool isStencilFormat(GraphicsFormat format) noexcept;
	static bool isDepthFormat(GraphicsFormat format) noexcept;
	static bool isDepthStencilFormat(GraphicsFormat format) noexcept;
};

_NAME_END

#endif