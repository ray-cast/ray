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
#ifndef _H_INPUT_KEY_H_
#define _H_INPUT_KEY_H_

#include <ray/input_types.h>

_NAME_BEGIN

class InputKey
{
public:
    enum Code
    {
        UNKNOWN = -1,

        ESCAPE,

        Key0,
        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        MINUS,
        EQUALS,
        BACKSPACE,
        TAB,
        SUB,
        DIVIDE,
        MULTIPLY,
        SUBTRACT,
        ADD,
        DECIMAL,
        CLEAR,
        GRAVE,
        COMMA,
        PERIOD,
        SLASH,
        LCTRL,
        RCTRL,
        LALT,
        RALT,
        LSHIFT,
        RSHIFT,
        LWIN,
        RWIN,
        SPACE,
        ENTER,
        LBRACKET,
        RBRACKET,
        BACKSLASH,
        COLON,
        SEMICOLON,
        UNDERLINE,
        TILDE,
        NP_PERIOD,
        NP_DIVIDE,
        PRINT,
        PAUSE,
        HOME,
        UP,
        PGUP,
        LEFT,
        RIGHT,
        END,
        DOWN,
        PGDN,
        INSERT,
        DELETE,
        APPS,
        OEM_102,

        NUMLOCK,
        CAPSLOCK,
        SCROLLLOCK,

        NP_MULTIPLY,
        NP_SUBSTRACT,
        NP_ADD,
        NP_ENTER,

        NP_0,
        NP_1,
        NP_2,
        NP_3,
        NP_4,
        NP_5,
        NP_6,
        NP_7,
        NP_8,
        NP_9,

        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,

        NumKeyCodes,
    };
};

_NAME_END

#endif