// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/render_scene.h>
#include <ray/render_system.h>

_NAME_BEGIN

RenderScene::RenderScene() noexcept
    : _ambientLight(Vector3(0.2, 0.2, 0.2))
{
}

RenderScene::~RenderScene() noexcept
{
}

void
RenderScene::addCamera(Camera* camera) noexcept
{
    auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
    if (it == _cameraList.end())
    {
        _cameraList.push_back(camera);
        this->sortCamera();
    }
}

void
RenderScene::removeCamera(Camera* camera) noexcept
{
    auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
    if (it != _cameraList.end())
    {
        _cameraList.erase(it);
    }
}

RenderScene::CameraList&
RenderScene::getCameraList() noexcept
{
    return _cameraList;
}

void
RenderScene::sortCamera() noexcept
{
    std::sort(_cameraList.begin(), _cameraList.end(),
        [](Camera* lhs, Camera* rhs)
    {
        return lhs->getCameraOrder() < rhs->getCameraOrder();
    }
    );
}

void
RenderScene::addLight(Light* light) noexcept
{
    assert(_lightList.end() == std::find(_lightList.begin(), _lightList.end(), light));

    _lightList.push_back(light);
}

void
RenderScene::removeLight(Light* light) noexcept
{
    auto it = std::find(_lightList.begin(), _lightList.end(), light);
    if (it != _lightList.end())
    {
        _lightList.erase(it);
    }
}

RenderScene::LightList&
RenderScene::getLightList() noexcept
{
    return _lightList;
}

void
RenderScene::sortLight() noexcept
{
}

void
RenderScene::addRenderObject(RenderObject* object) noexcept
{
    _renderObjectList.push_back(object);
}

void
RenderScene::removeRenderObject(RenderObject* object) noexcept
{
    auto it = std::find(_renderObjectList.begin(), _renderObjectList.end(), object);
    if (it != _renderObjectList.end())
    {
        _renderObjectList.erase(it);
    }
}

void
RenderScene::computVisiable(Camera* camera, std::vector<RenderObject*>& object) noexcept
{
    Frustum fru;
    fru.extract(camera->getViewProject());

    std::size_t visiable1 = 0;

    for (auto& it : _renderObjectList)
    {
        if (!fru.contains(it->getBoundingBoxInWorld().aabb()))
            continue;

        visiable1++;

        object.push_back(it);
    }
}

void
RenderScene::computVisiableLight(Camera* camera, std::vector<Light*>& lights) noexcept
{
    Frustum fru;
    fru.extract(camera->getViewProject());

    for (auto& light : _lightList)
    {
        Bound bound;

        LightType type = light->getLightType();

        auto direction = light->getLightDirection();
        auto range = light->getRange();

        if (type == LightType::LT_SUN)
        {
            float infinity = std::numeric_limits<float>::max();

            Vector3 min(-infinity, -infinity, -infinity);
            Vector3 max(infinity, infinity, infinity);

            bound.encapsulate(min);
            bound.encapsulate(max);
        }
        else if (type == LightType::LT_POINT)
        {
            Vector3 min(-range, -range, -range);
            Vector3 max(range, range, range);

            bound.encapsulate(min);
            bound.encapsulate(max);
        }
        else if (type == LightType::LT_AREA ||
            type == LightType::LT_SPOT ||
            type == LightType::LT_HEMI_SPHERE)
        {
            Vector3 min(-range, -range, -range);
            Vector3 max(range, range, range);

            bound.encapsulate(min);
            bound.encapsulate(max);
        }

        light->setBoundingBox(bound);

        if (fru.contains(light->getBoundingBoxInWorld().aabb()))
        {
            lights.push_back(light);
        }
    }
}

void
RenderScene::setAmbientColor(const Vector3& color) noexcept
{
    _ambientLight = color;
}

const Vector3&
RenderScene::getAmbientColor() const noexcept
{
    return _ambientLight;
}

_NAME_END