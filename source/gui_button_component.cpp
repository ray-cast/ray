// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderThread software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderThread list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderThread list of conditions and the
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
#if defined(_BUILD_GUI)
#include <ray/gui_button_component.h>
#include <ray/render_system.h>

_NAME_BEGIN

__ImplementSubClass(GUIButtonComponent, GUIBehaviourComponent, "GUIButton")

GUIButtonComponent::GUIButtonComponent() noexcept
{
	_normal.textColor = Vector4(0.5, 0.5, 0.5, 1.0);
	_hover.textColor = Vector4(0.6, 0.6, 0.6, 1.0);
	_active.textColor = Vector4(0.4, 0.4, 0.4, 1.0);
	_current.textColor = _normal.textColor;
}

GUIButtonComponent::~GUIButtonComponent() noexcept
{
}

GameComponentPtr 
GUIButtonComponent::clone() const except
{
	return std::make_shared<GUIButtonComponent>();
}

void
GUIButtonComponent::onActivate() except
{
	_material = RenderSystem::instance()->createMaterial("dlc:UI/materials/default.mat");
}

void 
GUIButtonComponent::buildUIControl(GUILayoutComponentPtr layout) noexcept
{
	Vector4 color;

	auto center = this->getGameObject()->getTranslate();
	auto half = this->getGameObject()->getScale() * 0.5;

	auto v0 = float3(center.x - half.x, center.y + half.y, center.z);
	auto v1 = float3(center.x - half.x, center.y - half.y, center.z);
	auto v2 = float3(center.x + half.x, center.y - half.y, center.z);
	auto v3 = float3(center.x + half.x, center.y + half.y, center.z);

	auto materialPass = _material->getTech(RenderQueue::RQ_OPAQUE)->getPass(RenderPass::RP_OPAQUES);

	layout->setMaterialPass(materialPass);
	layout->drawQuad(v0, v1, v2, v3, _current.textColor);

	this->needUpdate(false);
}

GameComponentPtr
GUIButtonComponent::hitTest(const Vector3& raycast) noexcept
{
	auto center = this->getGameObject()->getTranslate();
	auto half = this->getGameObject()->getScale() * 0.5;

	auto v0 = float3(center.x - half.x, center.y + half.y, center.z);
	auto v1 = float3(center.x - half.x, center.y - half.y, center.z);
	auto v2 = float3(center.x + half.x, center.y - half.y, center.z);
	auto v3 = float3(center.x + half.x, center.y + half.y, center.z);

	if (center.x - half.x <= raycast.x &&
		center.x + half.x >= raycast.x &&
		center.y - half.y <= raycast.y &&
		center.y + half.y >= raycast.y)
	{
		return std::dynamic_pointer_cast<GameComponent>(this->shared_from_this());
	}

	return nullptr;
}

void 
GUIButtonComponent::onMouseEnter(float x, float y) noexcept
{
	_current = _hover;
	this->needUpdate(true);
}

void 
GUIButtonComponent::onMouseOver(float x, float y) noexcept
{
	_current = _normal;
	this->needUpdate(true);
}

void 
GUIButtonComponent::onMouseButtonDown(int button, float x, float y) noexcept
{
	_current = _active;
	this->needUpdate(true);
}

void 
GUIButtonComponent::onMouseButtonUp(int button, float x, float y) noexcept
{
	_current = _normal;
	this->needUpdate(true);
}

_NAME_END
#endif