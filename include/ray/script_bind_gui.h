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
#ifndef _H_SCRIPT_BIND_GUI_H_
#define _H_SCRIPT_BIND_GUI_H_

#include <ray/script_binder.h>
#include <ray/gui.h>

_NAME_BEGIN

class ScriptBindGUI final : public ScriptBinder
{
public:
    ScriptBindGUI() noexcept;
    ~ScriptBindGUI() noexcept;

    void setGuiSystem(GuiSystemPtr input) noexcept;
    GuiSystemPtr setGuiSystem() const noexcept;

    void setup(asIScriptEngine* _engine) noexcept;

private:
    void beginFrame(int x, int y, int but, int scroll) noexcept;
    void endFrame() noexcept;
    void beginScrollArea(const std::string& name, int x, int y, int w, int h, int scroll) noexcept;
    void endScrollArea() noexcept;
    void button(const std::string& text, bool enabled = true) noexcept;
    void item(const std::string& text, bool enabled = true) noexcept;
    void check(const std::string& text, bool checked, bool enabled = true) noexcept;
    void collapse(const std::string& text, const std::string& subtext, bool checked, bool enabled = true) noexcept;
    void label(const std::string& text) noexcept;
    void value(const std::string& text) noexcept;
    void slider(const std::string& text, float val, float vmin, float vmax, float vinc, bool enabled = true) noexcept;
    void indent() noexcept;
    void unindent() noexcept;
    void separator() noexcept;
    void separatorLine() noexcept;

    void drawText(int x, int y, int align, const std::string& text, const float4& color) noexcept;
    void drawLine(float x0, float y0, float x1, float y1, float r, const float4& color) noexcept;
    void drawRoundedRect(float x, float y, float w, float h, float r, const float4& color) noexcept;
    void drawRect(float x, float y, float w, float h, const float4& color) noexcept;

private:

    GuiSystemPtr _gui;
};

_NAME_END

#endif