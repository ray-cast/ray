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

#include <ray/sdl_canvas.h>
#include <ray/ogl_impl.h>

#include <SDL.h>

_NAME_BEGIN

bool SDLCanvas::_initExtention = false;

SDLCanvas::SDLCanvas() noexcept
    : _context(nullptr)
    , _window(nullptr)
{
}

SDLCanvas::~SDLCanvas() noexcept
{
    this->close();
}

bool
SDLCanvas::setup(WindHandle wx) noexcept
{
    assert(!_window);

    _window = (SDL_Window*)wx;

    ::SDL_GL_SetAttribute(SDL_GL_RED_SIZE, _displayMode._redSize);
    ::SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, _displayMode._greenSize);
    ::SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, _displayMode._blueSize);
    ::SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, _displayMode._alphaSize);

    ::SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, _displayMode._bufferSize);
    ::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, _displayMode._depthSize);
    ::SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, _displayMode._stencilSize);

    ::SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, _displayMode._accumRedSize);
    ::SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, _displayMode._accumGreenSize);
    ::SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, _displayMode._accumBlueSize);
    ::SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, _displayMode._accumAlphaSize);

    ::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _displayMode._multisampleSamples);
    ::SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, _displayMode._acceleratedVisual);
    ::SDL_GL_SetAttribute(SDL_GL_STEREO, _displayMode._stereo);
    ::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _displayMode._doubleBuffer);

    if (_displayMode._debug)
    {
        ::SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    }

    _context = ::SDL_GL_CreateContext(_window);

    if (!_initExtention)
    {
        glewInit();
        _initExtention = true;
    }

    return _context ? true : false;
}

void
SDLCanvas::close() noexcept
{
    if (_context)
    {
        ::SDL_GL_DeleteContext(_context);
        _context = nullptr;
    }
}

void 
SDLCanvas::setSwapInterval(int interval) noexcept
{
    ::SDL_GL_SetSwapInterval(interval);
}

void 
SDLCanvas::lock() noexcept
{
    assert(_window && _context);
    ::SDL_GL_MakeCurrent(_window, _context);
}

void 
SDLCanvas::unlock() noexcept
{
    assert(_window && _context);
    ::SDL_GL_MakeCurrent(_window, nullptr);
}

void
SDLCanvas::present() noexcept
{
    assert(_window);
    ::SDL_GL_SwapWindow(_window);
}

WindHandle 
SDLCanvas::getWindHandle() noexcept
{
    return (WindHandle)_window;
}

_NAME_END

#endif