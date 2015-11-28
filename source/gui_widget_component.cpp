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
#if defined(_BUILD_GUI)
#include <ray/gui_widget_component.h>

_NAME_BEGIN

__ImplementSubInterface(GuiWidgetComponent, GameComponent, "GUIListener")

GuiWidgetComponent::GuiWidgetComponent() noexcept
{
}

GuiWidgetComponent::~GuiWidgetComponent() noexcept
{
}

void 
GuiWidgetComponent::setAlign(GuiWidgetAlign align) noexcept
{
	this->getGuiWidget()->setAlign(align);
}

GuiWidgetAlign
GuiWidgetComponent::getAlign() noexcept
{
	return this->getGuiWidget()->getAlign();
}

void
GuiWidgetComponent::setSkin(const std::string& skin) except
{
	if (_skin != skin)
	{
		this->getGuiWidget()->changeWidgetSkin(skin);
		_skin = skin;
	}
}

const std::string&
GuiWidgetComponent::getSkin() const noexcept
{
	return _skin;
}

void 
GuiWidgetComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	std::string skin;
	std::string align;

	reader >> make_alias(skin, "skin");
	reader >> make_alias(align, "align");

	this->setSkin(skin);
	this->setAlign(GuiWidgetAlign::parse(align));
}

void 
GuiWidgetComponent::save(oarchive& write) noexcept
{
	auto string = this->getAlign().print();
	auto skin = this->getSkin();

	write << make_alias(skin, "skin");
	write << make_alias(string, "align");
}

void 
GuiWidgetComponent::_updateParent() noexcept
{
	auto parent = this->getGameObject()->getParent();
	if (parent)
	{
		auto component = parent->getComponent<GuiWidgetComponent>();
		if (component)
		{
			this->getGuiWidget()->setParent(component->getGuiWidget());
		}
	}
}

void 
GuiWidgetComponent::_updateTransform() noexcept
{
	auto scale = this->getGameObject()->getScale();
	auto translate = this->getGameObject()->getTranslate();

	Viewport view;
	view.left = translate.x;
	view.top = translate.y;
	view.width = scale.x;
	view.height = scale.y;

	this->getGuiWidget()->setViewport(view);
}

void 
GuiWidgetComponent::onActivate() except
{
	this->_updateParent();
	this->_updateTransform();
}

void 
GuiWidgetComponent::onDeactivate() except
{
}

void
GuiWidgetComponent::onMoveAfter() noexcept
{
	this->_updateTransform();
}

void 
GuiWidgetComponent::onMouseDrag(float x, float y) noexcept
{
}

void 
GuiWidgetComponent::onMouseEnter(float x, float y) noexcept
{
}

void 
GuiWidgetComponent::onMouseOver(float x, float y) noexcept
{
}

void 
GuiWidgetComponent::onMouseMotion(float x, float y) noexcept
{
}

void 
GuiWidgetComponent::onMouseButtonDown(int button, float x, float y) noexcept
{
}

void 
GuiWidgetComponent::onMouseButtonUp(int button, float x, float y) noexcept
{
}

_NAME_END
#endif