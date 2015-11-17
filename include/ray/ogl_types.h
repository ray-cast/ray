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

#include <ray/render_device.h>
#include <ray/render_buffer.h>
#include <ray/render_texture.h>
#include <ray/render_state.h>
#include <ray/render_window.h>
#include <ray/render_command.h>
#include <ray/shader.h>

#if _BUILD_PLATFORM_WINDOWS
#include <GL\glew.h>
#elif _BUILD_PLATFORM_ANDROID
#include <EGL\egl.h>
#include <GLES2\gl2.h>
#include <GLES3\gl3.h>
#include <GLES3\gl31.h>
#include <GLES2\gl2ext.h>
#include <GLES3\gl3ext.h>
#elif _BUILD_PLATFORM_LINUX
#include <GL/glew.h>
#include <GL/glxew.h>
#endif

_NAME_BEGIN

#if _BUILD_OPENGL

#ifdef GLEW_MX
extern GLEWContext _glewctx;
#define glewGetContext() (&_glewctx)
#endif

#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLEXT_APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define GLEXT_APIENTRY __stdcall
#  else
#    define GLEXT_APIENTRY
#  endif
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

#   if defined(_BUILD_OPENGL_ES)
	EGLContext share;
#   elif defined(_BUILD_OPENGL)
#	if _BUILD_PLATFORM_WINDOWS
	HGLRC share;
#	endif
#endif
};

struct OGLFeatures
{
	static bool ARB_bindless_texture;
	static bool ARB_vertex_array_object;
	static bool ARB_vertex_attrib_binding;
	static bool ARB_provoking_vertex;
	static bool ARB_direct_state_access;
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

struct OGLShaderVariant
{
	GLuint ubo;
	GLuint64 bindlessUbo;

	OGLShaderVariant() noexcept
		: ubo(0)
		, bindlessUbo(0)
	{
	}
};

typedef std::shared_ptr<class OGLCanvas> OGLCanvasPtr;
typedef std::shared_ptr<class OGLFramebuffer> OGLRenderTexturePtr;
typedef std::shared_ptr<class OGLShader> OGLShaderPtr;
typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;

class OGLTypes
{
public:

	static GLenum asOGLVertexType(VertexType type) noexcept;
	static GLenum asOGLVertexUsage(VertexUsage usage) noexcept;
	static GLenum asOGLVertexFormat(VertexFormat format) noexcept;
	static GLenum asOGLIndexType(IndexType type) noexcept;
	static GLenum asOGLShaderType(ShaderType type) noexcept;
	static GLenum asOGLTarget(TextureDim mapping) noexcept;
	static GLenum asOGLFormat(PixelFormat format) noexcept;
	static GLenum asOGLType(PixelFormat format) noexcept;
	static GLint  asOGLInternalformat(PixelFormat format) noexcept;
	static GLenum asCompareFunction(CompareFunction func) noexcept;
	static GLenum asBlendFactor(BlendFactor func) noexcept;
	static GLenum asBlendOperation(BlendOperation op) noexcept;
	static GLenum asCullMode(CullMode mode) noexcept;
	static GLenum asFillMode(FillMode mode) noexcept;
	static GLenum asStencilOperation(StencilOperation stencilop) noexcept;
};

#define _USE_RENDER_COMMAND 0

_NAME_END

#endif