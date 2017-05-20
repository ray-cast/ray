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
#include "egl3_device_property.h"

_NAME_BEGIN

EGL3DeviceProperty::EGL3DeviceProperty() noexcept
{
}

EGL3DeviceProperty::~EGL3DeviceProperty() noexcept
{
	this->close();
}

bool
EGL3DeviceProperty::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
{
	EGLConfig config;
	if (!initPixelFormat(deviceDesc, config))
		return false;

	if (!initContext(deviceDesc, config))
		return false;

	return true;
}

void
EGL3DeviceProperty::close() noexcept
{
}

bool
EGL3DeviceProperty::initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept
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
EGL3DeviceProperty::initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept
{
	EGLint attribIndex = 0;

	EGLint attribs[80];

#if !defined(_BUILD_PLATFORM_ANDROID)
	attribs[attribIndex++] = EGL_CONTEXT_MAJOR_VERSION;
	attribs[attribIndex++] = 3;

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
EGL3DeviceProperty::initDeviceProperties() noexcept
{
	static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

	EGL3Types::setup();

	this->initTextureSupports();
	this->initVertexSupports();
	this->initTextureDimSupports();
	this->initShaderSupports();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension3D);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint*)&_deviceProperties.maxImageArrayLayers);
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE_EXT, (GLint*)&_deviceProperties.maxTexelBufferElements);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*)&_deviceProperties.maxUniformBufferRange);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, (GLint*)&_deviceProperties.maxStorageBufferRange);

	glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSamplers);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorUniformBuffers);
	glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageBuffers);
	glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorSampledImages);
	glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageDescriptorInputAttachments);
	glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint*)&_deviceProperties.maxPerStageDescriptorStorageImages);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&_deviceProperties.maxPerStageResources);

	glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&_deviceProperties.maxDescriptorSetSamplers);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffers);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetUniformBuffersDynamic);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffers);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint*)&_deviceProperties.maxDescriptorSetStorageBuffersDynamic);
	glGetIntegerv(GL_MAX_IMAGE_UNITS, (GLint*)&_deviceProperties.maxDescriptorSetSampledImages);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, (GLint*)&_deviceProperties.maxVertexInputBindings);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint*)&_deviceProperties.maxVertexInputAttributeOffset);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, (GLint*)&_deviceProperties.maxVertexInputBindingStride);
	glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_tessellation_shader))
	{
		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL_EXT, (GLint*)&_deviceProperties.maxTessellationGenerationLevel);
		glGetIntegerv(GL_MAX_PATCH_VERTICES_EXT, (GLint*)&_deviceProperties.maxTessellationPatchSize);
		glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationControlPerVertexInputComponents);
		glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationControlPerVertexOutputComponents);
		glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationControlPerPatchOutputComponents);
		glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationControlTotalOutputComponents);
		glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationEvaluationInputComponents);
		glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxTessellationEvaluationOutputComponents);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_geometry_shader))
	{
		glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS_EXT, (GLint*)&_deviceProperties.maxGeometryShaderInvocations);
		glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxGeometryInputComponents);
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxGeometryOutputComponents);
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, (GLint*)&_deviceProperties.maxGeometryOutputVertices);
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT, (GLint*)&_deviceProperties.maxGeometryTotalOutputComponents);
	}

	glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, (GLint*)&_deviceProperties.maxComputeSharedMemorySize);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupCount[2]);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, (GLint*)&_deviceProperties.maxComputeWorkGroupInvocations);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, (GLint*)&_deviceProperties.maxComputeWorkGroupSize[2]);

	glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint*)&_deviceProperties.maxFragmentInputComponents);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&_deviceProperties.maxFragmentOutputAttachments);
	glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, (GLint*)&_deviceProperties.maxFragmentCombinedOutputResources);
	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
	glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);
	glGetIntegerv(GL_MAX_ELEMENT_INDEX, (GLint*)&_deviceProperties.maxDrawIndexedIndexValue);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint*)&_deviceProperties.maxDrawIndirectCount);

	glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &_deviceProperties.maxSamplerLodBias);
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_deviceProperties.maxSamplerAnisotropy);
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
	glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minUniformBufferOffsetAlignment);
	glGetInteger64v(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, (GLint64*)&_deviceProperties.minStorageBufferOffsetAlignment);
	glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.minTexelOffset);
	glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, (GLint*)&_deviceProperties.maxTexelOffset);
	glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.minTexelGatherOffset);
	glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint*)&_deviceProperties.minTexelGatherOffset);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, (GLint*)&_deviceProperties.maxFramebufferWidth);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, (GLint*)&_deviceProperties.maxFramebufferHeight);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS_EXT, (GLint*)&_deviceProperties.maxFramebufferLayers);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*)&_deviceProperties.maxFramebufferColorAttachments);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferColorSampleCounts);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferDepthSampleCounts);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferStencilSampleCounts);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.framebufferNoAttachmentsSampleCounts);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES, (GLint*)&_deviceProperties.sampledImageIntegerSampleCounts);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageColorSampleCounts);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageDepthSampleCounts);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.sampledImageStencilSampleCounts);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint*)&_deviceProperties.storageImageSampleCounts);
	glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, (GLint*)&_deviceProperties.maxSampleMaskWords);

	glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

	return true;
}

bool
EGL3DeviceProperty::initTextureSupports() noexcept
{
	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_required_internalformat))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_read_format_bgra))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_ARM_rgba8) || EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_rgb8_rgba8))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_format_BGRA8888) || EGL3Types::isSupportFeature(EGL3Features::EGL3_APPLE_texture_format_BGRA8888))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_sRGB))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_NV_sRGB_formats))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_rg))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_norm16))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_half_float) || EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_color_buffer_half_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_float) || EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_color_buffer_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_type_2_10_10_10_REV))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_APPLE_texture_packed_float))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth_texture))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth24))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth32))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_stencil8))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm_S8UInt);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_compression_dxt1))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_compression_s3tc))
	{
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC2UNormBlock);
		_deviceProperties.supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_KHR_texture_compression_astc_ldr))
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
EGL3DeviceProperty::initTextureDimSupports() noexcept
{
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_cube_map_array))
		_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_3D))
		_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim3D);

	return true;
}

bool
EGL3DeviceProperty::initVertexSupports() noexcept
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

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_vertex_type_10_10_10_2))
	{
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_vertex_half_float))
	{
		_deviceProperties.supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	}

	return true;
}

bool
EGL3DeviceProperty::initShaderSupports() noexcept
{
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);
	_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_geometry_shader))
		_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit);

	return true;
}

const GraphicsDeviceProperties&
EGL3DeviceProperty::getGraphicsDeviceProperties() const noexcept
{
	return _deviceProperties;
}

_NAME_END