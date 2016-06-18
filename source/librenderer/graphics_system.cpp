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
#include <ray/graphics_system.h>

#if defined(_BUILD_OPENGL_CORE)
#	include "OpenGL Core/ogl_device.h"
#endif
#if defined(_BUILD_OPENGL_ES2)
#	include "OpenGL ES2/egl2_device.h"
#endif
#if defined(_BUILD_OPENGL_ES3)
#	include "OpenGL ES3/egl3_device.h"
#endif
#if defined(_BUILD_VULKAN)
#   include "Vulkan/vk_system.h"
#	include "Vulkan/vk_device.h"
#endif

_NAME_BEGIN

__ImplementSingleton(GraphicsSystem)

GraphicsSystem::GraphicsSystem() noexcept
	: _deviceType(GraphicsDeviceType::GraphicsDeviceTypeMaxEnum)
	, _debugMode(false)
{
}

GraphicsSystem::~GraphicsSystem() noexcept
{
	this->close();
}

bool 
GraphicsSystem::open(GraphicsDeviceType type, bool debugControl) noexcept
{
	assert(_deviceType == GraphicsDeviceType::GraphicsDeviceTypeMaxEnum);
	assert(type >= GraphicsDeviceType::GraphicsDeviceTypeBeginRange && type <= GraphicsDeviceType::GraphicsDeviceTypeEndRange);

	_debugMode = debugControl;

	if (type == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
	{
#if defined(_BUILD_VULKAN)
		if (!VulkanSystem::instance()->open())
			return false;
#endif
	}

	_deviceType = type;

	return true;
}

void 
GraphicsSystem::close() noexcept
{
	if (_deviceType != GraphicsDeviceType::GraphicsDeviceTypeMaxEnum)
	{
		if (_debugMode)
		{
			this->enableDebugControl(false);
			_debugMode = false;
		}

		for (auto& it : _devices)
		{
			it.reset();
		}

		if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		{
			VulkanSystem::instance()->close();
		}

		_deviceType = GraphicsDeviceType::GraphicsDeviceTypeMaxEnum;
	}
}

void
GraphicsSystem::enableDebugControl(bool enable) noexcept
{
	assert(_deviceType >= GraphicsDeviceType::GraphicsDeviceTypeBeginRange && _deviceType <= GraphicsDeviceType::GraphicsDeviceTypeEndRange);

	if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
	{
		if (enable)
			VulkanSystem::instance()->startDebugControl();
		else
			VulkanSystem::instance()->stopDebugControl();		
	}
	else if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL)
	{
		for (auto& it : _devices)
		{
			auto device = it.lock();
			if (device)
			{
				if (device->isInstanceOf<OGLDevice>())
					device->downcast<OGLDevice>()->enableDebugControl(enable);
				else if (device->isInstanceOf<EGL2Device>())
					device->downcast<EGL2Device>()->enableDebugControl(enable);
				else if (device->isInstanceOf<EGL3Device>())
					device->downcast<EGL3Device>()->enableDebugControl(enable);
			}
		}
	}

	_debugMode = true;
}

bool
GraphicsSystem::enableDebugControl() const noexcept
{
	return _debugMode;
}

GraphicsDevicePtr
GraphicsSystem::createDevice() noexcept
{
	assert(_deviceType >= GraphicsDeviceType::GraphicsDeviceTypeBeginRange && _deviceType <= GraphicsDeviceType::GraphicsDeviceTypeEndRange);

	GraphicsDeviceDesc deviceDesc;
	deviceDesc.setDeviceType(_deviceType);

#if defined(_BUILD_OPENGL_CORE)
	if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL)
	{
		auto device = std::make_shared<OGLDevice>();
		if (device->setup(deviceDesc))
		{
			_devices.push_back(device);
			return device;
		}

		return nullptr;
	}

#endif
#if defined(_BUILD_OPENGL_ES2)
	if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2)
	{
		auto device = std::make_shared<EGL2Device>();
		if (device->setup(deviceDesc))
		{
			_devices.push_back(device);
			return device;
		}

		return nullptr;
	}
#endif
#if defined(_BUILD_OPENGL_ES3)
	if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		_deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31)
	{
		auto device = std::make_shared<EGL3Device>();
		if (device->setup(deviceDesc))
		{
			_devices.push_back(device);
			return device;
		}

		return nullptr;
	}
#endif
#if defined(_BUILD_VULKAN)
	if (_deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
	{
		deviceDesc.setPhysicalDevice(VulkanSystem::instance()->getPhysicalDevices().front());

		auto device = std::make_shared<VulkanDevice>();
		if (device->setup(deviceDesc))
		{
			_devices.push_back(device);
			return device;
		}

		return nullptr;
	}
#endif
	return nullptr;
}

_NAME_END