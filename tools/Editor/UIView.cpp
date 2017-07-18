// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2017-2018.
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
#include "UIView.h"
#include "UITexts.h"
#include "nativefiledialog/nfd.h"
#include <ray/gui_message.h>
#include <ray/ioserver.h>
#include <ray/input.h>
#include <ray/input_feature.h>
#include <ray/game_server.h>
#include <ray/mesh_render_component.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/material.h>
#include <ray/res_manager.h>
#include <ray/render_pipeline_framebuffer.h>

__ImplementSubClass(GuiViewComponent, ray::GameComponent, "GuiView")

const char* itemsUVSlot[] = { "0", "1", "2", "3", "4" };
const char* itemsImageSize[] = { "512", "1024", "2048", "4096", "8192" };
const char* itemsSampleSize[] = { "32", "64", "128", "256", "512" };

const char* TEXTURE_MAP_FROM[] = { "Constant value", "Static Image", "Animation Image", "Diffuse map from model", "Sphere map from model", "Toon map from model", "Screen map (cannot preview)", "Ambient color from model", "Specular color from model" };
const char* TEXTURE_MAP_UV_FLIP[] = { "None", " Axis X", " Axis Y", " Axis X & Y" };
const char* TEXTURE_SUBALBEDO_TYPE[] = { "None", "Mulplay", "Exponent", "Add", "Melain", "Alpha Blend" };
const char* TEXTURE_SMOOTHNESS_TYPE[] = { "Smoothness", "Roughness", "Roughness" };
const char* TEXTURE_NORMAL_TYPE[] = { "RGB tangent space", "RG tangent space", "PerturbNormalLQ", "PerturbNormalHQ" };
const char* TEXTURE_SPECULAR_TYPE[] = { "Specular color", "Specular color", "Specular gray", "Specular gray" };
const char* TEXTURE_OCCLUSION_TYPE[] = { "linear", "sRGB", "linear with second UV", "sRGB with second UV" };
const char* TEXTURE_SHADING_MODEL[] = { "Default", "PreIntegrated Skin", "Unlit placeholder", "Reserved", "Glass", "Cloth", "Clear Coat", "Subsurface" };

const char g_SupportedFormats[] = { "bmp,png,jpg,tga,dds,hdr;fx;ies" };

std::vector<const char*> g_SupportedProject = { "map" };
std::vector<const char*> g_SupportedIES = { "ies" };
std::vector<const char*> g_SupportedModel = { "pmx" };
std::vector<const char*> g_SupportedImages = { "bmp", "png", "jpg", "tga", "dds", "hdr" };
std::vector<const char*> g_SupportedMaterial = { "fx" };

GuiViewComponent::GuiViewComponent() noexcept
	: _selectedSubset(std::numeric_limits<std::size_t>::max())
	, _selectedSubsetLast(std::numeric_limits<std::size_t>::max())
	, _selectedObject(nullptr)
	, _selectedObjectLast(nullptr)
	, _selectedItem(nullptr)
	, _selectedShift(std::numeric_limits<std::size_t>::max())
	, _selectedMaterial(nullptr)
	, _selectedMaterialLast(nullptr)
	, _lightMassType(LightMassType::UVMapper)
	, _viewport(0.0f, 0.0f, 0.0f, 0.0f)
	, _assetImageSize(ray::float2(76.0f, 76.0f))
	, _materialImageSize(ray::float2(128.0f, 128.0f))
{
	_progress = 0.0f;
	_isShowWindowAll = true;
	_isShowMainMenu = true;
	_isShowLightMassWindow = true;
	_isShowInspectorWindow = true;
	_isShowCameraWindow = true;
	_isShowAssertWindow = true;
	_isShowMaterialWindow = true;
	_isShowHierarchyWindow = true;
	_isShowInspectorWindow = true;
	_isShowStyleEditor = false;
	_isShowAboutWindow = false;
	_isShowAboutWindowFirst = false;
	_isShowMessage = false;
	_isShowMessageFirst = false;
	_isShowProcessMessage = false;
	_isShowProcessMessageFirst = false;
	_isMouseHoveringCameraDock = false;

	_styleDefault.AntiAliasedLines = false;
	_styleDefault.ItemSpacing.y = 3;
	_styleDefault.WindowPadding.x = 5;
	_styleDefault.WindowPadding.y = 10;
	_styleDefault.FramePadding.x = 6;
	_styleDefault.ScrollbarSize = 15;
	_styleDefault.IndentSpacing = 25;
	_styleDefault.Colors[ray::GuiCol::GuiColFrameBgActive] = ray::float4(0.3f, 0.3f, 0.3f, 0.58f);
	_styleDefault.Colors[ray::GuiCol::GuiColFrameBgHovered] = ray::float4(0.4f, 0.4f, 0.4f, 0.75f);
	_styleDefault.Colors[ray::GuiCol::GuiColWindowBg] = ray::float4(0.075f, 0.075f, 0.075f, 1.0f);
	_styleDefault.Colors[ray::GuiCol::GuiColChildWindowBg] = ray::float4(0.33333f, 0.33333f, 0.33333f, 0.45f);
	_styleDefault.Colors[ray::GuiCol::GuiColScrollbarGrab] = ray::float4(0.314f, 0.314f, 0.33333f, 1.0f);
	_styleDefault.Colors[ray::GuiCol::GuiColBorder] = ray::float4(0.0f, 0.0f, 0.0f, 1.0f);
	_style = _styleDefault;

	ray::Gui::setStyle(_styleDefault);

	::GetLangPackage(UILang::Lang::English, _langs);
}

GuiViewComponent::~GuiViewComponent() noexcept
{
}

ray::GameComponentPtr
GuiViewComponent::clone() const noexcept
{
	return std::make_shared<GuiViewComponent>();
}

void
GuiViewComponent::setEditorEvents(const EditorEvents& delegate) noexcept
{
	_event = delegate;
}

const EditorEvents&
GuiViewComponent::getEditorEvents() const noexcept
{
	return _event;
}

void
GuiViewComponent::onActivate() except
{
}

void
GuiViewComponent::onDeactivate() noexcept
{
}

void
GuiViewComponent::onMessage(const ray::MessagePtr& message) except
{
	if (message->isInstanceOf<ray::InputMessage>())
	{
		auto& event = message->downcast<ray::InputMessage>()->getEvent();
		switch (event.event)
		{
		case ray::InputEvent::Drop:
		{
			for (std::uint32_t i = 0; i < event.drop.count; i++)
			{
				char name[MAX_PATH];
				if (ray::util::ext_name(event.drop.files[i], name, sizeof(name)) == 0)
					continue;

				ray::util::string::value_type fullpath[PATHLIMIT];
				if (ray::util::toUnixPath(event.drop.files[i], fullpath, PATHLIMIT) == 0)
				{
					static const char* errorTips = "Cannot open file, check the spelling of the file path.";
					this->showPopupMessage(_langs[UILang::Error].c_str(), errorTips, std::hash<const char*>{}(errorTips));
					return;
				}

				std::vector<const char*>* supportedImport[] = { &g_SupportedIES, &g_SupportedImages, &g_SupportedMaterial, &g_SupportedModel };

				std::function<bool(const char*, char*&)>* delegate[] = { &_event.onImportIES, &_event.onImportTexture, &_event.onImportMaterial, &_event.onImportModel };

				bool _loaded = false;

				for (std::size_t j = 0; j < sizeof(delegate) / sizeof(delegate[0]) && !_loaded; j++)
				{
					for (const auto& format : *supportedImport[j])
					{
						if (ray::util::strcmp(name, format) != 0)
							continue;

						if (!*delegate[j])
							continue;

						ray::util::string::pointer error = nullptr;
						if (!(*delegate[j])(fullpath, error))
						{
							if (error)
								this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
						}

						_loaded = true;
						break;
					}
				}
			}

			static const char* succeededTips = "Resource loaded.";
			this->showPopupMessage(_langs[UILang::Succeeded].c_str(), succeededTips, std::hash<const char*>{}(succeededTips));
		}
		break;
		default:
		{
			auto inputFeature = ray::GameServer::instance()->getFeature<ray::InputFeature>();
			if (inputFeature)
			{
				auto input = inputFeature->getInput();
				if (!input)
					return;

				if (_isShowAboutWindow || _isShowStyleEditor || _isShowMessage || _isShowProcessMessage)
					return;

				if (input->isKeyDown(ray::InputKey::Code::Escape))
					_isShowWindowAll = !_isShowWindowAll;

				if (!_isMouseHoveringCameraDock)
					return;

				if (!input->isLockedCursor())
				{
					if (input->isKeyPressed(ray::InputKey::LeftControl) && input->isButtonDown(ray::InputButton::LEFT))
					{
						float x;
						float y;
						input->getMousePos(x, y);

						this->startModelPicker(x, y);
					}

					if (input->isButtonDown(ray::InputButton::Code::MOUSEWHEEL))
					{
						float speed = ray::Gui::isKeyDown(ray::InputKey::LeftShift) ? 5.0f : 1.0f;
						const ray::Vector3& forward = _cameraComponent.lock()->getGameObject()->getForward();
						_cameraComponent.lock()->getGameObject()->setTranslateAccum(-forward * speed);
					}

					if (input->isButtonUp(ray::InputButton::Code::MOUSEWHEEL))
					{
						float speed = ray::Gui::isKeyDown(ray::InputKey::LeftShift) ? 5.0f : 1.0f;
						const ray::Vector3& forward = _cameraComponent.lock()->getGameObject()->getForward();
						_cameraComponent.lock()->getGameObject()->setTranslateAccum(forward * speed);
					}

					if (input->isButtonPressed(ray::InputButton::Code::MIDDLE))
					{
						float speedX = input->getAxis(ray::InputAxis::MouseX) * 0.5f;
						float speedY = input->getAxis(ray::InputAxis::MouseY) * 0.5f;

						const ray::Vector3& up = _cameraComponent.lock()->getGameObject()->getUpVector();
						const ray::Vector3& right = _cameraComponent.lock()->getGameObject()->getRight();
						_cameraComponent.lock()->getGameObject()->setTranslateAccum(up * speedY);
						_cameraComponent.lock()->getGameObject()->setTranslateAccum(-right * speedX);
					}

					if (input->isButtonPressed(ray::InputButton::Code::RIGHT))
					{
						float axisX = input->getAxis(ray::InputAxis::MouseX);
						float axisY = input->getAxis(ray::InputAxis::MouseY);

						static ray::float3 euler = ray::math::eulerAngles(_cameraComponent.lock()->getGameObject()->getQuaternion());
						euler.x += axisY;
						euler.y += axisX;

						ray::float3 center = ray::float3::Zero;
						if (_selectedObject)
						{
							auto meshComponent = _selectedObject->getComponent<ray::MeshComponent>();
							if (meshComponent)
							{
								auto boundingBox = meshComponent->getMesh()->getMeshSubsets()[_selectedSubset].boundingBox;
								boundingBox.transform(_selectedObject->getTransform());

								center = boundingBox.center();
							}
							else
							{
								center = _selectedObject->getTranslate();
							}
						}

						float distance = ray::math::distance(center, _cameraComponent.lock()->getGameObject()->getTranslate());

						ray::Quaternion q(euler);
						_cameraComponent.lock()->getGameObject()->setTranslate(center - ray::math::rotate(q, ray::float3::Forward) * distance);
						_cameraComponent.lock()->getGameObject()->setQuaternion(q);
					}
				}
			}
		}
		break;
		}
	}
	else if (message->isInstanceOf<ray::GuiMessage>())
	{
		if (_isShowWindowAll)
		{
			this->showMainMenu();
			this->showStyleEditor();
			this->showLightMass();

			this->showHierarchyWindow();
			this->showCameraWindow();
			this->showInspectorWindow();
			this->showAssetsWindow();
			this->showMaterialsWindow();
			this->showDragImageWindow();

			this->showAboutWindow();
			this->showMessage();
			this->showProcessMessage();
		}
		else
		{
			this->showSceneWindow();
		}
	}
}

void
GuiViewComponent::showMainMenu() noexcept
{
	if (!_isShowMainMenu)
		return;

	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarWindowPadding, ray::float2(_style.WindowPadding.x * 2, _style.WindowPadding.y));

	if (ray::Gui::beginMainMenuBar())
	{
		ray::float2 size = ray::Gui::getDisplaySize();
		size.y -= ray::Gui::getWindowHeight();
		ray::Gui::rootDock(ray::float2(0, ray::Gui::getWindowHeight()), size);

		ray::Gui::pushStyleColor(ray::GuiCol::GuiColBorder, ray::float4::Zero);

		if (ray::Gui::beginMenu(_langs[UILang::File].c_str()))
		{
			if (ray::Gui::menuItem(_langs[UILang::Open].c_str(), "CTRL+O", false, false)) { this->showProjectOpenBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::Save].c_str(), "CTRL+S", false, false)) { this->showProjectSaveBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::SaveAs].c_str(), "CTRL+SHIFT+S", false, false)) { this->showProjectSaveAsBrowse(); }
			ray::Gui::separator();
			ray::Gui::separator();
			if (ray::Gui::menuItem(_langs[UILang::ImportModel].c_str())) { this->showImportModelBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::ExportModel].c_str())) { this->showExportModelBrowse(); }
			ray::Gui::separator();
			ray::Gui::separator();
			if (ray::Gui::menuItem(_langs[UILang::Exit].c_str())) { std::exit(0); }
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Edit].c_str()))
		{
			if (ray::Gui::beginMenu(_langs[UILang::Language].c_str()))
			{
				if (ray::Gui::menuItem(_langs[UILang::English].c_str())) { switchLangPackage(UILang::Lang::English); }
				if (ray::Gui::menuItem(_langs[UILang::Chinese].c_str())) { switchLangPackage(UILang::Lang::Chinese); }

				ray::Gui::endMenu();
			}

			ray::Gui::separator();
			ray::Gui::menuItem(_langs[UILang::StyleEditor].c_str(), 0, &_isShowStyleEditor);

			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::GameObject].c_str()))
		{
			if (ray::Gui::menuItem(_langs[UILang::CreateProbe].c_str())) { this->createLightProbe(); }
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Window].c_str()))
		{
			ray::Gui::menuItem(_langs[UILang::Assert].c_str(), 0, &_isShowAssertWindow);
			ray::Gui::menuItem(_langs[UILang::Camera].c_str(), 0, &_isShowCameraWindow);
			ray::Gui::menuItem(_langs[UILang::Inspector].c_str(), 0, &_isShowInspectorWindow);
			ray::Gui::menuItem(_langs[UILang::LightMass].c_str(), 0, &_isShowLightMassWindow);
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Help].c_str()))
		{
			ray::Gui::menuItem(_langs[UILang::About].c_str(), 0, &_isShowAboutWindowFirst);
			ray::Gui::endMenu();
		}

		ray::Gui::popStyleColor();

		ray::Gui::endMainMenuBar();
	}

	ray::Gui::popStyleVar();
}

bool
GuiViewComponent::showFileOpenBrowse(ray::util::string::pointer filebuffer, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept
{
	assert(filebuffer && max_length > 0 && ext_name);

	nfdchar_t* outpath = nullptr;

	try
	{
		nfdresult_t  result = NFD_OpenDialog(ext_name, nullptr, &outpath);
		if (result != NFD_OKAY)
			return false;

		if (outpath)
		{
			ray::util::strncpy(filebuffer, outpath, max_length);
			free(outpath);

			return true;
		}

		return false;
	}
	catch (...)
	{
		if (outpath) free(outpath);
		return false;
	}
}

bool
GuiViewComponent::showFileOpenBrowse(std::vector<ray::util::string>& paths, ray::util::string::const_pointer ext_name) noexcept
{
	nfdpathset_t outpaths;
	std::memset(&outpaths, 0, sizeof(outpaths));

	try
	{
		nfdresult_t  result = NFD_OpenDialogMultiple(ext_name, nullptr, &outpaths);
		if (result != NFD_OKAY)
			return false;

		if (outpaths.count)
		{
			paths.resize(outpaths.count);

			for (std::size_t i = 0; i < outpaths.count; i++)
				paths[i].assign(outpaths.buf + outpaths.indices[i]);

			free(outpaths.buf);
			free(outpaths.indices);
			return true;
		}

		return false;
	}
	catch (...)
	{
		if (outpaths.buf) free(outpaths.buf);
		if (outpaths.indices) free(outpaths.indices);

		return false;
	}
}

bool
GuiViewComponent::showFileSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept
{
	assert(path && max_length > 0 && ext_name);

	nfdchar_t* outpath = nullptr;

	try
	{
		nfdresult_t  result = NFD_SaveDialog(ext_name, nullptr, &outpath);
		if (result != NFD_OKAY)
			return false;

		if (outpath)
		{
			ray::util::strncpy(path, outpath, max_length);
			free(outpath);

			return true;
		}

		return false;
	}
	catch (...)
	{
		if (outpath) free(outpath);

		return false;
	}
}

bool
GuiViewComponent::showFolderSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length) noexcept
{
	assert(path && max_length > 0);

	nfdchar_t* outpath = nullptr;

	try
	{
		nfdresult_t result = NFD_PickFolder(nullptr, &outpath);
		if (result != NFD_OKAY)
			return false;

		if (outpath)
		{
			ray::util::strncpy(path, outpath, max_length);
			free(outpath);

			auto length = std::strlen(path) - 1;
			if (length > 0 && (length + 2) < PATHLIMIT)
			{
				if (!ray::util::isSeparator(path[length]))
				{
					path[length + 1] = SEPARATOR;
					path[length + 2] = '\0';
				}
			}

			return true;
		}

		return false;
	}
	catch (...)
	{
		if (outpath) free(outpath);
		return false;
	}
}

void
GuiViewComponent::showProjectOpenBrowse() noexcept
{
	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
		return;

	if (_event.onProjectOpen)
	{
		ray::util::string::pointer error = nullptr;
		if (!_event.onProjectOpen(filepath, error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}
	}
}

void
GuiViewComponent::showProjectSaveBrowse() noexcept
{
	if (_event.onProjectSave)
	{
		if (_pathProject.empty())
			return;

		ray::util::string::pointer error = nullptr;
		if (!_event.onProjectSave(_pathProject.c_str(), error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}
	}
	else
	{
		static const char* errorTips = "There are not anything callback function that can be used to load project";
		this->showPopupMessage(_langs[UILang::Error].c_str(), errorTips, std::hash<const char*>{}(errorTips));
	}
}

void
GuiViewComponent::showProjectSaveAsBrowse() noexcept
{
	if (_event.onProjectOpen)
	{
		ray::util::string::value_type filepath[PATHLIMIT];
		std::memset(filepath, 0, sizeof(filepath));

		if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
			return;

		if (std::strlen(filepath) < (PATHLIMIT - 5))
		{
			if (std::strstr(filepath, ".map") == 0)
				std::strcat(filepath, ".map");
		}

		ray::util::string::pointer error = nullptr;
		if (_event.onProjectOpen(filepath, error))
			_pathProject = filepath;
		else
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}
	}
	else
	{
		static const char* errorTips = "There are not anything callback function that can be used to save project";
		this->showPopupMessage(_langs[UILang::Error].c_str(), errorTips, std::hash<const char*>{}(errorTips));
	}
}

void
GuiViewComponent::showImportModelBrowse() noexcept
{
	if (_event.onImportModel)
	{
		ray::util::string::value_type filepath[PATHLIMIT];
		std::memset(filepath, 0, sizeof(filepath));

		if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
			return;

		ray::util::string::pointer error = nullptr;
		if (!_event.onImportModel(filepath, error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}

		_selectedObject = nullptr;
	}
	else
	{
		static const char* errorTips = "There are not anything callback function that can be used to import models";
		this->showPopupMessage(_langs[UILang::Error].c_str(), errorTips, std::hash<const char*>{}(errorTips));
	}
}

void
GuiViewComponent::showImportAssetBrowse() noexcept
{
	static std::vector<const char*>* supportedImport[] = { &g_SupportedIES, &g_SupportedImages, &g_SupportedMaterial };
	static std::function<bool(const char*, char*&)>* delegates[] = { &_event.onImportIES, &_event.onImportTexture, &_event.onImportMaterial };

	if (_event.onImportIES || _event.onImportTexture || _event.onImportMaterial)
	{
		std::vector<std::string> filepaths;
		if (!showFileOpenBrowse(filepaths, g_SupportedFormats))
			return;

		for (auto& it : filepaths)
		{
			ray::util::string::value_type name[PATHLIMIT];
			if (ray::util::ext_name(it.c_str(), name, sizeof(name)) == 0)
				continue;

			bool loaded = false;

			for (std::size_t i = 0; i < sizeof(delegates) / sizeof(delegates[0]) && !loaded; i++)
			{
				for (const auto& format : *supportedImport[i])
				{
					if (ray::util::strcmp(name, format) != 0)
						continue;

					if (!*delegates[i])
						continue;

					ray::util::string::pointer error = nullptr;
					if (!(*delegates[i])(it.c_str(), error))
					{
						if (error)
							this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
					}

					loaded = true;
					break;
				}
			}
		}

		static const char* succeededTips = "Resource loaded.";
		this->showPopupMessage(_langs[UILang::Succeeded].c_str(), succeededTips, std::hash<const char*>{}(succeededTips));
	}
	else
	{
		static const char* errorTips = "There are not anything callback function that can be used to import resources";
		this->showPopupMessage(_langs[UILang::Error].c_str(), errorTips, std::hash<const char*>{}(errorTips));
	}
}

void
GuiViewComponent::showExportModelBrowse() noexcept
{
	if (!_event.onExportModel)
		return;

	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
		return;

	if (std::strlen(filepath) < (PATHLIMIT - 5))
	{
		if (std::strstr(filepath, ".pmx") == 0)
			std::strcat(filepath, ".pmx");
	}

	ray::util::string::pointer error = nullptr;
	if (!_event.onExportModel(filepath, 0, error))
	{
		if (error)
			this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
	}
	else
	{
		this->showPopupMessage(_langs[UILang::OK].c_str(), _langs[UILang::Succeeded].c_str(), std::hash<const char*>{}(error));
	}
}

void
GuiViewComponent::showExportAssetBrowse() noexcept
{
	assert(_event.onExportTexture);

	if (_selectedTextures.end() == std::find_if(_selectedTextures.begin(), _selectedTextures.end(), [](std::uint8_t i) { return i > 0; }))
	{
		this->showPopupMessage(_langs[UILang::Error].c_str(), "No data was selected", std::hash<const char*>{}("No data was selected"));
		return;
	}

	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFolderSaveBrowse(filepath, PATHLIMIT))
		return;

	for (std::size_t i = 0; i < _selectedTextures.size(); i++)
	{
		auto selected = _selectedTextures[i];
		if (!selected)
			continue;

		ray::util::string::pointer error = nullptr;
		if (!_event.onExportTexture(filepath, i, error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}
		else
		{
			this->showPopupMessage(_langs[UILang::OK].c_str(), _langs[UILang::Succeeded].c_str(), std::hash<const char*>{}(error));
		}
	}
}

void
GuiViewComponent::showExportMaterialBrowse() noexcept
{
	assert(_event.onExportMaterial);

	if (_selectedMaterials.end() == std::find_if(_selectedMaterials.begin(), _selectedMaterials.end(), [](std::uint8_t i) { return i > 0; }))
	{
		this->showPopupMessage(_langs[UILang::Error].c_str(), "No data was selected", std::hash<const char*>{}("No data was selected"));
		return;
	}

	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFolderSaveBrowse(filepath, PATHLIMIT))
		return;

	for (std::size_t i = 0; i < _selectedMaterials.size(); i++)
	{
		auto selected = _selectedMaterials[i];
		if (!selected)
			continue;

		ray::util::string::pointer error = nullptr;
		if (!_event.onExportMaterial(filepath, i, error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));
		}
		else
		{
			this->showPopupMessage(_langs[UILang::OK].c_str(), _langs[UILang::Succeeded].c_str(), std::hash<const char*>{}(error));
		}
	}
}

void
GuiViewComponent::showMessage() noexcept
{
	if (_isShowMessageFirst)
	{
		_isShowMessageFirst = false;

		if (_ignoreMessage[_messageHash])
			return;

		ray::Gui::openPopup(_messageTitle.c_str());

		_isShowMessage = true;
	}

	if (!_isShowMessage)
		return;

	if (ray::Gui::beginPopupModal(_messageTitle.c_str(), &_isShowMessage, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysAutoResizeBit | ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit))
	{
		ray::Gui::textUnformatted(_messageText.c_str());
		ray::Gui::separator();

		ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarFramePadding, ray::float2::Zero);
		ray::Gui::checkbox(_langs[UILang::NoShowAgain].c_str(), &_ignoreMessage[_messageHash]);
		ray::Gui::popStyleVar();

		if (ray::Gui::button(_langs[UILang::OK].c_str(), ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); _isShowMessage = false; }
		ray::Gui::sameLine();

		if (ray::Gui::button(_langs[UILang::Cancel].c_str(), ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); _isShowMessage = false; }

		if (ray::Gui::isKeyDown(ray::InputKey::Enter) || ray::Gui::isKeyDown(ray::InputKey::KP_Enter))
		{
			_isShowMessage = false;
			ray::Gui::closeCurrentPopup();
		}

		ray::Gui::endPopup();
	}
}

void
GuiViewComponent::showPopupMessage(const ray::util::string& title, const ray::util::string& message, std::size_t hash) noexcept
{
	if (!_isShowMessageFirst)
	{
		_messageHash = hash;
		_messageText = message;
		_messageTitle = title;

		_isShowMessageFirst = true;
	}
}

void
GuiViewComponent::showProcessMessage() noexcept
{
	assert(_lightMassType == LightMassType::UVMapper || _lightMassType == LightMassType::LightBaking);

	if (_isShowProcessMessageFirst)
	{
		ray::Gui::openPopup(_langs[UILang::Process].c_str());
		_isShowProcessMessage = true;
		_isShowProcessMessageFirst = false;
	}

	if (!_isShowProcessMessage)
		return;

	if (ray::Gui::beginPopupModal(_langs[UILang::Process].c_str(), &_isShowProcessMessage, ray::GuiWindowFlagBits::GuiWindowFlagNoTitleBarBit | ray::GuiWindowFlagBits::GuiWindowFlagNoMoveBit | ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit | ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit))
	{
		ray::Gui::setWindowSize(ray::float2(ray::Gui::getDisplaySize().x / 3, 90));
		ray::Gui::progressBar(_progress);
		ray::Gui::textUnformatted("");
		ray::Gui::textUnformatted("");
		ray::Gui::sameLine((ray::Gui::getWindowWidth() - 100) / 2, 0.0);

		if (_lightMassType == LightMassType::UVMapper)
		{
			ray::util::string::pointer error = nullptr;
			if (!_event.onUVMapperProcess || !_event.onUVMapperProcess(_setting, _progress, error))
			{
				ray::Gui::closeCurrentPopup();
				if (error)
					this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));

				_isShowProcessMessage = false;
			}

			if (ray::Gui::button(_langs[UILang::Cancel].c_str(), ray::float2(100, 25)))
			{
				ray::Gui::closeCurrentPopup();

				if (_event.onUVMapperCancel)
					_event.onUVMapperCancel();

				_isShowProcessMessage = false;
			}
		}
		else if (_lightMassType == LightMassType::LightBaking)
		{
			ray::util::string::pointer error = nullptr;
			if (!_event.onLightMassProcess || !_event.onLightMassProcess(_setting, _progress, error))
			{
				ray::Gui::closeCurrentPopup();
				if (error)
					this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}(error));

				_isShowProcessMessage = false;
			}

			if (ray::Gui::button(_langs[UILang::Cancel].c_str(), ray::float2(100, 25)))
			{
				ray::Gui::closeCurrentPopup();

				if (_event.onLightMassCancel)
					_event.onLightMassCancel();

				_isShowProcessMessage = false;
			}
		}

		ray::Gui::endPopup();
	}
}

void
GuiViewComponent::showAboutWindow() noexcept
{
	if (_isShowAboutWindowFirst)
	{
		ray::Gui::openPopup("About");

		_isShowAboutWindow = true;
		_isShowAboutWindowFirst = false;
	}

	if (!_isShowAboutWindow)
		return;

	if (ray::Gui::beginPopupModal(_langs[UILang::About].c_str(), &_isShowAboutWindow, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysAutoResizeBit | ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit))
	{
		ray::Gui::textUnformatted("Ray Studio Ver.0.1 beta");
		ray::Gui::textUnformatted("Developer by : Rui (https://twitter.com/Rui_cg)");
		ray::Gui::textUnformatted("Licensed under the BSD 3-clause.");
		ray::Gui::textUnformatted("Copyright (c) 2017-2018. All rights reserved.");

		ray::Gui::pushStyleColor(ray::GuiCol::GuiColBorder, ray::float4::Zero);
		ray::Gui::separator();
		ray::Gui::separator();
		ray::Gui::popStyleColor();

		ray::Gui::sameLine(ray::Gui::getWindowWidth() - 130);
		if (ray::Gui::button(_langs[UILang::OK].c_str(), ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); _isShowAboutWindow = false; }

		ray::Gui::endPopup();
	}
}

void
GuiViewComponent::showStyleEditor() noexcept
{
	if (!_isShowStyleEditor)
		return;

	if (ray::Gui::begin(_langs[UILang::StyleEditor].c_str(), &_isShowStyleEditor, ray::float2(550, 500), -1.0f, ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit))
	{
		if (ray::Gui::button("Revert Style"))
			_style = _styleDefault;

		ray::Gui::sameLine();

		if (ray::Gui::button("Save Style"))
			_styleDefault = _style;

		ray::Gui::pushItemWidth(ray::Gui::getWindowWidth() * 0.55f);

		if (ray::Gui::treeNode("Rendering"))
		{
			ray::Gui::checkbox("Anti-aliased lines", &_style.AntiAliasedLines);
			ray::Gui::checkbox("Anti-aliased shapes", &_style.AntiAliasedShapes);
			ray::Gui::pushItemWidth(100);
			ray::Gui::dragFloat("Curve Tessellation Tolerance", &_style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
			if (_style.CurveTessellationTol < 0.0f) _style.CurveTessellationTol = 0.10f;
			ray::Gui::dragFloat("Global Alpha", &_style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
			ray::Gui::popItemWidth();
			ray::Gui::treePop();
		}

		if (ray::Gui::treeNode("Settings"))
		{
			ray::Gui::sliderFloat2("WindowPadding", (float*)&_style.WindowPadding, 0.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat("WindowRounding", &_style.WindowRounding, 0.0f, 16.0f, "%.0f");
			ray::Gui::sliderFloat("ChildWindowRounding", &_style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
			ray::Gui::sliderFloat2("FramePadding", (float*)&_style.FramePadding, 0.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat("FrameRounding", &_style.FrameRounding, 0.0f, 16.0f, "%.0f");
			ray::Gui::sliderFloat2("ItemSpacing", (float*)&_style.ItemSpacing, 0.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat2("ItemInnerSpacing", (float*)&_style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat2("TouchExtraPadding", (float*)&_style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
			ray::Gui::sliderFloat("IndentSpacing", &_style.IndentSpacing, 0.0f, 30.0f, "%.0f");
			ray::Gui::sliderFloat("ScrollbarSize", &_style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat("ScrollbarRounding", &_style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
			ray::Gui::sliderFloat("GrabMinSize", &_style.GrabMinSize, 1.0f, 20.0f, "%.0f");
			ray::Gui::sliderFloat("GrabRounding", &_style.GrabRounding, 0.0f, 16.0f, "%.0f");
			ray::Gui::textUnformatted("Alignment");
			ray::Gui::sliderFloat2("WindowTitleAlign", (float*)&_style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
			ray::Gui::sliderFloat2("ButtonTextAlign", (float*)&_style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
			ray::Gui::sameLine();
			ray::Gui::helpMarker("(?)", "Alignment applies when a button is larger than its text content.");
			ray::Gui::treePop();
		}

		if (ray::Gui::treeNode("Colors"))
		{
			static int edit_mode = (int)ray::GuiColorEditMode::GuiColorEditModeRGB;
			ray::Gui::radioButton("RGB", &edit_mode, (int)ray::GuiColorEditMode::GuiColorEditModeRGB);
			ray::Gui::sameLine();
			ray::Gui::radioButton("HSV", &edit_mode, (int)ray::GuiColorEditMode::GuiColorEditModeHSV);
			ray::Gui::sameLine();
			ray::Gui::radioButton("HEX", &edit_mode, (int)ray::GuiColorEditMode::GuiColorEditModeHEX);

			if (ray::Gui::beginChild("#colors", ray::float2(0, 300), true, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysVerticalScrollbarBit | ray::GuiWindowFlagBits::GuiWindowFlagNoCollapseBit))
			{
				ray::Gui::pushItemWidth(-175);
				ray::Gui::colorEditMode((ray::GuiColorEditMode)edit_mode);

				for (std::uint8_t i = 0; i < (std::uint8_t)ray::GuiCol::GuiColCOUNT; i++)
				{
					const char* name = ray::Gui::getStyleColName((ray::GuiCol)i);

					ray::Gui::pushID(i);
					ray::Gui::colorEdit4(name, (float*)&_style.Colors[i], true);

					if (_style.Colors[i] != _styleDefault.Colors[i])
					{
						ray::Gui::sameLine();

						if (ray::Gui::button(_langs[UILang::Revert].c_str()))
							_style.Colors[i] = _styleDefault.Colors[i];
					}

					ray::Gui::popID();
				}

				ray::Gui::popItemWidth();
				ray::Gui::endChild();
			}

			ray::Gui::treePop();
		}

		ray::Gui::popItemWidth();

		ray::Gui::end();
		ray::Gui::setStyle(_style);
	}
}

void
GuiViewComponent::showHierarchyWindow() noexcept
{
	assert(_event.onFetchCamera);
	assert(_event.onFetchMeshes);
	assert(_event.onSeletedMesh);

	if (!_isShowHierarchyWindow)
		return;

	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarFramePadding, ray::float2(_style.FramePadding.x * 2.0f, _style.FramePadding.y));
	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarIndentSpacing, _style.IndentSpacing * 1.5f);

	if (ray::Gui::beginDock("Hierarchy", &_isShowHierarchyWindow))
	{
		if (ray::Gui::treeNodeEx(_langs[UILang::Camera].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
		{
			const ray::GameObjects* objects = nullptr;
			_event.onFetchCamera(objects);

			if (objects)
			{
				for (std::size_t i = 0; i < objects->size(); i++)
				{
					auto& name = (*objects)[i]->getName();
					char buffer[MAX_PATH];
					sprintf_s(buffer, MAX_PATH, "|-%s", name.empty() ? "empty" : name.c_str());

					if (ray::Gui::selectable(buffer, _selectedObject == (*objects)[i].get() ? true : false))
					{
						_event.onSeletedCamera((*objects)[i].get());
						_selectedObject = (*objects)[i].get();
					}
				}
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNode(_langs[UILang::Lights].c_str()))
		{
			const ray::GameObjects* objects = nullptr;
			_event.onFetchLights(objects);

			if (objects)
			{
				for (std::size_t i = 0; i < objects->size(); i++)
				{
					auto& name = (*objects)[i]->getName();
					char buffer[MAX_PATH];
					sprintf_s(buffer, MAX_PATH, "|-%s", name.empty() ? "empty" : name.c_str());

					if (ray::Gui::selectable(buffer, _selectedObject == (*objects)[i].get() ? true : false))
					{
						_event.onSeletedCamera((*objects)[i].get());
						_selectedObject = (*objects)[i].get();
					}
				}
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNode(_langs[UILang::LightProbes].c_str()))
		{
			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Meshes].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
		{
			const ray::GameObjects* objects = nullptr;
			_event.onFetchMeshes(objects);

			if (objects)
			{
				for (std::size_t i = 0; i < objects->size(); i++)
				{
					auto object = (*objects)[i].get();

					auto meshRender = object->getComponent<ray::MeshRenderComponent>();
					if (!meshRender)
						continue;

					auto& name = object->getName();
					char objectName[MAX_PATH];
					if (name.empty())
						std::sprintf(objectName, "|-mesh%zu", i);
					else
						std::sprintf(objectName, "|-%s", name.c_str());

					const auto& materials = meshRender->getMaterials();
					if (materials.size() == 1)
					{
						if (ray::Gui::selectable(objectName, _selectedObject == object ? true : false))
						{
							_selectedSubset = 0;
							_selectedObject = object;
							_event.onSeletedMesh(_selectedObject, _selectedSubset);
						}
					}
					else
					{
						ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarFramePadding, ray::float2::Zero);
						ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarIndentSpacing, _style.IndentSpacing * 0.6f);

						if (ray::Gui::treeNode(objectName))
						{
							for (std::size_t j = 0; j < materials.size(); j++)
							{
								char buffer[MAX_PATH];
								if (materials[j]->getName().empty())
									std::sprintf(buffer, "|-subset%zu", j);
								else
									std::sprintf(buffer, "|-%s", materials[j]->getName().c_str());

								if (ray::Gui::selectable(buffer, (_selectedObject == object && _selectedSubset == j) ? true : false))
								{
									_selectedSubset = j;
									_selectedObject = object;
									_event.onSeletedMesh(_selectedObject, _selectedSubset);
								}
							}

							ray::Gui::treePop();
						}

						ray::Gui::popStyleVar(2);
					}
				}
			}

			ray::Gui::treePop();
		}

		ray::Gui::endDock();
	}

	ray::Gui::popStyleVar(2);
}

void
GuiViewComponent::showAssetsWindow() noexcept
{
	assert(_event.onFetchTextures);

	if (!_isShowAssertWindow)
		return;

	if (ray::Gui::beginDock("Assets", &_isShowAssertWindow))
	{
		ray::Gui::textUnformatted("", "");
		ray::Gui::sameLine();
		if (ray::Gui::button(_langs[UILang::Import].c_str())) { this->showImportAssetBrowse(); }
		ray::Gui::sameLine();
		if (ray::Gui::button(_langs[UILang::Export].c_str())) { this->showExportAssetBrowse(); }

		ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, ray::float4::Zero);
		ray::Gui::textUnformatted("", "");
		ray::Gui::sameLine();

		const EditorAssetItems* textures;
		if (_event.onFetchTextures(textures))
		{
			if (textures->size() != _selectedTextures.size())
				_selectedTextures.resize(textures->size());

			for (std::size_t i = 0; i < textures->size(); i++)
			{
				auto& texture = (*textures)[i];
				if (!texture->preview)
					continue;

				std::uint32_t width = texture->preview->getGraphicsTextureDesc().getWidth();
				std::uint32_t height = texture->preview->getGraphicsTextureDesc().getHeight();

				ray::float2 imageSize = _assetImageSize * ray::float2((float)width / height, 1.0);

				if (ray::Gui::getContentRegionAvailWidth() < imageSize.x)
				{
					ray::Gui::newLine();
					ray::Gui::textUnformatted("");
					ray::Gui::sameLine();
				}

				if (ray::Gui::imageButtonAndLabel(texture->name.c_str(), texture->preview.get(), imageSize, true, _selectedTextures[i], ray::float2::Zero, ray::float2::One, (int)_style.ItemInnerSpacing.x))
				{
					if (ray::Gui::isKeyDown(ray::InputKey::LeftControl))
						_selectedTextures[i] ^= true;
					else if (ray::Gui::isKeyDown(ray::InputKey::LeftShift))
					{
						_selectedTextures[i] = true;

						if (ray::Gui::isKeyDown(ray::InputKey::LeftShift))
						{
							if (_selectedShift > _selectedTextures.size())
							{
								std::memset(_selectedTextures.data(), 0, _selectedTextures.size());
								_selectedShift = i;
								_selectedTextures[i] = true;
							}
							else
							{
								auto min = std::min(_selectedShift, i);
								auto max = std::max(_selectedShift, i);
								std::memset(_selectedTextures.data() + min, true, max - min);
							}
						}
					}
					else
					{
						_selectedItem = texture;
					}
				}

				if (!ray::Gui::isKeyDown(ray::InputKey::LeftControl) && !ray::Gui::isKeyDown(ray::InputKey::LeftShift))
				{
					if (!ray::Gui::isKeyDown(ray::InputKey::LeftShift))
						_selectedShift = std::numeric_limits<std::size_t>::max();

					if (ray::Gui::isMouseDown(ray::InputButton::RIGHT) || ray::Gui::isMouseDown(ray::InputButton::MIDDLE))
						std::memset(_selectedTextures.data(), false, _selectedTextures.size());
				}

				ray::Gui::sameLine(0, _style.ItemSpacing.y);
			}
		}

		ray::Gui::popStyleColor();

		ray::Gui::endDock();
	}
}

void
GuiViewComponent::showMaterialsWindow() noexcept
{
	assert(_event.onFetchMaterials);

	if (!_isShowMaterialWindow)
		return;

	if (ray::Gui::beginDock("Materials", &_isShowMaterialWindow))
	{
		ray::Gui::textUnformatted("", "");
		ray::Gui::sameLine();
		if (ray::Gui::button(_langs[UILang::Import].c_str())) { this->showImportAssetBrowse(); }
		ray::Gui::sameLine();
		if (ray::Gui::button(_langs[UILang::Export].c_str())) { this->showExportMaterialBrowse(); }

		ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, ray::float4::Zero);
		ray::Gui::textUnformatted("", "");
		ray::Gui::sameLine();

		const EditorAssetItems* _materials;
		if (_event.onFetchMaterials(_materials))
		{
			if (_materials->size() != _selectedMaterials.size())
				_selectedMaterials.resize(_materials->size());

			for (std::size_t i = 0; i < _materials->size(); i++)
			{
				auto& material = (*_materials)[i];

				if (ray::Gui::getContentRegionAvailWidth() < _assetImageSize.x)
				{
					ray::Gui::newLine();
					ray::Gui::textUnformatted("");
					ray::Gui::sameLine();
				}

				if (ray::Gui::imageButtonAndLabel(material->name.c_str(), material->preview.get(), _assetImageSize, true, _selectedMaterials[i], ray::float2::Zero, ray::float2::One, (int)_style.ItemInnerSpacing.x))
				{
					if (ray::Gui::isKeyDown(ray::InputKey::LeftControl))
						_selectedMaterials[i] ^= true;
					else if (ray::Gui::isKeyDown(ray::InputKey::LeftShift))
					{
						_selectedMaterials[i] = true;

						if (ray::Gui::isKeyDown(ray::InputKey::LeftShift))
						{
							if (_selectedShift > _selectedMaterials.size())
							{
								std::memset(_selectedMaterials.data(), 0, _selectedMaterials.size());
								_selectedShift = i;
								_selectedMaterials[i] = true;
							}
							else
							{
								auto min = std::min(_selectedShift, i);
								auto max = std::max(_selectedShift, i);
								std::memset(_selectedMaterials.data() + min, true, max - min);
							}
						}
					}
					else
					{
						_selectedItem = material;
					}
				}

				ray::Gui::sameLine(0, _style.ItemSpacing.y);
			}

			if (!ray::Gui::isKeyDown(ray::InputKey::LeftControl) && !ray::Gui::isKeyDown(ray::InputKey::LeftShift))
			{
				if (!ray::Gui::isKeyDown(ray::InputKey::LeftShift))
					_selectedShift = std::numeric_limits<std::size_t>::max();

				if (ray::Gui::isMouseDown(ray::InputButton::RIGHT) || ray::Gui::isMouseDown(ray::InputButton::MIDDLE))
					std::memset(_selectedMaterials.data(), false, _selectedMaterials.size());
			}
		}

		ray::Gui::popStyleColor();

		ray::Gui::endDock();
	}
}

void
GuiViewComponent::showCameraWindow() noexcept
{
	auto cameraComponent = _cameraComponent.lock();
	if (!cameraComponent)
		_cameraComponent = cameraComponent = this->getGameObject()->getComponentInChildren<ray::CameraComponent>();

	if (ray::Gui::beginDock("Camera", &_isShowCameraWindow, ray::GuiWindowFlagAlwaysUseWindowPaddingBit | ray::GuiWindowFlagNoScrollWithMouseBit))
	{
		ray::Gui::setScrollY(_style.WindowPadding.y);

		_viewport = ray::float4(ray::Gui::getWindowPos() + _style.WindowPadding, ray::Gui::getWindowSize());

		auto texture = cameraComponent->getRenderPipelineFramebuffer()->getFramebuffer()->getGraphicsFramebufferDesc().getColorAttachment().getBindingTexture();
		if (texture)
			ray::Gui::image(texture.get(), _viewport.zw(), ray::float2::UnitY, ray::float2::UnitX);

		bool mouseHoveringCamera = ray::Gui::isMouseHoveringRect(_viewport.xy(), _viewport.xy() + _viewport.zw());
		if (_isMouseHoveringCameraDock != mouseHoveringCamera)
		{
			_event.onMouseHoveringCamera(_viewport, mouseHoveringCamera);
			_isMouseHoveringCameraDock = mouseHoveringCamera;
		}

		ray::Gui::endDock();
	}
}

void
GuiViewComponent::showSceneWindow() noexcept
{
	auto cameraComponent = _cameraComponent.lock();
	if (!cameraComponent)
		_cameraComponent = cameraComponent = this->getGameObject()->getComponentInChildren<ray::CameraComponent>();

	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarWindowPadding, ray::float2::Zero);

	ray::GuiWindowFlags flags =
		ray::GuiWindowFlagBits::GuiWindowFlagNoInputsBit |
		ray::GuiWindowFlagBits::GuiWindowFlagNoTitleBarBit |
		ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit |
		ray::GuiWindowFlagBits::GuiWindowFlagNoScrollbarBit |
		ray::GuiWindowFlagBits::GuiWindowFlagNoScrollWithMouseBit |
		ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit;

	if (ray::Gui::begin("Scene", 0, ray::Gui::getDisplaySize(), -1.0, flags))
	{
		ray::Gui::setWindowPos(ray::float2::Zero);

		auto texture = cameraComponent->getRenderPipelineFramebuffer()->getFramebuffer()->getGraphicsFramebufferDesc().getColorAttachment().getBindingTexture();
		if (texture)
			ray::Gui::image(texture.get(), ray::Gui::getWindowSize(), ray::float2::UnitY, ray::float2::UnitX);
		ray::Gui::end();
	}

	ray::Gui::popStyleVar();
}

void
GuiViewComponent::showInspectorWindow() noexcept
{
	if (!_isShowInspectorWindow)
		return;

	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarFramePadding, ray::float2(0.0, _style.FramePadding.y));
	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarIndentSpacing, _style.IndentSpacing * 0.7f);

	if (ray::Gui::beginDock("Inspector", &_isShowInspectorWindow))
	{
		if (_selectedObject)
		{
			this->showEditTransformWindow(_selectedObject);

			auto& components = _selectedObject->getComponents();
			for (auto& it : components)
			{
				if (it->isInstanceOf<ray::CameraComponent>())
					this->showEditCameraWindow(it->downcast<ray::CameraComponent>());
				else if (it->isInstanceOf<ray::LightComponent>())
					this->showEditLightWindow(it->downcast<ray::LightComponent>());
				else if (it->isInstanceOf<ray::SkyboxComponent>())
					this->showEditSkyboxWindow(it->downcast<ray::SkyboxComponent>());
				else if (it->isInstanceOf<ray::MeshRenderComponent>())
				{
					const auto& materials = it->downcast<ray::MeshRenderComponent>()->getMaterials();
					if (!materials.empty())
					{
						if (_selectedObject != _selectedObjectLast || _selectedSubset != _selectedSubsetLast)
						{
							auto material = (materials.size() > _selectedSubset) ? materials[_selectedSubset] : materials.front();

							const EditorAssetItems* _materials = nullptr;
							if (_event.onFetchMaterials(_materials))
							{
								auto it = std::find_if(_materials->begin(), _materials->end(), [material](const EditorAssetItemPtr& item)
								{
									return std::get<EditorAssetItem::material>(item->value) == material;
								});

								if (it != _materials->end())
								{
									_selectedMaterial = *it;

									if (_selectedMaterial != _selectedMaterialLast)
									{
										_event.onUpdateMaterial(*_selectedMaterial);
										_selectedMaterialLast = _selectedMaterial;
									}

									if (_selectedMaterial)
										this->showEditMaterialWindow(*_selectedMaterial);
								}
							}

							_selectedObjectLast = _selectedObject;
							_selectedSubsetLast = _selectedSubset;
						}
						else
						{
							if (_selectedMaterial)
								this->showEditMaterialWindow(*_selectedMaterial);
						}
					}
				}
			}
		}

		ray::Gui::endDock();
	}

	ray::Gui::popStyleVar(2);
}

void
GuiViewComponent::showDragImageWindow() noexcept
{
	if (_selectedItem)
	{
		if (ray::Gui::isMouseDown(ray::InputButton::RIGHT) || ray::Gui::isMouseDown(ray::InputButton::MIDDLE))
		{
			_selectedItem = nullptr;
		}
		else
		{
			ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarWindowPadding, ray::float2::Zero);
			ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarWindowRounding, 0.0f);
			ray::Gui::pushStyleColor(ray::GuiCol::GuiColWindowBg, ray::float4::Zero);

			ray::GuiWindowFlags flags =
				ray::GuiWindowFlagBits::GuiWindowFlagNoInputsBit |
				ray::GuiWindowFlagBits::GuiWindowFlagNoTitleBarBit |
				ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit |
				ray::GuiWindowFlagBits::GuiWindowFlagNoScrollbarBit |
				ray::GuiWindowFlagBits::GuiWindowFlagNoScrollWithMouseBit |
				ray::GuiWindowFlagBits::GuiWindowFlagNoSavedSettingsBit;

			if (ray::Gui::begin("##floating", 0, ray::float2::Zero, -1.0f, flags))
			{
				std::uint32_t width = _selectedItem->preview->getGraphicsTextureDesc().getWidth();
				std::uint32_t height = _selectedItem->preview->getGraphicsTextureDesc().getHeight();

				ray::float2 aspert = ray::float2((float)width / height, 1.0f);

				ray::Gui::setWindowSize(_assetImageSize * aspert);
				ray::Gui::setWindowPos(ray::Gui::getMousePos() - _assetImageSize * aspert * 0.5f);
				ray::Gui::image(_selectedItem->preview.get(), _assetImageSize * aspert, ray::float2::Zero, ray::float2::One, ray::float4(0.7f));

				ray::Gui::end();
			}

			ray::Gui::popStyleVar(2);
			ray::Gui::popStyleColor();
		}
	}
}

void
GuiViewComponent::showEditTransformWindow(ray::GameObject* object) noexcept
{
	if (!object)
		return;

	if (ray::Gui::treeNodeEx(_langs[UILang::Transform].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
	{
		auto translate = object->getTranslate();
		auto rotation = ray::math::eulerAngles(object->getQuaternion());
		auto rotationCache = rotation;
		auto scaling = object->getScale();

		ray::Gui::textUnformatted(_langs[UILang::Position].c_str(), _langs[UILang::Position].c_str() + _langs[UILang::Position].size());
		ray::Gui::sameLine(75.0f);
		ray::Gui::pushItemWidth(-1);
		if (ray::Gui::dragFloat3("##Position", translate.ptr(), 0.1f, -FLT_MAX, FLT_MAX))
		{
			object->setTranslate(translate);
			_event.onTransformObject(object, 0);
		}

		ray::Gui::popItemWidth();

		ray::Gui::textUnformatted(_langs[UILang::Rotation].c_str(), _langs[UILang::Rotation].c_str() + _langs[UILang::Rotation].size());
		ray::Gui::sameLine(75.0f);
		ray::Gui::pushItemWidth(-1);
		if (ray::Gui::dragFloat3("##Rotation", rotation.ptr(), 0.1f, -360, 360))
		{
			if (rotation.x > 90.0f) rotation.x = rotation.x - 180.0f;
			if (rotation.y > 180.0f) rotation.y = rotation.y - 360.0f;
			if (rotation.z > 180.0f) rotation.z = rotation.z - 360.0f;

			if (rotation.x < -90.0f) rotation.x = rotation.x + 180.0f;
			if (rotation.y < -180.0f) rotation.y = rotation.y + 360.0f;
			if (rotation.z < -180.0f) rotation.z = rotation.z + 360.0f;

			object->setQuaternion(ray::Quaternion(rotation));

			_event.onTransformObject(object, 0);
		}

		ray::Gui::popItemWidth();

		ray::Gui::textUnformatted(_langs[UILang::Scaling].c_str(), _langs[UILang::Scaling].c_str() + _langs[UILang::Scaling].size());
		ray::Gui::sameLine(75.0f);
		ray::Gui::pushItemWidth(-1);
		if (ray::Gui::dragFloat3("##Scale", scaling.ptr(), 0.1f, 1e-3f, FLT_MAX))
		{
			object->setScale(scaling);
			_event.onTransformObject(object, 0);
		}

		ray::Gui::popItemWidth();

		ray::Gui::treePop();
	}
}

void
GuiViewComponent::showEditCameraWindow(ray::CameraComponent* camera) noexcept
{
	if (ray::Gui::treeNodeEx("Camera", ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
	{
		float znear = camera->getNear();
		float zfar = camera->getFar();
		float fov = camera->getAperture();

		ray::Gui::textUnformatted("Field of view:");
		if (ray::Gui::dragFloat("##fov", &fov, 0.1f, 1, 125))
			camera->setAperture(fov);

		ray::Gui::textUnformatted("Near:");
		if (ray::Gui::dragFloat("##znear", &znear, 1e-3f, 1e-3, 1.0f, "%.03f"))
			camera->setNear(znear);

		ray::Gui::textUnformatted("Far:");
		if (ray::Gui::dragFloat("##zfar", &zfar, 0.1f, 0, 99999))
			camera->setFar(zfar);

		ray::Gui::treePop();
	}
}

void
GuiViewComponent::showEditLightWindow(ray::LightComponent* light) noexcept
{
	if (ray::Gui::treeNodeEx("Light", ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
	{
		float range = light->getLightRange();

		ray::Gui::textUnformatted("Range:");
		if (ray::Gui::dragFloat("##range", &range, 1.0f, 1.0f, 1000.0f, "%.03f"))
			light->setLightRange(range);

		ray::Gui::treePop();
	}
}

void
GuiViewComponent::showEditSkyboxWindow(ray::SkyboxComponent* skybox) noexcept
{
	ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, ray::float4::Zero);

	if (ray::Gui::treeNodeEx("Skybox", ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
	{
		auto skyTexture = skybox->getSkyBox();
		auto skyDiffTexture = skybox->getSkyLightDiffuse();
		auto skySpecTexture = skybox->getSkyLightSpecular();

		ray::float2 aspert = ray::float2::One;
		if (skyTexture)
		{
			std::uint32_t width = skyTexture->getGraphicsTextureDesc().getWidth();
			std::uint32_t height = skyTexture->getGraphicsTextureDesc().getHeight();
			aspert.set((float)width / height, 1.0f);
		}

		ray::Gui::textUnformatted("sky Texture (sRGB):");
		ray::Gui::imageButton(skyTexture.get(), _assetImageSize * aspert, ray::float2::Zero, ray::float2::One, (int)_style.ItemInnerSpacing.x, _style.Colors[ray::GuiCol::GuiColChildWindowBg]);

		ray::Gui::textUnformatted("sky irradiance:");
		ray::Gui::imageButton(skyDiffTexture.get(), _assetImageSize * aspert, ray::float2::Zero, ray::float2::One, (int)_style.ItemInnerSpacing.x, _style.Colors[ray::GuiCol::GuiColChildWindowBg]);

		ray::Gui::textUnformatted("sky radiance:");
		ray::Gui::imageButton(skySpecTexture.get(), _assetImageSize * aspert, ray::float2::Zero, ray::float2::One, (int)_style.ItemInnerSpacing.x, _style.Colors[ray::GuiCol::GuiColChildWindowBg]);

		ray::Gui::treePop();
	}

	ray::Gui::popStyleColor();
}

void
GuiViewComponent::showEditMaterialWindow(const EditorAssetItem& item) noexcept
{
	auto& material = *std::get<EditorAssetItem::material>(item.value);

	ray::float2 imageSize = _assetImageSize;

	if (ray::Gui::treeNodeEx(_langs[UILang::Material].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
	{
		bool isMaterialDraing = false;
		if (_selectedItem)
			isMaterialDraing = (_selectedItem->value.index() == EditorAssetItem::material);

		bool isTextureDraging = false;
		if (_selectedItem)
			isTextureDraging = (_selectedItem->value.index() == EditorAssetItem::texture);

		ray::Gui::bulletText(_langs[UILang::Summary].c_str(), material.getName().c_str());

		if (ray::Gui::imageButtonEx(item.preview.get(), _materialImageSize, "Click here update material", isMaterialDraing, true))
		{
			if (_selectedItem && isMaterialDraing)
			{
				auto& params = std::get<EditorAssetItem::material>(_selectedItem->value)->getParameters();
				for (auto& it : params)
					material[it.first]->uniformParam(*it.second);
			}

			_event.onUpdateMaterial(item);
			_selectedItem = nullptr;
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Albedo].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto albedoColor = ray::math::pow(material["albedo"]->value().getFloat3(), 1.0f / 2.2f);
			auto albedoMap = material["albedoMap"]->value().getTexture();
			auto albedoMapFrom = material["albedoMapFrom"]->value().getInt();
			auto albedoMapFlip = material["albedoMapFlip"]->value().getInt();
			auto albedoMapScaleDiffuse = false;
			auto albedoMapLoopNum = material["albedoMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##albedoMapFrom", &albedoMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["albedoMapFrom"]->uniform1i(albedoMapFrom);

			if (albedoMapFrom >= 1 && albedoMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##albedoMapFlip", &albedoMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["albedoMapFlip"]->uniform1i(albedoMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##albedoMapLoopNum", albedoMapLoopNum.ptr(), 0.05, 0.0, FLT_MAX))
					material["albedoMapLoopNum"]->uniform2f(albedoMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (albedoMap)
				{
					std::uint32_t width = albedoMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = albedoMap->getGraphicsTextureDesc().getHeight();
					aspert.set((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(albedoMap ? albedoMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["albedoMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Color_sRGB].c_str(), _langs[UILang::Color_sRGB].c_str() + _langs[UILang::Color_sRGB].size());
			if (ray::Gui::colorPicker3("##albedoColor", albedoColor.ptr()))
				material["albedo"]->uniform3f(ray::math::pow(albedoColor, 2.2f));

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::AlbedoSub].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto albedoSubType = material["albedoSubType"]->value().getInt();
			auto albedoSubColor = ray::math::pow(material["albedoSub"]->value().getFloat3(), 1.0f / 2.2f);
			auto albedoSubMap = material["albedoSubMap"]->value().getTexture();
			auto albedoSubMapFrom = material["albedoSubMapFrom"]->value().getInt();
			auto albedoSubMapFlip = material["albedoSubMapFlip"]->value().getInt();
			auto albedoSubMapLoopNum = material["albedoSubMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureType].c_str(), _langs[UILang::TextureType].c_str() + _langs[UILang::TextureType].size());
			if (ray::Gui::combo("##albedoSubType", &albedoSubType, TEXTURE_SUBALBEDO_TYPE, sizeof(TEXTURE_SUBALBEDO_TYPE) / sizeof(TEXTURE_SUBALBEDO_TYPE[0])))
				material["albedoSubType"]->uniform1i(albedoSubType);

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##albedoSubMapFrom", &albedoSubMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["albedoSubMapFrom"]->uniform1i(albedoSubMapFrom);

			if (albedoSubMapFrom >= 1 && albedoSubMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##albedoSubMapFlip", &albedoSubMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["albedoSubMapFlip"]->uniform1i(albedoSubMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##albedoSubMapLoopNum", albedoSubMapLoopNum.ptr(), 0.05, 0.0, FLT_MAX, "%.3f", 2.2f))
					material["albedoSubMapLoopNum"]->uniform2f(albedoSubMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (albedoSubMap)
				{
					std::uint32_t width = albedoSubMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = albedoSubMap->getGraphicsTextureDesc().getHeight();
					ray::float2 aspert = ray::float2((float)width / height, 1.0);
				}

				if (ray::Gui::imageButtonEx(albedoSubMap ? albedoSubMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["albedoSubMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Color_sRGB].c_str(), _langs[UILang::Color_sRGB].c_str() + _langs[UILang::Color_sRGB].size());
			if (ray::Gui::colorPicker3("##albedoSubColor", albedoSubColor.ptr()))
				material["albedoSub"]->uniform3f(ray::math::pow(albedoSubColor, 2.2f));

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Normal].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto normalMapType = 0;
			auto normalMap = material["normalMap"]->value().getTexture();
			auto normalMapFrom = material["normalMapFrom"]->value().getInt();
			auto normalMapFlip = material["normalMapFlip"]->value().getInt();
			auto normalMapScale = material["normalMapScale"]->value().getFloat();
			auto normalMapLoopNum = material["normalMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##normalMapFrom", &normalMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["normalMapFrom"]->uniform1i(normalMapFrom);

			if (normalMapFrom >= 1 && normalMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureType].c_str(), _langs[UILang::TextureType].c_str() + _langs[UILang::TextureType].size());
				if (ray::Gui::combo("##normalMapType", &normalMapType, TEXTURE_NORMAL_TYPE, sizeof(TEXTURE_NORMAL_TYPE) / sizeof(TEXTURE_NORMAL_TYPE[0])))
					material["normalMapType"]->uniform1i(normalMapType);

				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##normalMapFlip", &normalMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["normalMapFlip"]->uniform1i(normalMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##normalMapLoopNum", normalMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["normalMapLoopNum"]->uniform2f(normalMapLoopNum);

				ray::Gui::textUnformatted("Strength:");
				if (ray::Gui::dragFloat("##normalMapScale", &normalMapScale, 0.05f, -FLT_MAX, FLT_MAX))
					material["normalMapScale"]->uniform1f(normalMapScale);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (normalMap)
				{
					std::uint32_t width = normalMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = normalMap->getGraphicsTextureDesc().getHeight();
					ray::float2 aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(normalMap ? normalMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["normalMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::NormalSub].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto normalSubMapType = 0;
			auto normalSubMap = material["normalSubMap"]->value().getTexture();
			auto normalSubMapFrom = material["normalSubMapFrom"]->value().getInt();
			auto normalSubMapFlip = material["normalSubMapFlip"]->value().getInt();
			auto normalSubMapScale = material["normalSubMapScale"]->value().getFloat();
			auto normalSubMapLoopNum = material["normalSubMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##normalSubMapFrom", &normalSubMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["normalSubMapFrom"]->uniform1i(normalSubMapFrom);

			if (normalSubMapFrom >= 1 && normalSubMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureType].c_str(), _langs[UILang::TextureType].c_str() + _langs[UILang::TextureType].size());
				ray::Gui::combo("##normalSubMapType", &normalSubMapType, TEXTURE_NORMAL_TYPE, sizeof(TEXTURE_NORMAL_TYPE) / sizeof(TEXTURE_NORMAL_TYPE[0]));

				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##normalSubMapFlip", &normalSubMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["normalSubMapFlip"]->uniform1i(normalSubMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##normalSubMapLoopNum", normalSubMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["normalSubMapLoopNum"]->uniform2f(normalSubMapLoopNum);

				ray::Gui::textUnformatted("Strength:");
				if (ray::Gui::dragFloat("##normalSubMapScale", &normalSubMapScale, 0.05f, -FLT_MAX, FLT_MAX))
					material["normalSubMapScale"]->uniform1f(normalSubMapScale);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (normalSubMap)
				{
					std::uint32_t width = normalSubMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = normalSubMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(normalSubMap ? normalSubMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["normalSubMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Smoothness].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto smoothness = material["smoothness"]->value().getFloat();
			auto smoothnessMap = material["smoothnessMap"]->value().getTexture();
			auto smoothnessMapType = material["smoothnessMapType"]->value().getInt();
			auto smoothnessMapFrom = material["smoothnessMapFrom"]->value().getInt();
			auto smoothnessMapFlip = material["smoothnessMapFlip"]->value().getInt();
			auto smoothnessMapLoopNum = material["smoothnessMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##smoothnessMapFrom", &smoothnessMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["smoothnessMapFrom"]->uniform1i(smoothnessMapFrom);

			if (smoothnessMapFrom >= 1 && smoothnessMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureType].c_str(), _langs[UILang::TextureType].c_str() + _langs[UILang::TextureType].size());
				if (ray::Gui::combo("##smoothnessMapType", &smoothnessMapType, TEXTURE_SMOOTHNESS_TYPE, sizeof(TEXTURE_SMOOTHNESS_TYPE) / sizeof(TEXTURE_SMOOTHNESS_TYPE[0])))
					material["smoothnessMapType"]->uniform1i(smoothnessMapType);

				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##smoothness Flip", &smoothnessMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["smoothnessMapFlip"]->uniform1i(smoothnessMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##smoothnessMapLoopNum", smoothnessMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["smoothnessMapLoopNum"]->uniform2f(smoothnessMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::TextureSwizzle].c_str(), _langs[UILang::TextureSwizzle].c_str() + _langs[UILang::TextureSwizzle].size());
				ray::Gui::button("R", ray::float2(40, 20));

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("G", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("B", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::sameLine();
				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::button("A", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (smoothnessMap)
				{
					std::uint32_t width = smoothnessMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = smoothnessMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(smoothnessMap ? smoothnessMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["smoothnessMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Smoothness].c_str(), _langs[UILang::Smoothness].c_str() + _langs[UILang::Smoothness].size());

			if (smoothnessMapFrom >= 1 && smoothnessMapFrom <= 6)
			{
				if (ray::Gui::dragFloat("##Smoothness", &smoothness, 1e-2f, 0.0f, 10.0f))
					material["smoothness"]->uniform1f(smoothness);
			}
			else
			{
				if (ray::Gui::sliderFloat("##Smoothness", &smoothness, 0.0f, 1.0f))
					material["smoothness"]->uniform1f(smoothness);
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Metalness].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto metalness = material["metalness"]->value().getFloat();
			auto metalnessMap = material["metalnessMap"]->value().getTexture();
			auto metalnessMapFrom = material["metalnessMapFrom"]->value().getInt();
			auto metalnessMapFlip = material["metalnessMapFlip"]->value().getInt();
			auto metalnessMapLoopNum = material["metalnessMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##metalnessMapFrom", &metalnessMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["metalnessMapFrom"]->uniform1i(metalnessMapFrom);

			if (metalnessMapFrom >= 1 && metalnessMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##metalness Flip", &metalnessMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["metalnessMapFlip"]->uniform1i(metalnessMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##metalnessMapLoopNum", metalnessMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["metalnessMapLoopNum"]->uniform2f(metalnessMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::TextureSwizzle].c_str(), _langs[UILang::TextureSwizzle].c_str() + _langs[UILang::TextureSwizzle].size());
				ray::Gui::button("R", ray::float2(40, 20));

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("G", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("B", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::sameLine();
				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::button("A", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (metalnessMap)
				{
					std::uint32_t width = metalnessMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = metalnessMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(metalnessMap ? metalnessMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["metalnessMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Metalness].c_str(), _langs[UILang::Metalness].c_str() + _langs[UILang::Metalness].size());

			if (metalnessMapFrom >= 1 && metalnessMapFrom <= 6)
			{
				if (ray::Gui::dragFloat("##Metalness", &metalness, 1e-2f, 0.0f, 10.0f))
					material["metalness"]->uniform1f(metalness);
			}
			else
			{
				if (ray::Gui::sliderFloat("##Metalness", &metalness, 0.0f, 1.0f))
					material["metalness"]->uniform1f(metalness);
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Specular].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto specular = material["specular"]->value().getFloat3();
			auto specularMap = material["specularMap"]->value().getTexture();
			auto specularMapFrom = material["specularMapFrom"]->value().getInt();
			auto specularMapFlip = material["specularMapFlip"]->value().getInt();
			auto specularMapLoopNum = material["specularMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##specularMapFrom", &specularMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["specularMapFrom"]->uniform1i(specularMapFrom);

			if (specularMapFrom >= 1 && specularMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##specularMapFlip", &specularMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["specularMapFlip"]->uniform1i(specularMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##specularMapLoopNum", specularMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["specularMapLoopNum"]->uniform2f(specularMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (specularMap)
				{
					std::uint32_t width = specularMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = specularMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(specularMap ? specularMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["specularMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Color].c_str(), _langs[UILang::Color].c_str() + _langs[UILang::Color].size());
			if (ray::Gui::colorPicker3("##specular", specular.ptr()))
				material["specular"]->uniform3f(specular);

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Occlusion].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto occlusion = material["occlusion"]->value().getFloat();
			auto occlusionMap = material["occlusionMap"]->value().getTexture();
			auto occlusionMapFrom = material["occlusionMapFrom"]->value().getInt();
			auto occlusionMapFlip = material["occlusionMapFlip"]->value().getInt();
			auto occlusionMapLoopNum = material["occlusionMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##occlusionMapFrom ", &occlusionMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["occlusionMapFrom"]->uniform1i(occlusionMapFrom);

			if (occlusionMapFrom >= 1 && occlusionMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##occlusion Flip", &occlusionMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["occlusionMapFlip"]->uniform1i(occlusionMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##occlusionMapLoopNum", occlusionMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["occlusionMapLoopNum"]->uniform2f(occlusionMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::TextureSwizzle].c_str(), _langs[UILang::TextureSwizzle].c_str() + _langs[UILang::TextureSwizzle].size());
				ray::Gui::button("R", ray::float2(40, 20));

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("G", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("B", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::sameLine();
				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::button("A", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (occlusionMap)
				{
					std::uint32_t width = occlusionMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = occlusionMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(occlusionMap ? occlusionMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["occlusionMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Occlusion].c_str(), _langs[UILang::Occlusion].c_str() + _langs[UILang::Occlusion].size());

			if (occlusionMapFrom >= 1 && occlusionMapFrom <= 6)
			{
				if (ray::Gui::dragFloat("##occlusion", &occlusion, 1e-2f, 0.0f, 10.0f))
					material["occlusion"]->uniform1f(occlusion);
			}
			else
			{
				if (ray::Gui::sliderFloat("##occlusion", &occlusion, 0.0f, 1.0f))
					material["occlusion"]->uniform1f(occlusion);
			}

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::Emissive].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto emissiveIntensity = material["emissiveIntensity"]->value().getFloat();
			auto emissiveColor = ray::math::pow(material["emissive"]->value().getFloat3(), 1.0f / 2.2f);
			auto emissiveMap = material["emissiveMap"]->value().getTexture();
			auto emissiveMapFrom = material["emissiveMapFrom"]->value().getInt();
			auto emissiveMapFlip = material["emissiveMapFlip"]->value().getInt();
			auto emissiveMapLoopNum = material["emissiveMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##emissiveMapFrom", &emissiveMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["emissiveMapFrom"]->uniform1i(emissiveMapFrom);

			if (emissiveMapFrom >= 1 && emissiveMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##emissive Flip", &emissiveMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["emissiveMapFlip"]->uniform1i(emissiveMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##emissiveMapLoopNum", emissiveMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["emissiveMapLoopNum"]->uniform2f(emissiveMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (emissiveMap)
				{
					std::uint32_t width = emissiveMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = emissiveMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(emissiveMap ? emissiveMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["emissiveMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Emissive].c_str(), _langs[UILang::Emissive].c_str() + _langs[UILang::Emissive].size());
			if (ray::Gui::sliderFloat("##emissiveIntensity", &emissiveIntensity, 0.0f, 10.0f))
				material["emissiveIntensity"]->uniform1f(emissiveIntensity);

			ray::Gui::textUnformatted(_langs[UILang::Color_sRGB].c_str(), _langs[UILang::Color_sRGB].c_str() + _langs[UILang::Color_sRGB].size());
			if (ray::Gui::colorPicker3("##emissiveColor", emissiveColor.ptr()))
				material["emissive"]->uniform3f(ray::math::pow(emissiveColor, 2.2f));

			ray::Gui::treePop();
		}

		/*if (ray::Gui::treeNodeEx(_langs[UILang::ShadingModel].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagBulletBit))
		{
			auto customType = material["customType"]->value().getInt();
			auto customA = material["customA"]->value().getFloat();
			auto customAMap = material["customAMap"]->value().getTexture();
			auto customAMapFrom = material["customAMapFrom"]->value().getInt();
			auto customAMapFlip = material["customAMapFlip"]->value().getInt();
			auto customAMapLoopNum = material["customAMapLoopNum"]->value().getFloat2();
			auto customB = ray::math::pow(material["customB"]->value().getFloat3(), 1.0f / 2.2f);
			auto customBMap = material["customBMap"]->value().getTexture();
			auto customBMapFrom = material["customBMapFrom"]->value().getInt();
			auto customBMapFlip = material["customBMapFlip"]->value().getInt();
			auto customBMapLoopNum = material["customBMapLoopNum"]->value().getFloat2();

			ray::Gui::textUnformatted(_langs[UILang::ShadingModel].c_str(), _langs[UILang::ShadingModel].c_str() + _langs[UILang::ShadingModel].size());
			if (ray::Gui::combo("##customType", &customType, TEXTURE_SHADING_MODEL, sizeof(TEXTURE_SHADING_MODEL) / sizeof(TEXTURE_SHADING_MODEL[0])))
				material["customType"]->uniform1i(customType);

			ray::Gui::textUnformatted(_langs[UILang::CustomA].c_str(), _langs[UILang::CustomA].c_str() + _langs[UILang::CustomA].size());

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##customAMapFrom ", &customAMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["customAMapFrom"]->uniform1i(customAMapFrom);

			if (customAMapFrom >= 1 && customAMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##customA Flip", &customAMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["customAMapFlip"]->uniform1i(customAMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##customAMapLoopNum", customAMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["customAMapLoopNum"]->uniform2f(customAMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::TextureSwizzle].c_str(), _langs[UILang::TextureSwizzle].c_str() + _langs[UILang::TextureSwizzle].size());
				ray::Gui::button("R", ray::float2(40, 20));

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("G", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::sameLine();
				ray::Gui::button("B", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::sameLine();
				ray::Gui::pushStyleColor(ray::GuiCol::GuiColButton, _style.Colors[ray::GuiCol::GuiColTextDisabled]);
				ray::Gui::button("A", ray::float2(40, 20));
				ray::Gui::popStyleColor();

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (customAMap)
				{
					std::uint32_t width = customAMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = customAMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(customAMap ? customAMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["customAMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Occlusion].c_str(), _langs[UILang::Occlusion].c_str() + _langs[UILang::Occlusion].size());

			if (customAMapFrom >= 1 && customAMapFrom <= 6)
			{
				if (ray::Gui::dragFloat("##customA", &customA, 1e-2f, 0.0f, 10.0f))
					material["customA"]->uniform1f(customA);
			}
			else
			{
				if (ray::Gui::sliderFloat("##customA", &customA, 0.0f, 1.0f))
					material["customA"]->uniform1f(customA);
			}

			ray::Gui::textUnformatted(_langs[UILang::CustomB].c_str(), _langs[UILang::CustomB].c_str() + _langs[UILang::CustomB].size());

			ray::Gui::textUnformatted(_langs[UILang::TextureFrom].c_str(), _langs[UILang::TextureFrom].c_str() + _langs[UILang::TextureFrom].size());
			if (ray::Gui::combo("##customBMapFrom", &customBMapFrom, TEXTURE_MAP_FROM, sizeof(TEXTURE_MAP_FROM) / sizeof(TEXTURE_MAP_FROM[0])))
				material["customBMapFrom"]->uniform1i(customBMapFrom);

			if (customBMapFrom >= 1 && customBMapFrom <= 6)
			{
				ray::Gui::textUnformatted(_langs[UILang::TextureFlip].c_str(), _langs[UILang::TextureFlip].c_str() + _langs[UILang::TextureFlip].size());
				if (ray::Gui::combo("##customBMapFlip", &customBMapFlip, TEXTURE_MAP_UV_FLIP, sizeof(TEXTURE_MAP_UV_FLIP) / sizeof(TEXTURE_MAP_UV_FLIP[0])))
					material["customBMapFlip"]->uniform1i(customBMapFlip);

				ray::Gui::textUnformatted(_langs[UILang::TextureLoop].c_str(), _langs[UILang::TextureLoop].c_str() + _langs[UILang::TextureLoop].size());
				if (ray::Gui::dragFloat2("##customBMapLoopNum", customBMapLoopNum.ptr(), 0.05f, 0.0, FLT_MAX))
					material["customBMapLoopNum"]->uniform2f(customBMapLoopNum);

				ray::Gui::textUnformatted(_langs[UILang::Texture_sRGB].c_str(), _langs[UILang::Texture_sRGB].c_str() + _langs[UILang::Texture_sRGB].size());

				ray::float2 aspert = ray::float2::One;
				if (customBMap)
				{
					std::uint32_t width = customBMap->getGraphicsTextureDesc().getWidth();
					std::uint32_t height = customBMap->getGraphicsTextureDesc().getHeight();
					aspert = ray::float2((float)width / height, 1.0f);
				}

				if (ray::Gui::imageButtonEx(customBMap ? customBMap.get() : 0, imageSize * aspert, nullptr, isTextureDraging, isTextureDraging))
				{
					if (_selectedItem)
					{
						material["customBMap"]->uniformTexture(std::get<EditorAssetItem::texture>(_selectedItem->value));
						_selectedItem = nullptr;
					}
				}
			}

			ray::Gui::textUnformatted(_langs[UILang::Color_sRGB].c_str(), _langs[UILang::Color_sRGB].c_str() + _langs[UILang::Color_sRGB].size());
			if (ray::Gui::colorPicker3("##customB", customB.ptr()))
				material["customB"]->uniform3f(ray::math::pow(customB, 2.2f));

			ray::Gui::treePop();
		}*/

		ray::Gui::treePop();
	}
}

void
GuiViewComponent::showLightMass() noexcept
{
	if (!_isShowLightMassWindow)
		return;

	if (ray::Gui::beginDock("Lightmass", &_isShowLightMassWindow, ray::GuiWindowFlagBits::GuiWindowFlagNoCollapseBit))
	{
		if (ray::Gui::treeNodeEx(_langs[UILang::UvMapper].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
		{
			ray::Gui::textUnformatted(_langs[UILang::OutputUVSize].c_str(), _langs[UILang::OutputUVSize].c_str() + _langs[UILang::OutputUVSize].size());
			ray::Gui::comboWithRevert("##UV size", _langs[UILang::Revert].c_str(), &_setting.lightmass.imageSize, _default.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));

			ray::Gui::textUnformatted(_langs[UILang::OutputUVSlot].c_str(), _langs[UILang::OutputUVSlot].c_str() + _langs[UILang::OutputUVSlot].size());
			ray::Gui::comboWithRevert("##Output UV slot", _langs[UILang::Revert].c_str(), &_setting.uvmapper.slot, _default.uvmapper.slot, itemsUVSlot, sizeof(itemsUVSlot) / sizeof(itemsUVSlot[0]));

			ray::Gui::textUnformatted(_langs[UILang::UVMargin].c_str(), _langs[UILang::UVMargin].c_str() + _langs[UILang::UVMargin].size());
			ray::Gui::sliderFloatWithRevert("##margin", _langs[UILang::Revert].c_str(), &_setting.uvmapper.margin, _default.uvmapper.margin, 0.0f, 10.0f);

			ray::Gui::textUnformatted(_langs[UILang::UVStretch].c_str(), _langs[UILang::UVStretch].c_str() + _langs[UILang::UVStretch].size());
			ray::Gui::sliderFloatWithRevert("##stretch", _langs[UILang::Revert].c_str(), &_setting.uvmapper.stretch, _default.uvmapper.stretch, 0.0, 1.0, "%.5f", 2.2);

			ray::Gui::textUnformatted(_langs[UILang::UVChart].c_str(), _langs[UILang::UVChart].c_str() + _langs[UILang::UVChart].size());
			ray::Gui::sliderIntWithRevert("##chart", _langs[UILang::Revert].c_str(), &_setting.uvmapper.chart, _default.uvmapper.chart, 0, 65535);

			if (ray::Gui::button(_langs[UILang::StartUVMapper].c_str()))
				this->startUVMapper();

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNodeEx(_langs[UILang::LightMass].c_str(), ray::GuiTreeNodeFlagBits::GuiTreeNodeFlagDefaultOpenBit))
		{
			ray::Gui::checkbox(_langs[UILang::EnableGI].c_str(), &_setting.lightmass.enableGI);

			if (_setting.lightmass.enableGI)
				ray::Gui::checkbox(_langs[UILang::EnableIBL].c_str(), &_setting.lightmass.enableSkyLighting);

			ray::Gui::textUnformatted(_langs[UILang::OutputImageSize].c_str(), _langs[UILang::OutputImageSize].c_str() + _langs[UILang::OutputImageSize].size());
			ray::Gui::comboWithRevert("##Output size", _langs[UILang::Revert].c_str(), &_setting.lightmass.imageSize, _default.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));

			ray::Gui::textUnformatted(_langs[UILang::InputUVSlot].c_str(), _langs[UILang::InputUVSlot].c_str() + _langs[UILang::InputUVSlot].size());
			ray::Gui::comboWithRevert("##Output UV slot", _langs[UILang::Revert].c_str(), &_setting.uvmapper.slot, _default.uvmapper.slot, itemsUVSlot, sizeof(itemsUVSlot) / sizeof(itemsUVSlot[0]));

			ray::Gui::textUnformatted(_langs[UILang::SampleCount].c_str(), _langs[UILang::SampleCount].c_str() + _langs[UILang::SampleCount].size());
			ray::Gui::comboWithRevert("##Sample Count", _langs[UILang::Revert].c_str(), &_setting.lightmass.sampleCount, _default.lightmass.sampleCount, itemsSampleSize, sizeof(itemsSampleSize) / sizeof(itemsSampleSize[0]));

			ray::Gui::textUnformatted(_langs[UILang::EnvironmentColor].c_str(), _langs[UILang::EnvironmentColor].c_str() + _langs[UILang::EnvironmentColor].size());
			ray::Gui::colorPicker3WithRevert("##Environment Color", _langs[UILang::Revert].c_str(), _setting.lightmass.environmentColor.ptr(), _default.lightmass.environmentColor.ptr());

			ray::Gui::textUnformatted(_langs[UILang::EnvironmentIntensity].c_str(), _langs[UILang::EnvironmentIntensity].c_str() + _langs[UILang::EnvironmentIntensity].size());
			ray::Gui::sliderFloatWithRevert("##Environment Intensity", _langs[UILang::Revert].c_str(), &_setting.lightmass.environmentColor.w, _default.lightmass.environmentColor.w, 0.0f, 10.0f, "%.5f", 2.2);

			ray::Gui::textUnformatted(_langs[UILang::RayTracingZnear].c_str(), _langs[UILang::RayTracingZnear].c_str() + _langs[UILang::RayTracingZnear].size());
			ray::Gui::sliderFloatWithRevert("##Ray tracing znear", _langs[UILang::Revert].c_str(), &_setting.lightmass.hemisphereNear, _default.lightmass.hemisphereNear, 0.01f, 1.0, "%.5f", 2.2);

			ray::Gui::textUnformatted(_langs[UILang::RayTracingZfar].c_str(), _langs[UILang::RayTracingZfar].c_str() + _langs[UILang::RayTracingZfar].size());
			ray::Gui::sliderFloatWithRevert("##Ray tracing zfar", _langs[UILang::Revert].c_str(), &_setting.lightmass.hemisphereFar, _default.lightmass.hemisphereFar, 10.0f, 1000.0f, "%.5f", 2.2);

			ray::Gui::textUnformatted(_langs[UILang::InterpolationPasses].c_str(), _langs[UILang::InterpolationPasses].c_str() + _langs[UILang::InterpolationPasses].size());
			ray::Gui::sliderIntWithRevert("##Interpolation Passes", _langs[UILang::Revert].c_str(), &_setting.lightmass.interpolationPasses, _default.lightmass.interpolationPasses, 1, 5);

			ray::Gui::textUnformatted(_langs[UILang::InterpolationThreshold].c_str(), _langs[UILang::InterpolationThreshold].c_str() + _langs[UILang::InterpolationThreshold].size());
			ray::Gui::sliderFloatWithRevert("##Interpolation Threshold", _langs[UILang::Revert].c_str(), &_setting.lightmass.interpolationThreshold, _default.lightmass.interpolationThreshold, 1e-6f, 1e-2f, "%.6f", 2.2);

			if (ray::Gui::button(_langs[UILang::StartLightMass].c_str()))
				this->startLightMass();

			ray::Gui::sameLine();

			if (ray::Gui::button(_langs[UILang::SaveAs].c_str()))
				this->saveLightMass();

			ray::Gui::treePop();
		}

		ray::Gui::endDock();
	}
}

void
GuiViewComponent::switchLangPackage(UILang::Lang type) noexcept
{
	::GetLangPackage(type, _langs);
}

void
GuiViewComponent::startUVMapper() noexcept
{
	if (this->_event.onUVMapperWillStart)
	{
		ray::util::string::pointer error = nullptr;
		if (!this->_event.onUVMapperWillStart(_setting, error))
		{
			this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}("ChooseModel"));
			return;
		}
	}

	_lightMassType = LightMassType::UVMapper;
	_isShowProcessMessageFirst = true;
}

void
GuiViewComponent::startLightMass() noexcept
{
	if (this->_event.onLightMassWillStart)
	{
		ray::util::string::pointer error = nullptr;
		if (!this->_event.onLightMassWillStart(_setting, error))
		{
			this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}("ChooseModel"));
			return;
		}
	}

	_lightMassType = LightMassType::LightBaking;
	_isShowProcessMessageFirst = true;
}

void
GuiViewComponent::startModelPicker(float x, float y) noexcept
{
	if (_event.onModelPicker)
	{
		x -= _viewport.x;
		y -= _viewport.y;
		x = (x / _viewport.z) * ray::Gui::getDisplaySize().x;
		y = (y / _viewport.w) * ray::Gui::getDisplaySize().y;

		if (!_event.onModelPicker(x, y, _selectedObject, _selectedSubset))
		{
			_selectedObject = nullptr;
			_selectedSubset = std::numeric_limits<std::size_t>::max();
		}
	}
}

void
GuiViewComponent::createLightProbe() noexcept
{
}

void
GuiViewComponent::saveLightMass() noexcept
{
	if (_event.onLightMassSave)
	{
		ray::util::string::value_type filepath[PATHLIMIT];
		std::memset(filepath, 0, sizeof(filepath));

		if (!showFileSaveBrowse(filepath, PATHLIMIT, TEXT("Scene Flie(*.tga)\0*.tga;\0All File(*.*)\0*.*;\0\0")))
			return;

		if (std::strlen(filepath) < (PATHLIMIT - 5))
		{
			if (std::strstr(filepath, ".tga") == 0)
				std::strcat(filepath, ".tga");
		}

		ray::util::string::pointer error = nullptr;
		if (!_event.onLightMassSave(filepath, error))
		{
			if (error)
				this->showPopupMessage(_langs[UILang::Error].c_str(), error, std::hash<const char*>{}("saveLightMassFailed"));
		}
		else
		{
			this->showPopupMessage(_langs[UILang::OK].c_str(), _langs[UILang::Succeeded].c_str(), std::hash<const char*>{}("saveLightMass"));
		}
	}
}