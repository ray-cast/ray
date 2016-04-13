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
#ifndef _H_RENDER_PIPELINE_DEVICE_H_
#define _H_RENDER_PIPELINE_DEVICE_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT RenderPipelineDevice : public rtti::Interface
{
	__DeclareSubClass(RenderPipelineDevice, rtti::Interface)
public:
	RenderPipelineDevice() noexcept;
	virtual ~RenderPipelineDevice() noexcept;

	bool open(GraphicsDeviceType type) noexcept;
	void close() noexcept;

	RenderPipelinePtr createRenderPipeline(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	void destroyMaterial(MaterialPtr material) noexcept;

	MaterialVariantPtr createSemantic(const std::string& name, GraphicsUniformType type) noexcept;
	void destroySemantic(MaterialVariantPtr semantic) const noexcept;
	MaterialVariantPtr getSemantic(const std::string& semantic) const noexcept;

	GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept;
	GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name, GraphicsTextureDim dim) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;

	RenderMeshPtr createRenderMesh(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderMeshPtr createRenderMesh(const MeshProperty& mesh) noexcept;
	RenderMeshPtr createRenderMesh(const MeshPropertys& mesh) noexcept;

private:
	RenderPipelineDevice(const RenderPipelineDevice&) noexcept = delete;
	RenderPipelineDevice& operator=(const RenderPipelineDevice&) noexcept = delete;

private:
	GraphicsDevicePtr _graphicsDevice;
	MaterialManagerPtr _materialManager;
};

_NAME_END

#endif