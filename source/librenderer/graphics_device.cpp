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
#include <ray/graphics_device.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsDevice, rtti::Interface, "GraphicsDevice")
__ImplementSubInterface(GraphicsDevice2, GraphicsDevice, "GraphicsDevice2")

GraphicsPixelFormatDesc::GraphicsPixelFormatDesc() noexcept
	: _minColorBits(0)
	, _minRedBits(0)
	, _minGreenBits(0)
	, _minBlueBits(0)
	, _minAlphaBits(0)
	, _minDepthBits(0)
	, _minStencilBits(0)
	, _maxColorBits(0)
	, _maxRedBits(0)
	, _maxGreenBits(0)
	, _maxBlueBits(0)
	, _maxAlphaBits(0)
	, _maxDepthBits(0)
	, _maxStencilBits(0)
	, _pixelType(GraphicsFormatType::GraphicsFormatTypeNone)
{
}

GraphicsPixelFormatDesc::~GraphicsPixelFormatDesc() noexcept
{
}

void
GraphicsPixelFormatDesc::setFormatType(GraphicsFormatType type) noexcept
{
	_pixelType = type;
}

GraphicsFormatType
GraphicsPixelFormatDesc::getFormatType() const noexcept
{
	return _pixelType;
}

void
GraphicsPixelFormatDesc::setMinColorBits(std::uint8_t min) noexcept
{
    _minColorBits = min;
}

void
GraphicsPixelFormatDesc::setMinRedBits(std::uint8_t min) noexcept
{
    _minRedBits = min;
}

void
GraphicsPixelFormatDesc::setMinGreenBits(std::uint8_t min) noexcept
{
    _minGreenBits = min;
}

void
GraphicsPixelFormatDesc::setMinBlueBits(std::uint8_t min) noexcept
{
    _minBlueBits = min;
}

void
GraphicsPixelFormatDesc::setMinAlphaBits(std::uint8_t min) noexcept
{
    _minAlphaBits = min;
}

void
GraphicsPixelFormatDesc::setMinDepthBits(std::uint8_t min) noexcept
{
    _minDepthBits = min;
}

void
GraphicsPixelFormatDesc::setMinStencilBits(std::uint8_t min) noexcept
{
    _minStencilBits = min;
}

void
GraphicsPixelFormatDesc::setMaxColorBits(std::uint8_t max) noexcept
{
    _maxColorBits = max;
}

void
GraphicsPixelFormatDesc::setMaxRedBits(std::uint8_t max) noexcept
{
    _maxRedBits = max;
}

void
GraphicsPixelFormatDesc::setMaxGreenBits(std::uint8_t max) noexcept
{
    _maxGreenBits = max;
}

void
GraphicsPixelFormatDesc::setMaxBlueBits(std::uint8_t max) noexcept
{
    _maxBlueBits = max;
}

void
GraphicsPixelFormatDesc::setMaxAlphaBits(std::uint8_t max) noexcept
{
    _maxAlphaBits = max;
}

void
GraphicsPixelFormatDesc::setMaxDepthBits(std::uint8_t max) noexcept
{
    _maxDepthBits = max;
}

void
GraphicsPixelFormatDesc::setMaxStencilBits(std::uint8_t max) noexcept
{
    _maxStencilBits = max;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinColorBits() const noexcept
{
    return _minColorBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinRedBits() const noexcept
{
    return _minRedBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinGreenBits() const noexcept
{
    return _minGreenBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinBlueBits() const noexcept
{
    return _minBlueBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinAlphaBits() const noexcept
{
    return _minAlphaBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinDepthBits() const noexcept
{
    return _minDepthBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMinStencilBits() const noexcept
{
    return _minStencilBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxColorBits() const noexcept
{
    return _maxColorBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxRedBits() const noexcept
{
    return _maxRedBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxGreenBits() const noexcept
{
    return _maxGreenBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxBlueBits() const noexcept
{
    return _maxBlueBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxAlphaBits() const noexcept
{
    return _maxAlphaBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxDepthBits() const noexcept
{
    return _maxDepthBits;
}

std::uint8_t
GraphicsPixelFormatDesc::getMaxStencilBits() const noexcept
{
    return _maxStencilBits;
}

GraphicsDeviceDesc::GraphicsDeviceDesc() noexcept
	: _deviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore)
{
}

GraphicsDeviceDesc::~GraphicsDeviceDesc() noexcept
{
}

void 
GraphicsDeviceDesc::setDeviceType(GraphicsDeviceType type) noexcept
{
	_deviceType = type;
}

GraphicsDeviceType 
GraphicsDeviceDesc::getDeviceType() const noexcept
{
	return _deviceType;
}

GraphicsDevice::GraphicsDevice() noexcept
{
}

GraphicsDevice::~GraphicsDevice() noexcept
{
}

GraphicsDevice2::GraphicsDevice2() noexcept
{
}

GraphicsDevice2::~GraphicsDevice2() noexcept
{
}

_NAME_END