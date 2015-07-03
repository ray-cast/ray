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
    , _width(0)
    , _height(0)
    , _interval(SwapInterval::GPU_VSYNC)
{
    _fbconfig.redSize = 8;
    _fbconfig.greenSize = 8;
    _fbconfig.blueSize = 8;
    _fbconfig.alphaSize = 8;
    _fbconfig.bufferSize = 32;
    _fbconfig.depthSize = 24;
    _fbconfig.stencilSize = 8;
    _fbconfig.accumSize = 32;
    _fbconfig.accumRedSize = 8;
    _fbconfig.accumGreenSize = 8;
    _fbconfig.accumBlueSize = 8;
    _fbconfig.accumAlphaSize = 8;
    _fbconfig.samples = 1;

    _ctxconfig.major = 3;
    _ctxconfig.minor = 3;
    _ctxconfig.release = 0;
    _ctxconfig.robustness = 0;
    _ctxconfig.share = nullptr;
    _ctxconfig.profile = GPU_GL_CORE_PROFILE;
    _ctxconfig.forward = 0;
    _ctxconfig.multithread = false;
}

WGLCanvas::~WGLCanvas() noexcept
{
    this->close();
}

void
WGLCanvas::setup(WindHandle hwnd) except
{
    assert(hwnd);

    if ((_ctxconfig.major < 1 || _ctxconfig.minor < 0) ||
        (_ctxconfig.major == 1 && _ctxconfig.minor > 5) ||
        (_ctxconfig.major == 2 && _ctxconfig.minor > 1) ||
        (_ctxconfig.major == 3 && _ctxconfig.minor > 3))
    {
        throw failure("Invlid major and minor");
    }

    if (_ctxconfig.profile)
    {
        if (_ctxconfig.profile != GPU_GL_CORE_PROFILE &&
            _ctxconfig.profile != GPU_GL_COMPAT_PROFILE)
        {
            throw failure("Invlid profile");
        }

        if (_ctxconfig.major < 3 || (_ctxconfig.major == 3 && _ctxconfig.minor < 2))
        {
            throw failure("The version is small");
        }
    }

    if (_ctxconfig.forward && _ctxconfig.major < 3)
    {
        throw failure("The version is small");
    }

    if (!IsWindow((HWND)hwnd))
    {
        throw failure("Invlid HWND");
    }

    _hwnd = (HWND)hwnd;
    _hdc = ::GetDC(_hwnd);
    if (!_hdc)
    {
        throw failure("GetDC() fail");
    }

    PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_STEREO;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = _fbconfig.bufferSize;
    pfd.cRedBits = _fbconfig.redSize;
    pfd.cRedShift = 0;
    pfd.cGreenBits = _fbconfig.greenSize;
    pfd.cGreenShift = 0;
    pfd.cBlueBits = _fbconfig.blueSize;
    pfd.cBlueShift = 0;
    pfd.cAlphaBits = _fbconfig.alphaSize;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = _fbconfig.accumSize;
    pfd.cAccumRedBits = _fbconfig.accumRedSize;
    pfd.cAccumGreenBits = _fbconfig.accumGreenSize;
    pfd.cAccumBlueBits = _fbconfig.accumBlueSize;
    pfd.cAccumAlphaBits = _fbconfig.accumAlphaSize;
    pfd.cDepthBits = _fbconfig.depthSize;
    pfd.cStencilBits = _fbconfig.stencilSize;
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

    if (_ctxconfig.forward)
        flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

    if (_ctxconfig.profile)
    {
        if (_ctxconfig.profile == GPU_GL_CORE_PROFILE)
            mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

        if (_ctxconfig.profile == GPU_GL_COMPAT_PROFILE)
            mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    }

    if (OGLExtenstion::isSupport(ARB_create_context_robustness))
    {
        if (_ctxconfig.robustness)
        {
            if (_ctxconfig.robustness == GPU_GL_REST_NOTIFICATION)
                startegy = WGL_NO_RESET_NOTIFICATION_ARB;
            else if (_ctxconfig.robustness == GPU_GL_LOSE_CONTEXT_ONREST)
                startegy = WGL_LOSE_CONTEXT_ON_RESET_ARB;

            flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
        }
    }

    if (_ctxconfig.major != 1 || _ctxconfig.minor != 0)
    {
        attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[index++] = _ctxconfig.major;

        attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[index++] = _ctxconfig.minor;
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

    _fbconfig = _fbconfig;
    _ctxconfig = _ctxconfig;
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

void
WGLCanvas::setWindowResolution(std::size_t w, std::size_t h) noexcept
{
    _width = w;
    _height = h;
}

std::size_t
WGLCanvas::getWindowWidth() const noexcept
{
    return _width;
}

std::size_t
WGLCanvas::getWindowHeight() const noexcept
{
    return _height;
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
    wglSwapBuffers(_hdc);
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