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

XGLCanvas::XGLCanvas() noexcept
{
	initPixelFormat(_fbconfig, _ctxconfig);
}

XGLCanvas::~XGLCanvas() noexcept
{
	this->close();
}

void
XGLCanvas::open(WindHandle window) except
{
	GLint attribs[80];
	GLint index = 0, mask = 0, flags = 0, startegy = 0;

	if (_ctxconfig.api == GPU_OPENGL_API)
	{
		if (_ctxconfig.forward)
			flags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

		if (_ctxconfig.debug)
			flags |= GLX_CONTEXT_DEBUG_BIT_ARB;

		if (_ctxconfig.profile)
		{
			if (_ctxconfig.profile == GPU_OPENGL_CORE_PROFILE)
				mask |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
			else if (_ctxconfig.profile == GPU_OPENGL_COMPAT_PROFILE)
				mask |= GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
			else
				mask |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
		}
	}
	else
	{
		mask |= GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
	}

	if (_ctxconfig.robustness)
	{
		if (_ctxconfig.robustness == GLFW_NO_RESET_NOTIFICATION)
			strategy = GLX_NO_RESET_NOTIFICATION_ARB;
		else if (_ctxconfig.robustness == GLFW_LOSE_CONTEXT_ON_RESET)
			strategy = GLX_LOSE_CONTEXT_ON_RESET_ARB;

		flags |= GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	}

	if (_ctxconfig.major > 1 && _ctxconfig.major < 4 && _ctxconfig.minor != 0)
	{
		attribs[index++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
		attribs[index++] = _ctxconfig.major;

		attribs[index++] = GLX_CONTEXT_MINOR_VERSION_ARB;
		attribs[index++] = _ctxconfig.minor;
	}

	if (mask)
	{
		attribs[index++] = GLX_CONTEXT_PROFILE_MASK_ARB;
		attribs[index++] = mask;
	}

	if (flags)
	{
		attribs[index++] = GLX_CONTEXT_FLAGS_ARB;
		attribs[index++] = flags;
	}

	if (startegy)
	{
		attribs[index++] = GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
		attribs[index++] = startegy;
	}

	attribs[index++] = GL_NONE;
	attribs[index++] = GL_NONE;

    static int att[] =
    {
        GLX_X_RENDERABLE    , GL_TRUE,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , fbconfig.red_size,
        GLX_GREEN_SIZE      , fbconfig.greeen_size,
        GLX_BLUE_SIZE       , fbconfig.blue_size,
        GLX_ALPHA_SIZE      , fbconfig.alpha_size,
        GLX_DEPTH_SIZE      , fbconfig.depth_size,
        GLX_STENCIL_SIZE    , fbconfig.stencil_size,
        GLX_DOUBLEBUFFER    , GL_TRUE,
        GL_NONE
    };

	_display = ::XOpenDisplay(NULL);
	if (_display == nullptr)
		throw failure("Cannot connect to X server");

    int glx_major, glx_minor;
    if (!glXQueryVersion(_display, &glx_major, &glx_minor))
	{
		throw failure("Cannot query GLX version");
	}

    if (( glx_major == 1 ) && ( glx_minor < 3 ) || ( glx_major < 1))
    {
        throw failure("GLX version 1.3 is required");
    }

    int fbcount = 0;
    _cfg = glXChooseFBConfig(_display, 0, att, &fbcount);
    if (!_cfg)
    {
        throw failure("Failed to retrieve a framebuffer config.");
    }
    
	_window = window;

    GLXCREATECONTEXTATTRIBSARB glXCreateContextAttribsARB = nullptr;
    glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARB)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
    if (glXCreateContextAttribsARB)
    {
		_glc = glXCreateContextAttribsARB(_display, _cfg, _ctxconfig.share, true, attribs);
		if (!_glc)
		{
			if (_ctxconfig.api == GPU_OPENGL_API && 
				_ctxconfig.profile == GPU_GL_ANY_PROFILE &&
				_ctxconfig.forward == GL_FALSE)
			{
				_glc = glXCreateNewContext(_display, _cfg, GLX_RGBA_TYPE, _ctxconfig.share, GL_TRUE);
			}
		}
    }
    else
    {
		_glc = glXCreateNewContext(_display, _cfg, GLX_RGBA_TYPE, _ctxconfig.share, GL_TRUE);
    }

	if (!_glc)
	{
		throw failure("Failed to create context");
	}

	glXMakeContextCurrent(_display, _window, _window, _glc);

    if (GLEW_OK != glewInit())
    {
        throw failure("Unable to initialize glext");
    }
}

void
XGLCanvas::close(WindHandle wx) noexcept
{
	if (_cfg)
	{
		XFree(_cfg);
		_cfg = nullptr;
	}

	if (_glc)
	{
		glXDestroyContext(_display, _glc);
		_glc = nullptr;
	}
}

void
XGLCanvas::setSwapInterval(SwapInterval interval) noexcept
{
    if (_interval != interval)
    {
        glXSwapIntervalEXT(_display, _window, (int)interval);
        _interval = interval;
    }
}

SwapInterval
XGLCanvas::getSwapInterval(SwapInterval interval) noexcept
{
	return _interval;
}

void
XGLCanvas::onActivate() noexcepts
{
    glXMakeContextCurrent(_display, _window, _window, _glc);
}

void
XGLCanvas::onDeactivate() noexcept
{
    glXMakeContextCurrent(_display, 0, 0, 0);
}

bool
XGLCanvas::present() noexcept
{
    if (_mode.double_enable)
        glXSwapBuffers(_display, _window);
    else
        glFlush();

    return true;
}

void
XGLCanvas::initPixelFormat(GPUfbconfig& fbconfig, GPUctxconfig& ctxconfig) noexcept
{
	fbconfig.redSize = 8;
	fbconfig.greenSize = 8;
	fbconfig.blueSize = 8;
	fbconfig.alphaSize = 8;
	fbconfig.bufferSize = 32;
	fbconfig.depthSize = 24;
	fbconfig.stencilSize = 8;
	fbconfig.accumSize = 0;
	fbconfig.accumRedSize = 0;
	fbconfig.accumGreenSize = 0;
	fbconfig.accumBlueSize = 0;
	fbconfig.accumAlphaSize = 0;
	fbconfig.samples = 0;

	ctxconfig.major = 3;
	ctxconfig.minor = 0;
	ctxconfig.release = 0;
	ctxconfig.robustness = 0;
	ctxconfig.share = nullptr;
	ctxconfig.api = GPU_OPENGL_ES_API;
	ctxconfig.profile = GPU_GL_CORE_PROFILE;
	ctxconfig.forward = 0;
	ctxconfig.multithread = false;
}

_NAME_END

#endif