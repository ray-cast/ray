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
#include "egl2_swapchain.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Swapchain, GraphicsSwapchain, "EGL2Swapchain")

EGL2Swapchain::EGL2Swapchain() noexcept
	: _display(EGL_NO_DISPLAY)
	, _surface(EGL_NO_SURFACE)
	, _context(EGL_NO_CONTEXT)
	, _config(0)
	, _isActive(false)
{
	initPixelFormat(_fbconfig, _ctxconfig);
}

EGL2Swapchain::~EGL2Swapchain() noexcept
{
	this->close();
}

bool
EGL2Swapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	EGLint attribs[80];
	EGLint index = 0, mask = 0, startegy = 0;

	if (_ctxconfig.forward)
	{
		attribs[index++] = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE;
		attribs[index++] = EGL_TRUE;
	}

	if (_ctxconfig.robustness)
	{
		if (_ctxconfig.robustness == GL_REST_NOTIFICATION)
			startegy = EGL_NO_RESET_NOTIFICATION;
		else if (_ctxconfig.robustness == GL_LOSE_CONTEXT_ONREST)
			startegy = EGL_LOSE_CONTEXT_ON_RESET;

		if (startegy)
		{
			attribs[index++] = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY;
			attribs[index++] = startegy;

			attribs[index++] = EGL_CONTEXT_OPENGL_ROBUST_ACCESS;
			attribs[index++] = EGL_TRUE;
		}
	}

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

	EGLNativeWindowType hwnd = (EGLNativeWindowType)swapchainDesc.getWindHandle();

	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (_display == EGL_NO_DISPLAY)
	{
		GL_PLATFORM_LOG("eglGetDisplay() fail : %d", eglGetError());
		return false;
	}

	if (::eglInitialize(_display, &_ctxconfig.major, &_ctxconfig.minor) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglInitialize() fail : %d", eglGetError());
		return false;
	}

	if (::eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglBindAPI() fail : %d", eglGetError());
		return false;
	}

	EGLint num = 0;
	if (::eglChooseConfig(_display, pixelformat, &_config, 1, &num) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglChooseConfig() fail : %d", eglGetError());
		return false;
	}

	_surface = ::eglCreateWindowSurface(_display, _config, hwnd, NULL);
	if (eglGetError() != EGL_SUCCESS)
	{
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	_context = ::eglCreateContext(_display, _config, _ctxconfig.share, attribs);
	if (eglGetError() != EGL_SUCCESS)
	{
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	_swapchainDesc = swapchainDesc;
	return true;
}

void
EGL2Swapchain::close() noexcept
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
EGL2Swapchain::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
		{
			if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
			{
				GL_PLATFORM_LOG("eglMakeCurrent() fail : %d", eglGetError());
				return;
			}
		}
		else
		{
			if (eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
			{
				GL_PLATFORM_LOG("eglMakeCurrent() fail : %d", eglGetError());
				return;
			}
		}

		_isActive = active;
	}
}

bool
EGL2Swapchain::getActive() const noexcept
{
	return _isActive;
}

void
EGL2Swapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	assert(_display != EGL_NO_DISPLAY);

	switch (interval)
	{
	case GraphicsSwapInterval::GraphicsSwapIntervalFree:
		if (eglSwapInterval(_display, 0) == GL_FALSE)
			GL_PLATFORM_LOG("eglSwapInterval(SwapInterval::Free) fail : %d", eglGetError());
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalVsync:
		if (eglSwapInterval(_display, 1) == GL_FALSE)
			GL_PLATFORM_LOG("eglSwapInterval(SwapInterval::Vsync) fail : %d", eglGetError());
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalFps30:
		if (eglSwapInterval(_display, 2) == GL_FALSE)
			GL_PLATFORM_LOG("eglSwapInterval(SwapInterval::Fps30) fail : %d", eglGetError());
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalFps15:
		if (eglSwapInterval(_display, 3) == GL_FALSE)
			GL_PLATFORM_LOG("eglSwapInterval(SwapInterval::Fps15) fail : %d", eglGetError());
		break;
	default:
		GL_PLATFORM_LOG("Invlid SwapInterval");
		return;
	}

	_swapchainDesc.setSwapInterval(interval);
}

GraphicsSwapInterval
EGL2Swapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

void
EGL2Swapchain::present() noexcept
{
	assert(_isActive);
	assert(_display != EGL_NO_DISPLAY);
	assert(_surface != EGL_NO_SURFACE);

	if (::eglSwapBuffers(_display, _surface) == EGL_FALSE)
		GL_PLATFORM_LOG("eglSwapBuffers() fail : %d", eglGetError());
}

void
EGL2Swapchain::initPixelFormat(GPUfbconfig& fbconfig, GPUctxconfig& ctxconfig) noexcept
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
	ctxconfig.share = EGL_NO_CONTEXT;
	ctxconfig.profile = GLattr::GL_CORE_PROFILE;
	ctxconfig.forward = 0;
	ctxconfig.multithread = false;
}

const GraphicsSwapchainDesc&
EGL2Swapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
EGL2Swapchain::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Swapchain::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END