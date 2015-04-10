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
#ifndef _H_X11_CONTEXT_H_
#define _H_X11_CONTEXT_H_

#ifdef __LINUX__

#include <ray/render_canvas.h>

#include <GL/glew.h>
#include <GL/glx.h>

_NAME_BEGIN

class OGLCanvas : public RenderCanvas
{
public:
    OGLCanvas(std::shared_ptr<Window> wx) noexcept;
    ~OGLCanvas() noexcept;

    void setup(WindHandle wx) noexcept;
    void close() noexcept;

    void setSwapInterval(Canvas::Interval interval) noexcept;

    void lock();
    void unlock() noexcept;

    bool present() noexcept;

private:
    OGLCanvas(const OGLCanvas&) = delete;
    OGLCanvas& operator=(const OGLCanvas&) = delete;

private:
    XWindow _window;
    XDisplay* _display;
    GLXFBConfig* _cfg;

    XWindow _window;
    XDisplay* _display;

    GLXWindow _glw;
    GLXContext _glc;
};

_NAME_END


#endif

#endif