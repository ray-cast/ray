// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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

#include "ogl_types.h"

_NAME_BEGIN

class WGLCanvas : public GraphicsSwapchain
{
public:
	WGLCanvas() noexcept;
	virtual ~WGLCanvas() noexcept;

	bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
	void close() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() const noexcept;

	void setSwapInterval(SwapInterval interval) noexcept;
	SwapInterval getSwapInterval() const noexcept;

	void present() noexcept;

	const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept;

private:
	friend class OGLDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	static bool initWGLExtensions(HDC hdc) except;
	static void initPixelFormat(GPUfbconfig& fbconfig, GPUctxconfig& ctxconfig) noexcept;

private:
	WGLCanvas(const WGLCanvas&) noexcept = delete;
	WGLCanvas& operator=(const WGLCanvas&) noexcept = delete;

private:

	static int initWGLExtention;

	static bool _ARB_pixel_format;
	static bool _ARB_create_context;
	static bool _ARB_create_context_robustness;

	static bool _EXT_swap_control;

	bool _isActive;

	HDC _hdc;
	HGLRC _context;

	GPUfbconfig _fbconfig;
	GPUctxconfig _ctxconfig;

	GraphicsSwapchainDesc _swapchainDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif