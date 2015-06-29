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
#include <ray/camera_component.h>
#include <ray/game_server.h>
#include <ray/render_features.h>

_NAME_BEGIN

__ImplementSubClass(CameraComponent, GameComponent)

CameraComponent::CameraComponent() noexcept
{
    _camera = std::make_shared<Camera>();
    _camera->setCameraOrder(CameraOrder::CO_MAIN);
}

CameraComponent::~CameraComponent() noexcept
{
}

void
CameraComponent::makeLookAt(const Vector3& pos, const Vector3& lookat, const Vector3& up) noexcept
{
    _camera->makeLookAt(pos, lookat, up);
}

void
CameraComponent::makeOrtho(float left, float right, float bottom, float top, float znear, float zfar) noexcept
{
    _camera->makeOrtho(left, right, bottom, top, znear, zfar);
}

void
CameraComponent::makePerspective(float aperture, float ratio, float znear, float zfar) noexcept
{
    _camera->makePerspective(aperture, ratio, znear, zfar);
}

float
CameraComponent::getAperture() const noexcept
{
    return _camera->getAperture();
}

float
CameraComponent::getRatio() const noexcept
{
    return _camera->getRatio();
}

float
CameraComponent::getNear() const noexcept
{
    return _camera->getNear();
}

float
CameraComponent::getFar() const noexcept
{
    return _camera->getFar();
}

const Matrix4x4&
CameraComponent::getProject() const noexcept
{
    return _camera->getProject();
}

const Matrix4x4&
CameraComponent::getViewProject() const noexcept
{
    return _camera->getViewProject();
}

const Matrix4x4&
CameraComponent::getProjectInverse() const noexcept
{
    return _camera->getProjectInverse();
}

const Matrix4x4&
CameraComponent::getViewProjectInverse() const noexcept
{
    return _camera->getViewProjectInverse();
}

Vector3
CameraComponent::project(const Vector3& pos) const noexcept
{
    return _camera->project(pos);
}

Vector3
CameraComponent::unproject(const Vector3& pos) const noexcept
{
    return _camera->unproject(pos);
}

Vector3
CameraComponent::sceneToDirection(const Vector2& pos) const noexcept
{
    return _camera->sceneToDirection(pos);
}

void
CameraComponent::setViewport(const Viewport& viewport) noexcept
{
    _camera->setViewport(viewport);
}

const Viewport&
CameraComponent::getViewport() const noexcept
{
    return _camera->getViewport();
}

void
CameraComponent::setCameraType(CameraType type) noexcept
{
    _camera->setCameraType(type);
}

void
CameraComponent::setCameraOrder(CameraOrder order) noexcept
{
    _camera->setCameraOrder(order);
}

CameraType
CameraComponent::getCameraType() const noexcept
{
    return _camera->getCameraType();
}

CameraOrder
CameraComponent::getCameraOrder() const noexcept
{
    return _camera->getCameraOrder();
}

void
CameraComponent::load(iarchive& reader) noexcept
{
    float aperture = 70.0;
    float ratio = 1.0;
    float znear = 0.1;
    float zfar = 1000.0;
    int x = 0, y = 0, w = 0, h = 0;

    reader >> static_cast<GameComponent*>(this);
    reader >> rtti_name(aperture);
    reader >> rtti_name(ratio);
    reader >> rtti_name(znear);
    reader >> rtti_name(zfar);
    reader >> rtti_name(x);
    reader >> rtti_name(y);
    reader >> rtti_name(w);
    reader >> rtti_name(h);

    this->setViewport(Viewport(x, y, w, h));
    this->makePerspective(aperture, ratio, znear, zfar);
}

void
CameraComponent::save(oarchive& write) noexcept
{
}

void
CameraComponent::onActivate() noexcept
{
    auto renderer = this->getGameObject()->getGameServer()->getFeature<RenderFeatures>();
    if (renderer)
    {
        auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
        if (renderScene)
        {
            _camera->makeLookAt(
                this->getGameObject()->getTranslate(),
                this->getGameObject()->getLookAt(),
                this->getGameObject()->getUpVector()
                );
            _camera->makeViewProject();
            _camera->setRenderScene(renderScene);
        }
    }
}

void
CameraComponent::onDeactivate() noexcept
{
    _camera->setRenderScene(nullptr);
}

void
CameraComponent::onMoveAfter() noexcept
{
    _camera->makeLookAt(
        this->getGameObject()->getTranslate(),
        this->getGameObject()->getLookAt(),
        this->getGameObject()->getUpVector()
        );

    _camera->makeViewProject();
}

GameComponentPtr
CameraComponent::clone() const noexcept
{
    auto instance = std::make_shared<CameraComponent>();
    instance->setName(this->getName());
    instance->setVisible(this->getVisible());
    instance->_camera = _camera->clone();

    return instance;
}

_NAME_END