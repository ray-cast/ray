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
#ifndef _H_EGL3_TYPES_H_
#define _H_EGL3_TYPES_H_

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

#include <EGL\egl.h>
#include <GLES2\gl2.h>
#include <GLES2\gl2ext.h>
#include <GLES3\gl3.h>
#include <GLES3\gl3ext.h>
#include <GLES3\gl31.h>

_NAME_BEGIN

#if defined(GL_KHR_debug)
#	define GL_DEBUG_OUTPUT_SYNCHRONOUS   GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR
#   define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR
#   define GL_DEBUG_CALLBACK_FUNCTION    GL_DEBUG_CALLBACK_FUNCTION_KHR
#   define GL_DEBUG_CALLBACK_USER_PARAM  GL_DEBUG_CALLBACK_USER_PARAM_KHR
#   define GL_DEBUG_SOURCE_API           GL_DEBUG_SOURCE_API_KHR
#   define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
#   define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
#   define GL_DEBUG_SOURCE_THIRD_PARTY   GL_DEBUG_SOURCE_THIRD_PARTY_KHR
#   define GL_DEBUG_SOURCE_APPLICATION   GL_DEBUG_SOURCE_APPLICATION_KHR
#   define GL_DEBUG_SOURCE_OTHER         GL_DEBUG_SOURCE_OTHER_KHR
#   define GL_DEBUG_TYPE_ERROR           GL_DEBUG_TYPE_ERROR_KHR
#   define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
#   define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
#   define GL_DEBUG_TYPE_PORTABILITY     GL_DEBUG_TYPE_PORTABILITY_KHR
#   define GL_DEBUG_TYPE_PERFORMANCE     GL_DEBUG_TYPE_PERFORMANCE_KHR
#   define GL_DEBUG_TYPE_OTHER           GL_DEBUG_TYPE_OTHER_KHR
#   define GL_DEBUG_TYPE_MARKER          GL_DEBUG_TYPE_MARKER_KHR
#   define GL_DEBUG_TYPE_PUSH_GROUP      GL_DEBUG_TYPE_PUSH_GROUP_KHR
#   define GL_DEBUG_TYPE_POP_GROUP       GL_DEBUG_TYPE_POP_GROUP_KHR
#   define GL_DEBUG_SEVERITY_NOTIFICATION GL_DEBUG_SEVERITY_NOTIFICATION_KHR
#   define GL_MAX_DEBUG_GROUP_STACK_DEPTH GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR
#   define GL_DEBUG_GROUP_STACK_DEPTH    GL_DEBUG_GROUP_STACK_DEPTH_KHR
#   define GL_BUFFER                     GL_BUFFER_KHR
#   define GL_SHADER                     GL_SHADER_KHR
#   define GL_PROGRAM                    GL_PROGRAM_KHR
#   define GL_VERTEX_ARRAY               GL_VERTEX_ARRAY_KHR
#   define GL_QUERY                      GL_QUERY_KHR
#   define GL_MAX_LABEL_LENGTH           GL_MAX_LABEL_LENGTH_KHR
#   define GL_MAX_DEBUG_MESSAGE_LENGTH   GL_MAX_DEBUG_MESSAGE_LENGTH_KHR
#   define GL_MAX_DEBUG_LOGGED_MESSAGES  GL_MAX_DEBUG_LOGGED_MESSAGES_KHR
#   define GL_DEBUG_LOGGED_MESSAGES      GL_DEBUG_LOGGED_MESSAGES_KHR
#   define GL_DEBUG_SEVERITY_HIGH        GL_DEBUG_SEVERITY_HIGH_KHR
#   define GL_DEBUG_SEVERITY_MEDIUM      GL_DEBUG_SEVERITY_MEDIUM_KHR
#   define GL_DEBUG_SEVERITY_LOW         GL_DEBUG_SEVERITY_LOW_KHR
#   define GL_DEBUG_OUTPUT               GL_DEBUG_OUTPUT_KHR
#   define GL_CONTEXT_FLAG_DEBUG_BIT     GL_CONTEXT_FLAG_DEBUG_BIT_KHR
#   define GL_STACK_OVERFLOW             GL_STACK_OVERFLOW_KHR
#   define GL_STACK_UNDERFLOW            GL_STACK_UNDERFLOW_KHR
#   define GL_FRAMEBUFFER_SRGB           GL_FRAMEBUFFER_SRGB_EXT

#   define GLAPIENTRY GL_APIENTRY

#   define glDebugMessageCallback glDebugMessageCallbackKHR
#   define glDebugMessageControl glDebugMessageControlKHR
#endif

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; EGL3Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

#if _DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_LOG(format, ...) do { EGL3Check::debugOutput(format, __VA_ARGS__); } while(false)
#else
#	define GL_PLATFORM_LOG(format, ...)
#endif

#if _DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_PLATFORM_ASSERT(expr, format) if (!(expr)) { EGL3Check::debugOutput(format); assert(expr); }
#else
#	define GL_PLATFORM_ASSERT(expr, format)
#endif

typedef std::shared_ptr<class EGL3Device> EGL3DevicePtr;
typedef std::shared_ptr<class EGL3Surface> EGL3SurfacePtr;
typedef std::shared_ptr<class EGL3Swapchain> EGL3SwapchainPtr;
typedef std::shared_ptr<class EGL3DeviceContext> EGL3DeviceContextPtr;
typedef std::shared_ptr<class EGL3FramebufferLayout> EGL3FramebufferLayoutPtr;
typedef std::shared_ptr<class EGL3Framebuffer> EGL3FramebufferPtr;
typedef std::shared_ptr<class EGL3Shader> EGL3ShaderPtr;
typedef std::shared_ptr<class EGL3ShaderObject> EGL3ShaderObjectPtr;
typedef std::shared_ptr<class EGL3GraphicsData> EGL3GraphicsDataPtr;
typedef std::shared_ptr<class EGL3InputLayout> EGL3InputLayoutPtr;
typedef std::shared_ptr<class EGL3GraphicsState> EGL3GraphicsStatePtr;
typedef std::shared_ptr<class EGL3Texture> EGL3TexturePtr;
typedef std::shared_ptr<class EGL3Sampler> EGL3SamplerPtr;
typedef std::shared_ptr<class EGL3RenderPipeline> EGL3RenderPipelinePtr;
typedef std::shared_ptr<class EGL3DescriptorSetPool> EGL3DescriptorSetPoolPtr;
typedef std::shared_ptr<class EGL3DescriptorSet> EGL3DescriptorSetPtr;
typedef std::shared_ptr<class EGL3DescriptorSetLayout> EGL3DescriptorSetLayoutPtr;
typedef std::shared_ptr<class EGL3GraphicsAttribute> EGL3GraphicsAttributePtr;
typedef std::shared_ptr<class EGL3GraphicsUniform> EGL3GraphicsUniformPtr;
typedef std::shared_ptr<class EGL3GraphicsUniformBlock> EGL3GraphicsUniformBlockPtr;

typedef std::weak_ptr<class EGL3Device> EGL3DeviceWeakPtr;
typedef std::weak_ptr<class EGL3Surface> EGL3SurfaceWeakPtr;
typedef std::weak_ptr<class EGL3Swapchain> EGL3SwapchainWeakPtr;
typedef std::weak_ptr<class EGL3DeviceContext> EGL3DeviceContextWeakPtr;
typedef std::weak_ptr<class EGL3FramebufferLayout> EGL3FramebufferLayoutWeakPtr;
typedef std::weak_ptr<class EGL3Framebuffer> EGL3FramebufferWeakPtr;
typedef std::weak_ptr<class EGL3Shader> EGL3ShaderWeakPtr;
typedef std::weak_ptr<class EGL3ShaderObject> EGL3ShaderObjectWeakPtr;
typedef std::weak_ptr<class EGL3GraphicsData> EGL3GraphicsDataWeakPtr;
typedef std::weak_ptr<class EGL3InputLayout> EGL3InputLayoutWeakPtr;
typedef std::weak_ptr<class EGL3GraphicsState> EGL3GraphicsStateWeakPtr;
typedef std::weak_ptr<class EGL3Texture> EGL3TextureWeakPtr;
typedef std::weak_ptr<class EGL3Sampler> EGL3SamplerWeakPtr;
typedef std::weak_ptr<class EGL3RenderPipeline> EGL3RenderPipelineWeakPtr;
typedef std::weak_ptr<class EGL3DescriptorSetPool> EGL3DescriptorSetPoolWeakPtr;
typedef std::weak_ptr<class EGL3DescriptorSet> EGL3DescriptorSetWeakPtr;
typedef std::weak_ptr<class EGL3DescriptorSetLayout> EGL3DescriptorSetLayoutWeakPtr;
typedef std::weak_ptr<class EGL3GraphicsAttribute> EGL3GraphicsAttributeWeakPtr;
typedef std::weak_ptr<class EGL3GraphicsUniform> EGL3GraphicsUniformWeakPtr;
typedef std::weak_ptr<class EGL3GraphicsUniformBlock> EGL3GraphicsUniformBlockWeakPtr;

typedef std::vector<EGL3ShaderPtr> EGL3Shaders;

class EGL3Types
{
public:
	static GLenum asVertexType(GraphicsVertexType type) noexcept;
	static GLenum asVertexFormat(GraphicsFormat format) noexcept;
	static GLenum asIndexType(GraphicsIndexType type) noexcept;
	static GLenum asShaderType(GraphicsShaderStage type) noexcept;
	static GLenum asTextureTarget(GraphicsTextureDim mapping, bool multisampler) noexcept;
	static GLenum asTextureFormat(GraphicsFormat format) noexcept;
	static GLenum asTextureType(GraphicsFormat format) noexcept;
	static GLenum asTextureInternalFormat(GraphicsFormat format) noexcept;
	static GLenum asCompareFunction(GraphicsCompareFunc func) noexcept;
	static GLenum asBlendFactor(GraphicsBlendFactor func) noexcept;
	static GLenum asBlendOperation(GraphicsBlendOp op) noexcept;
	static GLenum asCullMode(GraphicsCullMode mode) noexcept;
	static GLenum asFillMode(GraphicsPolygonMode mode) noexcept;
	static GLenum asStencilOperation(GraphicsStencilOp stencilop) noexcept;
	static GLenum asSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
	static GLenum asSamplerFilter(GraphicsSamplerFilter filter) noexcept;

	static GLboolean isCompressedTexture(GraphicsFormat format) noexcept;
};

class EGL3Check
{
public:
	static bool checkError() noexcept;

	static void debugOutput(const std::string& message, ...) noexcept;
};

_NAME_END

#endif