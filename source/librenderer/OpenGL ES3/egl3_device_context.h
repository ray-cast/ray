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
#ifndef _H_EGL3_DEVICE_CONTEXT_H_
#define _H_EGL3_DEVICE_CONTEXT_H_

#include "egl3_types.h"

_NAME_BEGIN

class EGL3DeviceContext final : public GraphicsContext
{
	__DeclareSubClass(EGL3DeviceContext, GraphicsContext)
public:
	EGL3DeviceContext() noexcept;
	~EGL3DeviceContext() noexcept;

	bool setup(const GraphicsContextDesc& desc) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setViewport(std::uint32_t i, const Viewport& viewport) noexcept;
	const Viewport& getViewport(std::uint32_t i) const noexcept;

	void setScissor(std::uint32_t i, const Scissor& scissor) noexcept;
	const Scissor& getScissor(std::uint32_t i) const noexcept;

	void setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
	std::uint32_t getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept;

	void setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept;
	std::uint32_t getStencilReference(GraphicsStencilFaceFlagBits face) noexcept;

	void setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
	std::uint32_t getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept;

	void setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept;
	GraphicsPipelinePtr getRenderPipeline() const noexcept;

	void setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept;
	GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

	void setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept;
	GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept;

	void setIndexBufferData(GraphicsDataPtr data, std::intptr_t offset, GraphicsIndexType indexType) noexcept;
	GraphicsDataPtr getIndexBufferData() const noexcept;

	void generateMipmap(const GraphicsTexturePtr& texture) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discardFramebuffer(GraphicsAttachmentType attachments[], std::size_t i) noexcept;
	void blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept;
	GraphicsFramebufferPtr getFramebuffer() const noexcept;

	void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
	void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;

	bool isTextureSupport(GraphicsFormat format) noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) noexcept;
	bool isVertexSupport(GraphicsFormat format) noexcept;
	bool isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept;

	void present() noexcept;

	void enableDebugControl(bool enable) noexcept;
	void startDebugControl() noexcept;
	void stopDebugControl() noexcept;

private:
	bool initStateSystem() noexcept;

	static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3DeviceContext(const EGL3DeviceContext&) noexcept = delete;
	EGL3DeviceContext& operator=(const EGL3DeviceContext&) noexcept = delete;

private:

	GLfloat _clearDepth;
	GLint   _clearStencil;
	std::vector<float4> _clearColor;

	Viewport _viewport;
	Scissor _scissor;

	GLuint _inputLayout;

	GLenum  _indexType;
	GLintptr _indexOffset;

	EGL3PipelinePtr _pipeline;
	EGL3DescriptorSetPtr _descriptorSet;
	EGL3FramebufferPtr _framebuffer;
	EGL3VertexBuffers _vertexBuffers;
	EGL3GraphicsDataPtr _indexBuffer;
	EGL3ProgramPtr _program;
	EGL3SwapchainPtr _glcontext;
	EGL3GraphicsStatePtr _state;
	GraphicsStateDesc _stateCaptured;

	bool _needUpdatePipeline;
	bool _needUpdateDescriptor;
	bool _needUpdateVertexBuffers;
	bool _needEnableDebugControl;
	bool _needDisableDebugControl;

	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif