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
#include "egl3_swapchain.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Swapchain, GraphicsSwapchain, "EGL3Swapchain")

EGL3Swapchain::EGL3Swapchain() noexcept
    : _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT)
    , _config(0)
	, _isActive(false)
{
}

EGL3Swapchain::~EGL3Swapchain() noexcept
{
    this->close();
}

bool
EGL3Swapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	if (!initPixelFormat(swapchainDesc))
		return false;

	if (!initSurface(swapchainDesc))
		return false;

	if (!initSwapchain(swapchainDesc))
		return false;

	_swapchainDesc = swapchainDesc;
	return true;
}

void
EGL3Swapchain::close() noexcept
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
EGL3Swapchain::setActive(bool active) noexcept
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
EGL3Swapchain::getActive() const noexcept
{
	return _isActive;
}

void
EGL3Swapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
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
EGL3Swapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

void
EGL3Swapchain::present() noexcept
{
	assert(_isActive);
    assert(_display != EGL_NO_DISPLAY);
    assert(_surface != EGL_NO_SURFACE);

	if (::eglSwapBuffers(_display, _surface) == EGL_FALSE)
		GL_PLATFORM_LOG("eglSwapBuffers() fail : %d", eglGetError());
}

bool
EGL3Swapchain::initSurface(const GraphicsSwapchainDesc& swapchainDesc)
{
	EGLNativeWindowType hwnd = (EGLNativeWindowType)swapchainDesc.getWindHandle();
	_surface = ::eglCreateWindowSurface(_display, _config, hwnd, NULL);
	if (eglGetError() != EGL_SUCCESS)
	{
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	return true;
}

bool 
EGL3Swapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	EGLint pixelFormat[80];
	EGLint index = 0;

	pixelFormat[index++] = EGL_SURFACE_TYPE;
	pixelFormat[index++] = EGL_WINDOW_BIT;

	pixelFormat[index++] = EGL_RENDERABLE_TYPE;
	pixelFormat[index++] = EGL_OPENGL_ES3_BIT;

	if (swapchainDesc.getImageNums() != 2)
	{
		GL_PLATFORM_LOG("Invalid image number");
		return false;
	}

	auto colorFormat = swapchainDesc.getColorFormat();
	if (colorFormat == GraphicsFormat::GraphicsFormatB8G8R8A8UNorm)
	{
		pixelFormat[index++] = EGL_BUFFER_SIZE;
		pixelFormat[index++] = 32;

		pixelFormat[index++] = EGL_RED_SIZE;
		pixelFormat[index++] = 8;

		pixelFormat[index++] = EGL_GREEN_SIZE;
		pixelFormat[index++] = 8;

		pixelFormat[index++] = EGL_BLUE_SIZE;
		pixelFormat[index++] = 8;

		pixelFormat[index++] = EGL_ALPHA_SIZE;
		pixelFormat[index++] = 8;
	}
	else
	{
		GL_PLATFORM_LOG("Can't support color format");
		return false;
	}
	
	auto depthStencilFormat = swapchainDesc.getDepthStencilFormat();
	if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 16;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatX8_D24UNormPack32)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 24;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 32;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 16;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 24;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
	{
		pixelFormat[index++] = EGL_DEPTH_SIZE;
		pixelFormat[index++] = 32;

		pixelFormat[index++] = EGL_STENCIL_SIZE;
		pixelFormat[index++] = 8;
	}
	else
	{
		GL_PLATFORM_LOG("Can't support depth stencil format");
		return false;
	}

	pixelFormat[index++] = EGL_NONE;

	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (_display == EGL_NO_DISPLAY)
	{
		GL_PLATFORM_LOG("eglGetDisplay() fail : %d", eglGetError());
		return false;
	}

	if (::eglInitialize(_display, nullptr, nullptr) == EGL_FALSE)
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
	if (::eglChooseConfig(_display, pixelFormat, &_config, 1, &num) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglChooseConfig() fail : %d", eglGetError());
		return false;
	}

	return true;
}

bool 
EGL3Swapchain::initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	EGLint attribs[80];
	EGLint index = 0;

#if !defined(_BUILD_PLATFORM_ANDROID)
	attribs[index++] = EGL_CONTEXT_MAJOR_VERSION;
	attribs[index++] = 3;

	attribs[index++] = EGL_CONTEXT_MINOR_VERSION;
	attribs[index++] = 0;
#else
	attribs[index++] = EGL_CONTEXT_CLIENT_VERSION;
	attribs[index++] = 3;
#endif

	attribs[index++] = EGL_NONE;
	attribs[index++] = EGL_NONE;

	_context = ::eglCreateContext(_display, _config, EGL_NO_CONTEXT, attribs);
	if (eglGetError() != EGL_SUCCESS)
	{
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	this->setSwapInterval(swapchainDesc.getSwapInterval());
	return true;
}

const GraphicsSwapchainDesc&
EGL3Swapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
EGL3Swapchain::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Swapchain::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END