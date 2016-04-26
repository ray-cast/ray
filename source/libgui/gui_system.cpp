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
#include <ray/gui_system.h>

#if defined(_BUILD_MYGUI)
#	include "MYGUI/mygui_system.h"
#endif

_NAME_BEGIN

__ImplementSingleton(GuiSystem)

GuiSystem::GuiSystem() noexcept
{
}

GuiSystem::~GuiSystem() noexcept
{
}

bool
GuiSystem::open(GuiSystemBasePtr impl) except
{
	assert(!_system);
	if (impl)
		_system = impl;
	else
	{
#if defined(_BUILD_MYGUI)
		_system = std::make_shared<MyGuiSystem>();
#endif
	}
	return _system->open();
}

void
GuiSystem::close() noexcept
{
	if (_system)
	{
		_system.reset();
		_system = nullptr;
	}
}

void
GuiSystem::setGuiSystem(GuiSystemBasePtr& system) except
{
	assert(_system);
	if (_system != system)
	{
		_system = system;
	}
}

const GuiSystemBasePtr&
GuiSystem::getGuiSystem() const noexcept
{
	return _system;
}

void
GuiSystem::setCoreProfile(const std::string& core) except
{
	assert(_system);
	return _system->setCoreProfile(core);
}

const std::string&
GuiSystem::getCoreProfile() const noexcept
{
	assert(_system);
	return _system->getCoreProfile();
}

void
GuiSystem::setImageLoader(GuiImageLoaderPtr loader) noexcept
{
	assert(_system);
	return _system->setImageLoader(loader);
}

GuiImageLoaderPtr
GuiSystem::getImageLoader() const noexcept
{
	assert(_system);
	return _system->getImageLoader();
}

bool
GuiSystem::injectMouseMove(int _absx, int _absy, int _absz) noexcept
{
	assert(_system);
	return _system->injectMouseMove(_absx, _absy, _absz);
}

bool
GuiSystem::injectMousePress(int _absx, int _absy, GuiInputButton::Code _id) noexcept
{
	assert(_system);
	return _system->injectMousePress(_absx, _absy, _id);
}

bool
GuiSystem::injectMouseRelease(int _absx, int _absy, GuiInputButton::Code _id) noexcept
{
	assert(_system);
	return _system->injectMouseRelease(_absx, _absy, _id);
}

bool
GuiSystem::injectKeyPress(GuiInputKey::Code _key, GuiInputChar _char) noexcept
{
	assert(_system);
	return _system->injectKeyPress(_key, _char);
}

bool
GuiSystem::injectKeyRelease(GuiInputKey::Code _key) noexcept
{
	assert(_system);
	return _system->injectKeyRelease(_key);
}

bool
GuiSystem::isFocusMouse() const noexcept
{
	assert(_system);
	return _system->isFocusMouse();
}

bool
GuiSystem::isFocusKey() const noexcept
{
	assert(_system);
	return _system->isFocusKey();
}

bool
GuiSystem::isCaptureMouse() const noexcept
{
	assert(_system);
	return _system->isCaptureMouse();
}

void
GuiSystem::setViewport(int w, int h) noexcept
{
	assert(_system);
	_system->setViewport(w, h);
}

void
GuiSystem::getViewport(int& w, int& h) noexcept
{
	assert(_system);
	_system->getViewport(w, h);
}

GuiWidgetPtr
GuiSystem::createWidget(const rtti::Rtti* rtti)
{
	assert(_system);
	return _system->createWidget(rtti);
}

void
GuiSystem::render(float delta) except
{
	assert(_system);
	_system->render(delta);
}

_NAME_END