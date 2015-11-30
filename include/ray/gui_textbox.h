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
#ifndef _H_GUI_TEXTBOX_H_
#define _H_GUI_TEXTBOX_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiTextBox : public GuiWidget
{
	__DeclareSubInterface(GuiTextBox, GuiWidget)
public:
	GuiTextBox(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiTextBox() noexcept;

	virtual Rect getTextRegion() noexcept = 0;
	virtual void getTextSize(int& w, int& h) noexcept = 0;

	virtual void setText(const std::string& _value) noexcept = 0;
	virtual const std::string& getText() const noexcept = 0;

	virtual void setFontName(const std::string& _value) noexcept = 0;
	virtual const std::string& getFontName() noexcept = 0;

	virtual void setFontHeight(int _value) noexcept = 0;
	virtual int getFontHeight() noexcept = 0;

	virtual void setTextAlign(GuiWidgetAlign _value) noexcept = 0;
	virtual GuiWidgetAlign getTextAlign() noexcept = 0;

	virtual void setTextColour(const float4& value) noexcept = 0;
	virtual float4 getTextColour() noexcept = 0;

	virtual void setTextWithReplacing(const std::string& _value) noexcept = 0;

	virtual void setTextShadowColour(const float4& value) noexcept = 0;
	virtual float4 getTextShadowColour() noexcept = 0;

	virtual void setTextShadow(bool _value) noexcept = 0;
	virtual bool getTextShadow() const noexcept = 0;
};

_NAME_END

#endif