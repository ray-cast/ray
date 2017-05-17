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
#include <ray/imgui.h>
#include <imgui.h>

_NAME_BEGIN

float2 ImVec2ToFloat2(ImVec2 vec2)
{
	return float2(vec2.x, vec2.y);
}

float4 ImVec4ToFloat4(ImVec4 vec4)
{
	return float4(vec4.x, vec4.y, vec4.z, vec4.w);
}

GuiStyle::GuiStyle() noexcept
{
}

Gui::Gui() noexcept
{
}

Gui::~Gui() noexcept
{
}

void
Gui::newFrame() noexcept
{
	ImGui::NewFrame();
}

void
Gui::render() noexcept
{
	ImGui::Render();
}

void
Gui::shutdown() noexcept
{
	ImGui::Shutdown();
}

void
Gui::showUserGuide() noexcept
{
	ImGui::ShowUserGuide();
}

void
Gui::showStyleEditor(GuiStyle* ref) noexcept
{
	ImGui::ShowStyleEditor();
}

void
Gui::showTestWindow(bool* isOpened) noexcept
{
	if (!isOpened)
		ImGui::ShowTestWindow();
	else
		ImGui::ShowTestWindow(isOpened);
}

void
Gui::showMetricsWindow(bool* isOpened) noexcept
{
	if (!isOpened)
		ImGui::ShowMetricsWindow();
	else
		ImGui::ShowMetricsWindow(isOpened);
}

bool
Gui::begin(const char* name, bool* isOpened, GuiWindowFlags flags) noexcept
{
	return ImGui::Begin(name, isOpened, flags);
}

bool
Gui::begin(const char* name, bool* isOpened, const float2& size_on_first_use, float alpha, GuiWindowFlags flags) noexcept
{
	return ImGui::Begin(name, isOpened, (const ImVec2&)size_on_first_use, alpha, flags);
}

void
Gui::end() noexcept
{
	ImGui::End();
}

bool
Gui::beginChild(const char* str_id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
{
	return ImGui::BeginChild(str_id, (ImVec2&)size, border, extraFlags);
}

bool
Gui::beginChild(GuiID id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
{
	return ImGui::BeginChild((ImGuiID)id, (ImVec2&)size, border, extraFlags);
}

void
Gui::endChild() noexcept
{
	return ImGui::EndChild();
}

float2
Gui::getContentRegionMax() noexcept
{
	return ImVec2ToFloat2(ImGui::GetContentRegionMax());
}

float2
Gui::getContentRegionAvail() noexcept
{
	return ImVec2ToFloat2(ImGui::GetContentRegionAvail());
}

float
Gui::getContentRegionAvailWidth() noexcept
{
	return ImGui::GetContentRegionAvailWidth();
}

float2
Gui::getWindowContentRegionMin() noexcept
{
	return ImVec2ToFloat2(ImGui::GetWindowContentRegionMin());
}

float2
Gui::getWindowContentRegionMax() noexcept
{
	return ImVec2ToFloat2(ImGui::GetWindowContentRegionMax());
}

float
Gui::getWindowContentRegionWidth() noexcept
{
	return ImGui::GetWindowContentRegionWidth();
}

float2
Gui::getWindowPos() noexcept
{
	return ImVec2ToFloat2(ImGui::GetWindowPos());
}

float2
Gui::getWindowSize() noexcept
{
	return ImVec2ToFloat2(ImGui::GetWindowSize());
}

float
Gui::getWindowWidth() noexcept
{
	return ImGui::GetWindowWidth();
}

float
Gui::getWindowHeight() noexcept
{
	return ImGui::GetWindowHeight();
}

bool
Gui::isWindowCollapsed() noexcept
{
	return ImGui::IsWindowCollapsed();
}

void
Gui::setWindowFontScale(float scale) noexcept
{
	ImGui::SetWindowFontScale(scale);
}

void
Gui::setNextWindowPos(const float2& pos, GuiSetCondFlags cond) noexcept
{
	ImGui::SetNextWindowPos((const ImVec2&)pos, cond);
}

void
Gui::setNextWindowPosCenter(GuiSetCondFlags cond) noexcept
{
	ImGui::SetNextWindowPosCenter(cond);
}

void
Gui::setNextWindowSize(const float2& size, GuiSetCondFlags cond) noexcept
{
	ImGui::SetNextWindowSize((const ImVec2&)size);
}

void
Gui::setNextWindowSizeConstraints(const float2& min, const float2& max) noexcept
{
	ImGui::SetNextWindowSizeConstraints((const ImVec2&)min, (const ImVec2&)max);
}

void
Gui::setNextWindowContentSize(const float2& size) noexcept
{
	ImGui::SetNextWindowContentSize((const ImVec2&)size);
}

void
Gui::setNextWindowContentWidth(float width) noexcept
{
	ImGui::SetNextWindowContentWidth(width);
}

void
Gui::setNextWindowCollapsed(bool collapsed, GuiSetCondFlags cond) noexcept
{
	ImGui::SetNextWindowCollapsed(collapsed, cond);
}

void
Gui::setNextWindowFocus() noexcept
{
	ImGui::SetNextWindowFocus();
}

void
Gui::setWindowPos(const float2& pos, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowPos((const ImVec2&)pos, cond);
}

void
Gui::setWindowSize(const float2& size, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowSize((const ImVec2&)size, cond);
}

void
Gui::setWindowCollapsed(bool collapsed, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowCollapsed(collapsed, cond);
}

void
Gui::setWindowFocus() noexcept
{
	ImGui::SetWindowFocus();
}

void
Gui::setWindowPos(const char* name, const float2& pos, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowPos(name, (const ImVec2&)pos, cond);
}

void
Gui::setWindowSize(const char* name, const float2& size, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowSize(name, (const ImVec2&)size, cond);
}

void
Gui::setWindowCollapsed(const char* name, bool collapsed, GuiSetCondFlags cond) noexcept
{
	ImGui::SetWindowCollapsed(name, collapsed, cond);
}

void
Gui::setWindowFocus(const char* name) noexcept
{
	ImGui::SetWindowFocus();
}

float
Gui::getScrollX() noexcept
{
	return ImGui::GetScrollX();
}

float
Gui::getScrollY() noexcept
{
	return ImGui::GetScrollY();
}

float
Gui::getScrollMaxX() noexcept
{
	return ImGui::GetScrollMaxX();
}

float
Gui::getScrollMaxY() noexcept
{
	return ImGui::GetScrollMaxY();
}

void
Gui::setScrollX(float scroll_x) noexcept
{
	ImGui::SetScrollX(scroll_x);
}

void
Gui::setScrollY(float scroll_y) noexcept
{
	ImGui::SetScrollY(scroll_y);
}

void
Gui::setScrollHere(float center_y_ratio) noexcept
{
	ImGui::SetScrollHere(center_y_ratio);
}

void
Gui::setScrollFromPosY(float pos_y, float center_y_ratio) noexcept
{
	ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
}

void
Gui::setKeyboardFocusHere(int offset) noexcept
{
	ImGui::SetKeyboardFocusHere(offset);
}

void
Gui::pushStyleColor(GuiCol idx, const float4& col) noexcept
{
	ImGui::PushStyleColor(idx, (ImVec4&)col);
}

void
Gui::popStyleColor(int count) noexcept
{
	ImGui::PopStyleColor(count);
}

void
Gui::pushStyleVar(GuiStyleVar idx, float val) noexcept
{
	ImGui::PushStyleVar(idx, val);
}

void
Gui::pushStyleVar(GuiStyleVar idx, const float2& val) noexcept
{
	ImGui::PushStyleVar(idx, (ImVec2&)val);
}

void
Gui::popStyleVar(int count) noexcept
{
	ImGui::PopStyleVar(count);
}

float
Gui::getFontSize() noexcept
{
	return ImGui::GetFontSize();
}

float2
Gui::getFontTexUvWhitePixel() noexcept
{
	return ImVec2ToFloat2(ImGui::GetFontTexUvWhitePixel());
}

std::uint32_t
Gui::getColorU32(GuiCol idx, float alpha_mul) noexcept
{
	return ImGui::GetColorU32(idx, alpha_mul);
}

std::uint32_t
Gui::getColorU32(const float4& col) noexcept
{
	return ImGui::GetColorU32((ImVec4&)col);
}

void
Gui::pushItemWidth(float item_width) noexcept
{
	ImGui::PushItemWidth(item_width);
}

void
Gui::popItemWidth() noexcept
{
	ImGui::PopItemWidth();
}

float
Gui::calcItemWidth() noexcept
{
	return ImGui::CalcItemWidth();
}

void
Gui::pushTextWrapPos(float wrap_pos_x) noexcept
{
	ImGui::PushTextWrapPos();
}

void
Gui::popTextWrapPos() noexcept
{
	ImGui::PopTextWrapPos();
}

void
Gui::pushAllowKeyboardFocus(bool v) noexcept
{
	ImGui::PushAllowKeyboardFocus(v);
}

void
Gui::popAllowKeyboardFocus() noexcept
{
	ImGui::PopAllowKeyboardFocus();
}

void
Gui::pushButtonRepeat(bool repeat) noexcept
{
	ImGui::PushButtonRepeat(repeat);
}

void
Gui::popButtonRepeat() noexcept
{
	ImGui::PopButtonRepeat();
}

void
Gui::separator() noexcept
{
	ImGui::Separator();
}

void
Gui::sameLine(float pos_x, float spacing_w) noexcept
{
	ImGui::SameLine(pos_x, spacing_w);
}

void
Gui::newLine() noexcept
{
	ImGui::NewLine();
}

void
Gui::spacing() noexcept
{
	ImGui::Spacing();
}

void
Gui::dummy(const float2& size) noexcept
{
	ImGui::Dummy((ImVec2&)size);
}

void
Gui::indent(float indent_w) noexcept
{
	ImGui::Indent(indent_w);
}

void
Gui::unindent(float indent_w) noexcept
{
	ImGui::Unindent(indent_w);
}

void
Gui::beginGroup() noexcept
{
	ImGui::BeginGroup();
}

void
Gui::endGroup() noexcept
{
	ImGui::EndGroup();
}

float2
Gui::getCursorPos() noexcept
{
	return ImVec2ToFloat2(ImGui::GetCursorPos());
}

float
Gui::getCursorPosX() noexcept
{
	return ImGui::GetCursorPosX();
}

float
Gui::getCursorPosY() noexcept
{
	return ImGui::GetCursorPosY();
}

void
Gui::setCursorPos(const float2& local_pos) noexcept
{
	ImGui::SetCursorPos((ImVec2&)local_pos);
}

void
Gui::setCursorPosX(float x) noexcept
{
	ImGui::SetCursorPosX(x);
}

void
Gui::setCursorPosY(float y) noexcept
{
	ImGui::SetCursorPosY(y);
}

float2
Gui::getCursorStartPos() noexcept
{
	return ImVec2ToFloat2(ImGui::GetCursorStartPos());
}

float2
Gui::getCursorScreenPos() noexcept
{
	return ImVec2ToFloat2(ImGui::GetCursorScreenPos());
}

void
Gui::setCursorScreenPos(const float2& pos) noexcept
{
	return ImGui::SetCursorScreenPos((ImVec2&)pos);
}

void
Gui::alignFirstTextHeightToWidgets() noexcept
{
	ImGui::AlignFirstTextHeightToWidgets();
}

float
Gui::getTextLineHeight() noexcept
{
	return ImGui::GetTextLineHeight();
}

float
Gui::getTextLineHeightWithSpacing() noexcept
{
	return ImGui::GetTextLineHeightWithSpacing();
}

float
Gui::getItemsLineHeightWithSpacing() noexcept
{
	return ImGui::GetItemsLineHeightWithSpacing();
}

void
Gui::columns(int count, const char* id, bool border) noexcept
{
	ImGui::Columns(count, id, border);
}

void
Gui::pushID(const char* str_id_begin, const char* str_id_end) noexcept
{
	ImGui::PushID(str_id_begin, str_id_end);
}

void
Gui::pushID(const void* ptr_id) noexcept
{
	ImGui::PushID(ptr_id);
}

void
Gui::pushID(int int_id) noexcept
{
	ImGui::PushID(int_id);
}

void
Gui::popID() noexcept
{
	ImGui::PopID();
}

GuiID
Gui::getID(const char* str_id) noexcept
{
	return ImGui::GetID(str_id);
}

GuiID
Gui::getID(const char* str_id_begin, const char* str_id_end) noexcept
{
	return ImGui::GetID(str_id_begin, str_id_end);
}

GuiID
Gui::getID(const void* ptr_id) noexcept
{
	return ImGui::GetID(ptr_id);
}

void
Gui::text(const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextV(fmt, args);
	va_end(args);
}

void
Gui::textV(const char* fmt, va_list args) noexcept
{
	ImGui::TextV(fmt, args);
}

void
Gui::textColored(const float4& col, const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextColoredV((ImVec4&)col, fmt, args);
	va_end(args);
}

void
Gui::textColoredV(const float4& col, const char* fmt, va_list args) noexcept
{
	ImGui::TextColoredV((ImVec4&)col, fmt, args);
}

void
Gui::textDisabled(const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextDisabledV(fmt, args);
	va_end(args);
}

void
Gui::textDisabledV(const char* fmt, va_list args) noexcept
{
	ImGui::TextDisabledV(fmt, args);
}

void
Gui::textWrapped(const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextWrappedV(fmt, args);
	va_end(args);
}

void
Gui::textWrappedV(const char* fmt, va_list args) noexcept
{
	ImGui::TextWrappedV(fmt, args);
}

void
Gui::textUnformatted(const char* text, const char* text_end) noexcept
{
	ImGui::TextUnformatted(text, text_end);
}

void
Gui::labelText(const char* label, const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::LabelTextV(label, fmt, args);
	va_end(args);
}

void
Gui::labelTextV(const char* label, const char* fmt, va_list args) noexcept
{
	ImGui::LabelTextV(label, fmt, args);
}

void
Gui::bullet() noexcept
{
	ImGui::Bullet();
}

void
Gui::bulletText(const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::BulletTextV(fmt, args);
	va_end(args);
}

void
Gui::bulletTextV(const char* fmt, va_list args) noexcept
{
	ImGui::BulletTextV(fmt, args);
}

bool
Gui::button(const char* label, const float2& size) noexcept
{
	return ImGui::Button(label, (ImVec2&)size);
}

bool
Gui::smallButton(const char* label) noexcept
{
	return ImGui::SmallButton(label);
}

bool
Gui::invisibleButton(const char* str_id, const float2& size) noexcept
{
	return ImGui::InvisibleButton(str_id, (ImVec2&)size);
}

void
Gui::image(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, const float4& tint_col, const float4& border_col) noexcept
{
	ImGui::Image(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, (ImVec4&)tint_col, (ImVec4&)border_col);
}

bool
Gui::imageButton(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col) noexcept
{
	return ImGui::ImageButton(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, frame_padding, (ImVec4&)bg_col, (ImVec4&)tint_col);
}

bool
Gui::checkbox(const char* label, bool* v) noexcept
{
	return ImGui::Checkbox(label, v);
}

bool
Gui::checkboxFlags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept
{
	return ImGui::CheckboxFlags(label, flags, flags_value);
}

bool
Gui::radioButton(const char* label, bool active) noexcept
{
	return ImGui::RadioButton(label, active);
}

bool
Gui::radioButton(const char* label, int* v, int v_button) noexcept
{
	return ImGui::RadioButton(label, v, v_button);
}

bool
Gui::combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
{
	return ImGui::Combo(label, current_item, items, items_count, height_in_items);
}

bool
Gui::combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items) noexcept
{
	return ImGui::Combo(label, current_item, items_separated_by_zeros, height_in_items);
}

bool
Gui::combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items) noexcept
{
	return ImGui::Combo(label, current_item, items_getter, data, items_count, height_in_items);
}

bool
Gui::colorButton(const float4& col, bool small_height, bool outline_border) noexcept
{
	return ImGui::ColorButton((ImVec4&)col, small_height, outline_border);
}

bool
Gui::colorEdit3(const char* label, float col[3]) noexcept
{
	return ImGui::ColorEdit3(label, col);
}

bool
Gui::colorEdit4(const char* label, float col[4], bool show_alpha) noexcept
{
	return ImGui::ColorEdit4(label, col, show_alpha);
}

void
Gui::colorEditMode(GuiColorEditMode mode) noexcept
{
	ImGui::ColorEditMode(mode);
}

void
Gui::plotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
{
	ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
}

void
Gui::plotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
{
	ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size);
}

void
Gui::plotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
{
	ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
}

void
Gui::plotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
{
	ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max);
}

void
Gui::progressBar(float fraction, const float2& size_arg, const char* overlay) noexcept
{
	ImGui::ProgressBar(fraction, (ImVec2&)size_arg, overlay);
}

bool
Gui::dragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::DragFloat(label, v, v_speed, v_min, v_max, display_format, power);
}

bool
Gui::dragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, display_format, power);
}

bool
Gui::dragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, display_format, power);
}

bool
Gui::dragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, display_format, power);
}

bool
Gui::dragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power) noexcept
{
	return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
}

bool
Gui::dragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::DragInt(label, v, v_speed, v_min, v_max, display_format);
}

bool
Gui::dragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::DragInt2(label, v, v_speed, v_min, v_max, display_format);
}

bool
Gui::dragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::DragInt3(label, v, v_speed, v_min, v_max, display_format);
}

bool
Gui::dragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::DragInt4(label, v, v_speed, v_min, v_max, display_format);
}

bool
Gui::dragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max) noexcept
{
	return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format);
}

bool
Gui::inputText(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags) noexcept
{
	return ImGui::InputText(label, buf, buf_size, flags);
}

bool
Gui::inputTextMultiline(const char* label, char* buf, size_t buf_size, const float2& size, GuiInputTextFlags flags) noexcept
{
	return ImGui::InputTextMultiline(label, buf, buf_size, (const ImVec2&)size, flags);
}

bool
Gui::inputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extraFlags);
}

bool
Gui::inputFloat2(const char* label, float v[2], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputFloat2(label, v, decimal_precision, extraFlags);
}

bool
Gui::inputFloat3(const char* label, float v[3], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputFloat3(label, v, decimal_precision, extraFlags);
}

bool
Gui::inputFloat4(const char* label, float v[4], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputFloat4(label, v, decimal_precision, extraFlags);
}

bool
Gui::inputInt(const char* label, int* v, int step, int step_fast, GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputInt(label, v, step, step_fast, extraFlags);
}

bool
Gui::inputInt2(const char* label, int v[2], GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputInt2(label, v, extraFlags);
}

bool
Gui::inputInt3(const char* label, int v[3], GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputInt3(label, v, extraFlags);
}

bool
Gui::inputInt4(const char* label, int v[4], GuiInputTextFlags extraFlags) noexcept
{
	return ImGui::InputInt4(label, v, extraFlags);
}

bool
Gui::sliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::SliderFloat(label, v, v_min, v_max, display_format, power);
}

bool
Gui::sliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::SliderFloat2(label, v, v_min, v_max, display_format, power);
}

bool
Gui::sliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::SliderFloat3(label, v, v_min, v_max, display_format, power);
}

bool
Gui::sliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::SliderFloat4(label, v, v_min, v_max, display_format, power);
}

bool
Gui::sliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max) noexcept
{
	return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
}

bool
Gui::sliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::SliderInt(label, v, v_min, v_max, display_format);
}

bool
Gui::sliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::SliderInt2(label, v, v_min, v_max, display_format);
}

bool
Gui::sliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::SliderInt3(label, v, v_min, v_max, display_format);
}

bool
Gui::sliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::SliderInt4(label, v, v_min, v_max, display_format);
}

bool
Gui::vSliderFloat(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
{
	return ImGui::VSliderFloat(label, (ImVec2&)size, v, v_min, v_max, display_format, power);
}

bool
Gui::vSliderInt(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format) noexcept
{
	return ImGui::VSliderInt(label, (ImVec2&)size, v, v_min, v_max, display_format);
}

bool
Gui::treeNode(const char* label) noexcept
{
	return ImGui::TreeNode(label);
}

bool
Gui::treeNode(const char* str_id, const char* fmt, ...) noexcept
{
	bool result = false;
	va_list args;
	va_start(args, fmt);
	result = ImGui::TreeNodeExV(str_id, 0, fmt, args);
	va_end(args);
	return result;
}

bool
Gui::treeNode(const void* ptr_id, const char* fmt, ...) noexcept
{
	bool result = false;
	va_list args;
	va_start(args, fmt);
	result = ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
	va_end(args);
	return result;
}

bool
Gui::treeNodeV(const char* str_id, const char* fmt, va_list args) noexcept
{
	return ImGui::TreeNodeExV(str_id, 0, fmt, args);
}

bool
Gui::treeNodeV(const void* ptr_id, const char* fmt, va_list args) noexcept
{
	return ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
}

bool
Gui::treeNodeEx(const char* label, GuiTreeNodeFlags flags) noexcept
{
	return ImGui::TreeNodeEx(label, flags);
}

bool
Gui::treeNodeEx(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
{
	bool result = false;
	va_list args;
	va_start(args, fmt);
	result = ImGui::TreeNodeEx(str_id, flags, fmt, args);
	va_end(args);
	return result;
}

bool
Gui::treeNodeEx(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
{
	bool result = false;
	va_list args;
	va_start(args, fmt);
	result = ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
	va_end(args);
	return result;
}

bool
Gui::treeNodeExV(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
{
	return ImGui::TreeNodeEx(str_id, flags, fmt, args);
}

bool
Gui::treeNodeExV(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
{
	return ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
}

void
Gui::treePush(const char* str_id) noexcept
{
	return ImGui::TreePush(str_id);
}

void
Gui::treePush(const void* ptr_id) noexcept
{
	return ImGui::TreePush(ptr_id);
}

void
Gui::treePop() noexcept
{
	return ImGui::TreePop();
}

void
Gui::treeAdvanceToLabelPos() noexcept
{
	return ImGui::TreeAdvanceToLabelPos();
}

float
Gui::getTreeNodeToLabelSpacing() noexcept
{
	return ImGui::GetTreeNodeToLabelSpacing();
}

void
Gui::setNextTreeNodeOpen(bool is_open, GuiSetCondFlags cond) noexcept
{
	return ImGui::SetNextTreeNodeOpen(is_open, cond);
}

bool
Gui::collapsingHeader(const char* label, GuiTreeNodeFlags flags) noexcept
{
	return ImGui::CollapsingHeader(label, flags);
}

bool
Gui::collapsingHeader(const char* label, bool* isOpened, GuiTreeNodeFlags flags) noexcept
{
	return ImGui::CollapsingHeader(label, isOpened, flags);
}

bool
Gui::selectable(const char* label, bool selected, GuiSelectableFlags flags, const float2& size) noexcept
{
	return ImGui::Selectable(label, selected, flags, (const ImVec2&)size);
}

bool
Gui::selectable(const char* label, bool* p_selected, GuiSelectableFlags flags, const float2& size) noexcept
{
	return ImGui::Selectable(label, p_selected, flags, (const ImVec2&)size);
}

bool
Gui::listBox(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
{
	return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
}

bool
Gui::listBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items) noexcept
{
	return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items);
}

bool
Gui::listBoxHeader(const char* label, const float2& size) noexcept
{
	return ImGui::ListBoxHeader(label, (const ImVec2&)size);
}

bool
Gui::listBoxHeader(const char* label, int items_count, int height_in_items) noexcept
{
	return ImGui::ListBoxHeader(label, items_count, height_in_items);
}

void
Gui::listBoxFooter() noexcept
{
	ImGui::ListBoxFooter();
}

void
Gui::value(const char* prefix, bool b) noexcept
{
	ImGui::Value(prefix, b);
}

void
Gui::value(const char* prefix, int v) noexcept
{
	ImGui::Value(prefix, v);
}

void
Gui::value(const char* prefix, unsigned int v) noexcept
{
	ImGui::Value(prefix, v);
}

void
Gui::value(const char* prefix, float v, const char* float_format) noexcept
{
	ImGui::Value(prefix, v, float_format);
}

void
Gui::valueColor(const char* prefix, const float4& v) noexcept
{
	ImGui::ValueColor(prefix, (const ImVec4&)v);
}

void
Gui::valueColor(const char* prefix, std::uint32_t v) noexcept
{
	ImGui::ValueColor(prefix, v);
}

void
Gui::setTooltip(const char* fmt, ...) noexcept
{
	va_list args;
	va_start(args, fmt);
	ImGui::SetTooltipV(fmt, args);
	va_end(args);
}

void
Gui::setTooltipV(const char* fmt, va_list args) noexcept
{
	ImGui::SetTooltipV(fmt, args);
}

void
Gui::beginTooltip() noexcept
{
	ImGui::BeginTooltip();
}

void
Gui::endTooltip() noexcept
{
	ImGui::EndTooltip();
}

bool
Gui::beginMainMenuBar() noexcept
{
	return ImGui::BeginMainMenuBar();
}

void
Gui::endMainMenuBar() noexcept
{
	ImGui::EndMainMenuBar();
}

bool
Gui::beginMenuBar() noexcept
{
	return ImGui::BeginMenuBar();
}

void
Gui::endMenuBar() noexcept
{
	return ImGui::EndMenuBar();
}

bool
Gui::beginMenu(const char* label, bool enabled) noexcept
{
	return ImGui::BeginMenu(label, enabled);
}

void
Gui::endMenu() noexcept
{
	return ImGui::EndMenu();
}

bool
Gui::menuItem(const char* label, const char* shortcut, bool selected, bool enabled) noexcept
{
	return ImGui::MenuItem(label, shortcut, selected, enabled);
}

bool
Gui::menuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled) noexcept
{
	return ImGui::MenuItem(label, shortcut, p_selected, enabled);
}

void
Gui::openPopup(const char* str_id) noexcept
{
	return ImGui::OpenPopup(str_id);
}

bool
Gui::beginPopup(const char* str_id) noexcept
{
	return ImGui::BeginPopup(str_id);
}

bool
Gui::beginPopupModal(const char* name, bool* isOpened, GuiWindowFlags extraFlags) noexcept
{
	return ImGui::BeginPopupModal(name, isOpened, extraFlags);
}

bool
Gui::beginPopupContextItem(const char* str_id, int mouse_button) noexcept
{
	return ImGui::BeginPopupContextItem(str_id, mouse_button);
}

bool
Gui::beginPopupContextWindow(bool also_over_items, const char* str_id, int mouse_button) noexcept
{
	return ImGui::BeginPopupContextWindow(also_over_items, str_id, mouse_button);
}

bool
Gui::beginPopupContextVoid(const char* str_id, int mouse_button) noexcept
{
	return ImGui::BeginPopupContextVoid(str_id, mouse_button);
}

void
Gui::endPopup() noexcept
{
	return ImGui::EndPopup();
}

void
Gui::closeCurrentPopup() noexcept
{
	return ImGui::CloseCurrentPopup();
}

void
Gui::pushClipRect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept
{
	return ImGui::PushClipRect((const ImVec2&)clip_rect_min, (const ImVec2&)clip_rect_max, intersect_with_current_clip_rect);
}

void
Gui::popClipRect() noexcept
{
	return ImGui::PopClipRect();
}

bool
Gui::isItemHovered() noexcept
{
	return ImGui::IsItemHovered();
}

bool
Gui::isItemHoveredRect() noexcept
{
	return ImGui::IsItemHoveredRect();
}

bool
Gui::isItemActive() noexcept
{
	return ImGui::IsItemActive();
}

bool
Gui::isItemClicked(int mouse_button) noexcept
{
	return ImGui::IsItemClicked();
}

bool
Gui::isItemVisible() noexcept
{
	return ImGui::IsItemVisible();
}

bool
Gui::isAnyItemHovered() noexcept
{
	return ImGui::IsAnyItemHovered();
}

bool
Gui::isAnyItemActive() noexcept
{
	return ImGui::IsAnyItemActive();
}

float2
Gui::getItemRectMin() noexcept
{
	return ImVec2ToFloat2(ImGui::GetItemRectMin());
}

float2
Gui::getItemRectMax() noexcept
{
	return ImVec2ToFloat2(ImGui::GetItemRectMax());
}

float2
Gui::getItemRectSize() noexcept
{
	return ImVec2ToFloat2(ImGui::GetItemRectSize());
}

void
Gui::setItemAllowOverlap() noexcept
{
	return ImGui::SetItemAllowOverlap();
}

bool
Gui::isWindowHovered() noexcept
{
	return ImGui::IsWindowHovered();
}

bool
Gui::isWindowFocused() noexcept
{
	return ImGui::IsWindowFocused();
}

bool
Gui::isRootWindowFocused() noexcept
{
	return ImGui::IsRootWindowFocused();
}

bool
Gui::isRootWindowOrAnyChildFocused() noexcept
{
	return ImGui::IsRootWindowOrAnyChildFocused();
}

bool
Gui::isRootWindowOrAnyChildHovered() noexcept
{
	return ImGui::IsRootWindowOrAnyChildHovered();
}

bool
Gui::isRectVisible(const float2& size) noexcept
{
	return ImGui::IsRectVisible((const ImVec2&)size);
}

bool
Gui::isPosHoveringAnyWindow(const float2& pos) noexcept
{
	return ImGui::IsPosHoveringAnyWindow((const ImVec2&)pos);
}

float
Gui::getTime() noexcept
{
	return ImGui::GetTime();
}

int
Gui::getFrameCount() noexcept
{
	return ImGui::GetFrameCount();
}

const char*
Gui::getStyleColName(GuiCol idx) noexcept
{
	return ImGui::GetStyleColName(idx);
}

float2
Gui::calcItemRectClosestPoint(const float2& pos, bool on_edge, float outward) noexcept
{
	return ImVec2ToFloat2(ImGui::CalcItemRectClosestPoint((ImVec2&)(pos), on_edge, outward));
}

float2
Gui::calcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width) noexcept
{
	return ImVec2ToFloat2(ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width));
}

void
Gui::calcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept
{
	return ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
}

bool
Gui::beginChildFrame(GuiID id, const float2& size, GuiWindowFlags extraFlags) noexcept
{
	return ImGui::BeginChildFrame(id, (const ImVec2&)size, extraFlags);
}

void
Gui::endChildFrame() noexcept
{
	return ImGui::EndChildFrame();
}

float4
Gui::colorConvertU32ToFloat4(std::uint32_t in) noexcept
{
	return ImVec4ToFloat4(ImGui::ColorConvertU32ToFloat4(in));
}

std::uint32_t
Gui::colorConvertFloat4ToU32(const float4& in) noexcept
{
	return ImGui::ColorConvertFloat4ToU32((const ImVec4&)in);
}

void
Gui::colorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept
{
	return ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
}

void
Gui::colorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept
{
	return ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
}

bool
Gui::isKeyDown(int key_index) noexcept
{
	return ImGui::IsKeyDown(key_index);
}

bool
Gui::isKeyPressed(int key_index, bool repeat) noexcept
{
	return ImGui::IsKeyPressed(key_index, repeat);
}

bool
Gui::isKeyReleased(int key_index) noexcept
{
	return ImGui::IsKeyReleased(key_index);
}

bool
Gui::isMouseDown(int button) noexcept
{
	return ImGui::IsMouseDown(button);
}

bool
Gui::isMouseClicked(int button, bool repeat) noexcept
{
	return ImGui::IsMouseClicked(button, repeat);
}

bool
Gui::isMouseDoubleClicked(int button) noexcept
{
	return ImGui::IsMouseDoubleClicked(button);
}

bool
Gui::isMouseReleased(int button) noexcept
{
	return ImGui::IsMouseReleased(button);
}

bool
Gui::isMouseHoveringWindow() noexcept
{
	return ImGui::IsMouseHoveringWindow();
}

bool
Gui::isMouseHoveringAnyWindow() noexcept
{
	return ImGui::IsMouseHoveringAnyWindow();
}

bool
Gui::isMouseHoveringRect(const float2& min, const float2& max, bool clip) noexcept
{
	return ImGui::IsMouseHoveringRect((const ImVec2&)min, (const ImVec2&)max, clip);
}

bool
Gui::isMouseDragging(int button, float lock_threshold) noexcept
{
	return ImGui::IsMouseDragging();
}

float2
Gui::getMousePos() noexcept
{
	return ImVec2ToFloat2(ImGui::GetMousePos());
}

float2
Gui::getMousePosOnOpeningCurrentPopup() noexcept
{
	return ImVec2ToFloat2(ImGui::GetMousePosOnOpeningCurrentPopup());
}

float2
Gui::getMouseDragDelta(int button, float lock_threshold) noexcept
{
	return ImVec2ToFloat2(ImGui::GetMouseDragDelta());
}

void
Gui::resetMouseDragDelta(int button) noexcept
{
	return ImGui::ResetMouseDragDelta();
}

void
Gui::setMouseCursor(GuiMouseCursor type) noexcept
{
	return ImGui::SetMouseCursor(type);
}

void
Gui::captureKeyboardFromApp(bool capture) noexcept
{
	return ImGui::CaptureKeyboardFromApp(capture);
}

void
Gui::captureMouseFromApp(bool capture) noexcept
{
	return ImGui::CaptureMouseFromApp(capture);
}

_NAME_END