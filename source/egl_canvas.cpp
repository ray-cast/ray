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
#include <ray/egl_canvas.h>

_NAME_BEGIN

EGLCanvas::EGLCanvas() noexcept
    : _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT)
    , _config(0)
{
}

EGLCanvas::~EGLCanvas() noexcept
{
    this->close();
}

bool
EGLCanvas::setup(WindHandle hwnd, const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) noexcept
{
#if defined(__WINDOWS__)
    _hwnd = (EGLNativeWindowType)hwnd;
    _hdc = ::GetDC(_hwnd);
    _display = ::eglGetDisplay(_hdc);
#endif

    if (_display == EGL_NO_DISPLAY)
    {
        return false;
    }

    if (::eglInitialize(_display, 0, 0) == EGL_FALSE)
    {
        return false;
    }

    if (::eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
    {
        return false;
    }

    const EGLint pixelformat[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, fbconfig.redSize,
        EGL_GREEN_SIZE, fbconfig.greenSize,
        EGL_BLUE_SIZE, fbconfig.blueSize,
        EGL_ALPHA_SIZE, fbconfig.alphaSize,
        EGL_BUFFER_SIZE, fbconfig.bufferSize,
        EGL_DEPTH_SIZE, fbconfig.depthSize,
        EGL_STENCIL_SIZE, fbconfig.stencilSize,
        EGL_SAMPLES, fbconfig.samples ? EGL_TRUE : EGL_FALSE,
        EGL_NONE
    };

    EGLint num = 0;
    if (eglChooseConfig(_display, pixelformat, &_config, 1, &num) == EGL_FALSE)
    {
        return false;
    }

    _surface = eglCreateWindowSurface(_display, _config, _hwnd, NULL);
    if (_surface == EGL_NO_SURFACE)
    {
        return false;
    }

    EGLint attribs[80];

    EGLint index = 0, mask = 0, startegy = 0;

    if (ctxconfig.forward)
    {
        attribs[index++] = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE;
        attribs[index++] = EGL_TRUE;
    }

    if (ctxconfig.debug)
    {
        attribs[index++] = EGL_CONTEXT_OPENGL_DEBUG;
        attribs[index++] = EGL_TRUE;
    }

    if (ctxconfig.profile)
    {
        if (ctxconfig.profile == GPU_GL_CORE_PROFILE)
            mask = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;

        if (ctxconfig.profile == GPU_GL_COMPAT_PROFILE)
            mask = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;

        if (mask)
        {
            attribs[index++] = EGL_CONTEXT_OPENGL_PROFILE_MASK;
            attribs[index++] = mask;
        }
    }
    if (ctxconfig.robustness)
    {
        if (ctxconfig.robustness == GPU_GL_REST_NOTIFICATION)
            startegy = EGL_NO_RESET_NOTIFICATION;
        else if (ctxconfig.robustness == GPU_GL_LOSE_CONTEXT_ONREST)
            startegy = EGL_LOSE_CONTEXT_ON_RESET;

        if (startegy)
        {
            attribs[index++] = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY;
            attribs[index++] = startegy;

            attribs[index++] = EGL_CONTEXT_OPENGL_ROBUST_ACCESS;
            attribs[index++] = EGL_TRUE;
        }
    }

    if (ctxconfig.major != 1 || ctxconfig.minor != 0)
    {
        attribs[index++] = EGL_CONTEXT_MAJOR_VERSION;
        attribs[index++] = ctxconfig.major;

        attribs[index++] = EGL_CONTEXT_MINOR_VERSION;
        attribs[index++] = ctxconfig.minor;
    }

    attribs[index++] = 0;
    attribs[index++] = 0;

    EGLint attrib_list[] = 
    {
        EGL_CONTEXT_CLIENT_TYPE, 3,
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE 
    };

    _context = ::eglCreateContext(_display, _config, EGL_NO_CONTEXT, attrib_list);
    if (!_context)
    {
        std::cout << eglGetError();
        return false;
    }

    _fbconfig = fbconfig;
    _ctxconfig = ctxconfig;

    return true;
}

void
EGLCanvas::close() noexcept
{
    if (_surface != EGL_NO_SURFACE)
    {
        ::eglDestroySurface(_display, _surface);
        _surface = EGL_NO_SURFACE;
    }

    if (_display != EGL_NO_DISPLAY)
    {
        ::eglTerminate(_display);
        _display = EGL_NO_DISPLAY;
    }

    if (_context != EGL_NO_CONTEXT)
    {
        ::eglDestroyContext(_display, _context);
        _context = EGL_NO_CONTEXT;
    }
}

void
EGLCanvas::setSwapInterval(int interval) noexcept
{
    assert(_display != EGL_NO_DISPLAY);
    ::eglSwapInterval(_display, (EGLint)interval);
}

void
EGLCanvas::bind() noexcept
{
    if (::eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
    {
        assert(false);
    }
}

void
EGLCanvas::unbind() noexcept
{
    ::eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void
EGLCanvas::present() noexcept
{
    assert(_display != EGL_NO_DISPLAY);
    assert(_surface != EGL_NO_SURFACE);

    if (_fbconfig.doubleBuffer)
    {
        if (eglSwapBuffers(_display, _surface) == EGL_FALSE)
        {
            assert(false);
        }
    }
    else
    {
        glFlush();
    }
}

_NAME_END