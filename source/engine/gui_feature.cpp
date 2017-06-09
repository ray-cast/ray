// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/gui_feature.h>

#include <ray/image.h>

#include <ray/input_event.h>
#include <ray/input_feature.h>

#include <ray/game_server.h>
#include <ray/game_application.h>

#include <ray/imgui.h>
#include <ray/imgui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

class ImageLoader : public GuiImageLoader
{
public:
	ImageLoader() noexcept
	{
	}

	~ImageLoader() noexcept
	{
	}

	bool loadImage(int& _width, int& _height, PixelFormat& _format, const util::string& _filename, MemoryStream& stream)
	{
		image::Image image;
		if (image.load(_filename))
		{
			_width = image.width();
			_height = image.height();

			auto format = image.format();
			if (format == image::format_t::R8G8B8A8UNorm || format == image::format_t::R8G8B8A8SRGB)
				_format = PixelFormat::R8G8B8A8;
			else if (format == image::format_t::R8G8B8UNorm || format == image::format_t::R8G8B8SRGB)
				_format = PixelFormat::R8G8B8;
			else
			{
				assert(false);
				return false;
			}

			std::size_t size = image.size();

			stream.resize(size);
			stream.write((char*)image.data(), size);
			return true;
		}

		return false;
	}

	void saveImage(int _width, int _height, PixelFormat _format, void* _texture, const util::string& _filename)
	{
	}

private:
	ImageLoader(const ImageLoader&) noexcept = delete;
	ImageLoader& operator=(const ImageLoader&) noexcept = delete;
};

GuiFeature::GuiFeature() noexcept
	: _window(0)
	, _width(0)
	, _height(0)
	, _framebuffer_w(0)
	, _framebuffer_h(0)
	, _dpi(1.0)
{
}

GuiFeature::GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept
	: _window(window)
	, _width(w)
	, _height(h)
	, _framebuffer_w(framebuffer_w)
	, _framebuffer_h(framebuffer_h)
	, _dpi(dpi)
{
}

GuiFeature::~GuiFeature() noexcept
{
}

void
GuiFeature::setViewport(std::uint32_t w, std::uint32_t h) noexcept
{
	if (_width != w || _height != h)
	{
		_width = w;
		_height = h;

		IMGUISystem::instance()->setViewport(w, h);
	}
}

void
GuiFeature::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
{
	IMGUISystem::instance()->getViewport(w, h);
}

void
GuiFeature::setWindowFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
{
	if (_framebuffer_w != w || _framebuffer_h != h)
	{
		_framebuffer_w = w;
		_framebuffer_h = h;

		IMGUISystem::instance()->setFramebufferScale(w, h);
	}
}

void
GuiFeature::getWindowFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
{
	IMGUISystem::instance()->getFramebufferScale(w, h);
}

void
GuiFeature::onActivate() except
{
	if (!IMGUISystem::instance()->open(_window, _dpi))
		throw failure("GuiSystem::instance() fail");

	IMGUISystem::instance()->setImageLoader(std::make_shared<ImageLoader>());
#if _BUILD_MYGUI
	IMGUISystem::instance()->setCoreProfile("sys:media/UI/MyGUI_Core.xml");
#endif
	IMGUISystem::instance()->setViewport(_width, _height);
	IMGUISystem::instance()->setFramebufferScale(_framebuffer_w, _framebuffer_h);
}

void
GuiFeature::onDeactivate() noexcept
{
	IMGUISystem::instance()->close();
}

void
GuiFeature::onMessage(const MessagePtr& message) except
{
	if (message->isInstanceOf<InputMessage>())
	{
		auto inputMessage = message->downcast<InputMessage>();
		if (inputMessage)
		{
			InputEvent event = inputMessage->getEvent();
			switch (event.event)
			{
			case InputEvent::MouseMotion:
				IMGUISystem::instance()->injectMouseMove(event.motion.x, event.motion.y);
				break;
			case InputEvent::MouseButtonDown:
				IMGUISystem::instance()->injectMousePress(event.button.x, event.button.y, (InputButton::Code)event.button.button);
				break;
			case InputEvent::MouseButtonUp:
				IMGUISystem::instance()->injectMouseRelease(event.button.x, event.button.y, (InputButton::Code)event.button.button);
				break;
			case InputEvent::KeyDown:
				IMGUISystem::instance()->injectKeyPress((InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
				break;
			case InputEvent::KeyUp:
				IMGUISystem::instance()->injectKeyRelease((InputKey::Code)event.key.keysym.sym);
				break;
			case InputEvent::Character:
				IMGUISystem::instance()->injectKeyPress(InputKey::Code::None, event.key.keysym.unicode);
				break;
			case InputEvent::LostFocus:
				IMGUISystem::instance()->injectWindowFocus(false);
				break;
			case InputEvent::GetFocus:
				IMGUISystem::instance()->injectWindowFocus(true);
				break;
			case InputEvent::MouseWheelUp:
			case InputEvent::MouseWheelDown:
				IMGUISystem::instance()->injectMouseWheel((event.event == InputEvent::MouseWheelUp) ? 1.0f : -1.0f);
				break;
			case InputEvent::SizeChange:
				IMGUISystem::instance()->setViewport(event.change.w, event.change.h);
				break;
			case InputEvent::SizeChangeDPI:
				IMGUISystem::instance()->setFramebufferScale(event.change.w, event.change.h);
				break;
			default:
				return;
			}
		}
	}
}

_NAME_END