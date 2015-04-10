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
#ifdef _BUILD_PLATFORM_SDL2
#include <ray/SDL_wx.h>
#include <SDL.h>

_NAME_BEGIN

SDLTopLevelWindow::SDLTopLevelWindow() noexcept
    : _window(nullptr)
{
}

SDLTopLevelWindow::~SDLTopLevelWindow() noexcept
{
    this->close();
}

void
SDLTopLevelWindow::close() noexcept
{
    if (_window)
    {
        ::SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

void
SDLTopLevelWindow::hide() noexcept
{
    assert(_window);
    ::SDL_HideWindow(_window);
}

bool
SDLTopLevelWindow::realize() noexcept
{
    assert(_window);
    ::SDL_ShowWindow(_window);
    return true;
}

void
SDLTopLevelWindow::setWindowTitle(const std::string& title) noexcept
{
    assert(_window);
    ::SDL_SetWindowTitle(_window, title.c_str());
}

void
SDLTopLevelWindow::setWindowTitle(const char* title) noexcept
{
    assert(_window);
    ::SDL_SetWindowTitle(_window, title);
}

void
SDLTopLevelWindow::setWindowPosition(int x, int y) noexcept
{
    assert(_window);
    ::SDL_SetWindowPosition(_window, x, y);
}

void
SDLTopLevelWindow::setWindowSize(int w, int h) noexcept
{
    assert(_window);
    ::SDL_SetWindowSize(_window, w, h);
}

bool
SDLTopLevelWindow::create(const char* title, const char* classname, int x, int y, int w, int h, int flags)
{
    assert(!_window);
    _window = ::SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    return _window ? true : false;
}

SDL_Window*
SDLTopLevelWindow::getWindowHandle() const noexcept
{
    return _window;
}

_NAME_END

#endif