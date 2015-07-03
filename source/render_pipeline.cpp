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
#include <ray/post_process.h>
#include <ray/model.h>
#include <ray/render_scene.h>
#include <ray/render_factory.h>

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
    _visiable[RenderQueue::Opaque].clear();
    _visiable[RenderQueue::Transparent].clear();
    _visiable[RenderQueue::Lighting].clear();
    _visiable[RenderQueue::PostProcess].clear();
}

RenderPipeline::RenderPipeline() noexcept
    : _enableWireframe(false)
{
}

RenderPipeline::~RenderPipeline() noexcept
{
    this->close();
}

void
RenderPipeline::setup(RenderDevicePtr renderDevice, std::size_t width, std::size_t height) except
{
    _renderer = renderDevice;
    _width = width;
    _height = height;

    MeshProperty mesh;
    mesh.makePlane(2, 2, 1, 1);

    _renderSceneQuad = RenderFactory::createRenderBuffer(mesh);

    mesh.makeSphere(1, 16, 12);

    _renderSphere = RenderFactory::createRenderBuffer(mesh);

    mesh.makeCone(1, 1, 16);

    _renderCone = RenderFactory::createRenderBuffer(mesh);

    this->onActivate();
}

void
RenderPipeline::close() noexcept
{
    this->onDectivate();

    if (_renderSceneQuad)
    {
        _renderSceneQuad.reset();
        _renderSceneQuad = nullptr;
    }

    if (_renderSphere)
    {
        _renderSphere.reset();
        _renderSphere = nullptr;
    }

    if (_renderCone)
    {
        _renderCone.reset();
        _renderCone = nullptr;
    }

    for (auto& it : _postprocessors)
    {
        it->onDeactivate(*this);
    }
}

void
RenderPipeline::setCamera(Camera* camera) noexcept
{
    _camera = camera;
}

Camera*
RenderPipeline::getCamera() const noexcept
{
    return _camera;
}

void
RenderPipeline::addRenderData(RenderQueue queue, RenderObject* object) noexcept
{
    _renderDataManager.addRenderData(queue, object);
}

RenderData&
RenderPipeline::getRenderData(RenderQueue queue) noexcept
{
    return _renderDataManager.getRenderData(queue);
}

void
RenderPipeline::drawMesh(RenderBufferPtr buffer, const Renderable& renderable) noexcept
{
    _renderer->setRenderBuffer(buffer);
    if (_enableWireframe)
    {
        Renderable change = renderable;
        change.type = VertexType::GPU_LINE;
        _renderer->drawRenderBuffer(change);
    }
    else
    {
        _renderer->drawRenderBuffer(renderable);
    }
}

void
RenderPipeline::updateMesh(RenderBufferPtr buffer, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    _renderer->updateRenderBuffer(buffer);
}

void
RenderPipeline::setRenderTarget(RenderTargetPtr target) noexcept
{
    assert(target);
    _renderer->setRenderTarget(target);
}

void
RenderPipeline::setRenderTarget(MultiRenderTargetPtr target) noexcept
{
    assert(target);
    _renderer->setMultiRenderTarget(target);
}

void
RenderPipeline::readRenderTarget(RenderTargetPtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    _renderer->readRenderTarget(texture, pfd, w, h, data);
}

void
RenderPipeline::copyRenderTarget(RenderTargetPtr srcTarget, const Viewport& src, RenderTargetPtr destTarget, const Viewport& dest) noexcept
{
    _renderer->copyRenderTarget(srcTarget, src, destTarget, dest);
}

void
RenderPipeline::setViewport(const Viewport& viewport) noexcept
{
    _renderer->setViewport(viewport);
}

const Viewport&
RenderPipeline::getViewport() const noexcept
{
    return _renderer->getViewport();
}

void
RenderPipeline::setRenderState(RenderStatePtr state) noexcept
{
    _renderer->setRenderState(state);
}

void
RenderPipeline::setShaderObject(ShaderObjectPtr shader) noexcept
{
    _renderer->setShaderObject(shader);
}

void
RenderPipeline::setShaderVariant(ShaderVariantPtr constant, ShaderUniformPtr uniform) noexcept
{
    _renderer->setShaderUniform(uniform, constant);
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

    this->drawMesh(_renderSceneQuad, renderable);
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

    this->drawMesh(_renderSphere, renderable);
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

    this->drawMesh(_renderCone, renderable);
}

void
RenderPipeline::drawRenderable(RenderQueue queue, RenderPass pass, MaterialPassPtr material) noexcept
{
    auto& renderable = _renderDataManager.getRenderData(queue);
    for (auto& it : renderable)
    {
        this->onRenderObjectPre(*it, queue, pass, material);
        this->onRenderObject(*it, queue, pass, material);
        this->onRenderObjectPost(*it, queue, pass, material);
    }
}

void
RenderPipeline::setTechnique(MaterialPassPtr pass) noexcept
{
    this->setRenderState(pass->getRenderState());
    this->setShaderObject(pass->getShaderObject());
}

void
RenderPipeline::setWireframeMode(bool enable) noexcept
{
    _enableWireframe = enable;
}

bool
RenderPipeline::getWireframeMode() const noexcept
{
    return _enableWireframe;
}

void
RenderPipeline::setWindowResolution(std::size_t w, std::size_t h) noexcept
{
    if (_width != w || _height != h)
    {
        _width = w;
        _height = h;
    }
}

std::size_t
RenderPipeline::getWindowWidth() const noexcept
{
    return _width;
}

std::size_t
RenderPipeline::getWindowHeight() const noexcept
{
    return _height;
}

void
RenderPipeline::addPostProcess(RenderPostProcessPtr postprocess) except
{
    assert(std::find(_postprocessors.begin(), _postprocessors.end(), postprocess) == _postprocessors.end());

    postprocess->onActivate(*this);

    _postprocessors.push_back(postprocess);
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
    auto it = std::find(_postprocessors.begin(), _postprocessors.end(), postprocess);
    if (it != _postprocessors.end())
    {
        postprocess->onDeactivate(*this);

        _postprocessors.erase(it);
    }
}

void
RenderPipeline::applyPostProcess(RenderTargetPtr renderTexture) except
{
    auto clearFlags = renderTexture->getClearFlags();
    renderTexture->setClearFlags(ClearFlags::CLEAR_NONE);

    for (auto& it : _postprocessors)
        it->onPreRender(*this);

    for (auto& it : _postprocessors)
        it->onRender(*this, renderTexture);

    for (auto& it : _postprocessors)
        it->onPostRender(*this);

    renderTexture->setClearFlags(clearFlags);
}

void
RenderPipeline::render() noexcept
{
    assert(_camera);

    _renderDataManager.clear();

    this->onRenderPre();
    this->onRenderPipeline();
    this->onRenderPost();
}

void
RenderPipeline::assignVisiable(Camera* camera) noexcept
{
    assert(camera);

    auto scene = camera->getRenderScene();
    if (scene)
    {
        _visiable.clear();

        scene->computVisiable(camera, _visiable);

        for (auto& it : _visiable)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            this->collection(*it);
        }
    }
}

void
RenderPipeline::assignLight(Camera* camera) noexcept
{
    auto scene = camera->getRenderScene();
    if (scene)
    {
        _lights.clear();

        scene->computVisiableLight(camera, _lights);

        for (auto& it : _lights)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            _renderDataManager.addRenderData(RenderQueue::Lighting, it);
        }
    }
}

void
RenderPipeline::collection(RenderObject& object) noexcept
{
    if (this->getCamera()->getCameraOrder() == CameraOrder::CO_SHADOW)
    {
        if (object.getCastShadow())
        {
        }
    }

    auto material = object.getMaterial();
    if (material)
    {
        auto& techiniques = material->getTechs();
        for (auto& technique : techiniques)
        {
            auto queue = technique->getRenderQueue();
            _renderDataManager.addRenderData(queue, &object);
        }
    }
}

void
RenderPipeline::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) except
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setMatrixParam(GlobalMatrixSemantic::matModel, object.getTransform());
    semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverse, object.getTransformInverse());
    semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverseTranspose, object.getTransformInverseTranspose());

    auto pass = _pass ? _pass : object.getMaterial()->getTech(queue)->getPass(passType);

    if (pass)
    {
        if (!_pass)
        {
            RenderStencilState stencil = pass->getRenderState()->getStencilState();
            stencil.stencilEnable = true;
            stencil.stencilPass = StencilOperation::STENCILOP_REPLACE;
            stencil.stencilFunc = CompareFunction::GPU_ALWAYS;
            stencil.stencilRef = 1 << object.getLayer();
            stencil.stencilReadMask = 0xFFFFFFFF;

            pass->getRenderState()->setStencilState(stencil);
        }

        this->setRenderState(pass->getRenderState());
        this->setShaderObject(pass->getShaderObject());

        this->drawMesh(object.getRenderBuffer(), *object.getRenderable());
    }
}

void
RenderPipeline::onActivate() except
{
}

void
RenderPipeline::onDectivate() except
{
}

void
RenderPipeline::onRenderPre() except
{
}

void
RenderPipeline::onRenderPost() except
{
}

void
RenderPipeline::onRenderPipeline() except
{
}

_NAME_END