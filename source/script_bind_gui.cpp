// +----------------------------------------------------------------------
// | Project : Xiemoperor.
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
#include <ray/script_bind_gui.h>
#include <angelscript.h>

_NAME_BEGIN

ScriptBindGUI::ScriptBindGUI() noexcept
{
}

ScriptBindGUI::~ScriptBindGUI() noexcept
{
}

void
ScriptBindGUI::setGuiSystem(GuiSystemPtr gui) noexcept
{
	_gui = gui;
}

GuiSystemPtr
ScriptBindGUI::setGuiSystem() const noexcept
{
	return _gui;
}

void
ScriptBindGUI::setup(asIScriptEngine* _engine) noexcept
{
	int r = 0;

	r = _engine->RegisterObjectType("GuiSystem", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	r = _engine->RegisterGlobalProperty("GuiSystem GUI", this); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void beginFrame(int x, int y, int but, int scroll)", asMETHOD(ScriptBindGUI, beginFrame), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void endFrame()", asMETHOD(ScriptBindGUI, endFrame), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void beginScrollArea(const string& in, int x, int y, int w, int h, int scroll)", asMETHOD(ScriptBindGUI, beginScrollArea), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void endScrollArea()", asMETHOD(ScriptBindGUI, endScrollArea), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void button(const string& in, bool enabled = true)", asMETHOD(ScriptBindGUI, button), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void item(const string& in, bool enabled = true)", asMETHOD(ScriptBindGUI, item), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void check(const string& in, bool checked, bool enabled = true)", asMETHOD(ScriptBindGUI, check), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void collapse(const string& in, const string& in, bool checked, bool enabled = true)", asMETHOD(ScriptBindGUI, collapse), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void label(const string& in)", asMETHOD(ScriptBindGUI, label), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void value(const string& in)", asMETHOD(ScriptBindGUI, value), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void slider(const string& in, float val, float vmin, float vmax, float vinc, bool enabled = true)", asMETHOD(ScriptBindGUI, slider), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void indent()", asMETHOD(ScriptBindGUI, indent), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void unindent()", asMETHOD(ScriptBindGUI, unindent), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void separator()", asMETHOD(ScriptBindGUI, separator), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void separatorLine()", asMETHOD(ScriptBindGUI, separatorLine), asCALL_THISCALL); assert(r >= 0);

	r = _engine->RegisterObjectMethod("GuiSystem", "void drawText(int x, int y, int align, const string& in, const float4& in)", asMETHOD(ScriptBindGUI, drawText), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void drawLine(float x0, float y0, float x1, float y1, float r, const float4& in)", asMETHOD(ScriptBindGUI, drawLine), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void drawRoundedRect(float x, float y, float w, float h, float r, const float4& in)", asMETHOD(ScriptBindGUI, drawRoundedRect), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("GuiSystem", "void drawRect(float x, float y, float w, float h, const float4& in)", asMETHOD(ScriptBindGUI, drawRect), asCALL_THISCALL); assert(r >= 0);
}

void
ScriptBindGUI::beginFrame(int x, int y, int but, int scroll) noexcept
{
	if (_gui)
		_gui->beginFrame(x, y, but, scroll);
}

void
ScriptBindGUI::endFrame() noexcept
{
	if (_gui)
		_gui->endFrame();
}

void
ScriptBindGUI::beginScrollArea(const std::string& name, int x, int y, int w, int h, int scroll) noexcept
{
	if (_gui)
		_gui->beginScrollArea(name, x, y, w, h, scroll);
}

void
ScriptBindGUI::endScrollArea() noexcept
{
	if (_gui)
		_gui->endScrollArea();
}

void
ScriptBindGUI::button(const std::string& text, bool enabled) noexcept
{
	if (_gui)
		_gui->button(text, enabled);
}

void
ScriptBindGUI::item(const std::string& text, bool enabled) noexcept
{
	if (_gui)
		_gui->item(text, enabled);
}

void
ScriptBindGUI::check(const std::string& text, bool checked, bool enabled) noexcept
{
	if (_gui)
		_gui->check(text, checked, enabled);
}

void
ScriptBindGUI::collapse(const std::string& text, const std::string& subtext, bool checked, bool enabled) noexcept
{
	if (_gui)
		_gui->collapse(text, subtext, checked, enabled);
}

void
ScriptBindGUI::label(const std::string& text) noexcept
{
	if (_gui)
		_gui->label(text);
}

void
ScriptBindGUI::value(const std::string& text) noexcept
{
	if (_gui)
		_gui->value(text);
}

void
ScriptBindGUI::slider(const std::string& text, float val, float vmin, float vmax, float vinc, bool enabled) noexcept
{
	if (_gui)
		_gui->slider(text, val, vmin, vmax, vinc, enabled);
}

void
ScriptBindGUI::indent() noexcept
{
	if (_gui)
		_gui->indent();
}

void
ScriptBindGUI::unindent() noexcept
{
	if (_gui)
		_gui->unindent();
}

void
ScriptBindGUI::separator() noexcept
{
	if (_gui)
		_gui->separator();
}

void
ScriptBindGUI::separatorLine() noexcept
{
	if (_gui)
		_gui->separatorLine();
}

void
ScriptBindGUI::drawText(int x, int y, int align, const std::string& text, const float4& color) noexcept
{
	if (_gui)
		_gui->drawText(x, y, align, text, color);
}

void
ScriptBindGUI::drawLine(float x0, float y0, float x1, float y1, float r, const float4& color) noexcept
{
	if (_gui)
		_gui->drawLine(x0, y0, x1, y1, r, color);
}

void
ScriptBindGUI::drawRoundedRect(float x, float y, float w, float h, float r, const float4& color) noexcept
{
	if (_gui)
		_gui->drawRoundedRect(x, y, w, h, r, color);
}

void
ScriptBindGUI::drawRect(float x, float y, float w, float h, const float4& color) noexcept
{
	if (_gui)
		_gui->drawRect(x, y, w, h, color);
}

_NAME_END