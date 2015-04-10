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
#include <ray/window_features.h>
#include <ray/game_server.h>

_NAME_BEGIN

InputFeatures::InputFeatures() noexcept
{
    this->setName(typeid(InputFeatures).name());
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

void
InputFeatures::onActivate() except
{
    WindowPtr window = nullptr;

    auto features = this->getGameServer()->getFeature<WindowFeatures>();
    if (features)
    {
        window = features->getWindow();
        if (window)
        {
            this->_buildInput(window->getWindowHandle());
        }
    }
}

void
InputFeatures::onDeactivate() except
{
    this->setInput(nullptr);
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
InputFeatures::onEvent(const AppEvent& event) noexcept
{
    switch (event.event)
    {
    case AppEvent::LostFocus:
    {
        if (_input)
        {
            _input->releaseCapture();
        }
    }
    break;
    case AppEvent::GetFocus:
    {
        if (_input)
        {
            _input->obtainCapture();
        }
    }
    default:
        break;
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
}

void
InputFeatures::_buildInput(InputDevice::CaptureObject win) noexcept
{
    assert(!_input);

    _inputDevice = std::make_shared<DefaultInputDevice>();
    _inputDevice->capture(win);

    _inputKeyboard = std::make_shared<DefaultInputKeyboard>();
    _inputMouse = std::make_shared<DefaultInputMouse>();

    auto input = std::make_shared<DefaultInput>();
    input->open(_inputDevice);
    input->obtainKeyboardCapture(_inputKeyboard);
    input->obtainMouseCapture(_inputMouse);

    this->setInput(input);
}

_NAME_END