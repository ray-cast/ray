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
#if defined(_BUILD_PLATFORM_ANDROID)
#include <ray/ndk_input_device.h>

_NAME_BEGIN

__ImplementSubInterface(NDKInputDevice, InputDevice, "NDKInputDevice")

NDKInputDevice::NDKInputDevice() noexcept
{
}

void
NDKInputDevice::setCaptureObject(CaptureObject window) noexcept
{
}

CaptureObject
NDKInputDevice::getCaptureObject() const noexcept
{
	return nullptr;
}

bool 
NDKInputDevice::injectTouchMove(int _absx, int _absy, InputButton::Code _id) noexcept
{
	ray::InputEvent inputEvent;
	inputEvent.event = ray::InputEvent::TouchMotionMove;
	inputEvent.motion.x = _absx;
	inputEvent.motion.y = _absy;
	inputEvent.motion.xrel = _absx;
	inputEvent.motion.yrel = _absy;
	inputEvent.motion.state = _button[_id];
	inputEvent.motion.timestamp = ::clock();
	inputEvent.button.button = _id;

	this->postEvent(inputEvent);
}

bool 
NDKInputDevice::injectTouchPress(int _absx, int _absy, InputButton::Code _id) noexcept
{
	ray::InputEvent inputEvent;
	inputEvent.event = ray::InputEvent::TouchMotionDown;
	inputEvent.motion.x = _absx;
	inputEvent.motion.y = _absy;
	inputEvent.motion.xrel = _absx;
	inputEvent.motion.yrel = _absy;
	inputEvent.motion.state = _button[_id] = true;
	inputEvent.motion.timestamp = ::clock();
	inputEvent.button.button = _id;

	this->postEvent(inputEvent);
}

bool 
NDKInputDevice::injectTouchRelease(int _absx, int _absy, InputButton::Code _id) noexcept
{
	ray::InputEvent inputEvent;
	inputEvent.event = ray::InputEvent::TouchMotionUp;
	inputEvent.motion.x = _absx;
	inputEvent.motion.y = _absy;
	inputEvent.motion.xrel = _absx;
	inputEvent.motion.yrel = _absy;
	inputEvent.motion.state = _button[_id] = false;
	inputEvent.motion.timestamp = ::clock();
	inputEvent.button.button = _id;

	this->postEvent(inputEvent);
	return true;
}

bool
NDKInputDevice::injectTouchCancel(InputButton::Code _id) noexcept
{
	ray::InputEvent inputEvent;
	inputEvent.event = ray::InputEvent::TouchMotionCancel;
	inputEvent.motion.x = 0;
	inputEvent.motion.y = 0;
	inputEvent.motion.xrel = 0;
	inputEvent.motion.yrel = 0;
	inputEvent.motion.state = _button[_id] = false;
	inputEvent.motion.timestamp = ::clock();
	inputEvent.button.button = _id;

	this->postEvent(inputEvent);
	return true;
}

bool 
NDKInputDevice::injectCharacter(InputKey::Code _key, std::uint32_t _char) noexcept
{
	ray::InputEvent inputEvent;
	inputEvent.event = ray::InputEvent::Character;
	inputEvent.key.state = true;
	inputEvent.key.timestamp = ::clock();
	//inputEvent.key.keysym.raw = AKeyEvent_getScanCode(event);
	inputEvent.key.keysym.sym = _key;
	inputEvent.key.keysym.unicode = _char;

	this->postEvent(inputEvent);
	return true;
}

bool 
NDKInputDevice::peekEvents(InputEvent& event) noexcept
{
	return false;
}

bool 
NDKInputDevice::pollEvents(InputEvent& event) noexcept
{
	return false;
}

bool 
NDKInputDevice::waitEvents(InputEvent& event) noexcept
{
	return false;
}

bool 
NDKInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
{
	return false;
}

void 
NDKInputDevice::flushEvent() noexcept
{
}

_NAME_END

#endif