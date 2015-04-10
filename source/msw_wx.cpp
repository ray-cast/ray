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
#ifdef _BUILD_PLATFORM_WINDOWS
#include <ray/msw_wx.h>
#include <ray/msw_app.h>

_NAME_BEGIN

#define STYLE_BASIC         (WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define STYLE_FULLSCREEN    (WS_POPUP)
#define STYLE_BORDERLESS    (WS_POPUP)
#define STYLE_NORMAL        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_RESIZABLE     (WS_THICKFRAME | WS_MAXIMIZEBOX)
#define STYLE_MASK          (STYLE_FULLSCREEN | STYLE_BORDERLESS | STYLE_NORMAL | STYLE_RESIZABLE)

MSWTopLevelWindow::MSWTopLevelWindow() noexcept
    :_hwnd(nullptr)
{
    _hdc = nullptr;
    _mdc = nullptr;
    _style = STYLE_NORMAL;
    _style_ex = WS_EX_APPWINDOW;
}

MSWTopLevelWindow::~MSWTopLevelWindow() noexcept
{
    this->close();
}

void
MSWTopLevelWindow::setWindowTitle(const char* title) noexcept
{
    assert(_hwnd);
    ::SetWindowTextA(_hwnd, title);
}

void
MSWTopLevelWindow::setWindowTitle(const std::string& title) noexcept
{
    this->setWindowTitle(title.c_str());
}

#if defined(__UNICODE__)

void
MSWTopLevelWindow::setWindowTitle(const wchar_t* title) noexcept
{
    assert(_hwnd);
    ::SetWindowTextW(_hwnd, title);
}

void
MSWTopLevelWindow::setWindowTitle(const std::wstring& title) noexcept
{
    this->setWindowTitle(title.c_str());
}

#endif

void
MSWTopLevelWindow::setWindowPosition(int x, int y) noexcept
{
    assert(_hwnd);
    ::SetWindowPos(_hwnd, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void
MSWTopLevelWindow::setWindowSize(int w, int h) noexcept
{
    assert(_hwnd);
    ::SetWindowPos(_hwnd, 0, w, h, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void
MSWTopLevelWindow::getWindowSize(Size& sz) const noexcept
{
    RECT rc;
    ::GetClientRect(_hwnd, &rc);
    sz.x = rc.right - rc.left;
    sz.y = rc.bottom - rc.top;
}

void
MSWTopLevelWindow::getWindowPosition(Point& pt) const noexcept
{
    RECT rc1, rc2;
    ::GetWindowRect(_hwnd, &rc1);
    ::GetClientRect(_hwnd, &rc2);

    int windowWidth = rc1.right - rc1.left;
    int windowHeight = rc1.bottom - rc1.top;

    int clientWidth = rc2.right - rc2.left;
    int clientHeight = rc2.bottom - rc2.top;

    int captionHegiht = ::GetSystemMetrics(SM_CYCAPTION);

    int offsetX = (windowWidth - clientWidth) >> 1;
    int offsetY = (windowHeight - clientHeight - captionHegiht) >> 1;

    pt.x = rc1.left + offsetX;
    pt.y = rc1.top + offsetY + captionHegiht;
}

HWND
MSWTopLevelWindow::getWindowHandle() const noexcept
{
    return _hwnd;
}

bool
MSWTopLevelWindow::create(const char* title, int x, int y, int w, int h, int flags) noexcept
{
    RECT rc;
    if (_style)
    {
        rc.left = (::GetSystemMetrics(SM_CXSCREEN) - w) >> 1;
        rc.top = (::GetSystemMetrics(SM_CYSCREEN) - h) >> 1;
    }
    else
    {
        rc.left = x;
        rc.top = y;
    }

    rc.right = rc.left + w;
    rc.bottom = rc.top + h;
    ::AdjustWindowRectEx(&rc, _style, FALSE, _style_ex);

    _hwnd = CreateWindowEx(
        _style_ex,
        "AppFramework",
        title,
        _style,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        0,
        0,
        ::GetModuleHandle(0),
        0);

    if (_hwnd)
    {
        ::ShowWindow(_hwnd, TRUE);
        ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
        return true;
    }

    return false;
}

void
MSWTopLevelWindow::close() noexcept
{
    if (_hwnd)
    {
        if (::DestroyWindow(_hwnd))
        {
            _hwnd = nullptr;
        }
    }
}

void
MSWTopLevelWindow::hide() noexcept
{
    assert(_hwnd);
    ::ShowWindow(_hwnd, SW_HIDE);
}

int
MSWTopLevelWindow::setMinMaxAnimations(int enable) noexcept
{
    ANIMATIONINFO ai;

    int old_enable;

    ai.cbSize = sizeof(ANIMATIONINFO);

    ::SystemParametersInfo(SPI_GETANIMATION, ai.cbSize, &ai, 0);

    old_enable = ai.iMinAnimate;

    if (old_enable != enable)
    {
        ai.iMinAnimate = enable;
        ::SystemParametersInfo(SPI_SETMENUANIMATION, ai.cbSize, &ai, SPIF_SENDCHANGE);
    }

    return old_enable;
}

bool
MSWTopLevelWindow::realize() noexcept
{
    assert(_hwnd);

    ::SetForegroundWindow(_hwnd);

    if (::GetForegroundWindow() == _hwnd)
    {
        ::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDCHANGE);
    }

    int old_animate = this->setMinMaxAnimations(0);

    int try_count = 0;

    do
    {
        ::ShowWindow(_hwnd, SW_HIDE);
        ::ShowWindow(_hwnd, SW_SHOWMINIMIZED);
        ::ShowWindow(_hwnd, SW_SHOWNORMAL);

        ::BringWindowToTop(_hwnd);
        ::SetForegroundWindow(_hwnd);

        try_count++;
    } while (_hwnd != ::GetForegroundWindow() && try_count <= 5);

    ::SetFocus(_hwnd);

    this->setMinMaxAnimations(old_animate);

    ::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDCHANGE);
    return true;
}

_NAME_END

#endif