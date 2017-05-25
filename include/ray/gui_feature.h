// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_GUI_FEATURE_H_
#define _H_GUI_FEATURE_H_

#include <ray/game_features.h>

_NAME_BEGIN

class GuiFeature final : public GameFeature
{
	__DeclareSubClass(GuiFeature, GameFeature)
public:
	GuiFeature() noexcept;
	GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h) noexcept;
	~GuiFeature() noexcept;

private:
	void setViewport(std::uint32_t w, std::uint32_t h) noexcept;
	void getViewport(std::uint32_t& w, std::uint32_t& h) noexcept;

	void setWindowFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

protected:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onMessage(const MessagePtr& message) except;

private:
	WindHandle _window;
	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _dpi_w;
	std::uint32_t _dpi_h;
};

_NAME_END

#endif