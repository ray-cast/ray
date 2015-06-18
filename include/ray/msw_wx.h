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
#ifndef _H_MSW_WINDOW_H_
#define _H_MSW_WINDOW_H_

#include <ray/win.h>

_NAME_BEGIN

class EXPORT MSWTopLevelWindow : public BaseTopLevelWindow
{
public:
    MSWTopLevelWindow() noexcept;
    virtual ~MSWTopLevelWindow() noexcept;

    virtual bool create(const char* title, int x, int y, int w, int h, int flags) noexcept;

    virtual void close() noexcept;

    virtual void hide() noexcept;
    virtual bool realize() noexcept;

    virtual void setWindowTitle(const char* title) noexcept;
    virtual void setWindowTitle(const std::string& title) noexcept;

#if defined(__UNICODE__)
    virtual void setWindowTitle(const wchar_t* title) noexcept;
    virtual void setWindowTitle(const std::wstring& title) noexcept;
#endif

    virtual void setWindowPosition(int x, int y) noexcept;
    virtual void setWindowSize(int w, int h) noexcept;

    virtual void getWindowSize(Size& sz) const noexcept;
    virtual void getWindowPosition(Point& pt) const noexcept;

    virtual void getWindowTitle(std::string& title) const noexcept;

    virtual HWND getWindowHandle() const noexcept;

private:

    int setMinMaxAnimations(int enable) noexcept;

private:
    MSWTopLevelWindow(const MSWTopLevelWindow&) = delete;
    MSWTopLevelWindow& operator = (const MSWTopLevelWindow&) = delete;

private:
    HDC _hdc;
    HDC _mdc;
    HWND _hwnd;

    WPARAM _mouse_button_flags;

    DWORD _style;
    DWORD _style_ex;
};

_NAME_END

#endif