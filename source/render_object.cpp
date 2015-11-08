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

_NAME_BEGIN

RenderListener::RenderListener() noexcept
{
}

RenderListener::~RenderListener() noexcept
{
}

RenderObject::RenderObject() noexcept
	: _layer(0)
	, _isCastShadow(true)
	, _isReceiveShadow(true)
	, _renderListener(nullptr)
	, _needUpdate(true)
	, _transform(Matrix4x4::One)
	, _transformInverse(Matrix4x4::One)
	, _transformInverseTranspose(Matrix4x4::One)
{
}

RenderObject::~RenderObject() noexcept
{
}

void
RenderObject::setTransform(const Matrix4x4& transform) noexcept
{
	_transform = transform;
	_needUpdate = true;
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
	_needUpdate = true;
}

void
RenderObject::setReceiveShadow(bool enable) noexcept
{
	_isReceiveShadow = enable;
}

void
RenderObject::setLayer(std::uint8_t layer) noexcept
{
	_layer = layer;
}

std::uint8_t
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
	this->_updateBoundingBoxInWorld();
	return _worldBoundingxBox;
}

void
RenderObject::setMaterial(MaterialPtr) noexcept
{
	assert(false);
}

MaterialPtr
RenderObject::getMaterial() noexcept
{
	return nullptr;
}

void
RenderObject::setRenderBuffer(RenderBufferPtr geometry) noexcept
{
	assert(nullptr);
}

RenderBufferPtr
RenderObject::getRenderBuffer() noexcept
{
	return nullptr;
}

void
RenderObject::setRenderIndirect(RenderIndirectPtr renderable) noexcept
{
	assert(nullptr);
}

RenderIndirectPtr
RenderObject::getRenderIndirect() noexcept
{
	return nullptr;
}

void 
RenderObject::addSubRenderObject(RenderObjectPtr object) noexcept
{
	auto it = std::find(_renderObjects.begin(), _renderObjects.end(), object);
	if (it == _renderObjects.end())
	{
		_renderObjects.push_back(object);
	}
}

void 
RenderObject::removeSubRenderObject(RenderObjectPtr object) noexcept
{
	auto it = std::find(_renderObjects.begin(), _renderObjects.end(), object);
	if (it != _renderObjects.end())
	{
		_renderObjects.erase(it);
	}
}

RenderObjects& 
RenderObject::getSubeRenderObjects() noexcept
{
	return _renderObjects;
}

void
RenderObject::_updateBoundingBoxInWorld() const noexcept
{
	if (_needUpdate)
	{
		_worldBoundingxBox = _boundingBox;
		_worldBoundingxBox.center(_boundingBox.center() + _transform.getTranslate());
		_needUpdate = false;
	}
}

_NAME_END