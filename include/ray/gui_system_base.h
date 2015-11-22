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
#ifndef _H_GUI_SYSTEM_BASE_H_
#define _H_GUI_SYSTEM_BASE_H_

#include <ray/gui_input_key.h>
#include <ray/gui_input_button.h>
#include <ray/gui_imageloader.h>

_NAME_BEGIN

namespace Gui
{
	class EXPORT GuiSystem : public rtti::Interface
	{
		__DeclareSubInterface(GuiSystem, rtti::Interface)
	public:
		GuiSystem() noexcept;
		virtual ~GuiSystem() noexcept;

		virtual bool open() except = 0;
		virtual void close() noexcept = 0;

		virtual void setCoreProfile(const std::string& core) except = 0;
		virtual const std::string& getCoreProfile() const noexcept = 0;

		virtual void setImageLoader(GuiImageLoaderPtr loader) noexcept = 0;
		virtual GuiImageLoaderPtr getImageLoader() const noexcept = 0;

		virtual bool injectMouseMove(int _absx, int _absy, int _absz) noexcept = 0;
		virtual bool injectMousePress(int _absx, int _absy, GuiButton::Code _id) noexcept = 0;
		virtual bool injectMouseRelease(int _absx, int _absy, GuiButton::Code _id) noexcept = 0;
		virtual bool injectKeyPress(GuiKey::Code _key) noexcept = 0;
		virtual bool injectKeyRelease(GuiKey::Code _key) noexcept = 0;

		virtual bool isFocusMouse() const noexcept = 0;
		virtual bool isFocusKey() const noexcept = 0;
		virtual bool isCaptureMouse() const noexcept = 0;

		virtual void setViewport(int w, int h) noexcept = 0;
		virtual void getViewport(int& w, int& h) noexcept = 0;

		virtual void render(float delta) except = 0;
	};
}

_NAME_END

#endif
