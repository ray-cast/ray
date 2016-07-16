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
#if defined(_BUILD_PLATFORM_APPLE)
#include "nsgl_swapchain.h"

_NAME_BEGIN

__ImplementSubClass(NSGLSwapchain, GraphicsSwapchain, "NSGLSwapchain")

NSGLSwapchain* NSGLSwapchain::_swapchain = nullptr;

NSGLSwapchain::NSGLSwapchain() noexcept
	: _isActive(false)
	, _major(3)
	, _minor(3)
{
}

NSGLSwapchain::NSGLSwapchain(GLuint major, GLuint minor) noexcept
	: _isActive(false)
	, _major(major)
	, _minor(minor)
{
}

NSGLSwapchain::~NSGLSwapchain() noexcept
{
	this->close();
}

bool
NSGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	assert(swapchainDesc.getWindHandle());

	if (!initSurface(swapchainDesc))
		return false;

	if (!initPixelFormat(swapchainDesc))
		return false;

	if (!initSwapchain(swapchainDesc))
		return false;

	_swapchainDesc = swapchainDesc;
	return true;
}

void
NSGLSwapchain::close() noexcept
{
	this->setActive(false);
}

void
NSGLSwapchain::setActive(bool active) noexcept
{
}

bool
NSGLSwapchain::getActive() const noexcept
{
	return _isActive;
}

void
NSGLSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	_swapchainDesc.setSwapInterval(interval);
}

GraphicsSwapInterval
NSGLSwapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

void
NSGLSwapchain::present() noexcept
{
}

bool
NSGLSwapchain::initSurface(const GraphicsSwapchainDesc& swapchainDesc)
{
	return false;
}

bool
NSGLSwapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	return true;
}

const GraphicsSwapchainDesc&
NSGLSwapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
NSGLSwapchain::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
NSGLSwapchain::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END

#endif