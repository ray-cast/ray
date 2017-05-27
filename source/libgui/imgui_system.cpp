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
	this->close();
}

bool
IMGUISystem::open(void* _window) except
{
	assert(_window);
#if _WINDOWS
	assert(::IsWindow((HWND)_window));
#endif

	ImGuiIO& io = ImGui::GetIO();
	io.ImeWindowHandle = _window;

	GuiStyle style;
	IMGUI::setStyle(style);

	io.KeyMap[ImGuiKey_Tab] = InputKey::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = InputKey::ArrowLeft;
	io.KeyMap[ImGuiKey_RightArrow] = InputKey::ArrowRight;
	io.KeyMap[ImGuiKey_UpArrow] = InputKey::ArrowUp;
	io.KeyMap[ImGuiKey_DownArrow] = InputKey::ArrowDown;
	io.KeyMap[ImGuiKey_PageUp] = InputKey::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = InputKey::PageDown;
	io.KeyMap[ImGuiKey_Home] = InputKey::Home;
	io.KeyMap[ImGuiKey_End] = InputKey::End;
	io.KeyMap[ImGuiKey_Delete] = InputKey::Delete;
	io.KeyMap[ImGuiKey_Backspace] = InputKey::Backspace;
	io.KeyMap[ImGuiKey_Enter] = InputKey::Enter;
	io.KeyMap[ImGuiKey_Escape] = InputKey::Escape;
	io.KeyMap[ImGuiKey_A] = InputKey::A;
	io.KeyMap[ImGuiKey_C] = InputKey::C;
	io.KeyMap[ImGuiKey_V] = InputKey::V;
	io.KeyMap[ImGuiKey_X] = InputKey::X;
	io.KeyMap[ImGuiKey_Y] = InputKey::Y;
	io.KeyMap[ImGuiKey_Z] = InputKey::Z;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x4e00, 0x9FAF, // CJK Ideograms
		0,
	};

	std::uint8_t* pixels;
	int width, height;
	io.Fonts->ClearFonts();
	io.Fonts->AddFontFromFileTTF("../../engine/fonts/DroidSansFallback.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../engine/fonts/DroidSansFallback.ttf", 15.0f, 0, &ranges[0]);
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

	ray::GraphicsDataDesc dataDesc;
	dataDesc.setType(ray::GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	dataDesc.setStream(0);
	dataDesc.setStreamSize(4096 * sizeof(ImDrawVert));
	dataDesc.setUsage(ray::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
	_vbo = RenderSystem::instance()->createGraphicsData(dataDesc);
	if (!_vbo)
		return false;

	ray::GraphicsDataDesc elementDesc;
	elementDesc.setType(ray::GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	elementDesc.setStream(0);
	elementDesc.setStreamSize(4096 * sizeof(ImDrawIdx));
	elementDesc.setUsage(ray::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
	_ibo = RenderSystem::instance()->createGraphicsData(elementDesc);
	if (!_ibo)
		return false;

	_material = RenderSystem::instance()->createMaterial("sys:fx/uilayout.fxml");
	if (!_material)
		return false;

	_materialTech = _material->getTech("IMGUI");
	_materialDecal = _material->getParameter("decal");
	_materialDecal->uniformTexture(_texture);

	_materialProj = _material->getParameter("proj");

	return true;
}

void
IMGUISystem::close() noexcept
{
	_vbo.reset();
	_ibo.reset();
	_texture.reset();
	_material.reset();
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
IMGUISystem::injectMousePress(float absx, float absy, InputButton::Code code) noexcept
{
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = true;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return true;
}

bool
IMGUISystem::injectMouseRelease(float absx, float absy, InputButton::Code code) noexcept
{
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = false;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return true;
}

bool
IMGUISystem::injectMouseWheel(float wheel) noexcept
{
	auto& io = ImGui::GetIO();
	io.MouseWheel = wheel;
	return true;
}

bool
IMGUISystem::injectKeyPress(InputKey::Code _key, GuiInputChar _char) noexcept
{
	auto& io = ImGui::GetIO();
	if (_key != InputKey::Code::None)
	{
		if (_key == InputKey::Code::LeftControl)
			io.KeyCtrl = true;
		else if (_key == InputKey::Code::LeftShift)
			io.KeyShift = true;
		else if (_key == InputKey::Code::LeftAlt)
			io.KeyAlt = true;
		else if (_key == InputKey::Code::LeftSuper)
			io.KeySuper = true;

		io.KeysDown[_key] = true;
	}
	else
	{
		io.AddInputCharacter(_char);
	}

	return true;
}

bool
IMGUISystem::injectKeyRelease(InputKey::Code _key) noexcept
{
	auto& io = ImGui::GetIO();
	if (_key == InputKey::Code::LeftControl)
		io.KeyCtrl = false;
	else if (_key == InputKey::Code::LeftShift)
		io.KeyShift = false;
	else if (_key == InputKey::Code::LeftAlt)
		io.KeyAlt = false;
	else if (_key == InputKey::Code::LeftSuper)
		io.KeySuper = false;

	io.KeysDown[_key] = false;
	return true;
}

bool
IMGUISystem::injectWindowFocus(bool focus) noexcept
{
	if (focus)
	{
		auto& io = ImGui::GetIO();

		std::memset(io.KeysDown, 0, sizeof(io.KeysDown));
		std::memset(io.MouseDown, 0, sizeof(io.MouseDown));
	}

	return true;
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

			ImVec4 scissor((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

			if (RenderSystem::instance()->getRenderSetting().deviceType != GraphicsDeviceType::GraphicsDeviceTypeVulkan)
				scissor.y = io.DisplaySize.y - scissor.w - scissor.y;

			renderer->setScissor(0, ray::Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
			renderer->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

			idx_buffer_offset += pcmd->ElemCount;
		}

		vdx_buffer_offset += cmd_list->VtxBuffer.size();
	}
}

_NAME_END