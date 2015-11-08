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
#if defined(_BUILD_PLATFORM_ANDROID) || defined(_BUILD_PLATFORM_LINUX)
#include <ray/egl_canvas.h>

_NAME_BEGIN

EGLCanvas::EGLCanvas() noexcept
    : _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT)
    , _config(0)
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

EGLCanvas::~EGLCanvas() noexcept
{
    this->close();
}
void
EGLCanvas::open(WindHandle hwnd) except
{
#if defined(__WINDOWS__)
    _hwnd = (EGLNativeWindowType)hwnd;
    _hdc = ::GetDC(_hwnd);
    _display = ::eglGetDisplay(_hdc);
#endif

    if (_display == EGL_NO_DISPLAY)
    {
		throw failure(__TEXT("EGL_NO_DISPLAY"));
    }

    if (::eglInitialize(_display, 0, 0) == EGL_FALSE)
    {
		throw failure(__TEXT("eglInitialize() fail"));
    }

    if (::eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
    {
		throw failure(__TEXT("eglBindAPI() fail"));
    }

    const EGLint pixelformat[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, _fbconfig.redSize,
        EGL_GREEN_SIZE, _fbconfig.greenSize,
        EGL_BLUE_SIZE, _fbconfig.blueSize,
        EGL_ALPHA_SIZE, _fbconfig.alphaSize,
        EGL_BUFFER_SIZE, _fbconfig.bufferSize,
        EGL_DEPTH_SIZE, _fbconfig.depthSize,
        EGL_STENCIL_SIZE, _fbconfig.stencilSize,
        EGL_SAMPLES, _fbconfig.samples ? EGL_TRUE : EGL_FALSE,
        EGL_NONE
    };

    EGLint num = 0;
    if (eglChooseConfig(_display, pixelformat, &_config, 1, &num) == EGL_FALSE)
    {
		throw failure(__TEXT("eglChooseConfig() fail"));
    }

    _surface = eglCreateWindowSurface(_display, _config, _hwnd, NULL);
    if (_surface == EGL_NO_SURFACE)
    {
		throw failure(__TEXT("eglCreateWindowSurface() fail"));
    }

    EGLint attribs[80];
    EGLint index = 0, mask = 0, startegy = 0;

    if (_ctxconfig.forward)
    {
        attribs[index++] = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE;
        attribs[index++] = EGL_TRUE;
    }

    if (_ctxconfig.profile)
    {
        if (_ctxconfig.profile == GPU_GL_CORE_PROFILE)
            mask = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;

        if (_ctxconfig.profile == GPU_GL_COMPAT_PROFILE)
            mask = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;

        if (mask)
        {
            attribs[index++] = EGL_CONTEXT_OPENGL_PROFILE_MASK;
            attribs[index++] = mask;
        }
    }

    if (_ctxconfig.robustness)
    {
        if (_ctxconfig.robustness == GPU_GL_REST_NOTIFICATION)
            startegy = EGL_NO_RESET_NOTIFICATION;
        else if (_ctxconfig.robustness == GPU_GL_LOSE_CONTEXT_ONREST)
            startegy = EGL_LOSE_CONTEXT_ON_RESET;

        if (startegy)
        {
            attribs[index++] = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY;
            attribs[index++] = startegy;

            attribs[index++] = EGL_CONTEXT_OPENGL_ROBUST_ACCESS;
            attribs[index++] = EGL_TRUE;
        }
    }

    if (_ctxconfig.major != 1 || _ctxconfig.minor != 0)
    {
        attribs[index++] = EGL_CONTEXT_MAJOR_VERSION;
        attribs[index++] = _ctxconfig.major;

        attribs[index++] = EGL_CONTEXT_MINOR_VERSION;
        attribs[index++] = _ctxconfig.minor;
    }

    attribs[index++] = 0;
    attribs[index++] = 0;

    EGLint attrib_list[] = 
    {
        EGL_CONTEXT_CLIENT_TYPE, 3,
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE 
    };
	
    _context = ::eglCreateContext(_display, _config, _ctxconfig.share, attrib_list);
    if (!_context)
    {
		throw failure(__TEXT("eglCreateContext() fail"));
    }
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
EGLCanvas::setSwapInterval(SwapInterval interval) noexcept
{
    assert(_display != EGL_NO_DISPLAY);

	if (_interval != interval)
	{
		switch (interval)
		{
		case ray::GPU_FREE:
			eglSwapInterval(_display, 0);
			break;
		case ray::GPU_VSYNC:
			eglSwapInterval(_display, 1);
			break;
		case ray::GPU_FPS30:
			eglSwapInterval(_display, 2);
			break;
		case ray::GPU_FPS15:
			eglSwapInterval(_display, 3);
			break;
		default:
			assert(false);
			eglSwapInterval(_display, 1);
		}

		_interval = interval;
	}
}

SwapInterval
EGLCanvas::getSwapInterval() const noexcept
{
	return _interval;
}

void
EGLCanvas::bind() noexcept
{
    if (!eglMakeCurrent(_display, _surface, _surface, _context))
        assert(false);
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

	if (!eglSwapBuffers(_display, _surface))
		assert(false);
}

void
EGLCanvas::setWindowResolution(std::size_t w, std::size_t h) noexcept
{
	_width = w;
	_height = h;
}

std::size_t
EGLCanvas::getWindowWidth() const noexcept
{
	return _width;
}

std::size_t
EGLCanvas::getWindowHeight() const noexcept
{
	return _height;
}

WindHandle
EGLCanvas::getWindHandle() const noexcept
{
	return _hwnd;
}

_NAME_END
#endif