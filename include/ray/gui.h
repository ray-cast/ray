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
#ifndef _H_GUI_H_
#define _H_GUI_H_

#include <ray/math.h>

_NAME_BEGIN

typedef std::shared_ptr<class GuiSystem> GuiSystemPtr;

enum TextAlign
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
};

enum MouseButton
{
    MBUT_LEFT = 0x01,
    MBUT_RIGHT = 0x02,
};

struct GuiCommand
{
    struct LineCommand
    {
        float x0, y0, x1, y1, r;
    };

    struct RectCommand
    {
        float x, y, w, h, r;
    };

    struct TextCommand
    {
        float x, y;
        short align;
    };

    enum Type
    {
        CMD_RECT,
        CMD_TRIANGLE,
        CMD_LINE,
        CMD_TEXT,
        CMD_SCISSOR,
    };

    Type type;
    char flags;
    char pad[2];
    float4 col;
    std::string str;
    union
    {
        LineCommand line;
        RectCommand rect;
        TextCommand text;
    };
};

struct GuiTheme
{
    int buttonHeight;
    int checkSize;
    int defaultSpacing;
    int textHeight;
    int indentSize;
    int areaHeader;

    float4 textDefaultBG;

    float4 scrollBG;
    float4 scrollBar;
    float4 scrollBarActive;
    int scrollAreaPadding;

    float4 labelBG;

    int sliderHeight;
    int sliderMarkerWidth;

    GuiTheme() noexcept;
};

struct GuiState
{
    bool left;
    bool leftPressed, leftReleased;
    int mx, my;
    int scroll;
    unsigned int active;
    unsigned int hot;
    unsigned int hotToBe;
    bool isHot;
    bool isActive;
    bool wentActive;
    int dragX, dragY;
    float dragOrig;
    float widgetX;
    float widgetY;
    float widgetW;
    bool insideCurrentScroll;

    int scrollTop;
    int scrollBottom;
    int scrollRight;
    int scrollAreaTop;
    int scrollVal;
    int scrollId;
    bool scrollInsideArea;

    unsigned int areaId;
    unsigned int widgetId;

    int  focusTop;
    int  focusBottom;

    GuiState() noexcept;
};

typedef std::vector<GuiCommand> GuiCommands;

class EXPORT GuiSystem final
{
public:
    GuiSystem() noexcept;
    ~GuiSystem() noexcept;

    void open() noexcept;
    void close() noexcept;

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

    const GuiCommands& getGuiCommands() const noexcept;
    void resetCommand() noexcept;

private:

    bool anyActive() noexcept;
    bool isActive(unsigned int id) noexcept;
    void clearActive() noexcept;
    void setActive(unsigned int id) noexcept;

    bool isHot(unsigned int id) noexcept;
    void setHot(unsigned int id) noexcept;

    bool buttonLogic(unsigned int id, bool over) noexcept;

    void clearInput() noexcept;
    void updateInput(int mx, int my, unsigned char mbut, int scroll) noexcept;

    bool inRect(int x, int y, int w, int h, bool checkScroll = true) noexcept;

    void scissorCommand(int x, int y, int w, int h) noexcept;

    void drawRectCommand(float x, float y, float w, float h, const float4& color) noexcept;
    void drawLineCommand(float x0, float y0, float x1, float y1, float r, const float4& color) noexcept;
    void drawRoundedRectCommand(float x, float y, float w, float h, float r, const float4& color) noexcept;
    void drawTriangleCommand(int x, int y, int w, int h, int flags, const float4& color) noexcept;
    void drawTextCommand(int x, int y, int align, const std::string& text, const float4& color) noexcept;

private:

    GuiState _state;
    GuiTheme _theme;

    GuiCommands _commands;
};

_NAME_END

#endif