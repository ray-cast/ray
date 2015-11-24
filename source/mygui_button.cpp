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
#include <ray/mygui_button.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiButton, GuiButton, "MyGuiButton")
__ImplementSubClass(MyGuiButtonImpl, MyGuiWidget, "MyGuiButton")

MyGuiButtonImpl::MyGuiButtonImpl() noexcept
	: MyGuiWidget(_widget)
{
}

MyGuiButtonImpl::~MyGuiButtonImpl() noexcept
{
}

void
MyGuiButtonImpl::create(const std::string& skin, int left, int top, int width, int height, GuiWidgetAlign align, const std::string& name, void* widget) except
{
	if (widget)
		_button = ((MyGUI::Widget*)widget)->createWidget<MyGUI::Button>(MyGUI::WidgetStyle::Child, "Button", MyGUI::IntCoord(left, top, width, height), MyGUI::Align::Default, "Main", name);
	else
		_button = MyGUI::Gui::getInstance().createWidget<MyGUI::Button>("Button", MyGUI::IntCoord(left, top, width, height), MyGUI::Align::Default, "Main", name);
	_widget = _button;
}

MyGuiButton::MyGuiButton() noexcept
	: GuiButton(_impl)
{
}

MyGuiButton::~MyGuiButton() noexcept
{
}

_NAME_END