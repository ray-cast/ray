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
#include <ray/glx_canvas.h>

_NAME_BEGIN

OGLCanvas::OGLCanvas(std::shared_ptr<Window> wx) noexcept
{
    this->setup(wx);
}

OGLCanvas::~OGLCanvas() noexcept
{
}

void
OGLCanvas::setup(WindHandle wx)
{
    _display = ::XOpenDisplay(NULL);
    if (_display == nullptr)
    {
        throw failure("Cannot connect to X server");
    }

    static int att[] =
    {
        GLX_X_RENDERABLE    , true,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , GL_TRUE,
        0
    };

    int glx_major, glx_minor;
    if (glXQueryVersion(_display, &glx_major, &glx_minor))
    {
        if (( glx_major == 1 ) && ( glx_minor < 3 ) || ( glx_major < 1))
        {
            throw failure("invlid GLX version");
        }
    }
    else
    {
        throw failure("Cannot query GLX version");
    }

    int fbcount = 0;
    _cfg = glXChooseFBConfig(_display, 0, att, &fbcount);
    if (!_cfg)
    {
        throw failure("Failed to retrieve a framebuffer config.");
    }

    XVisualInfo* vi = glXGetVisualFromFBConfig(_display, *_cfg);
    if (vi == nullptr)
    {
        throw failure("Failed to Chosen visual");
    }

    wx->setDisplay(_display);
    wx->setVisualInfo(*vi);

    if (!wx->setVideoMode(wx->getWindowSize(), wx->getWindowPosition(), wx->getWindowTitle()))
    {
        throw failure("Failed to Create Window");
    }

    XFree(vi);

    wx->realize();

    const OGLCanvas* glcanvas = dynamic_cast<const OGLCanvas*>(&canvas);
    if (glcanvas)
    {
        _display = glcanvas->display();
        _window = glcanvas->window();

        _glw = glXCreateWindow(_display, *glcanvas->config(), _window, NULL);

        GLXCREATECONTEXTATTRIBSARB glXCreateContextAttribsARB = nullptr;
        glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARB)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
        if (glXCreateContextAttribsARB)
        {
            int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
            };

            if (share)
                _glc = glXCreateContextAttribsARB(_display, *glcanvas->config(), share->_glc, true, context_attribs);
            else
                _glc = glXCreateContextAttribsARB(_display, *glcanvas->config(), 0, true, context_attribs);
        }
        else
        {
            if (share)
                _glc = glXCreateNewContext(_display, *glcanvas->config(), GLX_RGBA_TYPE, share->_glc, GL_TRUE);
            else
                _glc = glXCreateNewContext(_display, *glcanvas->config(), GLX_RGBA_TYPE, NULL, GL_TRUE);
        }

        this->lock();

        if (GLEW_OK != glewInit())
        {
            throw failure("Unable to initialize glext");
        }

        this->unlock();
    }
}

void
OGLCanvas::setSwapInterval(Canvas::Interval interval) noexcept
{
    if (_interval != interval)
    {
        //glXSwapIntervalEXT(_display, _window, (int)interval);
        _interval = interval;
    }
}

void
OGLCanvas::bind()
{
    glXMakeContextCurrent(_display, _window, _window, _glc);
}

void
OGLCanvas::unbind() noexcept
{
    glXMakeContextCurrent(_display, 0, 0, 0);
}

bool
OGLCanvas::present() noexcept
{
    if (_mode.double_enable)
        glXSwapBuffers(_display, _window);
    else
        glFlush();

    return true;
}

_NAME_END

#endif