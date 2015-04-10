// +----------------------------------------------------------------------
// | Project : Xiemoperor.
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
#ifndef _H_SCRIPT_BIND_INPUT_H_
#define _H_SCRIPT_BIND_INPUT_H_

#include <ray/script_binder.h>
#include <ray/input.h>

_NAME_BEGIN

class ScriptBindInput final : public ScriptBinder
{
public:
    ScriptBindInput() noexcept;
    ~ScriptBindInput() noexcept;

    void setInput(InputPtr input) noexcept;
    void setWindow(WindowPtr window) noexcept;

    InputPtr getInput() const noexcept;

    void setup(asIScriptEngine* _engine) noexcept;

private:

    void setMousePosX(int x)  noexcept;
    void setMousePosY(int y) noexcept;
    void setMousePos(int x, int y)  noexcept;

    int getMousePosX() const noexcept;
    int getMousePosY() const noexcept;

    bool getKeyDown(InputKey::Code key) const noexcept;
    bool getKeyUp(InputKey::Code key) const noexcept;

    bool getKey(InputKey::Code key) const noexcept;

    bool getButtonDown(InputButton::Code key) const noexcept;
    bool getButtonUp(InputButton::Code key) const noexcept;

    bool getButton(InputButton::Code key) const noexcept;

    void showCursor(bool show) noexcept;
    bool isShowCursor() const noexcept;

    void lockCursor(bool lock) noexcept;
    bool isLockedCursor() const noexcept;

private:

    InputPtr _input;
    WindowPtr _window;
};

_NAME_END

#endif