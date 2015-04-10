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
#if defined(_BUILD_PLATFORM_SDL2)
#include <ray/sdl_input_device.h>

#include <SDL.h>

_NAME_BEGIN

SDLInputDevice::SDLInputDevice() noexcept
    : _window(0)
{
}

void
SDLInputDevice::capture(CaptureObject window) noexcept
{
    _window = window;
}

void
SDLInputDevice::update() noexcept
{
    SDL_Event event;
    InputEvent inputEvent;

    while (::SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            inputEvent.event = InputEvent::AppQuit;
            inputEvent.quit.type = InputEvent::AppQuit;
            inputEvent.quit.timestamp = event.quit.timestamp;
            inputEvent.quit.code = 0;

            this->postEvent(inputEvent);
        }

        SDL_Window* wx = ::SDL_GetWindowFromID(event.window.windowID);

        if (wx != _window)
        {
            continue;
        }

        switch (event.type)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
        }
        break;
        case SDL_KEYDOWN:
        {
        }
        break;
        case SDL_KEYUP:
        {
        }
        break;
        case SDL_MOUSEMOTION:
        {
            inputEvent.event = InputEvent::MouseMotion;
            inputEvent.motion.x = event.motion.x;
            inputEvent.motion.y = event.motion.y;
            inputEvent.motion.xrel = event.motion.xrel;
            inputEvent.motion.yrel = event.motion.yrel;
            inputEvent.motion.state = event.motion.state == SDL_PRESSED ? true : false;
            inputEvent.motion.timestamp = event.motion.timestamp;
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                inputEvent.button.button = InputButton::MOUSE0;
                break;
            case SDL_BUTTON_MIDDLE:
                inputEvent.button.button = InputButton::MOUSE1;
                break;
            case SDL_BUTTON_RIGHT:
                inputEvent.button.button = InputButton::MOUSE2;
                break;
            default:
                break;
            }

            this->postEvent(inputEvent);
        }
        break;
        case SDL_MOUSEBUTTONDOWN:
        {
            inputEvent.event = InputEvent::MouseButtonDown;
            inputEvent.button.clicks = true;
            inputEvent.button.x = event.button.x;
            inputEvent.button.y = event.button.y;
            inputEvent.button.timestamp = event.button.timestamp;
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                inputEvent.button.button = InputButton::MOUSE0;
                break;
            case SDL_BUTTON_MIDDLE:
                inputEvent.button.button = InputButton::MOUSE1;
                break;
            case SDL_BUTTON_RIGHT:
                inputEvent.button.button = InputButton::MOUSE2;
                break;
            default:
                break;
            }

            this->postEvent(inputEvent);
        }
        break;
        case SDL_MOUSEBUTTONUP:
        {
            inputEvent.event = InputEvent::MouseButtonUp;
            inputEvent.button.clicks = false;
            inputEvent.button.x = event.button.x;
            inputEvent.button.y = event.button.y;
            inputEvent.button.timestamp = event.button.timestamp;
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                inputEvent.button.button = InputButton::MOUSE0;
                break;
            case SDL_BUTTON_MIDDLE:
                inputEvent.button.button = InputButton::MOUSE1;
                break;
            case SDL_BUTTON_RIGHT:
                inputEvent.button.button = InputButton::MOUSE2;
                break;
            default:
                break;
            }

            this->postEvent(inputEvent);
        }
        break;
        }
    }
}

void
SDLInputDevice::peekEvents(InputEvent& event) noexcept
{
    this->update();
    InputDevice::peekEvents(event);
}

bool
SDLInputDevice::pollEvents(InputEvent& event) noexcept
{
    this->update();
    return InputDevice::pollEvents(event);
}

bool
SDLInputDevice::waitEvents(InputEvent& event) noexcept
{
    this->update();
    return InputDevice::waitEvents(event);
}

bool
SDLInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
{
    this->update();
    return InputDevice::waitEvents(event, timeout);
}

void
SDLInputDevice::flushEvent() noexcept
{
    this->update();
    InputDevice::flushEvent();
}

/*
void
SDLKeyboardDevice::setupKeyNames() noexcept
{
    mapSymbol(SDL_SCANCODE_A, InputKey::SCANCODE_A, "a");
    mapSymbol(SDL_SCANCODE_B, InputKey::SCANCODE_B, "b");
    mapSymbol(SDL_SCANCODE_C, InputKey::SCANCODE_C, "c");
    mapSymbol(SDL_SCANCODE_D, InputKey::SCANCODE_D, "d");
    mapSymbol(SDL_SCANCODE_E, InputKey::SCANCODE_E, "e");
    mapSymbol(SDL_SCANCODE_F, InputKey::SCANCODE_F, "f");
    mapSymbol(SDL_SCANCODE_G, InputKey::SCANCODE_G, "g");
    mapSymbol(SDL_SCANCODE_H, InputKey::SCANCODE_H, "h");
    mapSymbol(SDL_SCANCODE_I, InputKey::SCANCODE_I, "i");
    mapSymbol(SDL_SCANCODE_J, InputKey::SCANCODE_J, "j");
    mapSymbol(SDL_SCANCODE_K, InputKey::SCANCODE_K, "k");
    mapSymbol(SDL_SCANCODE_L, InputKey::SCANCODE_L, "l");
    mapSymbol(SDL_SCANCODE_M, InputKey::SCANCODE_M, "m");
    mapSymbol(SDL_SCANCODE_N, InputKey::SCANCODE_N, "n");
    mapSymbol(SDL_SCANCODE_O, InputKey::SCANCODE_O, "o");
    mapSymbol(SDL_SCANCODE_P, InputKey::SCANCODE_P, "p");
    mapSymbol(SDL_SCANCODE_Q, InputKey::SCANCODE_Q, "q");
    mapSymbol(SDL_SCANCODE_R, InputKey::SCANCODE_R, "r");
    mapSymbol(SDL_SCANCODE_S, InputKey::SCANCODE_S, "s");
    mapSymbol(SDL_SCANCODE_T, InputKey::SCANCODE_T, "t");
    mapSymbol(SDL_SCANCODE_U, InputKey::SCANCODE_U, "u");
    mapSymbol(SDL_SCANCODE_V, InputKey::SCANCODE_V, "v");
    mapSymbol(SDL_SCANCODE_W, InputKey::SCANCODE_W, "w");
    mapSymbol(SDL_SCANCODE_X, InputKey::SCANCODE_X, "x");
    mapSymbol(SDL_SCANCODE_Y, InputKey::SCANCODE_Y, "y");
    mapSymbol(SDL_SCANCODE_Z, InputKey::SCANCODE_Z, "z");

    mapSymbol(SDL_SCANCODE_0, InputKey::SCANCODE_0, "0");
    mapSymbol(SDL_SCANCODE_1, InputKey::SCANCODE_1, "1");
    mapSymbol(SDL_SCANCODE_2, InputKey::SCANCODE_2, "2");
    mapSymbol(SDL_SCANCODE_3, InputKey::SCANCODE_3, "3");
    mapSymbol(SDL_SCANCODE_4, InputKey::SCANCODE_4, "4");
    mapSymbol(SDL_SCANCODE_5, InputKey::SCANCODE_5, "5");
    mapSymbol(SDL_SCANCODE_6, InputKey::SCANCODE_6, "6");
    mapSymbol(SDL_SCANCODE_7, InputKey::SCANCODE_7, "7");
    mapSymbol(SDL_SCANCODE_8, InputKey::SCANCODE_8, "8");
    mapSymbol(SDL_SCANCODE_9, InputKey::SCANCODE_9, "9");

    mapSymbol(SDL_SCANCODE_RETURN, InputKey::SCANCODE_ENTER, "enter");
    mapSymbol(SDL_SCANCODE_ESCAPE, InputKey::SCANCODE_ESCAPE, "escape");
    mapSymbol(SDL_SCANCODE_BACKSPACE, InputKey::SCANCODE_BACKSPACE, "backspace");
    mapSymbol(SDL_SCANCODE_TAB, InputKey::SCANCODE_TAB, "tab");
    mapSymbol(SDL_SCANCODE_SPACE, InputKey::SCANCODE_SPACE, "space");

    mapSymbol(SDL_SCANCODE_MINUS, InputKey::SCANCODE_MINUS, "minus");
    mapSymbol(SDL_SCANCODE_EQUALS, InputKey::SCANCODE_EQUALS, "equals");
    mapSymbol(SDL_SCANCODE_LEFTBRACKET, InputKey::SCANCODE_LBRACKET, "lbracket");
    mapSymbol(SDL_SCANCODE_RIGHTBRACKET, InputKey::SCANCODE_RBRACKET, "rbracket");
    mapSymbol(SDL_SCANCODE_BACKSLASH, InputKey::SCANCODE_BACKSLASH, "backslash");

    mapSymbol(SDL_SCANCODE_GRAVE, InputKey::SCANCODE_GRAVE, "grave");
    mapSymbol(SDL_SCANCODE_COMMA, InputKey::SCANCODE_COMMA, "comma");
    mapSymbol(SDL_SCANCODE_PERIOD, InputKey::SCANCODE_PERIOD, "period");
    mapSymbol(SDL_SCANCODE_SLASH, InputKey::SCANCODE_SLASH, "slash");

    mapSymbol(SDL_SCANCODE_F1, InputKey::SCANCODE_F1, "f1");
    mapSymbol(SDL_SCANCODE_F2, InputKey::SCANCODE_F2, "f2");
    mapSymbol(SDL_SCANCODE_F3, InputKey::SCANCODE_F3, "f3");
    mapSymbol(SDL_SCANCODE_F4, InputKey::SCANCODE_F4, "f4");
    mapSymbol(SDL_SCANCODE_F5, InputKey::SCANCODE_F5, "f5");
    mapSymbol(SDL_SCANCODE_F6, InputKey::SCANCODE_F6, "f6");
    mapSymbol(SDL_SCANCODE_F7, InputKey::SCANCODE_F7, "f7");
    mapSymbol(SDL_SCANCODE_F8, InputKey::SCANCODE_F8, "f8");
    mapSymbol(SDL_SCANCODE_F9, InputKey::SCANCODE_F9, "f9");
    mapSymbol(SDL_SCANCODE_F10, InputKey::SCANCODE_F10, "f10");
    mapSymbol(SDL_SCANCODE_F11, InputKey::SCANCODE_F11, "f11");
    mapSymbol(SDL_SCANCODE_F12, InputKey::SCANCODE_F12, "f12");
    mapSymbol(SDL_SCANCODE_F13, InputKey::SCANCODE_F13, "f13");
    mapSymbol(SDL_SCANCODE_F14, InputKey::SCANCODE_F14, "f14");
    mapSymbol(SDL_SCANCODE_F15, InputKey::SCANCODE_F15, "f15");

    mapSymbol(SDL_SCANCODE_PRINTSCREEN, InputKey::SCANCODE_PRINT, "print");
    mapSymbol(SDL_SCANCODE_NUMLOCKCLEAR, InputKey::SCANCODE_NUMLOCK, "numlock", InputSymbol::Toggle, InputModifier::mod_modifiers);
    mapSymbol(SDL_SCANCODE_CAPSLOCK, InputKey::SCANCODE_CAPSLOCK, "capslock", InputSymbol::Toggle, InputModifier::mod_capslock);
    mapSymbol(SDL_SCANCODE_SCROLLLOCK, InputKey::SCANCODE_SCROLLLOCK, "scrolllock", InputSymbol::Toggle, InputModifier::mod_scrolllock);

    mapSymbol(SDL_SCANCODE_INSERT, InputKey::SCANCODE_INSERT, "insert");
    mapSymbol(SDL_SCANCODE_DELETE, InputKey::SCANCODE_DELETE, "delete");
    mapSymbol(SDL_SCANCODE_HOME, InputKey::SCANCODE_HOME, "home");
    mapSymbol(SDL_SCANCODE_END, InputKey::SCANCODE_END, "end");
    mapSymbol(SDL_SCANCODE_PAGEUP, InputKey::SCANCODE_PGUP, "pgup");
    mapSymbol(SDL_SCANCODE_PAGEDOWN, InputKey::SCANCODE_PGDN, "pgdn");
    mapSymbol(SDL_SCANCODE_UP, InputKey::SCANCODE_UP, "up");
    mapSymbol(SDL_SCANCODE_DOWN, InputKey::SCANCODE_DOWN, "down");
    mapSymbol(SDL_SCANCODE_RIGHT, InputKey::SCANCODE_RIGHT, "right");
    mapSymbol(SDL_SCANCODE_LEFT, InputKey::SCANCODE_LEFT, "left");

    mapSymbol(SDL_SCANCODE_KP_PERIOD, InputKey::SCANCODE_NP_PERIOD, "np_period");
    mapSymbol(SDL_SCANCODE_KP_DIVIDE, InputKey::SCANCODE_NP_DIVIDE, "np_divide");
    mapSymbol(SDL_SCANCODE_KP_MULTIPLY, InputKey::SCANCODE_NP_MULTIPLY, "np_multiply");
    mapSymbol(SDL_SCANCODE_KP_MINUS, InputKey::SCANCODE_NP_SUBSTRACT, "np_subtract");
    mapSymbol(SDL_SCANCODE_KP_PLUS, InputKey::SCANCODE_NP_ADD, "np_add");
    mapSymbol(SDL_SCANCODE_KP_ENTER, InputKey::SCANCODE_NP_ENTER, "np_enter");
    mapSymbol(SDL_SCANCODE_KP_EQUALS, InputKey::SCANCODE_NP_ENTER, "np_enter");
    mapSymbol(SDL_SCANCODE_KP_0, InputKey::SCANCODE_NP_0, "np_0");
    mapSymbol(SDL_SCANCODE_KP_1, InputKey::SCANCODE_NP_1, "np_1");
    mapSymbol(SDL_SCANCODE_KP_2, InputKey::SCANCODE_NP_2, "np_2");
    mapSymbol(SDL_SCANCODE_KP_3, InputKey::SCANCODE_NP_3, "np_3");
    mapSymbol(SDL_SCANCODE_KP_4, InputKey::SCANCODE_NP_4, "np_4");
    mapSymbol(SDL_SCANCODE_KP_5, InputKey::SCANCODE_NP_5, "np_5");
    mapSymbol(SDL_SCANCODE_KP_6, InputKey::SCANCODE_NP_6, "np_6");
    mapSymbol(SDL_SCANCODE_KP_7, InputKey::SCANCODE_NP_7, "np_7");
    mapSymbol(SDL_SCANCODE_KP_8, InputKey::SCANCODE_NP_8, "np_8");
    mapSymbol(SDL_SCANCODE_KP_9, InputKey::SCANCODE_NP_9, "np_9");

    mapSymbol(SDL_SCANCODE_APPLICATION, InputKey::SCANCODE_APPS, "apps");
    mapSymbol(SDL_SCANCODE_CLEAR, InputKey::SCANCODE_CLEAR, "clear");
    mapSymbol(SDL_SCANCODE_PAUSE, InputKey::SCANCODE_PAUSE, "pause");
    mapSymbol(SDL_SCANCODE_SEMICOLON, InputKey::SCANCODE_SEMICOLON, "semicolon");

    mapSymbol(SDL_SCANCODE_RSHIFT, InputKey::SCANCODE_RSHIFT, "rshift", InputSymbol::Button, InputModifier::mod_rshift);
    mapSymbol(SDL_SCANCODE_LSHIFT, InputKey::SCANCODE_LSHIFT, "lshift", InputSymbol::Button, InputModifier::mod_lshift);
    mapSymbol(SDL_SCANCODE_RCTRL, InputKey::SCANCODE_RCTRL, "rctrl", InputSymbol::Button, InputModifier::mod_rctrl);
    mapSymbol(SDL_SCANCODE_LCTRL, InputKey::SCANCODE_LCTRL, "lctrl", InputSymbol::Button, InputModifier::mod_lctrl);
    mapSymbol(SDL_SCANCODE_RALT, InputKey::SCANCODE_RALT, "ralt", InputSymbol::Button, InputModifier::mod_ralt);
    mapSymbol(SDL_SCANCODE_LALT, InputKey::SCANCODE_LALT, "lalt", InputSymbol::Button, InputModifier::mod_lalt);
    mapSymbol(SDL_SCANCODE_RGUI, InputKey::SCANCODE_LWIN, "lwin", InputSymbol::Button, InputModifier::mod_lwin);
    mapSymbol(SDL_SCANCODE_LGUI, InputKey::SCANCODE_RWIN, "rwin", InputSymbol::Button, InputModifier::mod_rwin);
}
*/

_NAME_END

#endif