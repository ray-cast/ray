// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_UI_CONTROLLER_H_
#define _H_UI_CONTROLLER_H_

#include <ray/gui.h>
#include <ray/game_component.h>

#include "UIParams.h"
#include "UITexts.h"

class GuiViewComponent final : public ray::GameComponent
{
	__DeclareSubClass(GuiViewComponent, ray::GameComponent)
public:
	GuiViewComponent() noexcept;
	~GuiViewComponent() noexcept;

	ray::GameComponentPtr clone() const noexcept;

	void setProjectImportListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;
	void setProjectSaveListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;
	void setProjectSaveAsListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;

	void setModelImportListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;
	void setModelSaveAsListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;

	void setUVMapperCancel(std::function<bool()> delegate) noexcept;
	void setUVMapperWillStartListener(std::function<bool(const GuiParams&, ray::util::string::pointer&)> delegate) noexcept;
	void setUVMapperProgressListener(std::function<bool(const GuiParams&, float& progressing)> delegate) noexcept;

	void setLightMassCancel(std::function<bool()> delegate) noexcept;
	void setLightMassWillStartListener(std::function<bool(const GuiParams&, ray::util::string::pointer&)> delegate) noexcept;
	void setLightMassProgressListener(std::function<bool(const GuiParams&, float& progressing)> delegate) noexcept;
	void setLightMassSaveAsListener(std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> delegate) noexcept;

private:
	void onMessage(const ray::MessagePtr& message) noexcept;

private:
	void showMainMenu() noexcept;
	void showStyleEditor() noexcept;
	void showLightMass() noexcept;
	void showAboutWindow() noexcept;

	void showMessage() noexcept;
	void showPopupMessage(const ray::util::string& title, const ray::util::string& message, std::size_t hash) noexcept;
	void showProcessMessage() noexcept;

	bool showFileOpenBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept;
	bool showFileSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept;

	void showModelImportBrowse() noexcept;
	void showModelExportBrowse() noexcept;

	void showProjectOpenBrowse() noexcept;
	void showProjectSaveBrowse() noexcept;
	void showProjectSaveAsBrowse() noexcept;

	void showMaterialEditor() noexcept;

	void startUVMapper() noexcept;
	void startLightMass() noexcept;
	void saveLightMass() noexcept;

	void switchLangPackage(UILang::Lang type) noexcept;

private:
	GuiViewComponent(const GuiViewComponent&) = delete;
	GuiViewComponent& operator=(const GuiViewComponent&) = delete;

private:
	float _progress;

	bool _showMainMenu;
	bool _showLightMassWindow;
	bool _showStyleEditor;
	bool _showAboutWindow;
	bool _showAboutWindowFirst;
	bool _showMessage;
	bool _showMessageFirst;
	bool _showProcessMessage;
	bool _showProcessMessageFirst;

	LightMassType _lightMassType;

	std::string _pathProject;
	std::vector<const char*> _langs;

	std::size_t _messageHash;
	std::string _messageTitle;
	std::string _messageText;
	std::map<std::size_t, bool> _ignoreMessage;

	ray::GuiStyle _style;
	ray::GuiStyle _styleDefault;

	GuiParams _default;
	GuiParams _setting;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onProjectOpen;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onProjectSave;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onProjectSaveAs;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onModelImport;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onModelSaveAs;

	std::function<bool()> _onUVMapperCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> _onUVMapperWillStart;
	std::function<bool(const GuiParams&, float&)> _onUVMapperProcess;

	std::function<bool()> _onLightMassCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> _onLightMassWillStart;
	std::function<bool(const GuiParams&, float&)> _onLightMassProcess;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> _onLightMassSave;
};

#endif