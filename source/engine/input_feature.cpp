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
#if defined(_BUILD_INPUT)
#include <ray/input_feature.h>
#include <ray/input.h>

_NAME_BEGIN

__ImplementSubClass(InputMessage, Message, "InputMessage")
__ImplementSubClass(InputFeature, GameFeature, "Input")

InputMessage::InputMessage() noexcept
{
}

InputMessage::~InputMessage() noexcept
{
}

void
InputMessage::setEvent(const InputEvent& event) noexcept
{
	_event = event;
}

const InputEvent&
InputMessage::getEvent() const noexcept
{
	return _event;
}

class InputEventListener : public InputListener
{
public:
	InputEventListener(InputFeature& input)
		: _input(input)
		, _message(std::make_shared<InputMessage>())
	{
	}

	void onInputEvent(const InputEvent& event) noexcept
	{
		_message->setEvent(event);
		_input.sendMessage(_message);
	}

private:
	InputEventListener(const InputEventListener&) noexcept = delete;
	InputEventListener& operator=(const InputEventListener&)noexcept = delete;

private:
	InputFeature& _input;
	std::shared_ptr<InputMessage> _message;
};

InputFeature::InputFeature() noexcept
	: _window(0)
{
}

InputFeature::InputFeature(CaptureObject hwnd) noexcept
	: _window(hwnd)
{
}

InputFeature::~InputFeature() noexcept
{
}

void
InputFeature::setInput(InputPtr input) noexcept
{
	assert(input);
	_input = input;
}

InputPtr
InputFeature::getInput() const noexcept
{
	return _input;
}

bool
InputFeature::sendInputEvent(const InputEvent& event) noexcept
{
	if (_input)
		return _input->sendInputEvent(event);
	return false;
}

bool 
InputFeature::postInputEvent(const InputEvent& event) noexcept
{
	if (_input)
		return _input->postInputEvent(event);
	return false;
}

void
InputFeature::onActivate() noexcept
{
	_input = std::make_shared<DefaultInput>();
	_input->open(nullptr);
	_input->addInputListener(std::make_shared<InputEventListener>(*this));
	_input->setCaptureObject(_window);
	_input->obtainCapture();
}

void
InputFeature::onDeactivate() noexcept
{
	_input.reset();
}

void
InputFeature::onFrameBegin() noexcept
{
	assert(_input);
	_input->updateBegin();
	_input->update();
}

void
InputFeature::onFrameEnd() noexcept
{
	assert(_input);
	_input->updateEnd();
}

void
InputFeature::onReset() noexcept
{
	assert(_input);
	_input->reset();
}

_NAME_END

#endif