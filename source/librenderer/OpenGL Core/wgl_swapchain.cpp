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
#if defined(_BUILD_PLATFORM_WINDOWS)
#include "wgl_swapchain.h"

_NAME_BEGIN

__ImplementSubClass(WGLSwapchain, GraphicsSwapchain, "WGLSwapchain")

WGLSwapchain::WGLSwapchain() noexcept
	: _hdc(nullptr)
	, _context(nullptr)
	, _isActive(true)
{
	initPixelFormat(_fbconfig, _ctxconfig);
}

WGLSwapchain::~WGLSwapchain() noexcept
{
	this->close();
}

bool
WGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	assert(swapchainDesc.getWindHandle());

	if ((_ctxconfig.major < 1 || _ctxconfig.minor < 0) ||
		(_ctxconfig.major == 1 && _ctxconfig.minor > 5) ||
		(_ctxconfig.major == 2 && _ctxconfig.minor > 1) ||
		(_ctxconfig.major == 3 && _ctxconfig.minor > 3))
	{
		GL_PLATFORM_LOG("Invlid major and minor");
		return false;
	}

	if (_ctxconfig.profile)
	{
		if (_ctxconfig.profile != GLattr::GL_CORE_PROFILE &&
			_ctxconfig.profile != GLattr::GL_COMPAT_PROFILE)
		{
			GL_PLATFORM_LOG("Invlid profile");
			return false;
		}

		if (_ctxconfig.major < 3 || (_ctxconfig.major == 3 && _ctxconfig.minor < 2))
		{
			GL_PLATFORM_LOG("The version is small");
			return false;
		}
	}

	if (_ctxconfig.forward && _ctxconfig.major < 3)
	{
		GL_PLATFORM_LOG("The version is small");
		return false;
	}

	if (!IsWindow((HWND)swapchainDesc.getWindHandle()))
	{
		GL_PLATFORM_LOG("Invlid HWND");
		return false;
	}

	HWND hwnd = (HWND)swapchainDesc.getWindHandle();
	_hdc = ::GetDC(hwnd);
	if (!_hdc)
	{
		GL_PLATFORM_LOG("GetDC() fail");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));

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
		GL_PLATFORM_LOG("ChoosePixelFormat() fail");
		return false;
	}

	if (!::DescribePixelFormat(_hdc, pixelFormat, sizeof(pfd), &pfd))
	{
		GL_PLATFORM_LOG("DescribePixelFormat() fail");
		return false;
	}

	if (!::SetPixelFormat(_hdc, pixelFormat, &pfd))
	{
		GL_PLATFORM_LOG("SetPixelFormat() fail");
		return false;
	}

	if (!initWGLExtensions(_hdc))
	{
		GL_PLATFORM_LOG("initWGLExtensions() fail");
		return false;
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
		if (_ctxconfig.profile == GLattr::GL_CORE_PROFILE)
			mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		else if (_ctxconfig.profile == GLattr::GL_COMPAT_PROFILE)
			mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	}

	if (_ctxconfig.robustness)
	{
		if (_ctxconfig.robustness == GLattr::GL_REST_NOTIFICATION)
			startegy = WGL_NO_RESET_NOTIFICATION_ARB;
		else if (_ctxconfig.robustness == GLattr::GL_LOSE_CONTEXT_ONREST)
			startegy = WGL_LOSE_CONTEXT_ON_RESET_ARB;

		flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
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

	_context = __wglCreateContextAttribsARB(_hdc, nullptr, attribs);
	if (!_context)
	{
		GL_PLATFORM_LOG("wglCreateContextAttribs fail");
		return false;
	}

	::wglMakeCurrent(_hdc, _context);
	this->setSwapInterval(_swapchainDesc.getSwapInterval());

	_isActive = false;
	_fbconfig = _fbconfig;
	_ctxconfig = _ctxconfig;

	_swapchainDesc = swapchainDesc;
	return true;
}

void
WGLSwapchain::close() noexcept
{
	if (_hdc)
	{
		::ReleaseDC((HWND)_swapchainDesc.getWindHandle(), _hdc);
		_hdc = nullptr;
	}

	if (_context)
	{
		::wglDeleteContext(_context);
		_context = nullptr;
	}
}

void
WGLSwapchain::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
		{
			if (!::wglMakeCurrent(_hdc, _context))
				GL_PLATFORM_LOG("wglMakeCurrent() fail");
		}
		else
		{
			if (!::wglMakeCurrent(0, 0))
				GL_PLATFORM_LOG("wglMakeCurrent() fail");
		}

		_isActive = active;
	}
}

bool
WGLSwapchain::getActive() const noexcept
{
	return _isActive;
}

void
WGLSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	assert(__wglSwapIntervalEXT);

	switch (interval)
	{
	case GraphicsSwapInterval::GraphicsSwapIntervalFree:
		if (!__wglSwapIntervalEXT(0))
			GL_PLATFORM_LOG("wglSwapInterval(SwapInterval::Free) fail");
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalVsync:
		if (!__wglSwapIntervalEXT(1))
			GL_PLATFORM_LOG("wglSwapInterval(SwapInterval::Vsync) fail");
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalFps30:
		if (!__wglSwapIntervalEXT(2))
			GL_PLATFORM_LOG("wglSwapInterval(SwapInterval::Fps30) fail");
		break;
	case GraphicsSwapInterval::GraphicsSwapIntervalFps15:
		if (!__wglSwapIntervalEXT(3))
			GL_PLATFORM_LOG("wglSwapInterval(SwapInterval::Fps15) fail");
		break;
	default:
		GL_PLATFORM_LOG("Invalid swap interval");
	}

	_swapchainDesc.setSwapInterval(interval);
}

GraphicsSwapInterval
WGLSwapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

void
WGLSwapchain::present() noexcept
{
	assert(__wglSwapBuffers);
	__wglSwapBuffers(_hdc);
}

bool
WGLSwapchain::initWGLExtensions(HDC hdc) except
{
	HGLRC context = ::wglCreateContext(hdc);
	if (!context)
	{
		GL_PLATFORM_LOG("wglCreateContext fail");
		return false;
	}

	if (!::wglMakeCurrent(hdc, context))
	{
		GL_PLATFORM_LOG("wglMakeCurrent fail");
		::wglDeleteContext(context);
		return false;
	}

	if (!::initWGLExtenstion())
	{
		GL_PLATFORM_LOG("initWGLExtenstion fail");
		::wglDeleteContext(context);
		return false;
	}

	return ::wglDeleteContext(context) ? true : false;
}

void
WGLSwapchain::initPixelFormat(GPUfbconfig& fbconfig, GPUctxconfig& ctxconfig) noexcept
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

	ctxconfig.major = 3;
	ctxconfig.minor = 3;
	ctxconfig.release = 0;
	ctxconfig.robustness = 0;
	ctxconfig.share = nullptr;
	ctxconfig.api = 0;
	ctxconfig.profile = GLattr::GL_CORE_PROFILE;
	ctxconfig.forward = 0;
	ctxconfig.multithread = false;
}

const GraphicsSwapchainDesc&
WGLSwapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
WGLSwapchain::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
WGLSwapchain::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END

#endif