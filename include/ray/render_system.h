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
#ifndef _H_RENDER_SYSTEM_H_
#define _H_RENDER_SYSTEM_H_

#include <ray/render_setting.h>
#include <ray/graphics_data.h>
#include <ray/graphics_layout.h>

_NAME_BEGIN

class EXPORT RenderSystem final
{
	__DeclareSingleton(RenderSystem)
public:
	RenderSystem() noexcept;
	~RenderSystem() noexcept;

	void open(WindHandle window, std::size_t w, std::size_t h) except;
	void close() noexcept;

	void setRenderSetting(const RenderSetting& setting) except;
	const RenderSetting& getRenderSetting() const noexcept;

	void setRenderPipeline(RenderPipelinePtr pipeline) except;
	RenderPipelinePtr getRenderPipeline() const noexcept;

	void setWireframeMode(bool enable) noexcept;
	bool getWireframeMode() const noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) except;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void setSwapInterval(SwapInterval interval) except;
	SwapInterval getSwapInterval() const noexcept;

	bool addRenderScene(RenderScenePtr scene) noexcept;
	void removeRenderScene(RenderScenePtr scene) noexcept;

	TexturePtr createTexture() noexcept;
	TexturePtr createTexture(const std::string& name) except;

	MaterialPtr createMaterial(const std::string& name) noexcept;

	RenderTexturePtr createRenderTexture() noexcept;
	MultiRenderTexturePtr createMultiRenderTexture() noexcept;

	RenderStatePtr createRenderState() noexcept;

	ShaderPtr createShader() noexcept;
	ShaderObjectPtr createShaderObject() noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	GraphicsLayoutPtr createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept;

	RenderBufferPtr createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) except;
	RenderBufferPtr createRenderBuffer(const MeshPropertys& meshes) except;

	void render() noexcept;

private:
	void onAddCamera(CameraPtr camera);

private:
	RenderSystem(const RenderSystem&) noexcept = delete;
	RenderSystem& operator=(const RenderSystem&) noexcept = delete;

private:

	RenderSetting _setting;

	RenderScenes _sceneList;
	RenderPipelinePtr _renderPipeline;

	RenderPostProcessPtr _SSGI;
	RenderPostProcessPtr _SSAO;
	RenderPostProcessPtr _SAT;
	RenderPostProcessPtr _SSR;
	RenderPostProcessPtr _SSSS;
	RenderPostProcessPtr _DOF;
	RenderPostProcessPtr _fog;
	RenderPostProcessPtr _lightShaft;
	RenderPostProcessPtr _fimicToneMapping;
	RenderPostProcessPtr _FXAA;
	RenderPostProcessPtr _colorGrading;
};

_NAME_END

#endif