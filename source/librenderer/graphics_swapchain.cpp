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
#include <ray/graphics_swapchain.h>

_NAME_BEGIN

GraphicsSwapchainDesc::GraphicsSwapchainDesc() noexcept
	: _format(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm)
	, _depthFormat(GraphicsFormat::GraphicsFormatD16UNorm)
	, _interval(GraphicsSwapInterval::GraphicsSwapIntervalVsync)
	, _imageNums(2)
	, _window(0)
	, _width(0)
	, _height(0)
{
}

GraphicsSwapchainDesc::~GraphicsSwapchainDesc() noexcept
{
}

void
GraphicsSwapchainDesc::setWindHandle(WindHandle hwnd) noexcept
{
	_window = hwnd;
}

WindHandle
GraphicsSwapchainDesc::getWindHandle() const noexcept
{
	return _window;
}

void
GraphicsSwapchainDesc::setWidth(std::uint32_t width) noexcept
{
	_width = width;
}

std::uint32_t
GraphicsSwapchainDesc::getWidth() const noexcept
{
	return _width;
}

void
GraphicsSwapchainDesc::setHeight(std::uint32_t height) noexcept
{
	_height = height;
}

std::uint32_t
GraphicsSwapchainDesc::getHeight() const noexcept
{
	return _height;
}

void 
GraphicsSwapchainDesc::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	_interval = interval;
}

GraphicsSwapInterval
GraphicsSwapchainDesc::getSwapInterval() const noexcept
{
	return _interval;
}

void
GraphicsSwapchainDesc::setColorFormat(GraphicsFormat format) noexcept
{
	_format = format;
}

GraphicsFormat
GraphicsSwapchainDesc::getColorFormat() const noexcept
{
	return _format;
}

void
GraphicsSwapchainDesc::setDepthFormat(GraphicsFormat format) noexcept
{
	assert( format == GraphicsFormat::GraphicsFormatD16UNorm || 
			format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 || 
			format == GraphicsFormat::GraphicsFormatD32_SFLOAT ||
			format == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt ||
			format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 ||
			format == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt);
	_depthFormat = format;
}

GraphicsFormat
GraphicsSwapchainDesc::getDepthFormat() const noexcept
{
	return _depthFormat;
}

void
GraphicsSwapchainDesc::setImageNums(std::uint32_t imageNums) noexcept
{
	_imageNums = imageNums;
}

std::uint32_t
GraphicsSwapchainDesc::getImageNums() const noexcept
{
	return _imageNums;
}

GraphicsSwapchain::GraphicsSwapchain() noexcept
{
}

GraphicsSwapchain::~GraphicsSwapchain() noexcept
{
}

_NAME_END