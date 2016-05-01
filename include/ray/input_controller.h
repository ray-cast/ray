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
#ifndef _H_INPUT_CONTROLLER_H_
#define _H_INPUT_CONTROLLER_H_

#include <ray/input_event.h>

_NAME_BEGIN

class EXPORT InputListener : public rtti::Interface
{
	__DeclareSubInterface(InputListener, rtti::Interface)
public:
	InputListener() noexcept;
	virtual ~InputListener() noexcept;

	virtual void onAttach() noexcept;
	virtual void onDetach() noexcept;

	virtual void onInputEvent(const InputEvent& event) except = 0;

private:
	InputListener(const InputListener&) noexcept = delete;
	InputListener& operator=(const InputListener&)noexcept = delete;
};

class EXPORT InputController : public InputListener
{
	__DeclareSubInterface(InputController, InputListener)
public:
	InputController() noexcept;
	virtual ~InputController() noexcept;

	virtual void obtainCapture() noexcept;
	virtual void releaseCapture() noexcept;
	virtual bool capture() const noexcept;

	virtual void onFrameBegin() noexcept;
	virtual void onFrameEnd() noexcept;

	virtual void onReset() noexcept;

	virtual void onObtainCapture() noexcept;
	virtual void onReleaseCapture() noexcept;

private:
	InputController(const InputController&) noexcept = delete;
	InputController& operator=(const InputController&) noexcept = delete;

private:

	bool _capture;
};

_NAME_END

#endif