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
#include "egl2_device_property.h"

_NAME_BEGIN

EGL2DeviceProperty::EGL2DeviceProperty() noexcept
{
}

EGL2DeviceProperty::~EGL2DeviceProperty() noexcept
{
	this->close();
}

bool
EGL2DeviceProperty::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
{
	EGLConfig config;
	if (!initPixelFormat(deviceDesc, config))
		return false;

	if (!initContext(deviceDesc, config))
		return false;

	return true;
}

void
EGL2DeviceProperty::close() noexcept
{
}

bool
EGL2DeviceProperty::initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept
{
	EGLint pixelFormat[80];
	EGLint index = 0;

	pixelFormat[index++] = EGL_SURFACE_TYPE;
	pixelFormat[index++] = EGL_WINDOW_BIT;

	pixelFormat[index++] = EGL_RENDERABLE_TYPE;
	pixelFormat[index++] = EGL_OPENGL_ES2_BIT;

	pixelFormat[index++] = EGL_BUFFER_SIZE;
	pixelFormat[index++] = 32;

	pixelFormat[index++] = EGL_RED_SIZE;
	pixelFormat[index++] = 8;

	pixelFormat[index++] = EGL_GREEN_SIZE;
	pixelFormat[index++] = 8;

	pixelFormat[index++] = EGL_BLUE_SIZE;
	pixelFormat[index++] = 8;

	pixelFormat[index++] = EGL_ALPHA_SIZE;
	pixelFormat[index++] = 8;

	pixelFormat[index++] = EGL_DEPTH_SIZE;
	pixelFormat[index++] = 16;

	pixelFormat[index++] = EGL_STENCIL_SIZE;
	pixelFormat[index++] = 0;

	pixelFormat[index++] = EGL_NONE;

	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY)
	{
		GL_PLATFORM_LOG("eglGetDisplay() fail : %d", eglGetError());
		return false;
	}

	if (::eglInitialize(display, nullptr, nullptr) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglInitialize() fail : %d", eglGetError());
		return false;
	}

	if (::eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglBindAPI() fail : %d", eglGetError());
		return false;
	}

	EGLint num = 0;
	if (::eglChooseConfig(display, pixelFormat, &config, 1, &num) == EGL_FALSE)
	{
		GL_PLATFORM_LOG("eglChooseConfig() fail : %d", eglGetError());
		return false;
	}

	return true;
}

bool
EGL2DeviceProperty::initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept
{
	EGLint attribIndex = 0;

	EGLint attribs[80];

#if !defined(_BUILD_PLATFORM_ANDROID)
	attribs[attribIndex++] = EGL_CONTEXT_MAJOR_VERSION;
	attribs[attribIndex++] = 2;

	attribs[attribIndex++] = EGL_CONTEXT_MINOR_VERSION;
	attribs[attribIndex++] = 0;
#else
	attribs[attribIndex++] = EGL_CONTEXT_CLIENT_VERSION;
	attribs[attribIndex++] = 2;
#endif

	attribs[attribIndex++] = EGL_NONE;
	attribs[attribIndex++] = EGL_NONE;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLNativeWindowType hwnd = (EGLNativeWindowType)deviceDesc.getWindHandle();
	EGLSurface surface = ::eglCreateWindowSurface(display, config, hwnd, NULL);
	if (::eglGetError() != EGL_SUCCESS)
	{
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	EGLContext context = ::eglCreateContext(display, config, EGL_NO_CONTEXT, attribs);
	if (eglGetError() != EGL_SUCCESS)
	{
		::eglDestroySurface(display, surface);
		GL_PLATFORM_LOG("eglCreateContext() fail : %d", eglGetError());
		return false;
	}

	::eglMakeCurrent(display, surface, surface, context);

	this->initDeviceProperties();

	::eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	::eglDestroyContext(display, context);
	::eglDestroySurface(display, surface);

	return true;
}

bool
EGL2DeviceProperty::initDeviceProperties() noexcept
{
	static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

	EGL2Types::setup();

	this->initTextureSupports();
	this->initVertexSupports();
	this->initTextureDimSupports();
	this->initShaderSupports();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputBindings);
	glGetIntegerv(GL_MAX_VARYING_VECTORS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxFragmentInputComponents);

	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);

	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
	
	glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

	return true;
}

bool
EGL2DeviceProperty::initTextureSupports() noexcept
{
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_required_internalformat))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_read_format_bgra))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_ARM_rgba8) || EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_rgb8_rgba8))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_format_BGRA8888) || EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_format_BGRA8888))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_sRGB))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_NV_sRGB_formats))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_rg))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_norm16))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_half_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_half_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_type_2_10_10_10_REV))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_packed_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth_texture))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth24))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth32))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_stencil8))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	}

	/*if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_packed_depth_stencil))
	{
	_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}*/

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_dxt1))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_s3tc))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC2UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_KHR_texture_compression_astc_ldr))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);
	}

	return true;
}

bool
EGL2DeviceProperty::initTextureDimSupports() noexcept
{
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_cube_map_array))
		_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	return true;
}

bool
EGL2DeviceProperty::initVertexSupports() noexcept
{
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_type_10_10_10_2))
	{
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_half_float))
	{
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	}

	return true;
}

bool
EGL2DeviceProperty::initShaderSupports() noexcept
{
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);
	return true;
}

const GraphicsDeviceProperties&
EGL2DeviceProperty::getGraphicsDeviceProperties() const noexcept
{
	return _deviceProperties;
}

_NAME_END