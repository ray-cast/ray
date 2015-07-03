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
#ifndef _H_WGL_CANVAS_H_
#define _H_WGL_CANVAS_H_

#include <ray/ogl_extenstion.h>

_NAME_BEGIN

class WGLCanvas final : public RenderWindow
{
public:
    WGLCanvas() noexcept;
    ~WGLCanvas() noexcept;

    void setup(WindHandle hwnd) except;
    void close() noexcept;

    void setSwapInterval(SwapInterval interval) noexcept;
    SwapInterval getSwapInterval() const noexcept;

    void bind();
    void unbind() noexcept;

    void present() noexcept;

    void setWindowResolution(std::size_t w, std::size_t h) noexcept;

    std::size_t getWindowWidth() const noexcept;
    std::size_t getWindowHeight() const noexcept;

    WindHandle getWindHandle() const noexcept;

private:
    WGLCanvas(const WGLCanvas&) noexcept = delete;
    WGLCanvas& operator=(const WGLCanvas&) noexcept = delete;

private:

    HWND _hwnd;
    HDC _hdc;
    HGLRC _context;

    std::size_t _width;
    std::size_t _height;

    GPUfbconfig _fbconfig;
    GPUctxconfig _ctxconfig;

    SwapInterval _interval;
};

_NAME_END

#endif