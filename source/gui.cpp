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
#include <ray/gui.h>

_NAME_BEGIN

GuiState::GuiState() noexcept
{
    left = false;
    leftPressed = false;
    leftReleased = false;
    mx = -1;
    my = -1;
    scroll = 0;
    active = 0;
    hot = 0;
    hotToBe = 0;
    isHot = false;
    isActive = false;
    wentActive = false;
    dragX = 0;
    dragY = 0;
    dragOrig = 0;
    widgetX = 0;
    widgetY = 0;
    widgetW = 1;
    insideCurrentScroll = false;
    areaId = 0;
    widgetId = 0;

    scrollTop = 0;
    scrollBottom = 0;
    scrollRight = 0;
    scrollAreaTop = 0;
    scrollVal = 0;
    scrollId = 0;
    scrollInsideArea = false;

    focusTop = 0;
    focusBottom = 0;
}

GuiTheme::GuiTheme() noexcept
{
    buttonHeight = 20;
    sliderHeight = 20;
    sliderMarkerWidth = 10;
    checkSize = 8;
    defaultSpacing = 0.1;
    textHeight = 8;

    indentSize = 0.1;
    areaHeader = 0.1;

    scrollBG = float4(0.0f, 0.0f, 0.0f, 0.9);
    scrollBarActive = float4(1.0f, 0.9f, 0.0f, 0.9f);
    scrollAreaPadding = 6;

    labelBG = float4(1.0f, 1.0f, 1.0f, 1.0f);

    textDefaultBG = float4(0.5f, 0.5f, 0.5f, 0.8);
}

GuiSystem::GuiSystem() noexcept
{
}

GuiSystem::~GuiSystem() noexcept
{
}

void
GuiSystem::open() noexcept
{
}

void
GuiSystem::close() noexcept
{
}

void
GuiSystem::beginFrame(int x, int y, int but, int scroll) noexcept
{
    _state.hot = _state.hotToBe;
    _state.hotToBe = 0;

    _state.wentActive = false;
    _state.isActive = false;
    _state.isHot = false;

    _state.widgetX = 0;
    _state.widgetY = 0;
    _state.widgetW = 0;

    _state.areaId = 1;
    _state.widgetId = 1;

    this->updateInput(x, y, but, scroll);
    this->resetCommand();
}

void
GuiSystem::endFrame() noexcept
{
    this->clearInput();
}

void
GuiSystem::beginScrollArea(const std::string& name, int x, int y, int w, int h, int scroll) noexcept
{
    _state.areaId++;
    _state.widgetId = 0;
    _state.widgetX = x + _theme.scrollAreaPadding;
    _state.widgetY = y + h - _theme.areaHeader + scroll;
    _state.widgetW = w - _theme.scrollAreaPadding * 4;

    _state.scrollTop = y - _theme.areaHeader + h;
    _state.scrollBottom = y + _theme.scrollAreaPadding;
    _state.scrollRight = x + w - _theme.scrollAreaPadding * 3;
    _state.scrollVal = scroll;
    _state.scrollId = (_state.areaId << 16) | _state.widgetId;
    _state.scrollAreaTop = _state.widgetY;
    _state.scrollInsideArea = inRect(x, y, w, h, false);

    _state.focusTop = y - _theme.areaHeader;
    _state.focusBottom = y - _theme.areaHeader + h;

    _state.insideCurrentScroll = _state.scrollInsideArea;
}

void
GuiSystem::endScrollArea() noexcept
{
    scissorCommand(-1, -1, -1, -1);

    // Draw scroll bar
    int x = _state.scrollRight + _theme.scrollAreaPadding * 0.5;
    int y = _state.scrollBottom;
    int w = _theme.scrollAreaPadding * 2;
    int h = _state.scrollTop - _state.scrollBottom;

    int stop = _state.scrollAreaTop;
    int sbot = _state.widgetY;
    int sh = stop - sbot; // The scrollable area height.

    float barHeight = (float)h / (float)sh;

    if (barHeight < 1)
    {
        float barY = (float)(y - sbot) / (float)sh;
        if (barY < 0) barY = 0;
        if (barY > 1) barY = 1;

        // Handle scroll bar logic.
        int hid = _state.scrollId;
        int hx = x;
        int hy = y + (int)(barY*h);
        int hw = w;
        int hh = (int)(barHeight*h);

        const int range = h - (hh - 1);
        bool over = inRect(hx, hy, hw, hh);
        buttonLogic(hid, over);
        if (isActive(hid))
        {
            float u = (float)(hy - y) / (float)range;
            if (_state.wentActive)
            {
                _state.dragY = _state.my;
                _state.dragOrig = u;
            }
            if (_state.dragY != _state.my)
            {
                u = _state.dragOrig + (_state.my - _state.dragY) / (float)range;
                if (u < 0) u = 0;
                if (u > 1) u = 1;
                _state.scrollVal = (int)((1 - u) * (sh - h));
            }
        }

        // BG
        drawRoundedRectCommand((float)x, (float)y, (float)w, (float)h, (float)w * 0.5 - 1, _theme.scrollBG);
        // Bar
        if (isActive(hid))
            drawRoundedRectCommand((float)hx, (float)hy, (float)hw, (float)hh, (float)w * 0.5 - 1, _theme.scrollBarActive);
        else
            drawRoundedRectCommand((float)hx, (float)hy, (float)hw, (float)hh, (float)w * 0.5 - 1, isHot(hid) ? float4(1.0f, 0.9f, 0.0f, 0.375) : float4(1.0f, 1.0f, 1.0f, 0.25f));

        // Handle mouse scrolling.
        if (_state.scrollInsideArea) // && !anyActive())
        {
            if (_state.scroll)
            {
                _state.scrollVal += 20 * _state.scroll;
                if (_state.scrollVal < 0) _state.scrollVal = 0;
                if (_state.scrollVal > (sh - h)) _state.scrollVal = (sh - h);
            }
        }
    }

    _state.insideCurrentScroll = false;
}

void
GuiSystem::button(const std::string& text, bool enabled) noexcept
{
    _state.widgetId++;
    int id = (_state.areaId << 16) | _state.widgetId;

    float x = _state.widgetX;
    float y = _state.widgetY - _theme.buttonHeight;
    float w = _state.widgetW;
    float h = _theme.buttonHeight;
    _state.widgetY -= _theme.buttonHeight + _theme.defaultSpacing;

    bool over = enabled && inRect(x, y, w, h);
    bool res = buttonLogic(id, over);

    drawRoundedRectCommand(x, y, w, h, _theme.buttonHeight * 0.5 - 1, float4(0.5f, 0.5f, 0.5f, isActive(id) ? 0.9 : 0.375));
    if (enabled)
        drawTextCommand(x + _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, isHot(id) ? float4(1.0f, 0.9, 0, 255) : float4(1.0f, 1.0f, 1.0f, 200));
    else
        drawTextCommand(x + _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.textDefaultBG);
}

void
GuiSystem::item(const std::string& text, bool enabled) noexcept
{
    _state.widgetId++;
    int id = (_state.areaId << 16) | _state.widgetId;

    int x = _state.widgetX;
    int y = _state.widgetY - _theme.buttonHeight;
    int w = _state.widgetW;
    int h = _theme.buttonHeight;
    _state.widgetY -= _theme.buttonHeight + _theme.defaultSpacing;

    bool over = enabled && inRect(x, y, w, h);
    bool res = buttonLogic(id, over);

    if (isHot(id))
        drawRoundedRectCommand((float)x, (float)y, (float)w, (float)h, 2.0f, float4(1.0f, 0.9, 0, isActive(id) ? 0.9 : 0.375));

    if (enabled)
        drawTextCommand(x + _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, float4(1.0f, 1.0f, 1.0f, 200));
    else
        drawTextCommand(x + _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.textDefaultBG);
}

void
GuiSystem::check(const std::string& text, bool checked, bool enabled) noexcept
{
    _state.widgetId++;

    int id = (_state.areaId << 16) | _state.widgetId;

    int x = _state.widgetX;
    int y = _state.widgetY - _theme.buttonHeight;
    int w = _state.widgetW;
    int h = _theme.buttonHeight;
    _state.widgetY -= _theme.buttonHeight + _theme.defaultSpacing;

    bool over = enabled && inRect(x, y, w, h);
    bool res = buttonLogic(id, over);

    const int cx = x + _theme.buttonHeight * 0.5 - _theme.checkSize * 0.5;
    const int cy = y + _theme.buttonHeight * 0.5 - _theme.checkSize * 0.5;

    drawRoundedRectCommand((float)cx - 3, (float)cy - 3, (float)_theme.checkSize + 6, (float)_theme.checkSize + 6, 4, float4(0.5f, 0.5f, 0.5f, isActive(id) ? 0.9 : 0.375));

    if (checked)
    {
        if (enabled)
            drawRoundedRectCommand((float)cx, (float)cy, (float)_theme.checkSize, (float)_theme.checkSize, (float)_theme.checkSize * 0.5 - 1, float4(1.0f, 1.0f, 1.0f, isActive(id) ? 1.0 : 0.9f));
        else
            drawRoundedRectCommand((float)cx, (float)cy, (float)_theme.checkSize, (float)_theme.checkSize, (float)_theme.checkSize * 0.5 - 1, _theme.textDefaultBG);
    }

    if (enabled)
        drawTextCommand(x + _theme.buttonHeight, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, isHot(id) ? float4(1.0f, 0.9f, 0.0f, 1.0f) : float4(1.0f, 1.0f, 1.0f, 0.9f));
    else
        drawTextCommand(x + _theme.buttonHeight, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.textDefaultBG);
}

void
GuiSystem::collapse(const std::string& text, const std::string& subtext, bool checked, bool enabled) noexcept
{
    _state.widgetId++;
    int id = (_state.areaId << 16) | _state.widgetId;

    int x = _state.widgetX;
    int y = _state.widgetY - _theme.buttonHeight;
    int w = _state.widgetW;
    int h = _theme.buttonHeight;
    _state.widgetY -= _theme.buttonHeight;

    const int cx = x + _theme.buttonHeight * 0.5 - _theme.checkSize * 0.5;
    const int cy = y + _theme.buttonHeight * 0.5 - _theme.checkSize * 0.5;

    bool over = enabled && inRect(x, y, w, h);
    bool res = buttonLogic(id, over);

    if (checked)
        drawTriangleCommand(cx, cy, _theme.checkSize, _theme.checkSize, 2, float4(1.0f, 1.0f, 1.0f, isActive(id) ? 1.0f : 0.8f));
    else
        drawTriangleCommand(cx, cy, _theme.checkSize, _theme.checkSize, 1, float4(1.0f, 1.0f, 1.0f, isActive(id) ? 1.0f : 0.8f));

    if (enabled)
        drawTextCommand(x + _theme.buttonHeight, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, isHot(id) ? float4(1.0f, 0.9f, 0.0f, 1.0f) : float4(1.0f, 1.0f, 1.0f, 0.8f));
    else
        drawTextCommand(x + _theme.buttonHeight, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.textDefaultBG);

    if (!subtext.empty())
        drawTextCommand(x + w - _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_RIGHT, subtext, float4(1.0f, 1.0f, 1.0f, 0.5f));
}

void
GuiSystem::label(const std::string& text) noexcept
{
    int x = _state.widgetX;
    int y = _state.widgetY - _theme.buttonHeight;

    _state.widgetY -= _theme.buttonHeight;

    drawTextCommand(x, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.labelBG);
}

void
GuiSystem::value(const std::string& text) noexcept
{
    int x = _state.widgetX;
    int y = _state.widgetY;
    int w = _state.widgetW;

    _state.widgetY -= _theme.buttonHeight;

    this->drawTextCommand(x + w - _theme.buttonHeight * 0.5, y + _theme.buttonHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_RIGHT, text, float4(1.0f, 1.0f, 1.0f, 0.8));
}

void
GuiSystem::slider(const std::string& text, float val, float vmin, float vmax, float vinc, bool enabled) noexcept
{
    _state.widgetId++;
    int id = (_state.areaId << 16) | _state.widgetId;

    int x = _state.widgetX;
    int y = _state.widgetY - _theme.buttonHeight;
    int w = _state.widgetW;
    int h = _theme.sliderHeight;
    _state.widgetY -= _theme.sliderHeight + _theme.defaultSpacing;

    drawRoundedRectCommand((float)x, (float)y, (float)w, (float)h, 4.0f, float4(0, 0, 0, 0.5f));

    const int range = w - _theme.sliderMarkerWidth;

    float u = (val - vmin) / (vmax - vmin);
    if (u < 0) u = 0;
    if (u > 1) u = 1;
    int m = (int)(u * range);

    bool over = enabled && inRect(x + m, y, _theme.sliderMarkerWidth, _theme.sliderHeight);
    bool res = buttonLogic(id, over);
    bool valChanged = false;

    if (isActive(id))
    {
        if (_state.wentActive)
        {
            _state.dragX = _state.mx;
            _state.dragOrig = u;
        }
        if (_state.dragX != _state.mx)
        {
            u = _state.dragOrig + (float)(_state.mx - _state.dragX) / (float)range;
            if (u < 0) u = 0;
            if (u > 1) u = 1;
            val = vmin + u * (vmax - vmin);
            val = floorf(val / vinc + 0.5f)*vinc; // Snap to vinc
            m = (int)(u * range);
            valChanged = true;
        }
    }

    if (isActive(id))
        drawRoundedRectCommand((float)(x + m), (float)y, (float)_theme.sliderMarkerWidth, (float)_theme.sliderHeight, 4.0f, float4(1.0f, 1.0f, 1.0f, 1.0));
    else
        drawRoundedRectCommand((float)(x + m), (float)y, (float)_theme.sliderMarkerWidth, (float)_theme.sliderHeight, 4.0f, isHot(id) ? float4(1.0f, 0.9, 0, 0.5f) : float4(1.0f, 1.0f, 1.0f, 64));

    // TODO: fix this, take a look at 'nicenum'.
    int digits = (int)(ceilf(log10f(vinc)));
    char fmt[16];
    _snprintf(fmt, 16, "%%.%df", digits >= 0 ? 0 : -digits);
    char msg[128];
    _snprintf(msg, 128, fmt, val);

    if (enabled)
    {
        drawTextCommand(x + _theme.sliderHeight * 0.5, y + _theme.sliderHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, isHot(id) ? float4(1.0f, 0.9, 0, 1.0) : float4(1.0f, 1.0f, 1.0f, 0.8));
        drawTextCommand(x + w - _theme.sliderHeight * 0.5, y + _theme.sliderHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_RIGHT, msg, isHot(id) ? float4(1.0f, 0.9, 0, 1.0) : float4(1.0f, 1.0f, 1.0f, 0.8));
    }
    else
    {
        drawTextCommand(x + _theme.sliderHeight * 0.5, y + _theme.sliderHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_LEFT, text, _theme.textDefaultBG);
        drawTextCommand(x + w - _theme.sliderHeight * 0.5, y + _theme.sliderHeight * 0.5 - _theme.textHeight * 0.5, ALIGN_RIGHT, msg, _theme.textDefaultBG);
    }
}

void
GuiSystem::indent() noexcept
{
    _state.widgetX += _theme.indentSize;
    _state.widgetW += _theme.indentSize;
}

void
GuiSystem::unindent() noexcept
{
    _state.widgetX -= _theme.indentSize;
    _state.widgetY -= _theme.indentSize;
}

void
GuiSystem::separator() noexcept
{
    _state.widgetX -= _theme.defaultSpacing * 3;
}

void
GuiSystem::separatorLine() noexcept
{
    int x = _state.widgetX;
    int y = _state.widgetY;
    int w = _state.widgetW;
    int h = 1;
    _state.widgetY -= _theme.defaultSpacing * 4;

    this->drawRectCommand(x, y, w, h, float4(1.0f, 1.0f, 1.0f, 32));
}

void
GuiSystem::drawText(int x, int y, int align, const std::string& text, const float4& color) noexcept
{
    this->drawTextCommand(x, y, align, text, color);
}

void
GuiSystem::drawLine(float x0, float y0, float x1, float y1, float r, const float4& color) noexcept
{
    this->drawLineCommand(x0, y0, x1, y1, r, color);
}

void
GuiSystem::drawRoundedRect(float x, float y, float w, float h, float r, const float4& color) noexcept
{
    this->drawRoundedRectCommand(x, y, w, h, r, color);
}

void
GuiSystem::drawRect(float x, float y, float w, float h, const float4& color) noexcept
{
    this->drawRectCommand(x, y, w, h, color);
}

const GuiCommands&
GuiSystem::getGuiCommands() const noexcept
{
    return _commands;
}

void
GuiSystem::resetCommand() noexcept
{
    _commands.clear();
}

void
GuiSystem::scissorCommand(int x, int y, int w, int h) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_SCISSOR;
    cmd.flags = x < 0 ? 0 : 1;
    cmd.col = float4(0, 0, 0, 0);
    cmd.rect.x = x;
    cmd.rect.y = y;
    cmd.rect.w = w;
    cmd.rect.h = h;

    _commands.push_back(cmd);
}

void
GuiSystem::drawRectCommand(float x, float y, float w, float h, const float4& color) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_RECT;
    cmd.flags = 0;
    cmd.col = color;
    cmd.rect.x = x;
    cmd.rect.y = y;
    cmd.rect.w = w;
    cmd.rect.h = h;
    cmd.rect.r = 0;

    _commands.push_back(cmd);
}

void
GuiSystem::drawLineCommand(float x0, float y0, float x1, float y1, float r, const float4& color) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_LINE;
    cmd.flags = 0;
    cmd.col = color;
    cmd.line.x0 = x0;
    cmd.line.y0 = y0;
    cmd.line.x1 = x1;
    cmd.line.y1 = y1;
    cmd.line.r = r;

    _commands.push_back(cmd);
}

void
GuiSystem::drawRoundedRectCommand(float x, float y, float w, float h, float r, const float4& color) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_RECT;
    cmd.flags = 0;
    cmd.col = color;
    cmd.rect.x = x;
    cmd.rect.y = y;
    cmd.rect.w = w;
    cmd.rect.h = h;
    cmd.rect.r = r;

    _commands.push_back(cmd);
}

void
GuiSystem::drawTriangleCommand(int x, int y, int w, int h, int flags, const float4& color) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_TRIANGLE;
    cmd.flags = 0;
    cmd.col = color;
    cmd.rect.x = x;
    cmd.rect.y = y;
    cmd.rect.w = w;
    cmd.rect.h = h;

    _commands.push_back(cmd);
}

void
GuiSystem::drawTextCommand(int x, int y, int align, const std::string& text, const float4& color) noexcept
{
    GuiCommand cmd;
    cmd.type = GuiCommand::CMD_TRIANGLE;
    cmd.flags = 0;
    cmd.col = color;
    cmd.text.x = x;
    cmd.text.y = y;
    cmd.text.align = (short)align;
    cmd.str = text;

    _commands.push_back(cmd);
}

bool
GuiSystem::anyActive() noexcept
{
    return _state.active != 0;
}

bool
GuiSystem::isActive(unsigned int id) noexcept
{
    return _state.active == id;
}

bool
GuiSystem::isHot(unsigned int id) noexcept
{
    return _state.hot == id;
}

void
GuiSystem::clearActive() noexcept
{
    _state.active = 0;
    // mark all UI for this frame as processed
    clearInput();
}

void
GuiSystem::setActive(unsigned int id) noexcept
{
    _state.active = id;
    _state.wentActive = true;
}

void
GuiSystem::setHot(unsigned int id) noexcept
{
    _state.hotToBe = id;
}

bool
GuiSystem::inRect(int x, int y, int w, int h, bool checkScroll) noexcept
{
    return (!checkScroll || _state.insideCurrentScroll) && _state.mx >= x && _state.mx <= x + w && _state.my >= y && _state.my <= y + h;
}

bool
GuiSystem::buttonLogic(unsigned int id, bool over) noexcept
{
    bool res = false;
    // process down
    if (!anyActive())
    {
        if (over)
            setHot(id);
        if (isHot(id) && _state.leftPressed)
            setActive(id);
    }

    // if button is active, then react on left up
    if (isActive(id))
    {
        _state.isActive = true;
        if (over)
            setHot(id);
        if (_state.leftReleased)
        {
            if (isHot(id))
                res = true;
            clearActive();
        }
    }

    if (isHot(id))
        _state.isHot = true;

    return res;
}

void
GuiSystem::clearInput() noexcept
{
    _state.leftPressed = false;
    _state.leftReleased = false;
    _state.scroll = 0;
}

void
GuiSystem::updateInput(int mx, int my, unsigned char mbut, int scroll) noexcept
{
    bool left = (mbut & MBUT_LEFT) != 0;

    _state.mx = mx;
    _state.my = my;

    _state.leftPressed = !_state.left && left;
    _state.leftReleased = _state.left && !left;
    _state.left = left;

    _state.scroll = scroll;
}

_NAME_END