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

__ImplementSubInterface(GuiWidgetComponent, GameComponent, "GuiWidget")

GuiWidgetComponent::GuiWidgetComponent() noexcept
{
}

GuiWidgetComponent::~GuiWidgetComponent() noexcept
{
}

void
GuiWidgetComponent::setAlign(GuiWidgetAlign align) noexcept
{
	assert(_widget);
	_widget->setAlign(align);
}

GuiWidgetAlign
GuiWidgetComponent::getAlign() noexcept
{
	assert(_widget);
	return _widget->getAlign();
}

void
GuiWidgetComponent::setSkin(const std::string& skin) except
{
	assert(_widget);
	if (_skin != skin)
	{
		_widget->changeWidgetSkin(skin);
		_skin = skin;
	}
}

const std::string&
GuiWidgetComponent::getSkin() const noexcept
{
	return _skin;
}

void
GuiWidgetComponent::addKeySetFocus(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addKeySetFocus(func);
}

void
GuiWidgetComponent::addKeyLostFocus(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addKeyLostFocus(func);
}

void
GuiWidgetComponent::addKeyButtonPressed(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addKeyButtonPressed(func);
}

void
GuiWidgetComponent::addKeyButtonReleased(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addKeyButtonReleased(func);
}

void
GuiWidgetComponent::addMouseMove(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseMove(func);
}

void
GuiWidgetComponent::addMouseDrag(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseDrag(func);
}

void
GuiWidgetComponent::addMouseWheel(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseWheel(func);
}

void
GuiWidgetComponent::addMouseSetFocusListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseSetFocusListener(func);
}

void
GuiWidgetComponent::addMouseLostFocusListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseLostFocusListener(func);
}

void
GuiWidgetComponent::addMouseButtonPressedListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseButtonPressedListener(func);
}

void
GuiWidgetComponent::addMouseButtonReleasedListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseButtonReleasedListener(func);
}

void
GuiWidgetComponent::addMouseButtonClickListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseButtonClickListener(func);
}

void
GuiWidgetComponent::addMouseButtonDoubleClickListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->addMouseButtonDoubleClickListener(func);
}

void
GuiWidgetComponent::removeKeySetFocus(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeKeySetFocus(func);
}

void
GuiWidgetComponent::removeKeyLostFocus(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeKeyLostFocus(func);
}

void
GuiWidgetComponent::removeKeyButtonPressed(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeKeyButtonPressed(func);
}

void
GuiWidgetComponent::removeKeyButtonReleased(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeKeyButtonReleased(func);
}

void
GuiWidgetComponent::removeMouseMove(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseMove(func);
}

void
GuiWidgetComponent::removeMouseDrag(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseDrag(func);
}

void
GuiWidgetComponent::removeMouseWheel(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseWheel(func);
}

void
GuiWidgetComponent::removeMouseSetFocusListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseSetFocusListener(func);
}

void
GuiWidgetComponent::removeMouseLostFocusListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseLostFocusListener(func);
}

void
GuiWidgetComponent::removeMouseButtonPressedListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseButtonPressedListener(func);
}

void
GuiWidgetComponent::removeMouseButtonReleasedListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseButtonReleasedListener(func);
}

void
GuiWidgetComponent::removeMouseButtonClickListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseButtonClickListener(func);
}

void
GuiWidgetComponent::removeMouseButtonDoubleClickListener(binder<void()>&& func) noexcept
{
	assert(_widget);
	_widget->removeMouseButtonDoubleClickListener(func);
}

void
GuiWidgetComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	std::string skin;
	std::string align;

	reader >> make_archive(skin, "skin");
	reader >> make_archive(align, "align");

	this->setSkin(skin);
	this->setAlign(GuiWidgetAlign::parse(align));
}

void
GuiWidgetComponent::save(oarchive& write) noexcept
{
	auto string = this->getAlign().print();
	auto skin = this->getSkin();

	write << make_archive(skin, "skin");
	write << make_archive(string, "align");
}

void
GuiWidgetComponent::setGuiWidget(GuiWidgetPtr widget) noexcept
{
	assert(widget);
	_widget = widget;
}

GuiWidgetPtr
GuiWidgetComponent::getGuiWidget() const noexcept
{
	return _widget;
}

void
GuiWidgetComponent::_updateParent() noexcept
{
	assert(_widget);

	auto parent = this->getGameObject()->getParent();
	if (parent)
	{
		auto component = parent->getComponent<GuiWidgetComponent>();
		if (component)
		{
			_widget->setParent(component->getGuiWidget());
		}
	}
}

void
GuiWidgetComponent::_updateTransform() noexcept
{
	assert(_widget);

	auto scale = this->getGameObject()->getScale();
	auto translate = this->getGameObject()->getTranslate();

	Rect view;
	view.x = translate.x;
	view.y = translate.y;
	view.w = scale.x;
	view.h = scale.y;

	_widget->setViewport(view);
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

_NAME_END

#endif