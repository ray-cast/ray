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
#include <ray/wx.h>

_NAME_BEGIN

Window::Window() noexcept
    : _sizeX(0)
    , _sizeY(0)
    , _positionX(0)
    , _positionY(0)
    , _title("")
{
}

Window::Window(int w, int h, const char* title, int x, int y, int flags)
{
    this->create(title, x, y, w, h, flags);
}

Window::Window(int w, int h, const std::string& title, int x, int y, int flags)
{
    this->create(title, x, y, w, h, flags);
}

Window::Window(int w, int h, int x, int y, const char* title, int flags)
{
    this->create(w, h, x, y, title, flags);
}

Window::Window(int w, int h, int x, int y, const std::string& title, int flags)
{
    this->create(w, h, x, y, title, flags);
}

Window::Window(const Size& size, const Point& pt, const char* title, int flags)
{
    this->create(size, pt, title, flags);
}

Window::Window(const std::string& title, int x, int y, int w, int h, int flags)
{
    this->create(title.c_str(), x, y, w, h, flags);
}

Window::Window(const char* title, int x, int y, int w, int h, int flags)
{
    this->create(title, x, y, w, h, flags);
}

Window::~Window() noexcept
{
}

bool
Window::create(int w, int h, const char* title, int x, int y, int flags)
{
    return this->create(title, x, y, w, h, flags);
}

bool
Window::create(int w, int h, const std::string& title, int x, int y, int flags)
{
    return this->create(title, x, y, w, h, flags);
}

bool
Window::create(int w, int h, int x, int y, const std::string& title, int flags)
{
    return this->create(title, x, y, w, h, flags);
}

bool
Window::create(const Size& size, const Point& pt, const char* title, int flags)
{
    return this->create(size.x, size.y, title, pt.x, pt.y, flags);
}

bool
Window::create(const Size& size, const Point& pt, const std::string& title, int flags)
{
    return this->create(size.x, size.y, title, pt.x, pt.y, flags);
}

bool
Window::create(const std::string& title, int x, int y, int w, int h, int flags)
{
    return this->create(title.c_str(), x, y, w, h, flags);
}

bool
Window::create(const char* title, int x, int y, int w, int h, int flags) noexcept
{
    Size sz;
    Point pt;

    if (TopLevelWindow::create(title, x, y, w, h, flags))
    {
        TopLevelWindow::getWindowSize(sz);
        TopLevelWindow::getWindowPosition(pt);

        _sizeX = sz.x;
        _sizeY = sz.y;

        _positionX = pt.x;
        _positionY = pt.y;

        return true;
    }

    return false;
}

void
Window::setWindowPosition(int x, int y) noexcept
{
    assert(this->getWindowHandle());

    _positionX = x;
    _positionY = y;
    TopLevelWindow::setWindowPosition(x, y);
}

void
Window::setWindowSize(int w, int h) noexcept
{
    assert(this->getWindowHandle());

    _sizeX = w;
    _sizeY = h;
    TopLevelWindow::setWindowSize(w, h);
}

#if !defined(_UNICODE_)
void
Window::setWindowTitle(const std::string& title) noexcept
{
    assert(this->getWindowHandle());

    _title = title;
    TopLevelWindow::setWindowTitle(_title);
}

void
Window::setWindowTitle(const char* title) noexcept
{
    assert(this->getWindowHandle());

    _title = title;
    TopLevelWindow::setWindowTitle(_title);
}
#else
void
Window::setWindowTitle(const std::wstring& title) noexcept
{
    assert(this->getWindowHandle());

    _title = title;
    TopLevelWindow::setWindowTitle(_title);
}

void
Window::setWindowTitle(const wchar_t* title) noexcept
{
    assert(this->getWindowHandle());

    _title = title;
    TopLevelWindow::setWindowTitle(_title);
}
#endif

int
Window::getWindowWidth() const noexcept
{
    return _sizeX;
}

int
Window::getWindowHeight() const noexcept
{
    return _sizeY;
}

int
Window::getWindowPosX() const noexcept
{
    return _positionX;
}

int
Window::getWindowPosY() const noexcept
{
    return _positionY;
}

void
Window::getWindowSize(Size& sz) const noexcept
{
    sz.x = _sizeX;
    sz.y = _sizeY;
}

void
Window::getWindowSize(int& w, int& h) const noexcept
{
    w = _sizeX;
    h = _sizeY;
}

void
Window::getWindowPosition(Point& pt) const noexcept
{
    pt.x = _positionX;
    pt.y = _positionY;
}

void
Window::getWindowPosition(int& x, int& y) const noexcept
{
    x = _positionX;
    y = _positionY;
}

#if !defined(_UNICODE_)
const std::string&
Window::getWindowTitle() const noexcept
{
    return _title;
}
#else
const std::string&
Window::getWindowTitle() const noexcept
{
    return _title;
}
#endif

_NAME_END