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
#include <ray/imgui_system.h>
#include <ray/render_system.h>
#include <ray/graphics_data.h>
#include <ray/graphics_texture.h>
#include <ray/material.h>
#include <imgui.h>

_NAME_BEGIN

__ImplementSingleton(IMGUISystem)

IMGUISystem::IMGUISystem() noexcept
{
}

IMGUISystem::~IMGUISystem() noexcept
{
}

bool
IMGUISystem::open() except
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 8.0f;
	style.GrabRounding = 5.0f;
	style.FramePadding.x = 2;
	style.FramePadding.y = 3;
	style.WindowTitleAlign.x = 0.5;

	style.Colors[ImGuiCol_MenuBarBg] = ImColor(222, 150, 0, 255);
	style.Colors[ImGuiCol_Text] = ImColor(245, 245, 245, 255);
	style.Colors[ImGuiCol_TextDisabled] = ImColor(150, 150, 150, 255);
	style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 220);
	style.Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(34, 134, 230, 179);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(24, 96, 200, 179);
	style.Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 179);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 179);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 179);
	style.Colors[ImGuiCol_Button] = ImColor(0, 95, 168, 255);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(230, 179, 0, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(255, 153, 0, 255);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.2f, 0.5f, 1.0f, 0.5f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.2f, 0.5f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.2f, 0.5f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0, 0.6, 1.0, 0.75f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20, 0.20f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(125, 125, 125, 100);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(75, 75, 75, 100);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.7, 0.0, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.4, 0.0, 1.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.10f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.25f);

	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.90f);

	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.125f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	std::memcpy(&IMGUI::_defalutStyle, &style, sizeof(style));

	io.KeyMap[ImGuiKey_Tab] = GuiInputKey::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = GuiInputKey::ArrowLeft;
	io.KeyMap[ImGuiKey_RightArrow] = GuiInputKey::ArrowRight;
	io.KeyMap[ImGuiKey_UpArrow] = GuiInputKey::ArrowUp;
	io.KeyMap[ImGuiKey_DownArrow] = GuiInputKey::ArrowDown;
	io.KeyMap[ImGuiKey_PageUp] = GuiInputKey::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = GuiInputKey::PageDown;
	io.KeyMap[ImGuiKey_Home] = GuiInputKey::Home;
	io.KeyMap[ImGuiKey_End] = GuiInputKey::End;
	io.KeyMap[ImGuiKey_Delete] = GuiInputKey::Delete;
	io.KeyMap[ImGuiKey_Backspace] = GuiInputKey::Backspace;
	io.KeyMap[ImGuiKey_Enter] = GuiInputKey::Return;
	io.KeyMap[ImGuiKey_Escape] = GuiInputKey::Escape;
	io.KeyMap[ImGuiKey_A] = GuiInputKey::A;
	io.KeyMap[ImGuiKey_C] = GuiInputKey::C;
	io.KeyMap[ImGuiKey_V] = GuiInputKey::V;
	io.KeyMap[ImGuiKey_X] = GuiInputKey::X;
	io.KeyMap[ImGuiKey_Y] = GuiInputKey::Y;
	io.KeyMap[ImGuiKey_Z] = GuiInputKey::Z;

	std::uint8_t* pixels;
	int width, height;
	io.Fonts->ClearFonts();
	io.Fonts->AddFontFromFileTTF("../../engine/fonts/NotoSansHans-DemiLight.otf", 12.0f, 0, io.Fonts->GetGlyphRangesChinese());
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	ray::GraphicsTextureDesc fontDesc;
	fontDesc.setSize(width, height);
	fontDesc.setTexDim(ray::GraphicsTextureDim::GraphicsTextureDim2D);
	fontDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	fontDesc.setStream(pixels);
	fontDesc.setStreamSize(width * height * sizeof(std::uint32_t));
	fontDesc.setTexTiling(ray::GraphicsImageTiling::GraphicsImageTilingLinear);
	fontDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	fontDesc.setSamplerWrap(ray::GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_texture = RenderSystem::instance()->createTexture(fontDesc);
	if (!_texture)
		return false;

	io.Fonts->TexID = (void*)_texture.get();

	_material = RenderSystem::instance()->createMaterial("sys:fx/uilayout.fxml");
	if (!_material)
		return false;

	_materialTech = _material->getTech("IMGUI");
	_materialDecal = _material->getParameter("decal");
	_materialProj = _material->getParameter("proj");

	_materialDecal->uniformTexture(_texture);

	return true;
}

void
IMGUISystem::close() noexcept
{
}

void
IMGUISystem::setStyle(GuiStyle* style)
{
}

bool
IMGUISystem::injectMouseMove(float absx, float absy) noexcept
{
	auto& io = ImGui::GetIO();
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return true;
}

bool
IMGUISystem::injectMousePress(float absx, float absy, GuiInputButton::Code code) noexcept
{
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = true;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return true;
}

bool
IMGUISystem::injectMouseRelease(float absx, float absy, GuiInputButton::Code code) noexcept
{
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = false;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return true;
}

bool
IMGUISystem::injectKeyPress(GuiInputKey::Code _key, GuiInputChar _char) noexcept
{
	auto& io = ImGui::GetIO();
	if (_key == GuiInputKey::Code::LeftControl)
		io.KeyCtrl = true;
	else if (_key == GuiInputKey::Code::LeftAlt)
		io.KeyAlt = true;
	io.KeysDown[_key] = true;
	return true;
}

bool
IMGUISystem::injectKeyRelease(GuiInputKey::Code _key) noexcept
{
	auto& io = ImGui::GetIO();
	if (_key == GuiInputKey::Code::LeftControl)
		io.KeyCtrl = false;
	else if (_key == GuiInputKey::Code::LeftAlt)
		io.KeyAlt = false;
	io.KeysDown[_key] = false;
	return true;
}

bool
IMGUISystem::isFocusMouse() const noexcept
{
	return false;
}

bool
IMGUISystem::isFocusKey() const noexcept
{
	return false;
}

bool
IMGUISystem::isCaptureMouse() const noexcept
{
	return false;
}

void
IMGUISystem::setViewport(std::uint32_t w, std::uint32_t h) noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = w;
	io.DisplaySize.y = h;
}

void
IMGUISystem::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	w = io.DisplaySize.x;
	h = io.DisplaySize.y;
}

void
IMGUISystem::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplayFramebufferScale.x = w;
	io.DisplayFramebufferScale.y = h;

	ray::float4x4 project;
	if (RenderSystem::instance()->getRenderSetting().deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		project.makeOrtho_lh(0, w, 0, h, 0, 1);
	else
		project.makeOrtho_lh(0, w, h, 0, 0, 1);

	_materialProj->uniform4fmat(project);
}

void
IMGUISystem::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	w = io.DisplayFramebufferScale.x;
	h = io.DisplayFramebufferScale.y;
}

void
IMGUISystem::render(float delta) except
{
	auto renderer = RenderSystem::instance();

	auto drawData = ImGui::GetDrawData();

	std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
	std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
	if (totalVertexSize == 0 || totalIndirectSize == 0)
		return;

	if (!_vbo || _vbo->getGraphicsDataDesc().getStreamSize() < totalVertexSize)
	{
		ray::GraphicsDataDesc dataDesc;
		dataDesc.setType(ray::GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
		dataDesc.setStream(0);
		dataDesc.setStreamSize(totalVertexSize);
		dataDesc.setUsage(ray::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
		_vbo = renderer->createGraphicsData(dataDesc);
		if (!_vbo)
			return;
	}

	if (!_ibo || _ibo->getGraphicsDataDesc().getStreamSize() < totalIndirectSize)
	{
		ray::GraphicsDataDesc elementDesc;
		elementDesc.setType(ray::GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
		elementDesc.setStream(0);
		elementDesc.setStreamSize(totalIndirectSize);
		elementDesc.setUsage(ray::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
		_ibo = renderer->createGraphicsData(elementDesc);
		if (!_ibo)
			return;
	}

	ImDrawVert* vbo;
	ImDrawIdx* ibo;

	_vbo->map(0, totalVertexSize, (void**)&vbo);
	_ibo->map(0, totalIndirectSize, (void**)&ibo);

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		std::memcpy(vbo, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
		std::memcpy(ibo, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
		vbo += cmd_list->VtxBuffer.size();
		ibo += cmd_list->IdxBuffer.size();
	}

	_vbo->unmap();
	_ibo->unmap();

	auto& io = ImGui::GetIO();
	renderer->setViewport(0, ray::Viewport(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));
	renderer->setScissor(0, ray::Scissor(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));

	renderer->setVertexBuffer(0, _vbo, 0);
	renderer->setIndexBuffer(_ibo, 0, ray::GraphicsIndexType::GraphicsIndexTypeUInt16);

	renderer->setMaterialPass(_materialTech->getPass(0));

	std::uint32_t vdx_buffer_offset = 0;
	std::uint32_t idx_buffer_offset = 0;

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			auto texture = (ray::GraphicsTexture*)pcmd->TextureId;
			_materialDecal->uniformTexture(texture->downcast_pointer<ray::GraphicsTexture>());

			auto scissor = ImVec4((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
#if !defined(_BUILD_VULKAN)
			scissor.y = io.DisplaySize.y - scissor.w - scissor.y;
#endif

			renderer->setScissor(0, ray::Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
			renderer->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

			idx_buffer_offset += pcmd->ElemCount;
		}

		vdx_buffer_offset += cmd_list->VtxBuffer.size();
	}
}

_NAME_END