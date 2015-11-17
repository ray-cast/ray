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
#if defined(_BUILD_RENDERER)
#include <ray/render_component.h>
#include <ray/render_factory.h>
#include <ray/render_features.h>

#include <ray/game_server.h>
#include <ray/game_event.h>

_NAME_BEGIN

__ImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

RenderComponent::RenderComponent() noexcept
	: _isCastShadow(true)
	, _isReceiveShadow(true)
{
}

RenderComponent::~RenderComponent() noexcept
{
	if (_renderObject)
		_renderObject->setRenderScene(nullptr);
}

void
RenderComponent::setCastShadow(bool value) noexcept
{
	_isCastShadow = value;
}

bool
RenderComponent::getCastShadow() const noexcept
{
	return _isCastShadow;
}

void
RenderComponent::setReceiveShadow(bool value) noexcept
{
	_isReceiveShadow = value;
}

bool
RenderComponent::getReceiveShadow() const noexcept
{
	return _isReceiveShadow;
}

void
RenderComponent::setMaterial(MaterialPtr material) noexcept
{
	_material = material;
}

void
RenderComponent::setSharedMaterial(MaterialPtr material) noexcept
{
	_material = material;
}

MaterialPtr
RenderComponent::getMaterial() const noexcept
{
	return _material;
}

MaterialPtr
RenderComponent::getSharedMaterial() const noexcept
{
	return _sharedMaterial;
}

bool
RenderComponent::hasMaterial() const noexcept
{
	return _material != nullptr;
}

bool
RenderComponent::hasSharedMaterial() const noexcept
{
	return _sharedMaterial != nullptr;
}

void
RenderComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	reader >> make_alias(_isCastShadow, "castshadow");
	reader >> make_alias(_isReceiveShadow, "receiveshadow");

	std::string material;
	reader >> make_name(material);

	this->setName(material);
}

void
RenderComponent::save(oarchive& write) noexcept
{
	write << make_alias(_isCastShadow, "castshadow");
	write << make_alias(_isReceiveShadow, "receiveshadow");
}

void
RenderComponent::_attacRenderObjects() noexcept
{
	auto gameServer = this->getGameObject()->getGameServer();
	if (!gameServer) { assert(gameServer); return;}
		
	auto renderer = gameServer->getFeature<RenderFeatures>();
	if (!renderer) { assert(renderer); return; }

	auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
	if (!renderScene) { assert(renderScene); return; }

	if (_renderObject)
		_renderObject->setRenderScene(renderScene);
}

void
RenderComponent::_dettachRenderhObjects() noexcept
{
	if (_renderObject)
	{
		_renderObject->setRenderScene(nullptr);
		_renderObject = nullptr;
	}
}

void 
RenderComponent::_setRenderObject(RenderObjectPtr object) noexcept
{
	this->_dettachRenderhObjects();

	_renderObject = object;
	_renderObject->setRenderListener(this);

	this->_attacRenderObjects();
}

RenderObjectPtr
RenderComponent::_getRenderObject() noexcept
{
	return _renderObject;
}

void
RenderComponent::onMoveAfter() noexcept
{
	_renderObject->setTransform(this->getGameObject()->getTransform());
	_renderObject->setTransformInverse(this->getGameObject()->getTransformInverse());
	_renderObject->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());
}

void
RenderComponent::onLayerChangeAfter() noexcept
{
	_renderObject->setLayer(this->getGameObject()->getLayer());
}

void 
RenderComponent::onWillRenderObject(const Camera& camera) noexcept
{
	//WillRenderObjectEvent event;
	//this->sendMessage(&event);
}

void 
RenderComponent::onRenderObject(const Camera& camera) noexcept
{
	//RenderObjectEvent event;
	//this->sendMessage(&event);
}

_NAME_END
#endif