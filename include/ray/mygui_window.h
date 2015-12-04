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

#include <ray/mygui_textbox.h>

_NAME_BEGIN

class MyGuiWindowImpl final : public MyGuiWidget
{
	__DeclareSubClass(MyGuiWindowImpl, MyGuiWidget)
public:
	MyGuiWindowImpl() noexcept;
	~MyGuiWindowImpl() noexcept;
	
	bool create() except;

	GuiTextBoxPtr getGuiTextBox() const noexcept;

	void setVisibleSmooth(bool _value) noexcept;
	void destroySmooth() noexcept;

	void setAutoAlpha(bool _value) noexcept;
	bool getAutoAlpha() const noexcept;

	void setMinSize(int _width, int _height) noexcept;
	void getMinSize(int& w, int& h) const noexcept;

	void setMaxSize(int _width, int _height) noexcept;
	void getMaxSize(int& w, int& h) const noexcept;

	void addWindowButtonPressListener(std::function<void()>& func) noexcept;
	void addWindowCoordChangeListener(std::function<void()>& func) noexcept;

	void removeWindowButtonPressListener(std::function<void()>& func) noexcept;
	void removeWindowCoordChangeListener(std::function<void()>& func) noexcept;

private:
	void onWindowButtonPress(MyGUI::Widget*, const std::string&) except;
	void onWindowCoordChange(MyGUI::Widget*) except;

private:
	MyGuiWindowImpl(const MyGuiWindowImpl&) noexcept = delete;
	MyGuiWindowImpl& operator=(const MyGuiWindowImpl&) noexcept = delete;

private:

	std::string _name;

	MyGUI::Widget* _parent;
	MyGUI::Window* _window;

	GuiTextBoxPtr _textbox;
	
	delegate<void()> _onWindowButtonPress;
	delegate<void()> _onWindowCoordChange;
};

class MyGuiWindow final : public GuiWindow
{
	__DeclareSubClass(MyGuiWindow, GuiWindow)
public:
	MyGuiWindow() noexcept;
	virtual ~MyGuiWindow() noexcept;

	virtual GuiTextBoxPtr getGuiTextBox() const noexcept;

	virtual void setVisibleSmooth(bool _value) noexcept;
	virtual void destroySmooth() noexcept;

	virtual void setAutoAlpha(bool _value) noexcept;
	virtual bool getAutoAlpha() const noexcept;

	virtual void setMinSize(int width, int height) noexcept;
	virtual void getMinSize(int& w, int& h) const noexcept;

	virtual void setMaxSize(int width, int height) noexcept;
	virtual void getMaxSize(int& w, int& h) const noexcept;

	virtual void addWindowButtonPressListener(std::function<void()>& func) noexcept;
	virtual void addWindowCoordChangeListener(std::function<void()>& func) noexcept;

	virtual void removeWindowButtonPressListener(std::function<void()>& func) noexcept;
	virtual void removeWindowCoordChangeListener(std::function<void()>& func) noexcept;

private:
	MyGuiWindow(const MyGuiWindow&) noexcept = delete;
	MyGuiWindow& operator=(const MyGuiWindow&) noexcept = delete;

private:

	MyGuiWindowImpl _impl;	
};

_NAME_END

#endif