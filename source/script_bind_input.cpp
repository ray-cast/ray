// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/script_bind_input.h>
#include <angelscript.h>

_NAME_BEGIN

ScriptBindInput::ScriptBindInput() noexcept
    : _input(nullptr)
{
}

ScriptBindInput::~ScriptBindInput() noexcept
{
}

void
ScriptBindInput::setup(asIScriptEngine* _engine) noexcept
{
    int r = 0;

    r = _engine->RegisterEnum("keycode"); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "a", InputKey::A); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "b", InputKey::B); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "c", InputKey::C); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "d", InputKey::D); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "e", InputKey::E); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f", InputKey::F); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "g", InputKey::G); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "h", InputKey::H); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "i", InputKey::I); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "j", InputKey::J); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "k", InputKey::K); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "l", InputKey::L); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "m", InputKey::M); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "n", InputKey::N); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "o", InputKey::O); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "p", InputKey::P); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "q", InputKey::Q); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "r", InputKey::R); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "s", InputKey::S); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "t", InputKey::T); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "u", InputKey::U); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "v", InputKey::V); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "w", InputKey::W); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "x", InputKey::X); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "y", InputKey::Y); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "z", InputKey::Z); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "esc", InputKey::ESCAPE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "minus", InputKey::MINUS); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "equals", InputKey::EQUALS); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "backspace", InputKey::BACKSPACE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "tab", InputKey::TAB); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "sub", InputKey::SUB); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "divide", InputKey::DIVIDE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "multiply", InputKey::MULTIPLY); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "subtract", InputKey::SUBTRACT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "add", InputKey::ADD); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "decimal", InputKey::DECIMAL); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "clear", InputKey::CLEAR); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "grave", InputKey::GRAVE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "comma", InputKey::COMMA); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "period", InputKey::PERIOD); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "slash", InputKey::SLASH); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "lctrl", InputKey::LCTRL); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "rctrl", InputKey::RCTRL); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "lalt", InputKey::LALT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "ralt", InputKey::RALT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "lshift", InputKey::LSHIFT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "rshift", InputKey::RSHIFT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "lwin", InputKey::LWIN); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "rwin", InputKey::RWIN); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "space", InputKey::SPACE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "enter", InputKey::ENTER); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "lbracket", InputKey::LBRACKET); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "rbracket", InputKey::RBRACKET); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "backslash", InputKey::BACKSLASH); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "colon", InputKey::COLON); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "semicolon", InputKey::SEMICOLON); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "underline", InputKey::UNDERLINE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "tilde", InputKey::TILDE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_period", InputKey::NP_PERIOD); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_divide", InputKey::NP_DIVIDE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "print", InputKey::PRINT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "pause", InputKey::PAUSE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "home", InputKey::HOME); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "up", InputKey::UP); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "pgup", InputKey::PGUP); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "left", InputKey::LEFT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "right", InputKey::RIGHT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "end", InputKey::END); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "down", InputKey::DOWN); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "pgdn", InputKey::PGDN); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "insert", InputKey::INSERT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "delete", InputKey::DELETE); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "apps", InputKey::APPS); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "oem_102", InputKey::OEM_102); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "numlock", InputKey::NUMLOCK); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "capslock", InputKey::CAPSLOCK); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "scrolllock", InputKey::SCROLLLOCK); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_multiply", InputKey::NP_MULTIPLY); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_substract", InputKey::NP_SUBSTRACT); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_add", InputKey::NP_ADD); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_enter", InputKey::NP_ENTER); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_1", InputKey::NP_1); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_2", InputKey::NP_2); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_3", InputKey::NP_3); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_4", InputKey::NP_4); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_5", InputKey::NP_5); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_6", InputKey::NP_6); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_7", InputKey::NP_7); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_8", InputKey::NP_8); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_9", InputKey::NP_9); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "np_0", InputKey::NP_0); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f1", InputKey::F1); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f2", InputKey::F2); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f3", InputKey::F3); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f4", InputKey::F4); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f5", InputKey::F5); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f6", InputKey::F6); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f7", InputKey::F7); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f8", InputKey::F8); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f9", InputKey::F9); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f10", InputKey::F10); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f11", InputKey::F11); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f12", InputKey::F12); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f13", InputKey::F13); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f14", InputKey::F14); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f15", InputKey::F15); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f16", InputKey::F16); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f17", InputKey::F17); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f18", InputKey::F18); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f19", InputKey::F19); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f20", InputKey::F20); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f21", InputKey::F21); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f22", InputKey::F22); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f23", InputKey::F23); assert(r >= 0);
    r = _engine->RegisterEnumValue("keycode", "f24", InputKey::F24); assert(r >= 0);

    r = _engine->RegisterEnum("button");
    r = _engine->RegisterEnumValue("button", "mouse0", InputButton::MOUSE0); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse1", InputButton::MOUSE1); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse2", InputButton::MOUSE2); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse3", InputButton::MOUSE3); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse4", InputButton::MOUSE4); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse5", InputButton::MOUSE5); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse6", InputButton::MOUSE6); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse7", InputButton::MOUSE7); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouse8", InputButton::MOUSE8); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mousewheel", InputButton::MOUSEWHEEL); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mousex", InputButton::MOUSEX); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mousey", InputButton::MOUSEY); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mousez", InputButton::MOUSEZ); assert(r >= 0);
    r = _engine->RegisterEnumValue("button", "mouselast", InputButton::MOUSELAST); assert(r >= 0);

    r = _engine->RegisterObjectType("Input", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
    r = _engine->RegisterGlobalProperty("Input input", this); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "void set_mousePositionX(int)", asMETHOD(ScriptBindInput, setMousePosX), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "void set_mousePositionY(int)", asMETHOD(ScriptBindInput, setMousePosY), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "int get_mousePositionX() const", asMETHOD(ScriptBindInput, getMousePosX), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "int get_mousePositionY() const", asMETHOD(ScriptBindInput, getMousePosY), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "void setMousePos(int, int)", asMETHOD(ScriptBindInput, setMousePos), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getKeyDown(keycode code) const", asMETHOD(ScriptBindInput, getKeyDown), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getKeyUp(keycode code) const", asMETHOD(ScriptBindInput, getKeyUp), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getKey(keycode code) const", asMETHOD(ScriptBindInput, getKey), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getButtonDown(button code) const", asMETHOD(ScriptBindInput, getButtonDown), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getButtonUp(button code) const", asMETHOD(ScriptBindInput, getButtonUp), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool getButton(button code) const", asMETHOD(ScriptBindInput, getButton), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "void showCursor(bool)", asMETHOD(ScriptBindInput, showCursor), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool isShowCursor() const", asMETHOD(ScriptBindInput, isShowCursor), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "void lockCursor(bool)", asMETHOD(ScriptBindInput, lockCursor), asCALL_THISCALL); assert(r >= 0);
    r = _engine->RegisterObjectMethod("Input", "bool isLockedCursor() const", asMETHOD(ScriptBindInput, isLockedCursor), asCALL_THISCALL); assert(r >= 0);
}

void
ScriptBindInput::setInput(InputPtr input) noexcept
{
    _input = input;
}

InputPtr
ScriptBindInput::getInput() const noexcept
{
    return _input;
}

void
ScriptBindInput::setMousePosX(int x)  noexcept
{
    assert(_input);
    _input->setMousePosX(x);
}

void
ScriptBindInput::setMousePosY(int y) noexcept
{
    assert(_input);
    _input->setMousePosX(y);
}

void
ScriptBindInput::setMousePos(int x, int y)  noexcept
{
    assert(_input);
    _input->setMousePos(x, y);
}

int
ScriptBindInput::getMousePosX() const noexcept
{
    assert(_input);
    return _input->getMousePosX();
}

int
ScriptBindInput::getMousePosY() const noexcept
{
    assert(_input);
    return _input->getMousePosY();
}

bool
ScriptBindInput::getKeyDown(InputKey::Code key) const noexcept
{
    assert(_input);
    return _input->getKeyDown(key);
}

bool
ScriptBindInput::getKeyUp(InputKey::Code key) const noexcept
{
    assert(_input);
    return _input->getKeyUp(key);
}

bool
ScriptBindInput::getKey(InputKey::Code key) const noexcept
{
    assert(_input);
    return _input->getKey(key);
}

bool
ScriptBindInput::getButtonDown(InputButton::Code key) const noexcept
{
    assert(_input);
    return _input->getButtonDown(key);
}

bool
ScriptBindInput::getButtonUp(InputButton::Code key) const noexcept
{
    assert(_input);
    return _input->getButtonUp(key);
}

bool
ScriptBindInput::getButton(InputButton::Code key) const noexcept
{
    assert(_input);
    return _input->getButton(key);
}

void
ScriptBindInput::showCursor(bool show) noexcept
{
    assert(_input);
    return _input->showCursor(show);
}

bool
ScriptBindInput::isShowCursor() const noexcept
{
    assert(_input);
    return _input->isShowCursor();
}

void
ScriptBindInput::lockCursor(bool lock) noexcept
{
    assert(_input);
    return _input->lockCursor(lock);
}

bool
ScriptBindInput::isLockedCursor() const noexcept
{
    assert(_input);
    return _input->isLockedCursor();
}

_NAME_END