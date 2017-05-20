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
#ifndef _H_EGL2_DEVICE_PROPERTY_H_
#define _H_EGL2_DEVICE_PROPERTY_H_

#include "egl2_types.h"

_NAME_BEGIN

class EGL2DeviceProperty final : public GraphicsDeviceProperty
{
public:
	EGL2DeviceProperty() noexcept;
	~EGL2DeviceProperty() noexcept;

	bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
	void close() noexcept;

	const GraphicsDeviceProperties& getGraphicsDeviceProperties() const noexcept;

private:
#if defined(_WIN32)
	bool setupWGLEnvironment(HWND& hwnd, HDC& hdc, HINSTANCE& hinstance) noexcept;
	bool setupWGLPixelFormat(HDC hdc) noexcept;
	bool setupWGLExtensions(HDC hdc) noexcept;
	void closeWGLEnvironment(HWND hwnd, HDC hdc, HINSTANCE hinstance) noexcept;
#endif

	bool initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept;
	bool initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept;
	bool initDeviceProperties() noexcept;

	bool initTextureSupports() noexcept;
	bool initTextureDimSupports() noexcept;
	bool initVertexSupports() noexcept;
	bool initShaderSupports() noexcept;

private:
	EGL2DeviceProperty(const EGL2DeviceProperty&) = delete;
	EGL2DeviceProperty& operator=(const EGL2DeviceProperty&) = delete;

private:
	GraphicsDeviceProperties _deviceProperties;
};

_NAME_END

#endif