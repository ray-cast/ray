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

#include <ray/render_object.h>
#include <ray/render_buffer.h>
#include <ray/renderer.h>

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

    virtual void setup(std::size_t width, std::size_t height) except;
    virtual void close() noexcept = 0;

    virtual void render() noexcept = 0;

    void drawSceneQuad() noexcept;
    void drawSphere() noexcept;
    void drawCone() noexcept;
    void drawRenderable(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr) noexcept;

    void setRenderTexture(RenderTexturePtr texture) noexcept;
    void setRenderTexture(MultiRenderTexturePtr texture) noexcept;

    void setTechnique(MaterialPassPtr pass) noexcept;

    void readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
    void copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept;

    void onPostProcess(RenderTexturePtr src) noexcept;

private:

    RenderBufferPtr _renderSceneQuad;
    RenderBufferPtr _renderSphere;
    RenderBufferPtr _renderCone;

    RenderTexturePtr _swapMap;

    bool _enableSSAO;
    bool _enableSSR;
    bool _enableDOF;
    bool _enableHDR;
    bool _enableFXAA;

    RenderPostProcessPtr _ssao;
    RenderPostProcessPtr _ssr;
    RenderPostProcessPtr _dof;
    RenderPostProcessPtr _hdr;
    RenderPostProcessPtr _fxaa;

public:
    RendererPtr renderer;

    Camera* camera;
    RenderScene* scene;

    std::vector<Camera*> shadow;
    std::vector<Light*> lights;
    std::vector<RenderObject*> visiable;

    RenderDataManager renderDataManager;
};

_NAME_END

#endif