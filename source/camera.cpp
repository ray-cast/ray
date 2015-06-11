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
#include <ray/camera.h>
#include <ray/render_scene.h>
#include <ray/render_window.h>
#include <ray/render_texture.h>

_NAME_BEGIN

CameraListener::CameraListener() noexcept
{
}

CameraListener::~CameraListener() noexcept
{
}

Camera::Camera() noexcept
    : _left(0)
    , _right(0)
    , _top(0)
    , _bottom(0)
    , _aperture(0)
    , _ratio(0)
    , _zNear(0)
    , _zFar(0)
    , _translate(0, 0, 0)
    , _lookat(0, 0, 1)
    , _up(0, 1, 0)
    , _project(Matrix4x4::One)
    , _projectInverse(Matrix4x4::One)
    , _view(Matrix4x4::One)
    , _viewInverse(Matrix4x4::One)
    , _viewInverseTranspose(Matrix4x4::One)
    , _viewProejct(Matrix4x4::One)
    , _viewProjectInverse(Matrix4x4::One)
    , _projLength(0, 0)
    , _projConstant(0, 0, 0, 0)
    , _viewport(0, 0, 0, 0)
    , _cameraType(CameraType::CT_PERSPECTIVE)
    , _cameraOrder(CameraOrder::CO_MAIN)
    , _cameraRender(CameraRender::CR_RENDER_TO_TEXTURE)
    , _cameraListener(nullptr)
    , _renderScene(nullptr)
    , _renderWindow(nullptr)
{
}

Camera::~Camera() noexcept
{
    this->close();
}

void
Camera::setup(std::size_t width, std::size_t height) noexcept
{
    if (_cameraOrder == CameraOrder::CO_MAIN)
    {
        _deferredDepthMap = RenderTexture::create();
        _deferredDepthMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::DEPTH24_STENCIL8);

        _deferredGraphicMap = RenderTexture::create();
        _deferredGraphicMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::SR8G8B8A8, ClearFlags::CLEAR_COLOR);

        _deferredNormalMap = RenderTexture::create();
        _deferredNormalMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16A16F, ClearFlags::CLEAR_COLOR);

        _deferredLightMap = RenderTexture::create();
        _deferredLightMap->setSharedStencilTexture(_deferredDepthMap);
        _deferredLightMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16A16F);

        _deferredGraphicMaps = MultiRenderTexture::create();
        _deferredGraphicMaps->attach(_deferredDepthMap, Attachment::DEPTH_STENCIL);
        _deferredGraphicMaps->attach(_deferredGraphicMap, Attachment::COLOR0);
        _deferredGraphicMaps->attach(_deferredNormalMap, Attachment::COLOR1);
        _deferredGraphicMaps->setup(width, height);

        _renderTexture = std::make_shared<RenderTexture>();
        _renderTexture->setSharedStencilTexture(_deferredDepthMap);
        _renderTexture->setup(width, height, TextureDim::DIM_2D, PixelFormat::SR8G8B8A8);

        _swapTexture = std::make_shared<RenderTexture>();
        _swapTexture->setSharedStencilTexture(_deferredDepthMap);
        _swapTexture->setup(width, height, TextureDim::DIM_2D, PixelFormat::SR8G8B8A8);

        if (_cameraRender == CR_RENDER_TO_CUBEMAP)
        {
            _cubeTexture = std::make_shared<RenderTexture>();
            _cubeTexture->setSharedStencilTexture(_deferredDepthMap);
            _cubeTexture->setup(512, 512, TextureDim::DIM_CUBE, PixelFormat::SR8G8B8A8);
        }
    }
    else
    {
        _renderTexture = RenderTexture::create();
        _renderTexture->setup(width, height, TextureDim::DIM_2D, PixelFormat::R32F);

        _swapTexture = RenderTexture::create();
        _swapTexture->setup(width, height, TextureDim::DIM_2D, PixelFormat::R32F);
    }

    this->makeViewProject();
}

void
Camera::close() noexcept
{
    if (_deferredDepthMap)
    {
        _deferredDepthMap.reset();
        _deferredDepthMap = nullptr;
    }

    if (_deferredGraphicMap)
    {
        _deferredGraphicMap.reset();
        _deferredGraphicMap = nullptr;
    }

    if (_deferredNormalMap)
    {
        _deferredNormalMap.reset();
        _deferredNormalMap = nullptr;
    }

    if (_deferredLightMap)
    {
        _deferredLightMap.reset();
        _deferredLightMap = nullptr;
    }

    if (_deferredGraphicMaps)
    {
        _deferredGraphicMaps.reset();
        _deferredGraphicMaps = nullptr;
    }

    if (_renderTexture)
    {
        _renderTexture.reset();
        _renderTexture = nullptr;
    }

    if (_swapTexture)
    {
        _swapTexture.reset();
        _swapTexture = nullptr;
    }

    if (_renderWindow)
    {
        _renderWindow.reset();
        _renderWindow = nullptr;
    }

    this->setRenderScene(nullptr);
}

float
Camera::getAperture() const noexcept
{
    return _aperture;
}

float
Camera::getRatio() const noexcept
{
    return _ratio;
}

float
Camera::getNear() const noexcept
{
    return _zNear;
}

float
Camera::getFar() const noexcept
{
    return _zFar;
}

const Vector3&
Camera::getTranslate() const noexcept
{
    return _translate;
}

const Vector3&
Camera::getLookAt() const noexcept
{
    return _lookat;
}

const Vector3&
Camera::getUpVector() const noexcept
{
    return _up;
}

const Matrix4x4&
Camera::getView() const noexcept
{
    return _view;
}

const Matrix4x4&
Camera::getViewInverse() const noexcept
{
    return _viewInverse;
}

const Matrix4x4&
Camera::getViewInverseTranspose() const noexcept
{
    return _viewInverseTranspose;
}

const Matrix4x4&
Camera::getProject() const noexcept
{
    return _project;
}

const Matrix4x4&
Camera::getViewProject() const noexcept
{
    return _viewProejct;
}

const Matrix4x4&
Camera::getProjectInverse() const noexcept
{
    return _projectInverse;
}

const Matrix4x4&
Camera::getViewProjectInverse() const noexcept
{
    return _viewProjectInverse;
}

const Vector2&
Camera::getProjLength() const noexcept
{
    return _projLength;
}

const Vector4&
Camera::getProjConstant() const noexcept
{
    return _projConstant;
}

const Vector4&
Camera::getClipConstant() const noexcept
{
    return _clipConstant;
}

void
Camera::makeLookAt(const Vector3& pos, const Vector3& lookat, const Vector3& up) noexcept
{
    _translate = pos;
    _lookat = lookat;
    _up = up;

    _viewInverse = _view.makeLookAt(_translate, _lookat, _up).inverse();
    _viewInverseTranspose = Matrix4x4(_viewInverse).transpose();
}

void
Camera::makeOrtho(float left, float right, float bottom, float top, float znear, float zfar) noexcept
{
    assert(_cameraType == CameraType::CT_ORTHO);

    _left = left;
    _right = right;
    _bottom = bottom;
    _top = top;
    _zNear = znear;
    _zFar = zfar;
    _projectInverse = _project.makeOrtho(_left, _right, _bottom, _top, _zNear, _zFar).inverse();

    _projLength.x = _project.a1;
    _projLength.y = _project.b2;

    _projConstant.x = -2.0 / _projLength.x;
    _projConstant.y = -2.0 / _projLength.y;
    _projConstant.z = 1.0 / _projLength.x;
    _projConstant.w = 1.0 / _projLength.y;

    _clipConstant.x = (2.0 * _zNear * _zFar);
    _clipConstant.y = _zFar - _zNear;
    _clipConstant.z = _zFar + _zNear;
    _clipConstant.w = 1.0;
}

void
Camera::makePerspective(float aperture, float ratio, float znear, float zfar) noexcept
{
    assert(_cameraType == CameraType::CT_PERSPECTIVE);

    _aperture = aperture;
    _ratio = ratio;
    _zNear = znear;
    _zFar = zfar;

    _projectInverse = _project.makePerspective(_aperture, _ratio, _zNear, _zFar).inverse();

    _projLength.x = _project.a1;
    _projLength.y = _project.b2;

    _projConstant.x = -2.0 / (_projLength.x);
    _projConstant.y = -2.0 / (_projLength.y);
    _projConstant.z = (1.0 - _project.a3) / _projLength.x;
    _projConstant.w = (1.0 + _project.b3) / _projLength.y;

    _clipConstant.x = (2.0 * _zNear * _zFar);
    _clipConstant.y = _zFar - _zNear;
    _clipConstant.z = _zFar + _zNear;
    _clipConstant.w = 1.0;
}

void
Camera::makeViewProject() noexcept
{
    _viewProejct = _project * _view;
    _viewProjectInverse = _viewProejct.inverse();
}

Vector3
Camera::project(const Vector3& pos) const noexcept
{
    Vector4 v(pos);

    v = _viewProejct * v;
    if (v.w != 0)
        v /= v.w;

    int w = (int)_viewport.width >> 1;
    int h = (int)_viewport.height >> 1;

    v.x = v.x * w + w + _viewport.left;
    v.y = v.y * h + h + _viewport.top;
    v.z = v.z * 0.5f + 0.5f;

    return Vector3(v.x, v.y, v.z);
}

Vector3
Camera::unproject(const Vector3& pos) const noexcept
{
    Vector4 v(pos);

    int w = (int)_viewport.width >> 1;
    int h = (int)_viewport.height >> 1;

    v.y = _viewport.height - pos.y;

    v.x = v.x / w - 1.0f - _viewport.left;
    v.y = v.y / h - 1.0f - _viewport.top;
    v.z = v.z * 2.0f - 1.0f;

    v = _viewProjectInverse * v;
    v /= v.w;

    return Vector3(v.x, v.y, v.z);
}

void
Camera::setViewport(const Viewport& viewport) noexcept
{
    _viewport = viewport;
}

void
Camera::setCameraType(CameraType type) noexcept
{
    _cameraType = type;
}

void
Camera::setCameraOrder(CameraOrder order) noexcept
{
    _cameraOrder = order;
}

void
Camera::setCameraRender(CameraRender render) noexcept
{
    _cameraRender = render;
}

void
Camera::setCameraListener(CameraListener* listener) noexcept
{
    if (_cameraListener)
    {
        _cameraListener->onChangeListener(this);
    }

    _cameraListener = listener;
}

const Viewport&
Camera::getViewport() const noexcept
{
    return _viewport;
}

CameraType
Camera::getCameraType() const noexcept
{
    return _cameraType;
}

CameraOrder
Camera::getCameraOrder() const noexcept
{
    return _cameraOrder;
}

CameraRender
Camera::getCameraRender() const noexcept
{
    return _cameraRender;
}

CameraListener*
Camera::getCameraListener() const noexcept
{
    return _cameraListener;
}

void
Camera::setRenderScene(RenderScene* scene) noexcept
{
    if (_renderScene)
    {
        _renderScene->removeCamera(this);
    }

    _renderScene = scene;

    if (_renderScene)
    {
        _renderScene->addCamera(this);
    }
}

RenderScene*
Camera::getRenderScene() const noexcept
{
    return _renderScene;
}

void
Camera::setRenderWindow(RenderWindowPtr view) noexcept
{
    _renderWindow = view;
}

void
Camera::setRenderTexture(RenderTexturePtr texture) noexcept
{
    _renderTexture = texture;
}

RenderWindowPtr
Camera::getRenderWindow() const noexcept
{
    return _renderWindow;
}

RenderTexturePtr
Camera::getRenderTexture() const noexcept
{
    return _renderTexture;
}

RenderTexturePtr
Camera::getSwapTexture() const noexcept
{
    return _swapTexture;
}

RenderTexturePtr
Camera::getDeferredDepthMap() const noexcept
{
    return _deferredDepthMap;
}

RenderTexturePtr
Camera::getDeferredGraphicMap() const noexcept
{
    return _deferredGraphicMap;
}

RenderTexturePtr
Camera::getDeferredNormalMap() const noexcept
{
    return _deferredNormalMap;
}

RenderTexturePtr
Camera::getDeferredLightMap() const noexcept
{
    return _deferredLightMap;
}

MultiRenderTexturePtr
Camera::getDeferredGraphicsMaps() const noexcept
{
    return _deferredGraphicMaps;
}

CameraPtr
Camera::clone() const noexcept
{
    auto camera = std::make_shared<Camera>();
    camera->setCameraOrder(this->getCameraOrder());
    camera->setCameraType(this->getCameraType());
    camera->setViewport(this->getViewport());
    camera->setTransform(this->getTransform());
    camera->setTransformInverse(this->getTransformInverse());
    camera->setTransformInverseTranspose(this->getTransformInverseTranspose());
    camera->setBoundingBox(this->getBoundingBox());
    camera->makeLookAt(this->getTranslate(), this->getLookAt(), this->getUpVector());

    if (CameraType::CT_PERSPECTIVE == camera->getCameraType())
        camera->makePerspective(this->getAperture(), this->getRatio(), this->getNear(), this->getFar());
    else if (CameraType::CT_ORTHO == camera->getCameraType())
        camera->makeOrtho(_left, _right, _bottom, _top, _zNear, _zFar);

    return camera;
}

_NAME_END