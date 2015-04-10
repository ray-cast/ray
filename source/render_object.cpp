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
#include <ray/render_object.h>
#include <ray/render_scene.h>

_NAME_BEGIN

RenderObject::RenderObject() noexcept
    : _isCastShadow(true)
    , _isReceiveShadow(true)
    , _renderScene(nullptr)
    , _renderListener(nullptr)
    , _isNeedUpdate(false)
    , _layer(0)
{
    _transform.loadIdentity();
    _transformInverse.loadIdentity();
    _transformInverseTranspose.loadIdentity();
}

RenderObject::~RenderObject() noexcept
{
}

void
RenderObject::setTransform(const Matrix4x4& transform) noexcept
{
    _transform = transform;
    _isNeedUpdate = true;
}

void
RenderObject::setTransformInverse(const Matrix4x4& transform) noexcept
{
    _transformInverse = transform;
}

void
RenderObject::setTransformInverseTranspose(const Matrix4x4& transform) noexcept
{
    _transformInverseTranspose = transform;
}

void
RenderObject::setBoundingBox(const Bound& bound) noexcept
{
    _boundingBox = bound;
    _isNeedUpdate = true;
}

void
RenderObject::setReceiveShadow(bool enable) noexcept
{
    _isReceiveShadow = enable;
}

void
RenderObject::setLayer(int layer) noexcept
{
    _layer = layer;
}

int
RenderObject::getLayer() const noexcept
{
    return _layer;
}

void
RenderObject::setCastShadow(bool value) noexcept
{
    _isCastShadow = value;
}

void
RenderObject::setRenderListener(RenderListener* listener) noexcept
{
    _renderListener = listener;
}

void
RenderObject::setRenderScene(RenderScene* scene) noexcept
{
    if (_renderScene)
    {
        _renderScene->removeRenderObject(this);
    }

    _renderScene = scene;

    if (_renderScene)
    {
        _renderScene->addRenderObject(this);
    }
}

const Matrix4x4&
RenderObject::getTransform() const noexcept
{
    return _transform;
}

const Matrix4x4&
RenderObject::getTransformInverse() const noexcept
{
    return _transformInverse;
}

const Matrix4x4&
RenderObject::getTransformInverseTranspose() const noexcept
{
    return _transformInverseTranspose;
}

bool
RenderObject::getReceiveShadow() const noexcept
{
    return _isReceiveShadow;
}

bool
RenderObject::getCastShadow()  const noexcept
{
    return _isCastShadow;
}

RenderScene*
RenderObject::getRenderScene() const noexcept
{
    return _renderScene;
}

RenderListener*
RenderObject::getRenderListener() noexcept
{
    return _renderListener;
}

const Bound&
RenderObject::getBoundingBox() const noexcept
{
    return _boundingBox;
}

const Bound&
RenderObject::getBoundingBoxInWorld() const noexcept
{
    this->updateBoundingBoxInWorld();
    return _worldBoundingxBox;
}

void
RenderObject::updateBoundingBoxInWorld() const noexcept
{
    if (_isNeedUpdate)
    {
        _worldBoundingxBox = _boundingBox;
        _worldBoundingxBox.center(_boundingBox.center() + _transform.getTranslate());
        _isNeedUpdate = false;
    }
}

void
RenderObject::collection(RenderDataManager&) noexcept
{
}

void
RenderObject::render(RendererPtr renderer, RenderQueue queue, RenderPass pass, MaterialPassPtr material) noexcept
{
}

_NAME_END