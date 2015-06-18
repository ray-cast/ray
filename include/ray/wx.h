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
#ifndef _H_WX_H_
#define _H_WX_H_

#if defined(_BUILD_PLATFORM_SDL2)
#    include <ray/sdl_wx.h>
#    define TopLevelWindow SDLTopLevelWindow
#else
#    if defined(_BUILD_PLATFORM_WINDOWS)
#        include <ray/msw_wx.h>
#    define TopLevelWindow MSWTopLevelWindow
#    elif defined(__LINUX__)
#        include <ray/x11_wx.h>
#    define TopLevelWindow TopLevelWindowLINUX
#    elif defined(__APPLE__)
#        include <ray/x11_wx.h>
#    define TopLevelWindow TopLevelWindowAPPLE
#    endif
#endif

_NAME_BEGIN

class EXPORT Window : public TopLevelWindow
{
public:
    Window() noexcept;
    Window(int w, int h, const char* title, int x = 0, int y = 0, int flags = 0);
    Window(int w, int h, const std::string& title, int x = 0, int y = 0, int flags = 0);
    Window(int w, int h, int x, int y, const char* title, int flags = 0);
    Window(int w, int h, int x, int y, const std::string& title, int flags = 0);
    Window(const Size& size, const Point& pt, const char* title, int flags = 0);
    Window(const std::string& title, int x, int y, int w, int h, int flags);
    Window(const char* title, int x, int y, int w, int h, int flags);
    virtual ~Window() noexcept;

    bool create(int w, int h, const char* title, int x = 0, int y = 0, int flags = 0);
    bool create(int w, int h, const std::string& title, int x = 0, int y = 0, int flags = 0);
    bool create(int w, int h, int x, int y, const std::string& title, int flags = 0);
    bool create(const Size& size, const Point& pt, const char* title, int flags = 0);
    bool create(const Size& size, const Point& pt, const std::string& title, int flags = 0);
    bool create(const std::string& title, int x, int y, int w, int h, int flags);
    bool create(const char* title, int x, int y, int w, int h, int flags) noexcept;

    int getWindowWidth() const noexcept;
    int getWindowHeight() const noexcept;

    int getWindowPosX() const noexcept;
    int getWindowPosY() const noexcept;
};

_NAME_END

#endif