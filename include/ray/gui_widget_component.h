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
#ifndef _H_GUI_WIDGET_COMPONENT_H_
#define _H_GUI_WIDGET_COMPONENT_H_

#include <ray/game_component.h>
#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiWidgetComponent : public GameComponent
{
	__DeclareSubInterface(GuiWidgetComponent, GameComponent)
public:
	GuiWidgetComponent() noexcept;
	virtual ~GuiWidgetComponent() noexcept;

	void setAlign(GuiWidgetAlign align) noexcept;
	GuiWidgetAlign getAlign() noexcept;

	void setSkin(const std::string& skin) except;
	const std::string& getSkin() const noexcept;

protected:
	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	void _updateParent() noexcept;
	void _updateTransform() noexcept;

	virtual void onActivate() except;
	virtual void onDeactivate() except;

	virtual void onMoveAfter() noexcept;

protected:
	virtual void setGuiWidget(GuiWidgetPtr widget) noexcept = 0;
	virtual GuiWidgetPtr getGuiWidget() const noexcept = 0;

protected:

	virtual void onMouseDrag(float x, float y) noexcept;
	virtual void onMouseEnter(float x, float y) noexcept;
	virtual void onMouseOver(float x, float y) noexcept;
	virtual void onMouseMotion(float x, float y) noexcept;
	virtual void onMouseButtonDown(int button, float x, float y) noexcept;
	virtual void onMouseButtonUp(int button, float x, float y) noexcept;
};

_NAME_END

#endif