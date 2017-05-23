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
#ifndef _H_INPUT_FEATURES_H_
#define _H_INPUT_FEATURES_H_

#include <ray/game_features.h>
#include <ray/input.h>

_NAME_BEGIN

class EXPORT InputMessage final : public Message
{
	__DeclareSubClass(InputMessage, Message)
public:
	InputMessage() noexcept;
	~InputMessage() noexcept;

	void setEvent(const InputEvent& event) noexcept;
	const InputEvent& getEvent() const noexcept;

private:
	InputEvent _event;
};

class EXPORT InputFeature final : public GameFeature
{
	__DeclareSubClass(InputFeature, GameFeature)
public:
	InputFeature() noexcept;
	InputFeature(CaptureObject hwnd) noexcept;
	virtual ~InputFeature() noexcept;

	void setInput(InputPtr input) noexcept;
	InputPtr getInput() const noexcept;

	bool sendInputEvent(const InputEvent& event) noexcept;
	bool postInputEvent(const InputEvent& event) noexcept;

private:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onReset() noexcept;

	virtual void onFrameBegin() noexcept;
	virtual void onFrameEnd() noexcept;

private:
	InputFeature(const InputFeature&) = delete;
	InputFeature& operator=(const InputFeature&) = delete;

private:
	InputPtr _input;
	CaptureObject _window;
};

_NAME_END

#endif