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
#include <ray/render_system.h>
#include <ray/material_maker.h>
#include <ray/forward_shading.h>
#include <ray/deferred_lighting.h>

_NAME_BEGIN

__ImplementClass(RenderSystem)

RenderSystem::RenderSystem() noexcept
    : _globalColor(1.0f, 1.0f, 1.0f, 0.5f)
{
}

RenderSystem::~RenderSystem() noexcept
{
    this->close();
}

bool
RenderSystem::setup(RenderDevicePtr device, RenderWindowPtr window, WindHandle win, int width, int height) except
{
    _renderer = Renderer::create();
    if (!_renderer->open(device))
        return false;

    _renderer->setSwapInterval(SwapInterval::GPU_VSYNC);

    _renderWindow = window;
    _renderWindow->setup(win);

    Material::setMaterialSemantic(std::make_shared<MaterialSemantic>());

    _renderPipeline = std::make_unique<DeferredLighting>();
    _renderPipeline->setup(width, height);

    _lineMaterial = MaterialMaker("sys:fx\\lines.glsl");

    VertexComponents components;
    components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
    components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));

    _dynamicBuffers = std::make_shared<VertexBufferData>();
    _dynamicBuffers->setup(1, sizeof(SimpleVertex), VertexUsage::GPU_USAGE_DYNAMIC);
    _dynamicBuffers->setVertexComponents(components);

    _renderBuffer = std::make_unique<RenderMesh>();
    _renderBuffer->setup(_dynamicBuffers, nullptr);

    float ratio = (float)width / height;
    _orthoCamera.makeOrtho(-ratio, ratio, -1, 1, 0, 1000);

    return true;
}

void
RenderSystem::close() noexcept
{
    auto semantic = Material::getMaterialSemantic();
    if (semantic)
    {
        semantic->close();
        Material::setMaterialSemantic(nullptr);
    }

    if (_lineMaterial)
    {
        _lineMaterial.reset();
        _lineMaterial = nullptr;
    }

    if (_renderBuffer)
    {
        _renderBuffer.reset();
        _renderBuffer = nullptr;
    }

    if (_renderPipeline)
    {
        _renderPipeline.reset();
        _renderPipeline = nullptr;
    }

    if (_renderWindow)
    {
        _renderWindow.reset();
        _renderWindow = nullptr;
    }

    if (_renderer)
    {
        _renderer.reset();
        _renderer = nullptr;
    }
}

void
RenderSystem::setSwapInterval(SwapInterval interval) noexcept
{
    _renderer->setSwapInterval(interval);
}

SwapInterval
RenderSystem::getSwapInterval() const noexcept
{
    return _renderer->getSwapInterval();
}

void
RenderSystem::setRenderWindow(RenderWindowPtr window) noexcept
{
    _renderWindow = window;
}

RenderWindowPtr
RenderSystem::getRenderWindow() const noexcept
{
    return _renderWindow;
}

void
RenderSystem::setTimer(TimerPtr timer) noexcept
{
    _timer = timer;
}

TimerPtr
RenderSystem::getTimer() const noexcept
{
    return _timer;
}

bool
RenderSystem::addRenderScene(RenderScenePtr scene) noexcept
{
    auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
    if (it == _sceneList.end())
    {
        _sceneList.push_back(scene);
        return true;
    }

    return false;
}

void
RenderSystem::removeRenderScene(RenderScenePtr scene) noexcept
{
    auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
    if (it != _sceneList.end())
    {
        _sceneList.erase(it);
    }
}

void
RenderSystem::drawAABB(const Vector3& min, const Vector3& max, const Vector4& color) noexcept
{
    drawLineColor(Vector3(min[0], min[1], min[2]), Vector3(max[0], min[1], min[2]), color);
    drawLineColor(Vector3(max[0], min[1], min[2]), Vector3(max[0], max[1], min[2]), color);
    drawLineColor(Vector3(max[0], max[1], min[2]), Vector3(min[0], max[1], min[2]), color);
    drawLineColor(Vector3(min[0], max[1], min[2]), Vector3(min[0], min[1], min[2]), color);
    drawLineColor(Vector3(min[0], min[1], min[2]), Vector3(min[0], min[1], max[2]), color);
    drawLineColor(Vector3(max[0], min[1], min[2]), Vector3(max[0], min[1], max[2]), color);
    drawLineColor(Vector3(max[0], max[1], min[2]), Vector3(max[0], max[1], max[2]), color);
    drawLineColor(Vector3(min[0], max[1], min[2]), Vector3(min[0], max[1], max[2]), color);
    drawLineColor(Vector3(min[0], min[1], max[2]), Vector3(max[0], min[1], max[2]), color);
    drawLineColor(Vector3(max[0], min[1], max[2]), Vector3(max[0], max[1], max[2]), color);
    drawLineColor(Vector3(max[0], max[1], max[2]), Vector3(min[0], max[1], max[2]), color);
    drawLineColor(Vector3(min[0], max[1], max[2]), Vector3(min[0], min[1], max[2]), color);
}

void
RenderSystem::drawAABB(const Vector3& min, const Vector3& max, const Matrix4x4& trans, const Vector4& color) noexcept
{
    drawLineColor(trans * Vector3(min[0], min[1], min[2]), trans * Vector3(max[0], min[1], min[2]), color);
    drawLineColor(trans * Vector3(max[0], min[1], min[2]), trans * Vector3(max[0], max[1], min[2]), color);
    drawLineColor(trans * Vector3(max[0], max[1], min[2]), trans * Vector3(min[0], max[1], min[2]), color);
    drawLineColor(trans * Vector3(min[0], max[1], min[2]), trans * Vector3(min[0], min[1], min[2]), color);
    drawLineColor(trans * Vector3(min[0], min[1], min[2]), trans * Vector3(min[0], min[1], max[2]), color);
    drawLineColor(trans * Vector3(max[0], min[1], min[2]), trans * Vector3(max[0], min[1], max[2]), color);
    drawLineColor(trans * Vector3(max[0], max[1], min[2]), trans * Vector3(max[0], max[1], max[2]), color);
    drawLineColor(trans * Vector3(min[0], max[1], min[2]), trans * Vector3(min[0], max[1], max[2]), color);
    drawLineColor(trans * Vector3(min[0], min[1], max[2]), trans * Vector3(max[0], min[1], max[2]), color);
    drawLineColor(trans * Vector3(max[0], min[1], max[2]), trans * Vector3(max[0], max[1], max[2]), color);
    drawLineColor(trans * Vector3(max[0], max[1], max[2]), trans * Vector3(min[0], max[1], max[2]), color);
    drawLineColor(trans * Vector3(min[0], max[1], max[2]), trans * Vector3(min[0], min[1], max[2]), color);
}

void
RenderSystem::drawArc(float x, float y, float radius, float angle, float angle2, float segments) noexcept
{
}

void
RenderSystem::drawArcSolid(float x, float y, float radius, float angle, float angle2, float segments) noexcept
{
}

void
RenderSystem::drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radius, float minAngle, float maxAngle, float segments) noexcept
{
    const Vector3& vx = axis;
    Vector3 vy = normal.cross(axis);
    float step = degrees(segments);
    int nSteps = (int)((maxAngle - minAngle) / step);

    if (!nSteps) nSteps = 1;
    Vector3 prev = center + radius * vx * std::cos(minAngle) + radius * vy * std::sin(minAngle);

    for (int i = 1; i <= nSteps; i++)
    {
        float angle = minAngle + (maxAngle - minAngle) * i / nSteps;
        Vector3 next = center + radius * vx * std::cos(angle) + radius * vy * std::sin(angle);
        this->drawLine(prev, next);
        prev = next;
    }
}

void
RenderSystem::drawCircle(float x, float  y, float radius, float segmenst) noexcept
{
}

void
RenderSystem::drawCircleSolid(float x, float  y, float radius, float segmenst) noexcept
{
}

void
RenderSystem::drawQuad(float x, float y, float w, float h, const Vector4& color) noexcept
{
    _polygons.push_back(SimpleVertex(Vector3(x, y + h, 0), color));
    _polygons.push_back(SimpleVertex(Vector3(x, y, 0), color));
    _polygons.push_back(SimpleVertex(Vector3(x + w, y, 0), color));
    _polygons.push_back(SimpleVertex(Vector3(x + w, y + h, 0), color));
    _polygons.push_back(SimpleVertex(Vector3(x, y + h, 0), color));
    _polygons.push_back(SimpleVertex(Vector3(x + w, y, 0), color));
}

void
RenderSystem::drawQuad(float dx, float dy, float dz, float x, float y, float z) noexcept
{
}

void
RenderSystem::drawQuad(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1) noexcept
{
}

void
RenderSystem::drawQuad(const Vector3& right, const Vector3& up, const Vector3& origin, int nFlipMode) noexcept
{
}

void
RenderSystem::drawQuad(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float ftx0, float fty0, float ftx1, float fty1) noexcept
{
}

void
RenderSystem::drawImage(float xpos, float ypos, float w, float h, int textureid, float s0, float t0, float s1, float t1, float r, float g, float b, float a) noexcept
{
}

void
RenderSystem::drawImage(float xpos, float ypos, float z, float w, float h, int textureid, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a) noexcept
{
}

void
RenderSystem::drawImageWithUV(float xpos, float ypos, float z, float w, float h, int texture_id, float *s, float *t, float r, float g, float b, float a) noexcept
{
}

void
RenderSystem::drawLine(const Vector3& pos1, const Vector3& pos2) noexcept
{
    this->drawLineColor(pos1, pos2, _globalColor);
}

void
RenderSystem::drawLineColor(const Vector3& pos1, const Vector3 & pos2, const Vector4& color) noexcept
{
    this->drawLineColor(pos1, color, pos2, color);
}

void
RenderSystem::drawLineColor(const Vector3& pos1, const Vector4& color1, const Vector3 & pos2, const Vector4& color2) noexcept
{
    _lines.push_back(SimpleVertex(pos1, color1));
    _lines.push_back(SimpleVertex(pos2, color2));
}

void
RenderSystem::drawLines(const Vector3 v[], std::size_t num, const Vector4& col, int flags, float ground) noexcept
{
    std::size_t index = 0;
    for (std::size_t i = 0; i < num; i++)
    {
        this->drawLineColor(v[index], col, v[index + 1], col);
        index += 2;
    }
}

void
RenderSystem::drawPoint(const Vector3& pt) noexcept
{
    this->drawPoint(pt, _globalColor);
}

void
RenderSystem::drawPoint(const Vector3& pt, const Vector4& color) noexcept
{
}

void
RenderSystem::drawPoints(const Vector3 pt[], std::size_t num) noexcept
{
    for (std::size_t i = 0; i < num; i++)
    {
        this->drawPoint(pt[i]);
    }
}

void
RenderSystem::drawText(const std::string& string) noexcept
{
}

void
RenderSystem::applyCamera(Camera* camera) noexcept
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setFloatParam(GlobalFloatSemantic::CameraAperture, camera->getAperture());
    semantic->setFloatParam(GlobalFloatSemantic::CameraNear, camera->getNear());
    semantic->setFloatParam(GlobalFloatSemantic::CameraFar, camera->getFar());

    semantic->setFloat3Param(GlobalFloat3Semantic::CameraPosition, camera->getTranslate());
    semantic->setFloat3Param(GlobalFloat3Semantic::CameraView, camera->getLookAt());

    semantic->setMatrixParam(GlobalMatrixSemantic::matView, camera->getView());
    semantic->setMatrixParam(GlobalMatrixSemantic::matViewInverse, camera->getViewInverse());
    semantic->setMatrixParam(GlobalMatrixSemantic::matViewInverseTranspose, camera->getViewInverseTranspose());
    semantic->setMatrixParam(GlobalMatrixSemantic::matProject, camera->getProject());
    semantic->setMatrixParam(GlobalMatrixSemantic::matProjectInverse, camera->getProjectInverse());
    semantic->setMatrixParam(GlobalMatrixSemantic::matViewProject, camera->getViewProject());
    semantic->setMatrixParam(GlobalMatrixSemantic::matViewProjectInverse, camera->getViewProjectInverse());
}

void
RenderSystem::applyEnvironment(const RenderScene& scene) noexcept
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setFloat3Param(GlobalFloat3Semantic::LightAmbient, scene.getAmbientColor());
}

void
RenderSystem::applyTimer(TimerPtr timer) noexcept
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setFloatParam(GlobalFloatSemantic::Time, timer->elapsed());
    semantic->setFloatParam(GlobalFloatSemantic::TimeDelta, timer->delta());
    semantic->setFloatParam(GlobalFloatSemantic::TimeFps, timer->fps());
}

void
RenderSystem::assignVisiable() noexcept
{
    RenderScene* scene = _renderPipeline->camera->getRenderScene();
    if (scene)
    {
        _renderPipeline->visiable.clear();
        _renderPipeline->renderDataManager.clear();

        scene->computVisiable(_renderPipeline->camera, _renderPipeline->visiable);

        for (auto& it : _renderPipeline->visiable)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            it->collection(_renderPipeline->renderDataManager);
        }
    }
}

void
RenderSystem::assignLight() noexcept
{
    RenderScene* scene = _renderPipeline->camera->getRenderScene();
    if (scene)
    {
        _renderPipeline->lights.clear();

        scene->computVisiableLight(_renderPipeline->camera, _renderPipeline->lights);

        for (auto& it : _renderPipeline->lights)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            auto cameras = it->getShadowCamera();
            if (cameras)
            {
                _renderPipeline->shadow.push_back(cameras.get());
            }
        }
    }
}

void
RenderSystem::onPreRender(RendererPtr renderer, Camera* camera) noexcept
{
    _renderPipeline->renderer = renderer;
    _renderPipeline->camera = camera;

    RenderCanvasPtr canvas;

    auto window = camera->getRenderWindow();
    if (!window)
        window = this->getRenderWindow();

    _renderer->setRenderCanvas(window->getRenderCanvas());

    CameraListener* cameralistener = camera->getCameraListener();
    if (cameralistener)
        cameralistener->onCameraVisible(camera);

    RenderListener* renderListener = camera->getRenderListener();
    if (renderListener)
        renderListener->onWillRenderObject();
}

void
RenderSystem::onPostRender() noexcept
{
    RenderListener* listener = _renderPipeline->camera->getRenderListener();
    if (listener)
        listener->onRenderObject();

    _renderPipeline->camera = nullptr;
}

void
RenderSystem::onRenderPipeline() noexcept
{
    _renderPipeline->render();
}

void
RenderSystem::renderBegin() noexcept
{
    _renderer->renderBegin();

    this->applyTimer(_timer);
}

void
RenderSystem::renderCamera(Camera* camera) noexcept
{
    this->onPreRender(_renderer, camera);

    this->applyCamera(camera);

    this->assignVisiable();
    this->assignLight();

    this->onRenderPipeline();

    this->onPostRender();
}

void
RenderSystem::render() noexcept
{
    for (auto& scene : _sceneList)
    {
        this->applyEnvironment(*scene);

        auto& cameras = scene->getCameraList();
        for (auto& it : cameras)
        {
            this->renderCamera(it);
        }
    }
}

void
RenderSystem::renderEnd() noexcept
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setMatrixParam(GlobalMatrixSemantic::matProject, _orthoCamera);

    if (!_lines.empty())
    {
        _dynamicBuffers->resize(_lines.size());
        std::memcpy(_dynamicBuffers->data(), _lines.data(), _lines.size() * sizeof(SimpleVertex));

        auto pass = _lineMaterial->getTech(RenderQueue::Opaque)->getPass(RenderPass::RP_GBUFFER);

        _renderer->setShaderObject(pass->getShaderObject());
        _renderer->setRenderState(pass->getRenderState());
        _renderer->setShaderParamArgs(pass->getParamBindings()->getShaderParamArgs());

        Renderable renderable;
        renderable.type = VertexType::GPU_LINE;
        renderable.startVertice = 0;
        renderable.numVertices = _renderBuffer->getNumVertices();
        renderable.startIndice = 0;
        renderable.numIndices = _renderBuffer->getNumIndices();
        renderable.numInstances = 0;

        _renderer->updateMesh(_renderBuffer, _dynamicBuffers, nullptr);
        _renderer->drawMesh(_renderBuffer, renderable);

        _lines.clear();
    }

    if (!_polygons.empty())
    {
        _dynamicBuffers->resize(_polygons.size());
        std::memcpy(_dynamicBuffers->data(), _polygons.data(), _polygons.size() * sizeof(SimpleVertex));

        auto pass = _lineMaterial->getTech(RenderQueue::Opaque)->getPass(RenderPass::RP_GBUFFER);

        _renderer->setShaderObject(pass->getShaderObject());
        _renderer->setRenderState(pass->getRenderState());
        _renderer->setShaderParamArgs(pass->getParamBindings()->getShaderParamArgs());

        Renderable renderable;
        renderable.type = VertexType::GPU_LINE;
        renderable.startVertice = 0;
        renderable.numVertices = _renderBuffer->getNumVertices();
        renderable.startIndice = 0;
        renderable.numIndices = _renderBuffer->getNumIndices();
        renderable.numInstances = 0;

        _renderer->updateMesh(_renderBuffer, _dynamicBuffers, nullptr);
        _renderer->drawMesh(_renderBuffer, renderable);

        _polygons.clear();
    }

    for (auto& scene : _sceneList)
    {
        auto& cameras = scene->getCameraList();
        for (auto& it : cameras)
        {
            auto window = it->getRenderWindow();
            if (window)
            {
                _renderer->present(window->getRenderCanvas());
            }
        }
    }

    _renderer->present(_renderWindow->getRenderCanvas());
    _renderer->renderEnd();
}

_NAME_END