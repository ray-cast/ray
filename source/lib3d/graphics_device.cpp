// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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

GraphicsDeviceDesc::GraphicsDeviceDesc() noexcept
	: _hwnd(nullptr)
	, _deviceType(GraphicsDeviceType::GraphicsDeviceTypeUndefined)
{
}

GraphicsDeviceDesc::~GraphicsDeviceDesc() noexcept
{
}

void 
GraphicsDeviceDesc::setWindHandle(WindHandle hwnd) noexcept
{
	_hwnd = hwnd;
}

WindHandle 
GraphicsDeviceDesc::getWindHandle() const noexcept
{
	return _hwnd;
}

void
GraphicsDeviceDesc::setDeviceType(GraphicsDeviceType deviceType) noexcept
{
	_deviceType = deviceType;
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