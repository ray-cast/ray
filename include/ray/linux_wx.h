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
#ifndef _H_LINUX_WINDOW_H_
#define _H_LINUX_WINDOW_H_

#include <ray/win.h>
#include <ray/linux_app.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

typedef Window XWindow;
typedef Display XDisplay;

_NAME_BEGIN

class EXPORT Window final : public BaseTopLevelWindow
{
public:
    Window() noexcept;
    virtual ~Window() noexcept;

    void close();

    void hide();
    void realize();

    void setDisplay(XDisplay* display) { _display = display; }
    void setVisualInfo(XVisualInfo& vi) { _vi = vi; }

    void setWindowTitle(const std::string& title);
    void setWindowTitle(const char* title);
    void setWindowPosition(int x, int y);
    void setWindowSize(int w, int h);

    const std::string& getWindowTitle() const { return _title; }
    const Point& getWindowPosition() const { return _pt; };
    const Size&  getWindowSize() const { return _size; }

    XWindow getWindowHandle() const;
    XDisplay* getDisplay() const;

    explicit operator XWindow();
    explicit operator Input*();
private:
    bool doSetVideoMode(const char* title, const char* classname, int x, int y, int w, int h, int flags);

private:

    bool _created;

    Point _pt;
    Size _size;
    std::string _title;

    XWindow _window;
    XDisplay* _display;
    XVisualInfo _vi;
};

_NAME_END

#endif