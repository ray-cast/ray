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
#ifdef _BUILD_PLATFORM_LINUX
#include <ray/linux_wx.h>

_NAME_BEGIN

Window::Window() noexcept
    : _window(0)
    , _display(0)
{
}

Window::~Window() noexcept
{
    this->close();
}

void
Window::close()
{
    if (_display)
    {
        ::XDestroyWindow(_display, _window);
        ::XCloseDisplay(_display);
        _display = nullptr;
    }
}

void
Window::hide()
{
}

void
Window::realize()
{
    ::XMapWindow(_display, _window);
}

void
Window::setWindowTitle(const std::string& title)
{
    this->setWindowTitle(title.c_str());
}

void
Window::setWindowTitle(const char* title)
{
    assert(_display && _window)
        ::XStoreName(_display, _window, title);
}

void
Window::setWindowPosition(int x, int y)
{
    _pt = Point(x, y);
}

void
Window::setWindowSize(int w, int h)
{
    _size = Size(w, h);
}

XWindow
Window::getWindowHandle() const
{
    return _window;
}

XDisplay*
Window::getDisplay() const
{
    return _display;
}

bool
Window::doSetVideoMode(const char* title, const char* classname, int x, int y, int w, int h, int flags)
{
    if (_display == nullptr)
        _display = ::XOpenDisplay(NULL);

    int screen = ::XDefaultScreen(_display);

    int left = (::XDisplayWidth(_display, screen) - w) >> 1;
    int top = (::XDisplayHeight(_display, screen) - h) >> 1;

    XWindow root = DefaultRootWindow(_display);

    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(_display, root, _vi.visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask | ExposureMask | ButtonPressMask | StructureNotifyMask;

    _window = ::XCreateWindow(
        _display,
        root,
        left,
        top,
        w,
        h,
        0,
        _vi.depth,
        InputOutput,
        _vi.visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &swa
        );

    if (_window == 0)
        return false;

    this->setWindowTitle(title);
    return true;
}

Window::operator XWindow()
{
    return this->getWindowHandle();
}

Window::operator Input*()
{
}

_NAME_END

#endif