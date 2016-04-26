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

WGLSwapchain* WGLSwapchain::_swapchain = nullptr;

WGLSwapchain::WGLSwapchain() noexcept
	: _hdc(nullptr)
	, _context(nullptr)
	, _isActive(false)
	, _major(3)
	, _minor(3)
{
}

WGLSwapchain::WGLSwapchain(GLuint major, GLuint minor) noexcept
	: _hdc(nullptr)
	, _context(nullptr)
	, _isActive(false)
	, _major(major)
	, _minor(minor)
{
}

WGLSwapchain::~WGLSwapchain() noexcept
{
	this->close();
}

bool
WGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	assert(swapchainDesc.getWindHandle());

	if (!initSurface(swapchainDesc))
		return false;

	if (!initPixelFormat(swapchainDesc))
		return false;

	if (!initWGLExtensions())
		return false;

	if (!initSwapchain(swapchainDesc))
		return false;

	_swapchainDesc = swapchainDesc;
	return true;
}

void
WGLSwapchain::close() noexcept
{
	this->setActive(false);

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

			if (_swapchain)
				_swapchain->setActive(false);

			_swapchain = this;
		}
		else
		{
			if (!::wglMakeCurrent(0, 0))
				GL_PLATFORM_LOG("wglMakeCurrent() fail");

			if (_swapchain == this)
				_swapchain = nullptr;
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
WGLSwapchain::initSurface(const GraphicsSwapchainDesc& swapchainDesc)
{
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

	return true;
}

bool
WGLSwapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	PIXELFORMATDESCRIPTOR pfd;
	std::memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_STEREO;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cAccumBits = 0;
	pfd.cAccumRedBits = 0;
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.bReserved = 0;
	pfd.dwLayerMask = 0;
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	if (swapchainDesc.getImageNums() != 1 && swapchainDesc.getImageNums() != 2)
	{
		GL_PLATFORM_LOG("Invalid image number");
		return false;
	}

	if (swapchainDesc.getImageNums() == 2)
		pfd.dwFlags |= PFD_DOUBLEBUFFER;

	auto colorFormat = swapchainDesc.getColorFormat();
	if (colorFormat == GraphicsFormat::GraphicsFormatB8G8R8A8UNorm)
	{
		pfd.cColorBits = 32;
		pfd.cBlueBits = 8;
		pfd.cBlueShift = 0;
		pfd.cGreenBits = 8;
		pfd.cGreenShift = 8;
		pfd.cRedBits = 8;
		pfd.cRedShift = 16;
		pfd.cAlphaBits = 8;
		pfd.cAlphaShift = 24;
	}
	else
	{
		GL_PLATFORM_LOG("Can't support color format");
		return false;
	}

	auto depthStencilFormat = swapchainDesc.getDepthStencilFormat();
	if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm)
	{
		pfd.cDepthBits = 16;
		pfd.cStencilBits = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatX8_D24UNormPack32)
	{
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT)
	{
		pfd.cDepthBits = 32;
		pfd.cStencilBits = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt)
	{
		pfd.cDepthBits = 16;
		pfd.cStencilBits = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt)
	{
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
	{
		pfd.cDepthBits = 32;
		pfd.cStencilBits = 8;
	}
	else
	{
		GL_PLATFORM_LOG("Can't support depth stencil format");
		return false;
	}

	int pixelFormat = ::ChoosePixelFormat(_hdc, &pfd);
	if (!pixelFormat)
	{
		GL_PLATFORM_LOG("ChoosePixelFormat() fail");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd2;
	if (!::DescribePixelFormat(_hdc, pixelFormat, sizeof(pfd2), &pfd2))
	{
		GL_PLATFORM_LOG("DescribePixelFormat() fail");
		return false;
	}

	if (pfd2.cRedBits != pfd.cRedBits ||
		pfd2.cRedShift != pfd.cRedShift ||
		pfd2.cGreenBits != pfd.cGreenBits ||
		pfd2.cGreenShift != pfd.cGreenShift ||
		pfd2.cBlueBits != pfd.cBlueBits ||
		pfd2.cBlueShift != pfd.cBlueShift)
	{
		GL_PLATFORM_LOG("Can't support color format");
		return false;
	}

	if (pfd2.cDepthBits != pfd.cDepthBits ||
		pfd2.cStencilBits != pfd.cStencilBits)
	{
		GL_PLATFORM_LOG("Can't support depth stencil format");
		return false;
	}

	if (!::SetPixelFormat(_hdc, pixelFormat, &pfd))
	{
		GL_PLATFORM_LOG("SetPixelFormat() fail");
		return false;
	}

	return true;
}

bool
WGLSwapchain::initWGLExtensions() noexcept
{
	HGLRC context = ::wglCreateContext(_hdc);
	if (!context)
	{
		GL_PLATFORM_LOG("wglCreateContext fail");
		return false;
	}

	if (!::wglMakeCurrent(_hdc, context))
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

	::wglDeleteContext(context);
	return true;
}

bool
WGLSwapchain::initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	int index = 0;
	int mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	int flags = 0;

#if _DEBUG
	flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

	int major = 0;
	int minor = 0;

	auto deviceType = this->getDevice()->getGraphicsDeviceDesc().getDeviceType();
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore)
	{
		major = 4;
		minor = 5;
		mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	}
	else
	{
		major = 3;
		minor = 3;
	}

	int attribs[40];
	attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attribs[index++] = major;

	attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attribs[index++] = minor;

	attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
	attribs[index++] = flags;

	attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;
	attribs[index++] = mask;

	attribs[index] = 0;
	attribs[index] = 0;

	_context = __wglCreateContextAttribsARB(_hdc, nullptr, attribs);
	if (!_context)
	{
		GL_PLATFORM_LOG("wglCreateContextAttribs fail");
		return false;
	}

	::wglMakeCurrent(_hdc, _context);
	this->setSwapInterval(swapchainDesc.getSwapInterval());
	::wglMakeCurrent(NULL, NULL);

	return true;
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