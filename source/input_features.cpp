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
#include <ray/input_features.h>
#include <ray/input_device.h>
#include <ray/input_keyboard.h>
#include <ray/input_mouse.h>

_NAME_BEGIN

__ImplementSubClass(InputFeatures, GameFeatures)

InputFeatures::InputFeatures() noexcept
{
	auto inputDevice = std::make_shared<DefaultInputDevice>();
	auto inputKeyboard = std::make_shared<DefaultInputKeyboard>();
	auto inputMouse = std::make_shared<DefaultInputMouse>();

	_input = std::make_shared<DefaultInput>();
	_input->open(inputDevice);
	_input->addInputListener(this);
	_input->obtainKeyboardCapture(inputKeyboard);
	_input->obtainMouseCapture(inputMouse);
}

InputFeatures::~InputFeatures() noexcept
{
}

void
InputFeatures::setInput(InputPtr input) noexcept
{
	if (_input != input)
	{
		if (input)
			input->addInputListener(this);

		if (_input)
			_input->removeInputListener(this);

		_input = input;
	}
}

InputPtr
InputFeatures::getInput() const noexcept
{
	return _input;
}

GameFeaturePtr
InputFeatures::clone() const noexcept
{
	auto features = std::make_shared<InputFeatures>();
	if (_input)
		features->setInput(_input->clone());
	return features;
}

void
InputFeatures::onActivate() except
{
	if (_input)
		_input->obtainCapture();
}

void
InputFeatures::onDeactivate() except
{
	if (_input)
		_input->releaseCapture();
}

void
InputFeatures::onFrameBegin() noexcept
{
	if (_input)
	{
		_input->updateBegin();
		_input->update();
	}
}

void
InputFeatures::onFrameEnd() noexcept
{
	if (_input)
	{
		_input->updateEnd();
	}
}

void
InputFeatures::onMessage(const GameMessage& event) noexcept
{
	if (event.event()->code() == typeid(LostFocusEvent).hash_code())
	{
		if (_input)
		{
			_input->releaseCapture();
		}
	}
	else if (event.event()->code() == typeid(GetFocusEvent).hash_code())
	{
		if (_input)
		{
			_input->obtainCapture();
		}
	}
}

void
InputFeatures::onReset() noexcept
{
	if (_input)
	{
		_input->reset();
	}
}

void
InputFeatures::onInputEvent(const InputEvent& event) noexcept
{
	switch (event.event)
	{
	case InputEvent::MouseMotion:
	{
		MouseMotion motion;
		motion.mouse.x = event.motion.x;
		motion.mouse.y = event.motion.y;
		motion.mouse.xrel = event.motion.xrel;
		motion.mouse.yrel = event.motion.yrel;

		this->sendMessage(&motion);
	}
	break;
	case InputEvent::MouseButtonDown:
	{
		MouseButtonDown button;
		button.mouse.button = event.button.button;
		button.mouse.x = event.button.x;
		button.mouse.y = event.button.y;

		this->sendMessage(&button);
	}
	break;
	case InputEvent::MouseButtonUp:
	{
		MouseButtonUp button;
		button.mouse.button = event.button.button;
		button.mouse.x = event.button.x;
		button.mouse.y = event.button.y;

		this->sendMessage(&button);
	}
	break;
	default:
		break;
	}
}

_NAME_END