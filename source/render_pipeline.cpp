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
#include <ray/render_pipeline.h>
#include <ray/model.h>
#include <ray/ssao.h>
#include <ray/ssr.h>
#include <ray/dof.h>
#include <ray/hdr.h>
#include <ray/fxaa.h>

_NAME_BEGIN

void
RenderDataManager::addRenderData(RenderQueue queue, RenderObject* object) noexcept
{
    _visiable[queue].push_back(object);
}

RenderData&
RenderDataManager::getRenderData(RenderQueue queue) noexcept
{
    return _visiable[queue];
}

void
RenderDataManager::clear() noexcept
{
    _visiable[RenderQueue::Background].clear();
    _visiable[RenderQueue::Shadow].clear();
    _visiable[RenderQueue::Opaque].clear();
    _visiable[RenderQueue::Transparent].clear();
    _visiable[RenderQueue::DeferredLight].clear();
}

RenderPipeline::RenderPipeline() noexcept
    : _enableSSAO(true)
    , _enableSSR(false)
    , _enableDOF(false)
    , _enableHDR(true)
    , _enableFXAA(true)
{
}

RenderPipeline::~RenderPipeline() noexcept
{
    this->close();
}

void
RenderPipeline::setup(std::size_t width, std::size_t height) except
{
    MeshProperty mesh;
    mesh.makePlane(2, 2, 1, 1);

    _renderSceneQuad = std::make_shared<RenderBuffer>();
    _renderSceneQuad->setup(mesh);

    mesh.makeSphere(1, 16, 12);

    _renderSphere = std::make_shared<RenderBuffer>();
    _renderSphere->setup(mesh);

    mesh.makeCone(1, 1, 16);

    _renderCone = std::make_shared<RenderBuffer>();
    _renderCone->setup(mesh);

    _swapMap = RenderTexture::create();
    _swapMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16A16F);

    _ssao = std::make_shared<SSAO>();
    _ssr = std::make_shared<SSR>();
    //_dof = std::make_shared<DepthOfField>();
    _hdr = std::make_shared<HDR>();
    _fxaa = std::make_shared<FXAA>();
}

void
RenderPipeline::close() noexcept
{
    if (_swapMap)
    {
        _swapMap.reset();
        _swapMap = nullptr;
    }

    if (_renderSceneQuad)
    {
        _renderSceneQuad.reset();
        _renderSceneQuad = nullptr;
    }

    if (_ssao)
    {
        _ssao.reset();
        _ssao = nullptr;
    }

    if (_ssr)
    {
        _ssr.reset();
        _ssr = nullptr;
    }
}

void
RenderPipeline::drawSceneQuad() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderSceneQuad->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderSceneQuad->getNumIndices();
    renderable.numInstances = 0;
    renderer->drawMesh(_renderSceneQuad, renderable);
}

void
RenderPipeline::drawSphere() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderSphere->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderSphere->getNumIndices();
    renderable.numInstances = 0;
    renderer->drawMesh(_renderSphere, renderable);
}

void
RenderPipeline::drawCone() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderCone->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderCone->getNumIndices();
    renderable.numInstances = 0;
    renderer->drawMesh(_renderCone, renderable);
}

void
RenderPipeline::drawRenderable(RenderQueue queue, RenderPass pass, MaterialPassPtr material) noexcept
{
    auto& visiable = renderDataManager.getRenderData(queue);
    for (auto& it : visiable)
    {
        it->render(renderer, queue, pass, material);
    }
}

void
RenderPipeline::setRenderTexture(RenderTexturePtr texture) noexcept
{
    renderer->setRenderTexture(texture);
}

void
RenderPipeline::setRenderTexture(MultiRenderTexturePtr texture) noexcept
{
    renderer->setRenderTexture(texture);
}

void
RenderPipeline::setTechnique(MaterialPassPtr pass) noexcept
{
    renderer->setRenderState(pass->getRenderState());
    renderer->setShaderObject(pass->getShaderObject());
    renderer->setShaderConstantBuffer(pass->getShaderConstantBuffer());
}

void
RenderPipeline::readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    renderer->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipeline::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
    renderer->copyRenderTexture(srcTarget, src, destTarget, dest);
}

void
RenderPipeline::onPostProcess(RenderTexturePtr src) noexcept
{
    Viewport v1(0, 0, _swapMap->getWidth(), _swapMap->getHeight());
    Viewport v2(0, 0, src->getWidth(), src->getHeight());

    if (_enableSSAO)
    {
        _ssao->render(this, src, _swapMap);
        this->copyRenderTexture(_swapMap, v1, src, v2);
    }

    if (_enableSSR)
    {
        _ssr->render(this, src, _swapMap);
        this->copyRenderTexture(_swapMap, v1, src, v2);
    }

    if (_enableDOF)
    {
        _dof->render(this, src, _swapMap);
        this->copyRenderTexture(_swapMap, v1, src, v2);
    }

    if (_enableHDR)
    {
        _hdr->render(this, src, _swapMap);
        this->copyRenderTexture(_swapMap, v1, src, v2);
    }

    if (_enableFXAA)
    {
        _fxaa->render(this, src, _swapMap);
        this->copyRenderTexture(_swapMap, v1, src, v2);
    }
}

_NAME_END