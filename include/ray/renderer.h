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
#ifndef _H_RENDERER_H_
#define _H_RENDERER_H_

#include <ray/render_canvas.h>
#include <ray/render_mesh.h>
#include <ray/render_state.h>
#include <ray/render_texture.h>

_NAME_BEGIN

class EXPORT Renderer final
{
    __DeclareClass(Renderer);
public:
    Renderer() noexcept;
    ~Renderer() noexcept;

    bool open(RenderDevicePtr renderDevice) except;
    void close() noexcept;

    void drawMesh(RenderMeshPtr mesh, const Renderable& renderable) noexcept;
    void updateMesh(RenderMeshPtr mesh, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;

    void renderBegin() noexcept;
    void renderEnd() noexcept;

    void setSwapInterval(SwapInterval interval) noexcept;
    SwapInterval getSwapInterval() const noexcept;

    void setWireframeMode(bool enable) noexcept;
    bool getWireframeMode() const noexcept;

    void setBlendState(const RenderBlendState& state) noexcept;
    void setRasterState(const RenderRasterState& state) noexcept;
    void setDepthState(const RenderDepthState& state) noexcept;
    void setStencilState(const RenderStencilState& state) noexcept;
    void setRenderState(RenderStatePtr state) noexcept;

    void setRenderCanvas(RenderCanvasPtr canvas) noexcept;

    void setRenderTexture(RenderTexturePtr texture) noexcept;
    void setRenderTexture(MultiRenderTexturePtr texture) noexcept;
    void readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
    void copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept;

    void setShaderObject(ShaderObjectPtr shader) noexcept;
    void setShaderParamArgs(const ShaderParamArgs& args) noexcept;

    void present(RenderCanvasPtr canvas) noexcept;

private:

    bool _enableWireframe;

    GPUfbconfig _fbconfig;
    GPUctxconfig _ctxconfig;
};

_NAME_END

#endif