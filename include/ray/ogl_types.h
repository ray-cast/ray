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

#include <ray/platform.h>

#if defined(_BUILD_PLATFORM_SDL2)
#    include <ray/sdl_canvas.h>
#    define OGLCanvas SDLCanvas
#else
#    if defined(__WINDOWS__)
#       include <ray/wgl_canvas.h>
#       define OGLCanvas WGLCanvas
#    elif defined(__LINUX__)
#       include <ray/x11_wx.h>
#        define TopLevelCanvas TopLevelWindowLINUX
#    elif defined(__APPLE__)
#       include <ray/x11_wx.h>
#        define TopLevelCanvas TopLevelWindowAPPLE
#    endif
#endif

_NAME_BEGIN

#if defined(EGLAPI)

typedef EGLCanvas OGLCanvas;

#define GL_DEBUG_OUTPUT_SYNCHRONOUS   GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR
#define GL_DEBUG_CALLBACK_FUNCTION    GL_DEBUG_CALLBACK_FUNCTION_KHR
#define GL_DEBUG_CALLBACK_USER_PARAM  GL_DEBUG_CALLBACK_USER_PARAM_KHR
#define GL_DEBUG_SOURCE_API           GL_DEBUG_SOURCE_API_KHR
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
#define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
#define GL_DEBUG_SOURCE_THIRD_PARTY   GL_DEBUG_SOURCE_THIRD_PARTY_KHR
#define GL_DEBUG_SOURCE_APPLICATION   GL_DEBUG_SOURCE_APPLICATION_KHR
#define GL_DEBUG_SOURCE_OTHER         GL_DEBUG_SOURCE_OTHER_KHR
#define GL_DEBUG_TYPE_ERROR           GL_DEBUG_TYPE_ERROR_KHR
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
#define GL_DEBUG_TYPE_PORTABILITY     GL_DEBUG_TYPE_PORTABILITY_KHR
#define GL_DEBUG_TYPE_PERFORMANCE     GL_DEBUG_TYPE_PERFORMANCE_KHR
#define GL_DEBUG_TYPE_OTHER           GL_DEBUG_TYPE_OTHER_KHR
#define GL_DEBUG_TYPE_MARKER          GL_DEBUG_TYPE_MARKER_KHR
#define GL_DEBUG_TYPE_PUSH_GROUP      GL_DEBUG_TYPE_PUSH_GROUP_KHR
#define GL_DEBUG_TYPE_POP_GROUP       GL_DEBUG_TYPE_POP_GROUP_KHR
#define GL_DEBUG_SEVERITY_NOTIFICATION GL_DEBUG_SEVERITY_NOTIFICATION_KHR
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR
#define GL_DEBUG_GROUP_STACK_DEPTH    GL_DEBUG_GROUP_STACK_DEPTH_KHR
#define GL_BUFFER                     GL_BUFFER_KHR
#define GL_SHADER                     GL_SHADER_KHR
#define GL_PROGRAM                    GL_PROGRAM_KHR
#define GL_VERTEX_ARRAY               GL_VERTEX_ARRAY_KHR
#define GL_QUERY                      GL_QUERY_KHR
#define GL_MAX_LABEL_LENGTH           GL_MAX_LABEL_LENGTH_KHR
#define GL_MAX_DEBUG_MESSAGE_LENGTH   GL_MAX_DEBUG_MESSAGE_LENGTH_KHR
#define GL_MAX_DEBUG_LOGGED_MESSAGES  GL_MAX_DEBUG_LOGGED_MESSAGES_KHR
#define GL_DEBUG_LOGGED_MESSAGES      GL_DEBUG_LOGGED_MESSAGES_KHR
#define GL_DEBUG_SEVERITY_HIGH        GL_DEBUG_SEVERITY_HIGH_KHR
#define GL_DEBUG_SEVERITY_MEDIUM      GL_DEBUG_SEVERITY_MEDIUM_KHR
#define GL_DEBUG_SEVERITY_LOW         GL_DEBUG_SEVERITY_LOW_KHR
#define GL_DEBUG_OUTPUT               GL_DEBUG_OUTPUT_KHR
#define GL_CONTEXT_FLAG_DEBUG_BIT     GL_CONTEXT_FLAG_DEBUG_BIT_KHR
#define GL_STACK_OVERFLOW             GL_STACK_OVERFLOW_KHR
#define GL_STACK_UNDERFLOW            GL_STACK_UNDERFLOW_KHR

#define GLDEBUGPROC GL_APIENTRY
#endif

struct OGLTexture
{
    GLuint texture;
    GLuint64 resident;

    OGLTexture() noexcept
        : texture(0)
        , resident(0)
    {
    }
};

struct OGLConstantBuffer
{
    GLuint ubo;
    GLuint64 bindlessUbo;

    OGLConstantBuffer() noexcept
        : ubo(0)
        , bindlessUbo(0)
    {
    }
};

struct OGLVertexAttrib
{
    GLuint attrib;
    GLenum type;
    GLuint size;
    GLsizei stride;
    GLintptr offset;
};

struct OGLVertexArray
{
    GLuint vao;

    GLuint vbo;
    GLuint ibo;

    GLuint64 bindlessVbo;
    GLuint64 bindlessIbo;

    GLuint vertexCount;
    GLenum vertexUsage;
    GLsizeiptr vertexSize;
    GLsizeiptr vertexByteSize;

    GLuint indexCount;
    GLenum indexType;
    GLenum indexUsage;
    GLsizeiptr indexSize;

    std::vector<OGLVertexAttrib> attribs;

    OGLVertexArray() noexcept
        : vao(GL_NONE)
        , vbo(GL_NONE)
        , ibo(GL_NONE)
        , bindlessVbo(GL_NONE)
        , bindlessIbo(GL_NONE)
        , vertexUsage(GL_STATIC_DRAW)
        , vertexSize(0)
        , vertexCount(0)
        , indexType(GL_UNSIGNED_SHORT)
        , indexUsage(GL_STATIC_DRAW)
        , indexSize(0)
    {
    }
};

typedef std::vector<std::pair<FramebufferPtr, GLenum>> DrawBuffers;

struct OGLRenderTexture
{
    GLuint fbo;
    GLuint64 bindlessFbo;

    DrawBuffers drawbuffers;

    OGLRenderTexture()
        : fbo(0)
        , bindlessFbo(0)
    {
    }
};

typedef std::shared_ptr<class OGLCanvas> OGLCanvasPtr;
typedef std::shared_ptr<class OGLFramebuffer> OGLFramebufferPtr;
typedef std::shared_ptr<class OGLShader> OGLShaderPtr;
typedef std::shared_ptr<class OGLShaderProgram> OGLShaderProgramPtr;

class OGLTypes
{
public:

    static GLenum asOGLVertexType(VertexType type) noexcept;
    static GLenum asOGLVertexUsage(VertexUsage usage) noexcept;
    static GLenum asOGLVertexDataType(VertexDataType type) noexcept;
    static GLenum asOGLIndexType(IndexType type) noexcept;
    static GLenum asOGLShaderType(ShaderType type) noexcept;
    static GLenum asOGLAttachment(Attachment attrib) noexcept;
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

_NAME_END

#endif