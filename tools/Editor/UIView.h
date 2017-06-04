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
#include <ray/camera_component.h>

#include "UIParams.h"
#include "UITexts.h"

struct GuiViewDelegates
{
	std::function<bool()> onUVMapperCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> onUVMapperWillStart;
	std::function<bool(const GuiParams&, float&, ray::util::string::pointer&)> onUVMapperProcess;

	std::function<bool()> onLightMassCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> onLightMassWillStart;
	std::function<bool(const GuiParams&, float&, ray::util::string::pointer&)> onLightMassProcess;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onLightMassSave;

	std::function<bool(const ray::GameObjects*&)> onFetchMeshes;
	std::function<bool(const ray::GameObjects*&)> onFetchCamera;
	std::function<bool(const ray::GameObjects*&)> onFetchLights;
	std::function<bool(const ray::GameObjects*&)> onFetchLightProbes;

	std::function<bool(const ray::GameObject*)> onSeletedCamera;
	std::function<bool(const ray::GameObject*)> onSeletedLight;
	std::function<bool(const ray::GameObject*)> onSeletedLightProbe;
	std::function<bool(const ray::GameObject*, std::size_t)> onSeletedMesh;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectOpen;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectSave;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectSaveAs;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onModelImport;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onModelSaveAs;
};

class GuiViewComponent final : public ray::GameComponent
{
	__DeclareSubClass(GuiViewComponent, ray::GameComponent)
public:
	GuiViewComponent() noexcept;
	~GuiViewComponent() noexcept;

	void setGuiViewDelegates(const GuiViewDelegates& delegate) noexcept;
	const GuiViewDelegates& getGuiViewDelegates() const noexcept;

	ray::GameComponentPtr clone() const noexcept;

private:
	void onActivate() except;
	void onDeactivate() noexcept;

	void onMessage(const ray::MessagePtr& message) noexcept;
	void onModelPicker(float x, float y) noexcept;

private:
	bool makeCubeObject() noexcept;

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

	void showMeshesLists() noexcept;
	void showAssertLists() noexcept;
	void showMaterialEditor() noexcept;
	void showCameraWindow() noexcept;
	void showSceneWindow() noexcept;

	void startUVMapper() noexcept;
	void startLightMass() noexcept;
	void saveLightMass() noexcept;

	void switchLangPackage(UILang::Lang type) noexcept;

private:
	GuiViewComponent(const GuiViewComponent&) = delete;
	GuiViewComponent& operator=(const GuiViewComponent&) = delete;

private:
	ray::GameObject* _selectedObject;
	ray::GameObjectPtr _cube;

	std::weak_ptr<ray::CameraComponent> _cameraComponent;

	ray::float4 _viewport;

	float _progress;

	bool _showWindowAll;
	bool _showMainMenu;
	bool _showLightMassWindow;
	bool _showInspectorWindow;
	bool _showAssertWindow;
	bool _showCameraWindow;
	bool _showMaterialEditorWindow;
	bool _showStyleEditor;
	bool _showAboutWindow;
	bool _showAboutWindowFirst;
	bool _showMessage;
	bool _showMessageFirst;
	bool _showProcessMessage;
	bool _showProcessMessageFirst;
	bool _mouseHoveringCamera;

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

	GuiViewDelegates _event;
};

#endif