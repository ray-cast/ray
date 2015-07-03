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
#       include <ray/x11_canvas.h>
#       define TopLevelCanvas TopLevelWindowLINUX
#    elif defined(__APPLE__)
#       include <ray/x11_canvas.h>
#       define TopLevelCanvas TopLevelWindowAPPLE
#    elif defined(__ANDROID__)
#       include <ray/egl_canvas.h>
#       define EGLCanvas OGLCanvas;
#    endif
#endif

_NAME_BEGIN

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
typedef std::shared_ptr<class OGLFramebuffer> OGLRenderTargetPtr;
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

_NAME_END

#endif