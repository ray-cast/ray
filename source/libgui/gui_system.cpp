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
#include <ray/gui_system.h>
#include <ray/render_system.h>
#include <ray/graphics_data.h>
#include <ray/material.h>

#if defined(_BUILD_MYGUI)
#	include "MYGUI/mygui_system.h"
#endif

#include <imgui.h>

_NAME_BEGIN

__ImplementSingleton(GuiSystem)

GuiSystem::GuiSystem() noexcept
{
}

GuiSystem::~GuiSystem() noexcept
{
}

bool
GuiSystem::open(GuiSystemBasePtr custom) except
{
	assert(!_system);

	if (custom)
		_system = custom;
	else
	{
#if defined(_BUILD_MYGUI)
		_system = std::make_shared<MyGuiSystem>();
#endif
	}

	ImGuiIO& io = ImGui::GetIO();

	std::uint8_t* pixels;
	int width, height;
	io.Fonts->ClearFonts();
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
	_materialTech = _material->getTech("IMGUI");
	_materialDecal = _material->getParameter("decal");
	_materialProj = _material->getParameter("proj");

	_materialDecal->uniformTexture(_texture);

	return _system->open();
}

void
GuiSystem::close() noexcept
{
	if (_system)
	{
		_system.reset();
		_system = nullptr;
	}
}

void
GuiSystem::setGuiSystem(GuiSystemBasePtr& system) except
{
	assert(_system);
	if (_system != system)
	{
		_system = system;
	}
}

const GuiSystemBasePtr&
GuiSystem::getGuiSystem() const noexcept
{
	return _system;
}

void
GuiSystem::setCoreProfile(const std::string& core) except
{
	assert(_system);
	return _system->setCoreProfile(core);
}

const std::string&
GuiSystem::getCoreProfile() const noexcept
{
	assert(_system);
	return _system->getCoreProfile();
}

void
GuiSystem::setImageLoader(GuiImageLoaderPtr loader) noexcept
{
	assert(_system);
	return _system->setImageLoader(loader);
}

GuiImageLoaderPtr
GuiSystem::getImageLoader() const noexcept
{
	assert(_system);
	return _system->getImageLoader();
}

bool
GuiSystem::injectMouseMove(int absx, int absy, int _absz) noexcept
{
	assert(_system);
	auto& io = ImGui::GetIO();
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return _system->injectMouseMove(absx, absy, _absz);
}

bool
GuiSystem::injectMousePress(int absx, int absy, GuiInputButton::Code code) noexcept
{
	assert(_system);
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = true;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return _system->injectMousePress(absx, absy, code);
}

bool
GuiSystem::injectMouseRelease(int absx, int absy, GuiInputButton::Code code) noexcept
{
	assert(_system);
	auto& io = ImGui::GetIO();
	io.MouseDown[code] = false;
	io.MousePos.x = absx;
	io.MousePos.y = absy;
	return _system->injectMouseRelease(absx, absy, code);
}

bool
GuiSystem::injectKeyPress(GuiInputKey::Code _key, GuiInputChar _char) noexcept
{
	assert(_system);
	auto& io = ImGui::GetIO();
	return _system->injectKeyPress(_key, _char);
}

bool
GuiSystem::injectKeyRelease(GuiInputKey::Code _key) noexcept
{
	assert(_system);
	return _system->injectKeyRelease(_key);
}

bool
GuiSystem::isFocusMouse() const noexcept
{
	assert(_system);
	return _system->isFocusMouse();
}

bool
GuiSystem::isFocusKey() const noexcept
{
	assert(_system);
	return _system->isFocusKey();
}

bool
GuiSystem::isCaptureMouse() const noexcept
{
	assert(_system);
	return _system->isCaptureMouse();
}

void
GuiSystem::setViewport(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_system);
	_system->setViewport(w, h);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = w;
	io.DisplaySize.y = h;

	ray::float4x4 project;
	if (RenderSystem::instance()->getRenderSetting().deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		project.makeOrtho_lh(0, w, 0, h, 0, 1);
	else
		project.makeOrtho_lh(0, w, h, 0, 0, 1);

	_materialProj->uniform4fmat(project);
}

void
GuiSystem::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
{
	assert(_system);
	_system->getViewport(w, h);
}

GuiWidgetPtr
GuiSystem::createWidget(const rtti::Rtti* rtti)
{
	assert(_system);
	return _system->createWidget(rtti);
}

void
GuiSystem::render(float delta) except
{
	assert(_system);
	_system->render(delta);

	auto renderer = RenderSystem::instance();	

	auto& io = ImGui::GetIO();
	auto drawData = ImGui::GetDrawData();

	std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
	std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (!_vbo || _vbo->getGraphicsDataDesc().getStreamSize() <= totalVertexSize)
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

	if (!_ibo || _ibo->getGraphicsDataDesc().getStreamSize() <= totalIndirectSize)
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

	renderer->setViewport(0, ray::Viewport(0, 0, io.DisplaySize.x, io.DisplaySize.y));
	renderer->setScissor(0, ray::Scissor(0, 0, io.DisplaySize.x, io.DisplaySize.y));

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

			if (renderer->getRenderSetting().deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
			{
				auto scissor = ImVec4((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				renderer->setScissor(0, ray::Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
			}
			else
			{
				auto scissor = ImVec4((int)pcmd->ClipRect.x, (int)(io.DisplaySize.y - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				renderer->setScissor(0, ray::Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
			}		

			renderer->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

			idx_buffer_offset += pcmd->ElemCount;
		}

		vdx_buffer_offset += cmd_list->VtxBuffer.size();
	}
}

_NAME_END