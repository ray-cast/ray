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
#ifndef _H_OGL_TYPES_H_
#define _H_OGL_TYPES_H_

#include <ray/graphics_data.h>
#include <ray/graphics_layout.h>
#include <ray/graphics_device.h>
#include <ray/graphics_context.h>
#include <ray/graphics_state.h>
#include <ray/graphics_sampler.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_view.h>

#include <ray/shader.h>

#if _BUILD_PLATFORM_WINDOWS
#	include <GL/glew.h>
#	include <GL/wglew.h>
#elif _BUILD_PLATFORM_LINUX
#	include <GL/glew.h>
#	include <GL/glxew.h>
#endif

_NAME_BEGIN

#if _BUILD_OPENGL
#	ifdef GLEW_MX
	extern GLEWContext _glewctx;
#	define glewGetContext() (&_glewctx)
#	endif

#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLEXT_APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define GLEXT_APIENTRY __stdcall
#  else
#    define GLEXT_APIENTRY
#  endif
#endif

#if GL_DEBUG
#	if defined(_VISUAL_STUDIO_)
#		pragma warning(disable : 4127)
#	endif
#	define GL_CHECK(x) do { x; EGL2Check::checkError(); } while (false)
#else
#	define GL_CHECK(x) x
#endif

struct GPUfbconfig
{
	int redSize;
	int greenSize;
	int blueSize;
	int alphaSize;

	int bufferSize;
	int depthSize;
	int stencilSize;

	int accumSize;
	int accumRedSize;
	int accumGreenSize;
	int accumBlueSize;
	int accumAlphaSize;

	int samples;
};

struct GPUctxconfig
{
	int api;
	int major;
	int minor;
	int forward;
	int profile;
	int robustness;
	int release;
	int multithread;

#if _BUILD_PLATFORM_WINDOWS
	HGLRC share;
#endif
};

struct OGLFeatures
{
	static bool ARB_bindless_texture;
	static bool ARB_vertex_array_object;
	static bool ARB_vertex_attrib_binding;
	static bool ARB_provoking_vertex;
	static bool ARB_direct_state_access;
	static bool ARB_buffer_storage;
	static bool ARB_viewport_array;

	static bool KHR_debug;

	static bool NV_command_list;
	static bool NV_shader_buffer_load;
	static bool NV_vertex_buffer_unified_memory;
};

class OGLExtenstion final
{
public:
	OGLExtenstion() noexcept;
	~OGLExtenstion() noexcept;

	static bool initExtensions() except;

private:

	static int initExtention;
};

typedef std::shared_ptr<class OGLCanvas> OGLCanvasPtr;
typedef std::shared_ptr<class OGLRenderTexture> OGLRenderTexturePtr;
typedef std::shared_ptr<class OGLMultiRenderTexture> OGLMultiRenderTexturePtr;
typedef std::shared_ptr<class OGLShader> OGLShaderPtr;
typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;
typedef std::shared_ptr<class OGLVertexBuffer> OGLVertexBufferPtr;
typedef std::shared_ptr<class OGLIndexBuffer> OGLIndexBufferPtr;
typedef std::shared_ptr<class OGLGraphicsLayout> OGLGraphicsLayoutPtr;
typedef std::shared_ptr<class OGLDrawIndirectBuffer> OGLDrawIndirectBufferPtr;
typedef std::shared_ptr<class OGLGraphicsState> OGLGraphicsStatePtr;
typedef std::shared_ptr<class OGLTexture> OGLTexturePtr;
typedef std::shared_ptr<class OGLSampler> OGLSamplerPtr;

class OGLTypes
{
public:

	static GLenum asOGLVertexType(VertexType type) noexcept;
	static GLenum asOGLVertexFormat(VertexFormat format) noexcept;
	static GLenum asOGLIndexType(IndexType type) noexcept;
	static GLenum asOGLShaderType(ShaderType type) noexcept;
	static GLenum asOGLTarget(TextureDim mapping, bool multisampler) noexcept;
	static GLenum asOGLFormat(TextureFormat format) noexcept;
	static GLenum asOGLType(TextureFormat format) noexcept;
	static GLint  asOGLInternalformat(TextureFormat format) noexcept;
	static GLenum asCompareFunction(CompareFunction func) noexcept;
	static GLenum asBlendFactor(BlendFactor func) noexcept;
	static GLenum asBlendOperation(BlendOperation op) noexcept;
	static GLenum asCullMode(CullMode mode) noexcept;
	static GLenum asFillMode(FillMode mode) noexcept;
	static GLenum asStencilOperation(StencilOperation stencilop) noexcept;
};

class OGLCheck
{
public:
	static void checkError() noexcept;
};

_NAME_END

#endif