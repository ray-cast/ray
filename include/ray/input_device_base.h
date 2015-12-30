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
#ifndef _H_INPUT_DEVICE_BASE_H_
#define _H_INPUT_DEVICE_BASE_H_

#include <ray/input_controller.h>

_NAME_BEGIN

class EXPORT InputDevice : public rtti::Interface
{
	__DeclareSubInterface(DefaultInputDevice, rtti::Interface)
public:
	InputDevice() noexcept;
	virtual ~InputDevice() noexcept;

	virtual void setCaptureObject(CaptureObject window) noexcept = 0;
	virtual CaptureObject getCaptureObject() const noexcept = 0;

	virtual void addInputListener(InputListenerPtr listener) noexcept = 0;
	virtual void removeInputListener(InputListenerPtr listener) noexcept = 0;
	virtual void clearInputListener() noexcept = 0;

	virtual void enableEventPosting(bool enable) noexcept = 0;
	virtual bool enableEventPosting() const noexcept = 0;

	virtual void sendEvent(const InputEvent& event) noexcept = 0;
	virtual void postEvent(const InputEvent& event) noexcept = 0;

	virtual bool peekEvents(InputEvent& event) noexcept = 0;
	virtual bool pollEvents(InputEvent& event) noexcept = 0;
	virtual bool waitEvents(InputEvent& event) noexcept = 0;
	virtual bool waitEvents(InputEvent& event, int timeout) noexcept = 0;
	virtual void flushEvent() noexcept = 0;

	virtual InputDevicePtr clone() const noexcept = 0;

private:
	InputDevice(const InputDevice&) noexcept = delete;
	InputDevice& operator=(const InputDevice&) noexcept = delete;
};

_NAME_END

#endif