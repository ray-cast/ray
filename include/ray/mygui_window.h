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
#ifndef _H_MYGUI_WINDOW_H_
#define _H_MYGUI_WINDOW_H_

#include <ray/mygui_widget.h>

_NAME_BEGIN

class MyGuiWindowImpl : public MyGuiWidget
{
public:
	MyGuiWindowImpl() noexcept;
	~MyGuiWindowImpl() noexcept;
	
	void create(const std::string& skin, int left, int top, int width, int height, GuiWidgetAlign align, const std::string& name, void* widget) except;

	void setCaption(const std::string& name) noexcept;
	const std::string& getCaption() const noexcept;

	void setVisibleSmooth(bool _value) noexcept;
	void destroySmooth() noexcept;

	void setAutoAlpha(bool _value) noexcept;
	bool getAutoAlpha() const noexcept;

	void setMinSize(int _width, int _height) noexcept;
	void getMinSize(int& w, int& h) const noexcept;

	void setMaxSize(int _width, int _height) noexcept;
	void getMaxSize(int& w, int& h) const noexcept;

private:

	std::string _name;

	MyGUI::Widget* _widget;
	MyGUI::Window* _window;
};

class MyGuiWindow : public GuiWindow
{
public:
	MyGuiWindow() noexcept;
	MyGuiWindow(const std::string& skin, int left, int top, int width, int height, GuiWidgetAlign align, const std::string& name) noexcept;
	virtual ~MyGuiWindow() noexcept;

	virtual void setCaption(const std::string& name) noexcept;
	virtual const std::string& getCaption() const noexcept;

	virtual void setVisibleSmooth(bool _value) noexcept;
	virtual void destroySmooth() noexcept;

	virtual void setAutoAlpha(bool _value) noexcept;
	virtual bool getAutoAlpha() const noexcept;

	virtual void setMinSize(int width, int height) noexcept;
	virtual void getMinSize(int& w, int& h) const noexcept;

	virtual void setMaxSize(int width, int height) noexcept;
	virtual void getMaxSize(int& w, int& h) const noexcept;

private:

	MyGuiWindowImpl _impl;	
};

_NAME_END

#endif