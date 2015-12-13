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
#include "egl2_canvas.h"

_NAME_BEGIN

EGLCanvas::EGLCanvas() noexcept
    : _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT)
    , _config(0)
	, _interval(SwapInterval::GPU_VSYNC)
{
	initPixelFormat(_fbconfig, _ctxconfig);
}

EGLCanvas::EGLCanvas(WindHandle hwnd) except
	: _display(EGL_NO_DISPLAY)
	, _surface(EGL_NO_SURFACE)
	, _context(EGL_NO_CONTEXT)
	, _config(0)
	, _interval(SwapInterval::GPU_VSYNC)
{
	initPixelFormat(_fbconfig, _ctxconfig);

	this->open(hwnd);
}

EGLCanvas::~EGLCanvas() noexcept
{
    this->close();
}
void
EGLCanvas::open(WindHandle hwnd) except
{
	EGLint attribs[80];
	EGLint index = 0, mask = 0, startegy = 0;

#if !defined(__ANDROID__)
	if (_ctxconfig.forward)
	{
		attribs[index++] = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE;
		attribs[index++] = EGL_TRUE;
	}

    if (_ctxconfig.api != GPU_OPENGL_ES_API)
    {
        if (_ctxconfig.profile == GPU_GL_CORE_PROFILE)
            mask = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;

        if (_ctxconfig.profile == GPU_GL_COMPAT_PROFILE)
            mask = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
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
#endif

	if (_ctxconfig.major > 0 && _ctxconfig.major < 4)
	{
		attribs[index++] = EGL_CONTEXT_MAJOR_VERSION;
		attribs[index++] = _ctxconfig.major;

		attribs[index++] = EGL_CONTEXT_MINOR_VERSION;
		attribs[index++] = _ctxconfig.minor;
	}

	if (mask)
	{
		attribs[index++] = EGL_CONTEXT_OPENGL_PROFILE_MASK;
		attribs[index++] = mask;
	}

    attribs[index++] = EGL_NONE;
    attribs[index++] = EGL_NONE;

	const EGLint pixelformat[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
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

	_hwnd = (EGLNativeWindowType)hwnd;

	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (_display == EGL_NO_DISPLAY)
		throw failure(__TEXT("eglGetDisplay() fail"));

	if (::eglInitialize(_display, 0, 0) == EGL_FALSE)
		throw failure(__TEXT("eglInitialize() fail"));

	if (::eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
		throw failure(__TEXT("eglBindAPI() fail"));

	EGLint num = 0;
	if (::eglChooseConfig(_display, pixelformat, &_config, 1, &num) == EGL_FALSE)
		throw failure(__TEXT("eglChooseConfig() fail"));

	_surface = ::eglCreateWindowSurface(_display, _config, _hwnd, NULL);
	if (!_surface)
		throw failure(__TEXT("eglCreateContext() fail"));

    _context = ::eglCreateContext(_display, _config, _ctxconfig.share, attribs);
    if (!_context)
		throw failure(__TEXT("eglCreateContext() fail"));

	if (!::eglMakeCurrent(_display, _surface, _surface, _context))
		throw failure(__TEXT("eglMakeCurrent() fail"));
}

void
EGLCanvas::close() noexcept
{
    if (_surface != EGL_NO_SURFACE)
    {
        ::eglDestroySurface(_display, _surface);
        _surface = EGL_NO_SURFACE;
    }

    if (_context != EGL_NO_CONTEXT)
    {
        ::eglDestroyContext(_display, _context);
        _context = EGL_NO_CONTEXT;
    }

	if (_display != EGL_NO_DISPLAY)
	{
		::eglTerminate(_display);
		_display = EGL_NO_DISPLAY;
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
EGLCanvas::present() noexcept
{
    assert(_display != EGL_NO_DISPLAY);
    assert(_surface != EGL_NO_SURFACE);
	::eglSwapBuffers(_display, _surface);
}

WindHandle
EGLCanvas::getWindHandle() const noexcept
{
	return _hwnd;
}

void
EGLCanvas::onActivate() except
{
	if (!eglMakeCurrent(_display, _surface, _surface, _context))
		throw failure(__TEXT("eglMakeCurrent() fail"));
}

void
EGLCanvas::onDeactivate() except
{
	if (!eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
		throw failure(__TEXT("eglMakeCurrent() fail"));
}

void
EGLCanvas::initPixelFormat(GPUfbconfig& fbconfig, GPUctxconfig& ctxconfig) noexcept
{
	fbconfig.redSize = 8;
	fbconfig.greenSize = 8;
	fbconfig.blueSize = 8;
	fbconfig.alphaSize = 8;
	fbconfig.bufferSize = 32;
	fbconfig.depthSize = 24;
	fbconfig.stencilSize = 8;
	fbconfig.accumSize = 0;
	fbconfig.accumRedSize = 0;
	fbconfig.accumGreenSize = 0;
	fbconfig.accumBlueSize = 0;
	fbconfig.accumAlphaSize = 0;
	fbconfig.samples = 0;

	ctxconfig.major = 2;
	ctxconfig.minor = 0;
	ctxconfig.release = 0;
	ctxconfig.robustness = 0;
	ctxconfig.share = nullptr;
	ctxconfig.api = GPU_OPENGL_ES_API;
	ctxconfig.profile = GPU_GL_CORE_PROFILE;
	ctxconfig.forward = 0;
	ctxconfig.multithread = false;
}

_NAME_END