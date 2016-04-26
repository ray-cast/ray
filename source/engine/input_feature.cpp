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
#include <ray/input_device.h>
#include <ray/input_keyboard.h>
#include <ray/input_mouse.h>
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
		:_input(input)
	{
	}

	void onInputEvent(const InputEvent& event) noexcept
	{
		auto message = std::make_shared<InputMessage>();
		message->setEvent(event);

		_input.sendMessage(message);
	}

private:
	InputEventListener(const InputEventListener&) noexcept = delete;
	InputEventListener& operator=(const InputEventListener&)noexcept = delete;

private:
	InputFeature& _input;
};

InputFeature::InputFeature() noexcept
{
	auto inputDevice = std::make_shared<DefaultInputDevice>();

	_input = std::make_shared<DefaultInput>();
	_input->open(inputDevice);
	_input->addInputListener(std::make_shared<InputEventListener>(*this));

#if defined(ToplevelInputKeyboard)
	auto inputKeyboard = std::make_shared<DefaultInputKeyboard>();
	_input->obtainKeyboardCapture(inputKeyboard);
#endif

#if defined(ToplevelInputMouse)
	auto inputMouse = std::make_shared<DefaultInputMouse>();
	_input->obtainMouseCapture(inputMouse);
#endif
}

InputFeature::InputFeature(CaptureObject hwnd) noexcept
{
	auto inputDevice = std::make_shared<DefaultInputDevice>();

	_input = std::make_shared<DefaultInput>();
	_input->open(inputDevice);
	_input->addInputListener(std::make_shared<InputEventListener>(*this));
	_input->setCaptureObject(hwnd);

#if defined(ToplevelInputKeyboard)
	auto inputKeyboard = std::make_shared<DefaultInputKeyboard>();
	_input->obtainKeyboardCapture(inputKeyboard);
#endif

#if defined(ToplevelInputMouse)
	auto inputMouse = std::make_shared<DefaultInputMouse>();
	_input->obtainMouseCapture(inputMouse);
#endif
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

void
InputFeature::onActivate() except
{
	assert(_input);
	_input->obtainCapture();
}

void
InputFeature::onDeactivate() noexcept
{
	assert(_input);
	_input->releaseCapture();
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

void
InputFeature::onMessage(const MessagePtr& message) except
{
	assert(_input);

	if (message->isInstanceOf<InputMessage>())
	{
		auto inputMessage = message->downcast<InputMessage>();
		_input->sendInputEvent(inputMessage->getEvent());
	}
}

_NAME_END

#endif