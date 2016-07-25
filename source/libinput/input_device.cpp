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
#include <ray/input_device.h>

_NAME_BEGIN

__ImplementSubInterface(DefaultInputDevice, InputDevice, "DefaultInputDevice")

DefaultInputDevice::DefaultInputDevice() noexcept
	: _enableEventPosting(true)
{
}

DefaultInputDevice::~DefaultInputDevice() noexcept
{
	this->clearInputListener();
}

void 
DefaultInputDevice::setCaptureObject(CaptureObject window) noexcept
{
}

CaptureObject 
DefaultInputDevice::getCaptureObject() const noexcept
{
	return nullptr;
}

void
DefaultInputDevice::enableEventPosting(bool enable) noexcept
{
	_enableEventPosting = enable;
}

bool
DefaultInputDevice::enableEventPosting() const noexcept
{
	return _enableEventPosting;
}

void
DefaultInputDevice::addInputListener(InputListenerPtr& listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it == _inputListeners.end())
	{
		listener->onAttach();
		_inputListeners.push_back(listener);
	}
}

void
DefaultInputDevice::addInputListener(InputListenerPtr&& listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it == _inputListeners.end())
	{
		listener->onAttach();
		_inputListeners.push_back(std::move(listener));
	}
}

void
DefaultInputDevice::removeInputListener(InputListenerPtr& listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it != _inputListeners.end())
	{
		listener->onDetach();
		_inputListeners.erase(it);
	}
}

void
DefaultInputDevice::removeInputListener(InputListenerPtr&& listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it != _inputListeners.end())
	{
		listener->onDetach();
		_inputListeners.erase(it);
	}
}

void
DefaultInputDevice::clearInputListener() noexcept
{
	for (auto& listener : _inputListeners)
		listener->onDetach();
	_inputListeners.clear();
}

bool
DefaultInputDevice::sendEvent(const InputEvent& event) noexcept
{
	try
	{
		for (auto& it : _inputListeners)
			it->onInputEvent(event);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool
DefaultInputDevice::postEvent(const InputEvent& event) noexcept
{
	if (_enableEventPosting)
	{
		_mutex.lock();
		_events.push(event);
		_mutex.unlock();

		_dispose.notify_one();
	}

	return true;
}

bool
DefaultInputDevice::peekEvents(InputEvent& event) noexcept
{
	return true;
}

bool
DefaultInputDevice::pollEvents(InputEvent& event) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (lock)
	{
		if (!_events.empty())
		{
			event = _events.front();
			_events.pop();
			return true;
		}
	}

	return false;
}

bool
DefaultInputDevice::waitEvents(InputEvent& event) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait(lock);
	}

	return this->pollEvents(event);
}

bool
DefaultInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait_for(lock, std::chrono::milliseconds(timeout));
	}

	return this->pollEvents(event);
}

void
DefaultInputDevice::flushEvent() noexcept
{
	_mutex.lock();
	_events = std::queue<InputEvent>();
	_mutex.unlock();
}

InputDevicePtr 
DefaultInputDevice::clone() const noexcept
{
	return std::make_shared<DefaultInputDevice>();
}

_NAME_END