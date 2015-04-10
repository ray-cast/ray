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
#ifndef _H_WIN_H_
#define _H_WIN_H_

#include <ray/event.h>

_NAME_BEGIN

enum WINDOWPOS
{
    WINDOWPOS_CENTERED = 0x2FFF0000
};

enum WINDOWFLAGS
{
    WF_HIDE = 1 << 1
};

class EXPORT BaseTopLevelWindow : public EvtHandler
{
public:
    BaseTopLevelWindow() noexcept;
    virtual ~BaseTopLevelWindow() noexcept;

    virtual bool create(const char* title, int x, int y, int w, int h, int flags) noexcept = 0;

    virtual void hide() noexcept = 0;
    virtual bool realize() noexcept = 0;

    virtual void setWindowPosition(int x, int y) noexcept = 0;
    virtual void setWindowSize(int w, int h) noexcept = 0;

    virtual void setWindowTitle(const std::string& title) noexcept = 0;

    virtual void close() noexcept = 0;

private:
    BaseTopLevelWindow(const BaseTopLevelWindow&) noexcept = delete;
    BaseTopLevelWindow& operator = (const BaseTopLevelWindow&)noexcept = delete;
};

_NAME_END

#endif