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
#ifdef _BUILD_PLATFORM_WINDOWS
#include <ray/wgl_canvas.h>

_NAME_BEGIN

WGLCanvas::WGLCanvas() noexcept
    : _hwnd(nullptr)
    , _hdc(nullptr)
    , _context(nullptr)
    , _interval(SwapInterval::GPU_VSYNC)
{
}

WGLCanvas::~WGLCanvas() noexcept
{
    this->close();
}

void
WGLCanvas::setup(const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) except
{
    assert(ctxconfig.hwnd);

    _hwnd = (HWND)ctxconfig.hwnd;
    _hdc = ::GetDC(_hwnd);
    if (!_hdc)
    {
        throw failure("GetDC() fail");
    }

    PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.dwFlags |= fbconfig.stereo ? PFD_STEREO : 0;
    pfd.dwFlags |= fbconfig.doubleBuffer ? PFD_DOUBLEBUFFER : 0;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = fbconfig.bufferSize;
    pfd.cRedBits = fbconfig.redSize;
    pfd.cRedShift = 0;
    pfd.cGreenBits = fbconfig.greenSize;
    pfd.cGreenShift = 0;
    pfd.cBlueBits = fbconfig.blueSize;
    pfd.cBlueShift = 0;
    pfd.cAlphaBits = fbconfig.alphaSize;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = fbconfig.accumSize;
    pfd.cAccumRedBits = fbconfig.accumRedSize;
    pfd.cAccumGreenBits = fbconfig.accumGreenSize;
    pfd.cAccumBlueBits = fbconfig.accumBlueSize;
    pfd.cAccumAlphaBits = fbconfig.accumAlphaSize;
    pfd.cDepthBits = fbconfig.depthSize;
    pfd.cStencilBits = fbconfig.stencilSize;
    pfd.cAuxBuffers = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.bReserved = 0;
    pfd.dwLayerMask = 0;
    pfd.dwVisibleMask = 0;
    pfd.dwDamageMask = 0;

    int pixelFormat = ::ChoosePixelFormat(_hdc, &pfd);
    if (!pixelFormat)
    {
        throw failure("ChoosePixelFormat() fail");
    }

    if (!::DescribePixelFormat(_hdc, pixelFormat, sizeof(pfd), &pfd))
    {
        throw failure("DescribePixelFormat() fail");
    }

    if (!::SetPixelFormat(_hdc, pixelFormat, &pfd))
    {
        throw failure("SetPixelFormat() fail");
    }

    if (!OGLExtenstion::initWGLExtensions(_hdc))
    {
        throw failure("initWGLExtensions() fail");
    }

    if (!OGLExtenstion::isSupport(ARB_create_context))
    {
        throw failure("Platform does not support OpenGL");
    }

    int attribs[40];

    int index = 0, mask = 0, flags = 0, startegy = 0;

#if _DEBUG
    flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

    if (ctxconfig.forward)
        flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

    if (ctxconfig.profile)
    {
        if (ctxconfig.profile == GPU_GL_CORE_PROFILE)
            mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

        if (ctxconfig.profile == GPU_GL_COMPAT_PROFILE)
            mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    }

    if (OGLExtenstion::isSupport(ARB_create_context_robustness))
    {
        if (ctxconfig.robustness)
        {
            if (ctxconfig.robustness == GPU_GL_REST_NOTIFICATION)
                startegy = WGL_NO_RESET_NOTIFICATION_ARB;
            else if (ctxconfig.robustness == GPU_GL_LOSE_CONTEXT_ONREST)
                startegy = WGL_LOSE_CONTEXT_ON_RESET_ARB;

            flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
        }
    }

    if (ctxconfig.major != 1 || ctxconfig.minor != 0)
    {
        attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[index++] = ctxconfig.major;

        attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[index++] = ctxconfig.minor;
    }

    if (flags)
    {
        attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
        attribs[index++] = flags;
    }

    if (mask)
    {
        attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;
        attribs[index++] = mask;
    }

    if (startegy)
    {
        attribs[index++] = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
        attribs[index++] = startegy;
    }

    attribs[index] = 0;
    attribs[index] = 0;

    _context = wglCreateContextAttribs(_hdc, nullptr, attribs);
    if (!_context)
    {
        throw failure("wglCreateContextAttribs fail");
    }

    ::wglMakeCurrent(_hdc, _context);

    _fbconfig = fbconfig;
    _ctxconfig = ctxconfig;
    _interval = (SwapInterval)wglGetSwapIntervalEXT();
}

void
WGLCanvas::close() noexcept
{
    if (_hwnd && _hdc)
    {
        ::ReleaseDC(_hwnd, _hdc);
        _hwnd = nullptr;
        _hdc = nullptr;
    }

    if (_context)
    {
        ::wglDeleteContext(_context);
        _context = nullptr;
    }
}

WindHandle
WGLCanvas::getWindHandle() const noexcept
{
    return _hwnd;
}

void
WGLCanvas::setSwapInterval(SwapInterval interval) noexcept
{
    if (_interval != interval)
    {
        switch (interval)
        {
        case ray::GPU_FREE:
            wglSwapInterval(0);
            break;
        case ray::GPU_VSYNC:
            wglSwapInterval(1);
            break;
        case ray::GPU_FPS30:
            wglSwapInterval(2);
            break;
        case ray::GPU_FPS15:
            wglSwapInterval(3);
            break;
        default:
            assert(false);
            wglSwapInterval(1);
        }

        _interval = interval;
    }
}

SwapInterval
WGLCanvas::getSwapInterval() const noexcept
{
    return _interval;
}

void
WGLCanvas::present() noexcept
{
    if (_fbconfig.doubleBuffer)
        wglSwapBuffers(_hdc);
    else
        glFlush();
}

void
WGLCanvas::bind()
{
    if (!::wglMakeCurrent(_hdc, _context))
    {
        throw failure("wglMakeCurrent() fail");
    }
}

void
WGLCanvas::unbind() noexcept
{
    ::wglMakeCurrent(0, 0);
}

_NAME_END

#endif