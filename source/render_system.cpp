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
#include <ray/render_factory.h>
#include <ray/material_maker.h>
#include <ray/forward_shading.h>
#include <ray/deferred_lighting.h>
#include <ray/atmospheric.h>
#include <ray/light_shaft.h>
#include <ray/ssao.h>
#include <ray/ssgi.h>
#include <ray/ssr.h>
#include <ray/dof.h>
#include <ray/hdr.h>
#include <ray/fxaa.h>

_NAME_BEGIN

RenderSystem::RenderSystem() noexcept
    : _globalColor(1.0f, 1.0f, 1.0f, 0.5f)
{
}

RenderSystem::~RenderSystem() noexcept
{
    this->close();
}

bool
RenderSystem::setup(WindHandle win, std::size_t width, std::size_t height) except
{
    _renderDevice = RenderFactory::createRenderDevice();
    _renderDevice->open(win);

    Material::setMaterialSemantic(std::make_shared<MaterialSemantic>());

    _renderPipeline = std::make_shared<DeferredLighting>();
    _renderPipeline->setup(_renderDevice, width, height);

    _lineMaterial = MaterialMaker("sys:fx\\lines.glsl");

    VertexComponents components;
    components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
    components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));

    _dynamicBuffers = RenderFactory::createVertexBuffer();
    _dynamicBuffers->setVertexComponents(components);
    _dynamicBuffers->setup(1, VertexUsage::GPU_USAGE_DYNAMIC);

    _renderBuffer = RenderFactory::createRenderBuffer();
    _renderBuffer->setup(_dynamicBuffers, nullptr);

    float ratio = (float)width / height;
    _orthoCamera.makeOrtho(-ratio, ratio, -1, 1, 0, 1000);

    return true;
}

void
RenderSystem::close() noexcept
{
    if (_SAT)
    {
        _SAT.reset();
        _SAT = nullptr;
    }

    if (_SSAO)
    {
        _SSAO.reset();
        _SSAO = nullptr;
    }

    if (_SSGI)
    {
        _SSGI.reset();
        _SSGI = nullptr;
    }

    if (_SSR)
    {
        _SSR.reset();
        _SSR = nullptr;
    }

    if (_DOF)
    {
        _DOF.reset();
        _DOF = nullptr;
    }

    if (_HDR)
    {
        _HDR.reset();
        _HDR = nullptr;
    }

    if (_FXAA)
    {
        _FXAA.reset();
        _FXAA = nullptr;
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

    auto semantic = Material::getMaterialSemantic();
    if (semantic)
    {
        semantic->close();
        Material::setMaterialSemantic(nullptr);
    }

    if (_renderDevice)
    {
        _renderDevice.reset();
        _renderDevice = nullptr;
    }

    _setting = RenderSetting();
}

void
RenderSystem::setRenderSetting(const RenderSetting& setting) noexcept
{
    if (_setting.enableSAT != setting.enableSAT)
    {
        if (setting.enableSAT)
        {
            _SAT = std::make_shared<Atmospheric>();
            _renderPipeline->addPostProcess(_SAT);
        }
        else
        {
            _renderPipeline->removePostProcess(_SAT);
            _SAT.reset();
        }
    }

    if (_setting.enableSSGI != setting.enableSSGI)
    {
        if (setting.enableSSGI)
        {
            _SSGI = std::make_shared<SSGI>();
            _renderPipeline->addPostProcess(_SSGI);
        }
        else
        {
            _renderPipeline->removePostProcess(_SSGI);
            _SSGI.reset();
        }
    }

    if (_setting.enableSSAO != setting.enableSSAO)
    {
        if (setting.enableSSGI)
        {
            _SSAO = std::make_shared<SSAO>();
            _renderPipeline->addPostProcess(_SSAO);
        }
        else
        {
            _renderPipeline->removePostProcess(_SSAO);
            _SSAO.reset();
        }
    }

    if (_setting.enableSSR != setting.enableSSR)
    {
        if (setting.enableSSR)
        {
            _SSR = std::make_shared<SSR>();
            _renderPipeline->addPostProcess(_SSR);
        }
        else
        {
            _renderPipeline->removePostProcess(_SSR);
            _SSR.reset();
        }
    }

    if (_setting.enableDOF != setting.enableDOF)
    {
        if (setting.enableDOF)
        {
            _DOF = std::make_shared<DepthOfField>();
            _renderPipeline->addPostProcess(_DOF);
        }
        else
        {
            _renderPipeline->removePostProcess(_DOF);
            _DOF.reset();
        }
    }

    if (_setting.enableHDR != setting.enableHDR)
    {
        if (setting.enableHDR)
        {
            _HDR = std::make_shared<HDR>();
            _renderPipeline->addPostProcess(_HDR);
        }
        else
        {
            _renderPipeline->removePostProcess(_HDR);
            _HDR.reset();
        }
    }

    if (_setting.enableFXAA != setting.enableFXAA)
    {
        if (setting.enableFXAA)
        {
            _FXAA = std::make_shared<FXAA>();
            _renderPipeline->addPostProcess(_FXAA);
        }
        else
        {
            _renderPipeline->removePostProcess(_FXAA);
            _FXAA.reset();
        }
    }

    if (_setting.enableLightShaft != setting.enableLightShaft)
    {
        if (setting.enableLightShaft)
        {
            _lightShaft = std::make_shared<LightShaft>();
            _renderPipeline->addPostProcess(_lightShaft);
        }
        else
        {
            _renderPipeline->removePostProcess(_lightShaft);
            _lightShaft.reset();
        }
    }

    _renderDevice->setSwapInterval(setting.interval);
}

const RenderSetting&
RenderSystem::getRenderSetting() const noexcept
{
    return _setting;
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
RenderSystem::drawText(const Vector3& pt, const std::string& string) noexcept
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
RenderSystem::renderBegin() noexcept
{
    _renderDevice->renderBegin();

    this->applyTimer(_timer);
}

void
RenderSystem::renderCamera(Camera* camera) noexcept
{
    _renderPipeline->setCamera(camera);
    _renderPipeline->render();
}

void
RenderSystem::render() noexcept
{
    for (auto& scene : _sceneList)
    {
        this->applyEnvironment(*scene);

        auto& cameras = scene->getCameraList();
        for (auto& camera : cameras)
        {
            this->applyCamera(camera);
            this->renderCamera(camera);
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

        _renderPipeline->setRenderState(pass->getRenderState());
        _renderPipeline->setShaderObject(pass->getShaderObject());

        Renderable renderable;
        renderable.type = VertexType::GPU_LINE;
        renderable.startVertice = 0;
        renderable.numVertices = _renderBuffer->getNumVertices();
        renderable.startIndice = 0;
        renderable.numIndices = _renderBuffer->getNumIndices();
        renderable.numInstances = 0;

        _renderPipeline->updateMesh(_renderBuffer, _dynamicBuffers, nullptr);
        _renderPipeline->drawMesh(_renderBuffer, renderable);

        _lines.clear();
    }

    if (!_polygons.empty())
    {
        _dynamicBuffers->resize(_polygons.size());
        std::memcpy(_dynamicBuffers->data(), _polygons.data(), _polygons.size() * sizeof(SimpleVertex));

        auto pass = _lineMaterial->getTech(RenderQueue::Opaque)->getPass(RenderPass::RP_GBUFFER);

        _renderPipeline->setRenderState(pass->getRenderState());
        _renderPipeline->setShaderObject(pass->getShaderObject());

        Renderable renderable;
        renderable.type = VertexType::GPU_LINE;
        renderable.startVertice = 0;
        renderable.numVertices = _renderBuffer->getNumVertices();
        renderable.startIndice = 0;
        renderable.numIndices = _renderBuffer->getNumIndices();
        renderable.numInstances = 0;

        _renderPipeline->updateMesh(_renderBuffer, _dynamicBuffers, nullptr);
        _renderPipeline->drawMesh(_renderBuffer, renderable);

        _polygons.clear();
    }

    _renderDevice->renderEnd();
}

_NAME_END