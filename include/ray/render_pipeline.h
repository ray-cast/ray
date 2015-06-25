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

#include <ray/render_device.h>
#include <ray/render_object.h>

_NAME_BEGIN

typedef std::vector<RenderObject*> RenderData;
class RenderDataManager
{
public:

    void addRenderData(RenderQueue queue, RenderObject* object) noexcept;

    RenderData& getRenderData(RenderQueue queue) noexcept;

    void clear() noexcept;

private:

    RenderData _visiable[RenderQueue::RenderQueueComits];
};

class RenderPipeline
{
public:
    RenderPipeline() noexcept;
    virtual ~RenderPipeline() noexcept;

    virtual void setup(RenderDevicePtr renderDevice, std::size_t width, std::size_t height) except;
    virtual void close() noexcept;

    void setCamera(Camera* renderer) noexcept;
    Camera* getCamera() const noexcept;

    void addRenderData(RenderQueue queue, RenderObject* object) noexcept;
    RenderData& getRenderData(RenderQueue queue) noexcept;

    void drawMesh(RenderBufferPtr mesh, const Renderable& renderable) noexcept;
    void updateMesh(RenderBufferPtr mesh, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;

    void setRenderState(RenderStatePtr state) noexcept;

    void setRenderTarget(RenderTargetPtr texture) noexcept;
    void setRenderTarget(MultiRenderTargetPtr texture) noexcept;
    void readRenderTarget(RenderTargetPtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
    void copyRenderTarget(RenderTargetPtr srcTarget, const Viewport& src, RenderTargetPtr destTarget, const Viewport& dest) noexcept;

    void setShaderObject(ShaderObjectPtr shader) noexcept;
    void setShaderVariant(ShaderVariantPtr buffer, ShaderUniformPtr uniform) noexcept;

    void drawSceneQuad() noexcept;
    void drawSphere() noexcept;
    void drawCone() noexcept;
    void drawRenderable(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr) noexcept;

    void setWireframeMode(bool enable) noexcept;
    bool getWireframeMode() const noexcept;

    void setWindowResolution(std::size_t w, std::size_t h) noexcept;
    std::size_t getWindowWidth() const noexcept;
    std::size_t getWindowHeight() const noexcept;

    void setTechnique(MaterialPassPtr pass) noexcept;

    void addPostProcess(RenderPostProcessPtr postprocess) except;
    void removePostProcess(RenderPostProcessPtr postprocess) noexcept;

    void render() noexcept;

private:

    void assignVisiable() noexcept;
    void assignLight() noexcept;

    virtual void onActivate() except;
    virtual void onDectivate() except;

    virtual void onRenderPre() except;
    virtual void onRenderPost() except;
    virtual void onRenderPipeline() except;

private:

    std::size_t _width;
    std::size_t _height;

    bool _enableWireframe;

    Camera* _camera;

    RenderDevicePtr _renderer;

    RenderBufferPtr _renderSceneQuad;
    RenderBufferPtr _renderSphere;
    RenderBufferPtr _renderCone;

    RenderDataManager _renderDataManager;

    std::vector<Light*> _lights;
    std::vector<RenderObject*> _visiable;
    std::vector<RenderPostProcessPtr> _postprocessors;
};

_NAME_END

#endif