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
#include <ray/ogl_extenstion.h>

_NAME_BEGIN

#ifdef GLEW_MX
GLEWContext _glewctx;
WGLEWContext _wglewctx;
#endif

PFNWGLSWAPBUFFERSPROC      __wglSwapBuffers;
PFNWGLSWAPINTERVALEXTPROC __wglSwapIntervalEXT;
PFNWGLCREATECONTEXTATTRIBSARBPROC   __wglCreateContextAttribsARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC __wglGetPixelFormatAttribivARB;

PFNGLCREATESTATESNVPROC __glewCreateStatesNV;
PFNGLDELETESTATESNVPROC __glewDeleteStatesNV;
PFNGLISSTATENVPROC __glewIsStateNV;
PFNGLSTATECAPTURENVPROC __glewStateCaptureNV;
PFNGLDRAWCOMMANDSNVPROC __glewDrawCommandsNV;
PFNGLDRAWCOMMANDSADDRESSNVPROC __glewDrawCommandsAddressNV;
PFNGLDRAWCOMMANDSSTATESNVPROC __glewDrawCommandsStatesNV;
PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC __glewDrawCommandsStatesAddressNV;
PFNGLCREATECOMMANDLISTSNVPROC __glewCreateCommandListsNV;
PFNGLDELETECOMMANDLISTSNVPROC __glewDeleteCommandListsNV;
PFNGLISCOMMANDLISTNVPROC __glewIsCommandListNV;
PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC __glewListDrawCommandsStatesClientNV;
PFNGLCOMMANDLISTSEGMENTSNVPROC __glewCommandListSegmentsNV;
PFNGLCOMPILECOMMANDLISTNVPROC __glewCompileCommandListNV;
PFNGLCALLCOMMANDLISTNVPROC __glewCallCommandListNV;
PFNGLGETCOMMANDHEADERNVPROC __glewGetCommandHeaderNV;
PFNGLGETSTAGEINDEXNVPROC __glewGetStageIndexNV;

int OGLExtenstion::initWGLExtention = 0;
int OGLExtenstion::initedNVcommandList = 0;

bool OGLExtenstion::_ARB_pixel_format = 0;
bool OGLExtenstion::_ARB_create_context = 0;
bool OGLExtenstion::_ARB_create_context_robustness = 0;
bool OGLExtenstion::_ARB_bindless_texture = 0;
bool OGLExtenstion::_ARB_vertex_array_object = 0;
bool OGLExtenstion::_ARB_vertex_attrib_binding = 0;
bool OGLExtenstion::_ARB_provoking_vertex = 0;
bool OGLExtenstion::_ARB_direct_state_access = 0;
bool OGLExtenstion::_ARB_viewport_array = 0;
bool OGLExtenstion::_EXT_swap_control = 0;
bool OGLExtenstion::_NV_command_list = 0;
bool OGLExtenstion::_NV_shader_buffer_load = 0;
bool OGLExtenstion::_NV_vertex_buffer_unified_memory = 0;

OGLExtenstion::OGLExtenstion() noexcept
{
}

OGLExtenstion::~OGLExtenstion() noexcept
{
}

bool
OGLExtenstion::initWGLExtensions(HDC hdc) except
{
    if (initWGLExtention) return 0;

    _ARB_pixel_format = false;
    _ARB_create_context = false;

    __wglSwapBuffers = nullptr;
    __wglSwapIntervalEXT = nullptr;
    __wglGetPixelFormatAttribivARB = nullptr;
    __wglCreateContextAttribsARB = nullptr;

    HGLRC context = ::wglCreateContext(hdc);
    if (!context)
    {
        throw failure("wglCreateContext fail");
    }

    ::wglMakeCurrent(hdc, context);

    glewInit();

#if defined(GLEW_MX)
    wglewInit();
#endif

    __wglSwapBuffers = (PFNWGLSWAPBUFFERSPROC)::GetProcAddress(::LoadLibrary("OpenGL32"), "wglSwapBuffers");

    if (::wglewIsSupported("WGL_EXT_swap_control"))
    {
        __wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)::wglGetProcAddress("wglSwapIntervalEXT");

        if (__wglSwapIntervalEXT)
            _EXT_swap_control = true;
    }

    if (::wglewIsSupported("WGL_ARB_pixel_format"))
    {
        __wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)::wglGetProcAddress("wglGetPixelFormatAttribivARB");

        if (__wglGetPixelFormatAttribivARB)
            _ARB_pixel_format = true;
    }

    if (::wglewIsSupported("WGL_ARB_create_context"))
    {
        __wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)::wglGetProcAddress("wglCreateContextAttribsARB");

        if (__wglCreateContextAttribsARB)
            _ARB_create_context = true;
    }

    ::wglDeleteContext(context);

    _ARB_bindless_texture = GLEW_ARB_bindless_texture ? true : false;
    _ARB_vertex_array_object = GLEW_ARB_vertex_array_object ? true : false;
    _ARB_vertex_attrib_binding = GLEW_ARB_vertex_attrib_binding ? true : false;
    _ARB_provoking_vertex = GLEW_ARB_provoking_vertex ? true : false;
    _ARB_create_context = WGLEW_ARB_create_context ? true : false;
    _ARB_create_context_robustness = WGLEW_ARB_create_context_robustness ? true : false;
    _ARB_direct_state_access = GLEW_ARB_direct_state_access ? true : false;
    _ARB_viewport_array = GLEW_ARB_viewport_array ? true : false;
    _EXT_swap_control = WGLEW_EXT_swap_control ? true : false;
    _NV_shader_buffer_load = GLEW_NV_shader_buffer_load ? true : false;
    _NV_vertex_buffer_unified_memory = GLEW_NV_vertex_buffer_unified_memory ? true : false;

    initWGLExtention = true;

    return __wglSwapBuffers != NULL;
}

bool
OGLExtenstion::initCommandListNV() noexcept
{
    if (initedNVcommandList)
        return __glewCreateStatesNV != NULL;

    initedNVcommandList = 1;

    __glewCreateStatesNV = (PFNGLCREATESTATESNVPROC)wglGetProcAddress("glCreateStatesNV");
    __glewDeleteStatesNV = (PFNGLDELETESTATESNVPROC)wglGetProcAddress("glDeleteStatesNV");
    __glewIsStateNV = (PFNGLISSTATENVPROC)wglGetProcAddress("glIsStateNV");
    __glewStateCaptureNV = (PFNGLSTATECAPTURENVPROC)wglGetProcAddress("glStateCaptureNV");
    __glewDrawCommandsNV = (PFNGLDRAWCOMMANDSNVPROC)wglGetProcAddress("glDrawCommandsNV");
    __glewDrawCommandsAddressNV = (PFNGLDRAWCOMMANDSADDRESSNVPROC)wglGetProcAddress("glDrawCommandsAddressNV");
    __glewDrawCommandsStatesNV = (PFNGLDRAWCOMMANDSSTATESNVPROC)wglGetProcAddress("glDrawCommandsStatesNV");
    __glewDrawCommandsStatesAddressNV = (PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC)wglGetProcAddress("glDrawCommandsStatesAddressNV");
    __glewCreateCommandListsNV = (PFNGLCREATECOMMANDLISTSNVPROC)wglGetProcAddress("glCreateCommandListsNV");
    __glewDeleteCommandListsNV = (PFNGLDELETECOMMANDLISTSNVPROC)wglGetProcAddress("glDeleteCommandListsNV");
    __glewIsCommandListNV = (PFNGLISCOMMANDLISTNVPROC)wglGetProcAddress("glIsCommandListNV");
    __glewListDrawCommandsStatesClientNV = (PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC)wglGetProcAddress("glListDrawCommandsStatesClientNV");
    __glewCommandListSegmentsNV = (PFNGLCOMMANDLISTSEGMENTSNVPROC)wglGetProcAddress("glCommandListSegmentsNV");
    __glewCompileCommandListNV = (PFNGLCOMPILECOMMANDLISTNVPROC)wglGetProcAddress("glCompileCommandListNV");
    __glewCallCommandListNV = (PFNGLCALLCOMMANDLISTNVPROC)wglGetProcAddress("glCallCommandListNV");
    __glewGetCommandHeaderNV = (PFNGLGETCOMMANDHEADERNVPROC)wglGetProcAddress("glGetCommandHeaderNV");
    __glewGetStageIndexNV = (PFNGLGETSTAGEINDEXNVPROC)wglGetProcAddress("glGetStageIndexNV");

    _NV_command_list = __glewCreateStatesNV != NULL;

    return __glewCreateStatesNV != NULL;
}

bool
OGLExtenstion::isSupport(OGLFeatures feature) noexcept
{
    switch (feature)
    {
    case ARB_pixel_format:
        return _ARB_pixel_format;
    case ARB_create_context:
        return _ARB_create_context;
    case ARB_create_context_robustness:
        return _ARB_create_context_robustness;
    case ARB_bindless_texture:
        return _ARB_bindless_texture;
    case ARB_vertex_array_object:
        return _ARB_vertex_array_object;
    case ARB_vertex_attrib_binding:
        return _ARB_vertex_attrib_binding;
    case ARB_provoking_vertex:
        return _ARB_provoking_vertex;
    case ARB_direct_state_access:
        return _ARB_direct_state_access;
    case ARB_viewport_array:
        return _ARB_viewport_array;
    case EXT_swap_control:
        return _EXT_swap_control;
    case NV_command_list:
        return _NV_command_list;
    case NV_shader_buffer_load:
        return _NV_shader_buffer_load;
    case NV_vertex_buffer_unified_memory:
        return _NV_vertex_buffer_unified_memory;
    default:
        return false;
    }
}

_NAME_END