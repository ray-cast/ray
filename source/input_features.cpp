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
#include <ray/input_features.h>
#include <ray/input_device.h>
#include <ray/input_keyboard.h>
#include <ray/input_mouse.h>
#include <ray/fstream.h>
#include <ray/xmlreader.h>

_NAME_BEGIN

__ImplementSubClass(InputFeatures, GameFeature, "Input")

InputFeatures::InputFeatures() noexcept
{
	_input = std::make_shared<DefaultInput>();

#if !defined(__ANDROID__)
	auto inputDevice = std::make_shared<DefaultInputDevice>();
	auto inputKeyboard = std::make_shared<DefaultInputKeyboard>();
	auto inputMouse = std::make_shared<DefaultInputMouse>();
	_input->open(inputDevice);
	_input->obtainKeyboardCapture(inputKeyboard);
	_input->obtainMouseCapture(inputMouse);
#endif
}

InputFeatures::~InputFeatures() noexcept
{
}

void
InputFeatures::setInput(InputPtr input) noexcept
{
	assert(input);
	_input = input;
}

InputPtr
InputFeatures::getInput() const noexcept
{
	return _input;
}

void
InputFeatures::onActivate() except
{
	assert(_input);
	_input->obtainCapture();
}

void
InputFeatures::onDeactivate() except
{
	assert(_input);
	_input->releaseCapture();
}

void
InputFeatures::onFrameBegin() noexcept
{
	assert(_input);
	_input->updateBegin();
	_input->update();
}

void
InputFeatures::onFrameEnd() noexcept
{
	assert(_input);
	_input->updateEnd();
}

void
InputFeatures::onReset() noexcept
{
	assert(_input);
	_input->reset();
}

_NAME_END

#endif