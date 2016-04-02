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

#if defined(__WINDOWS__)
#	define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <vulkan/vulkan.h>

#define VK_MAX_SUBMIT_BUFFER 64
#define VK_MAX_VIEWPORT_ARRAY 8
#define VK_MAX_ATTACHMENT 8
#define VK_MAX_PRESENT 8

_NAME_BEGIN

#if _DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define VK_PLATFORM_LOG(format, ...)
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

class VulkanTypes
{
public:
	static VkFormat asGraphicsFormat(GraphicsFormat format) noexcept;
	static VkImageType asImageType(GraphicsTextureDim type) noexcept;
	static VkImageViewType asImageViewType(GraphicsTextureDim type) noexcept;
	static VkSampleCountFlagBits asTextureSample(GraphicsSampleFlagBits sample) noexcept;
	static VkSampleCountFlagBits asTextureSample(GraphicsSamplerAnis anis) noexcept;
	static VkImageUsageFlags asTextureUsage(std::uint32_t usage) noexcept;
	static VkShaderStageFlagBits asShaderStage(GraphicsShaderStage type) noexcept;
	static VkCullModeFlags asCullMode(GraphicsCullMode mode) noexcept;
	static VkPolygonMode asPolygonMode(GraphicsPolygonMode mode) noexcept;
	static VkFrontFace asFrontFace(GraphicsFrontFace face) noexcept;
	static VkCompareOp asCompareOp(GraphicsCompareFunc compare) noexcept;
	static VkStencilOp asStencilOp(GraphicsStencilOp stencilop) noexcept;
	static VkBufferUsageFlagBits asBufferUsageFlagBits(GraphicsDataType type) noexcept;
	static VkDescriptorType asDescriptorType(GraphicsUniformType type) noexcept;
	static VkPrimitiveTopology asPrimitiveTopology(GraphicsVertexType type) noexcept;
	static VkImageLayout asImageLayout(GraphicsViewLayout layout) noexcept;
};

_NAME_END

#endif