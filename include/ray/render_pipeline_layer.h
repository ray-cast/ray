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
#ifndef _H_RENDER_PIPELINE_H_
#define _H_RENDER_PIPELINE_H_

#include <ray/render_pipeline_base.h>
#include <ray/render_scene.h>

_NAME_BEGIN

class EXPORT RenderPipelineLayer : public RenderPipeline
{
public:
	RenderPipelineLayer() noexcept;
	virtual ~RenderPipelineLayer() noexcept;

	void open(RenderWindowPtr window, std::size_t w, std::size_t h) except;
	void close() noexcept;

	void setCamera(CameraPtr renderer) noexcept;
	CameraPtr getCamera() const noexcept;

	void setWireframeMode(bool enable) noexcept;
	bool getWireframeMode() const noexcept;

	void setSwapInterval(SwapInterval interval) except;
	SwapInterval getSwapInterval() const noexcept;

	void setRenderState(RenderStatePtr state) noexcept;
	RenderStatePtr getRenderState() const noexcept;

	void setWindowResolution(std::size_t w, std::size_t h) noexcept;
	void getWindowResolution(std::size_t& w, std::size_t& h) const noexcept;

	void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept;
	RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) noexcept;

	void setRenderTexture(RenderTexturePtr target) noexcept;
	void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept;
	void setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discradRenderTexture() noexcept;
	void readRenderTexture(RenderTexturePtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	void copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept;

	void setShaderObject(ShaderObjectPtr shader) noexcept;
	ShaderObjectPtr getShaderObject() const noexcept;

	void setMaterialPass(MaterialPassPtr pass) noexcept;
	MaterialPassPtr getMaterialPass() const noexcept;

	void drawCone() noexcept;
	void drawSphere() noexcept;
	void drawSceneQuad(MaterialPassPtr) noexcept;
	void drawRenderIndirect(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr, RenderTexturePtr target = nullptr) noexcept;

	void drawMesh(RenderBufferPtr mesh, const RenderIndirect& renderable) noexcept;
	void updateMesh(RenderBufferPtr mesh, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) except;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;
	void renderPostProcess(RenderTexturePtr renderTexture) except;

	void renderBegin() noexcept;
	void render(const RenderScene& scene) noexcept;
	void renderEnd() noexcept;

protected:

	virtual void assignVisiable(RenderScenePtr scene, CameraPtr camera) noexcept;
	virtual void assignLight(RenderScenePtr scene, CameraPtr camera) noexcept;

	virtual void onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;
	virtual void onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;
	virtual void onRenderObject(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;

	virtual void onActivate() except;
	virtual void onDeactivate() except;

	virtual void onRenderPre(CameraPtr camera) except;
	virtual void onRenderPost(CameraPtr camera) except;
	virtual void onRenderPipeline(CameraPtr camera) except;

private:
	
	std::size_t _width;
	std::size_t _height;

	CameraWeakPtr _camera;

	RenderDevicePtr _renderDevice;
	RenderWindowPtr _renderWindow;

	RenderBufferPtr _renderSceneQuad;
	RenderBufferPtr _renderSphere;
	RenderBufferPtr _renderCone;

	MaterialPassPtr _currentMaterial;

	OcclusionCullList _visiable;

	RenderObjects _renderQueue[RenderQueue::RQ_NUMS][RenderPass::RP_NUMPASS];
	std::vector<RenderPostProcessPtr> _postprocessors[RenderQueue::RQ_NUMS];
};

_NAME_END

#endif