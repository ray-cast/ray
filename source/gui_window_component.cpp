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
#if defined(_BUILD_GUI)
#include <ray/gui_window_component.h>
#include <ray/gui_window.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiWindowComponent, GuiLabelComponent, "GuiWindow")

GuiWindowComponent::GuiWindowComponent() noexcept
{	
	_window = GuiSystem::instance()->createWidget<GuiWindow>();
	_window->create();

	this->setGuiTextBox(_window->getGuiTextBox());
}

GuiWindowComponent::~GuiWindowComponent() noexcept
{
}

void 
GuiWindowComponent::setVisibleSmooth(bool value) noexcept
{
	assert(_window);
	_window->setVisibleSmooth(value);
}

void 
GuiWindowComponent::destroySmooth() noexcept
{
	assert(_window);
	_window->destroySmooth();
}

void 
GuiWindowComponent::setAutoAlpha(bool value) noexcept
{
	assert(_window);
	_window->setAutoAlpha(value);
}

bool 
GuiWindowComponent::getAutoAlpha() const noexcept
{
	assert(_window);
	return _window->getAutoAlpha();
}

void 
GuiWindowComponent::setMinSize(int width, int height) noexcept
{
	assert(_window);
	_window->setMinSize(width, height);
}

void 
GuiWindowComponent::getMinSize(int& w, int& h) const noexcept
{
	assert(_window);
	_window->getMinSize(w, h);
}

void 
GuiWindowComponent::setMaxSize(int width, int height) noexcept
{
	assert(_window);
	_window->setMaxSize(width, height);
}

void
GuiWindowComponent::getMaxSize(int& w, int& h) const noexcept
{
	assert(_window);
	_window->getMaxSize(w, h);
}

void 
GuiWindowComponent::load(iarchive& reader) noexcept
{
	GuiLabelComponent::load(reader);

	bool smooth = true;
	bool autoAlpha = false;
	int minWidth = 0, maxWidth = 10000;
	int minHeight = 0, maxHeight = 10000;
	
	reader >> make_archive(smooth, "smooth");
	reader >> make_archive(autoAlpha, "autoAlpha");
	reader >> make_archive(minWidth, "minWidth");
	reader >> make_archive(minWidth, "minHeight");
	reader >> make_archive(maxWidth, "maxWidth");
	reader >> make_archive(maxHeight, "maxHeight");

	this->setVisibleSmooth(smooth);
	this->setAutoAlpha(autoAlpha);
	this->setMinSize(minWidth, minHeight);
	this->setMaxSize(maxWidth, maxHeight);
}

void 
GuiWindowComponent::save(oarchive& write) noexcept
{
	GuiWidgetComponent::save(write);
}

GameComponentPtr
GuiWindowComponent::clone() const except
{
	return std::make_shared<GuiWindowComponent>();
}

void 
GuiWindowComponent::setGuiWidget(GuiWidgetPtr widget) noexcept
{
	assert(widget->isInstanceOf<GuiWidget>());
	_window = widget->downcast<GuiWindow>();
}

GuiWidgetPtr 
GuiWindowComponent::getGuiWidget() const noexcept
{
	return _window;
}

_NAME_END

#endif