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
#include "UIView.h"
#include "UITexts.h"
#include <ray/gui_message.h>
#include <ray/ioserver.h>

__ImplementSubClass(GuiViewComponent, ray::GameComponent, "GuiView")

const char* itemsUVSlot[] = { "0", "1", "2", "3", "4" };
const char* itemsImageSize[] = { "512", "1024", "2048", "4096", "8192" };
const char* itemsSampleSize[] = { "32", "64", "128", "256", "512" };

GuiViewComponent::GuiViewComponent() noexcept
	: _clearColor(ray::float4(114.f / 255.f, 144.f / 255.f, 154.f / 255.f))
{
	_fps = 0.0f;
	_process = 0.0f;
	_showMainMenu = true;
	_showLightMassWindow = true;
	_showStyleEditor = false;
	_showAboutWindow = false;
	_showAboutWindowFirst = false;
	_showErrorMessage = false;
	_showErrorMessageFirst = false;
	_showProcessMessage = false;
	_showProcessMessageFirst = false;

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
GuiViewComponent::setModelImportListener(std::function<bool(ray::util::string::const_pointer, ray::util::string&)> delegate)
{
	_onModelImport = delegate;
}

void
GuiViewComponent::setModelSaveAsListener(std::function<bool(ray::util::string::const_pointer, ray::util::string&)> delegate)
{
	_onModelSaveAs = delegate;
}

void
GuiViewComponent::setUVMapperWillStartListener(std::function<bool(const GuiParams&)> delegate) noexcept
{
	_onUVMapperWillStart = delegate;
}

void
GuiViewComponent::setUVMapperStartListener(std::function<bool(const GuiParams&)> delegate) noexcept
{
	_onUVMapperStart = delegate;
}

void
GuiViewComponent::setProjectImportListener(std::function<bool(ray::util::string::const_pointer, ray::util::string&)> delegate)
{
	_onProjectOpen = delegate;
}

void
GuiViewComponent::setProjectSaveListener(std::function<bool(ray::util::string::const_pointer, ray::util::string&)> delegate)
{
	_onProjectSave = delegate;
}

void
GuiViewComponent::setProjectSaveAsListener(std::function<bool(ray::util::string::const_pointer, ray::util::string&)> delegate)
{
	_onProjectSaveAs = delegate;
}

void
GuiViewComponent::onMessage(const ray::MessagePtr& message) noexcept
{
	if (!message->isInstanceOf<ray::GuiMessage>())
		return;

	this->showMainMenu();
	this->showStyleEditor();
	this->showLightMass();
	this->showAboutWindow();
	this->showErrorMessage();
	this->showProcessMessage();
}

void
GuiViewComponent::showMainMenu() noexcept
{
	if (!_showMainMenu)
		return;

	if (ray::Gui::beginMainMenuBar())
	{
		ray::Gui::pushStyleColor(ray::GuiCol::GuiColBorder, ray::float4::Zero);

		if (ray::Gui::beginMenu(_langs[UILang::File]))
		{
			if (ray::Gui::menuItem(_langs[UILang::Open], "CTRL+O")) { this->showProjectOpenBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::Save], "CTRL+S")) { this->showProjectSaveBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::SaveAs], "CTRL+SHIFT+S")) { this->showProjectSaveAsBrowse(); }
			ray::Gui::separator();
			ray::Gui::separator();
			if (ray::Gui::menuItem(_langs[UILang::ImportModel], "")) { this->showModelImportBrowse(); }
			if (ray::Gui::menuItem(_langs[UILang::ExportModel], "")) { this->showModelExportBrowse(); }
			ray::Gui::separator();
			ray::Gui::separator();
			if (ray::Gui::menuItem(_langs[UILang::Exit], "")) { std::exit(0); }
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Window]))
		{
			ray::Gui::menuItem(_langs[UILang::LightMass], 0, &_showLightMassWindow);
			ray::Gui::menuItem(_langs[UILang::StyleEditor], 0, &_showStyleEditor);
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Setting]))
		{
			if (ray::Gui::beginMenu(_langs[UILang::Language]))
			{
				if (ray::Gui::menuItem(_langs[UILang::English])) { switchLangPackage(UILang::Lang::English); }
				if (ray::Gui::menuItem(_langs[UILang::Chinese])) { switchLangPackage(UILang::Lang::Chinese); }

				ray::Gui::endMenu();
			}

			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu(_langs[UILang::Help]))
		{
			ray::Gui::menuItem(_langs[UILang::About], 0, &_showAboutWindowFirst);
			ray::Gui::endMenu();
		}

		ray::Gui::popStyleColor();

		ray::Gui::endMainMenuBar();
	}
}

bool
GuiViewComponent::showFileOpenBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept
{
	assert(path && max_length > 0 && ext_name);

#if __WINDOWS__
	OPENFILENAME ofn;
	std::memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = ext_name;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = max_length;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrTitle = _langs[UILang::ChooseFile];
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (::GetOpenFileName(&ofn))
		return true;

	return false;
#else
	return false;
#endif
}

bool
GuiViewComponent::showFileSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept
{
	assert(path && max_length > 0 && ext_name);

#if __WINDOWS__
	OPENFILENAME ofn;
	std::memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = ext_name;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = max_length;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrTitle = _langs[UILang::SaveAs];
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (::GetSaveFileName(&ofn))
		return true;

	return false;
#else
	return false;
#endif
}

void
GuiViewComponent::showProjectOpenBrowse() noexcept
{
	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileOpenBrowse(filepath, PATHLIMIT, TEXT("Scene Flie(*.map)\0*.map;\0All File(*.*)\0*.*;\0\0")))
		return;

	if (_onProjectOpen)
	{
		ray::util::string error;
		if (!_onProjectOpen(filepath, error))
		{
			if (!error.empty())
				this->showErrorPopupMessage(error, std::hash<const char*>{}("showProjectOpenBrowse"));
		}
	}
}

void
GuiViewComponent::showProjectSaveBrowse() noexcept
{
	if (_pathProject.empty())
		return;

	if (_onProjectSave)
	{
		ray::util::string error;
		if (!_onProjectSave(_pathProject.c_str(), error))
		{
			if (!error.empty())
				this->showErrorPopupMessage(error, std::hash<const char*>{}("showProjectSaveBrowse"));
		}
	}
}

void
GuiViewComponent::showProjectSaveAsBrowse() noexcept
{
	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileSaveBrowse(filepath, PATHLIMIT, TEXT("Scene Flie(*.map)\0*.map;\0All File(*.*)\0*.*;\0\0")))
		return;

	if (_onProjectOpen)
	{
		ray::util::string error;
		if (_onProjectOpen(filepath, error))
			_pathProject = filepath;
		else
		{
			if (!error.empty())
				this->showErrorPopupMessage(error, std::hash<const char*>{}("showProjectSaveAsBrowse"));
		}
	}
}

void
GuiViewComponent::showModelImportBrowse() noexcept
{
	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileOpenBrowse(filepath, PATHLIMIT, TEXT("PMX Flie(*.pmx)\0*.pmx;\0All File(*.*)\0*.*;\0\0")))
		return;

	if (_onModelImport)
	{
		ray::util::string error;
		if (!_onModelImport(filepath, error))
		{
			if (!error.empty())
				this->showErrorPopupMessage(_langs[UILang::NonReadableFile], std::hash<const char*>{}("showModelImportBrowse"));
		}
	}
}

void
GuiViewComponent::showModelExportBrowse() noexcept
{
	if (!_onModelSaveAs)
		return;

	ray::util::string::value_type filepath[PATHLIMIT];
	std::memset(filepath, 0, sizeof(filepath));

	if (!showFileSaveBrowse(filepath, PATHLIMIT, TEXT("PMX Flie(*.pmx)\0*.pmx;\0All File(*.*)\0*.*;\0\0")))
		return;

	if (std::strlen(filepath) < (PATHLIMIT - 5))
	{
		if (std::strstr(filepath, ".pmx") == 0)
			std::strcat(filepath, ".pmx");
	}

	ray::util::string error;
	if (!_onModelSaveAs(filepath, error))
	{
		if (!error.empty())
			this->showErrorPopupMessage(error, std::hash<const char*>{}("showModelExportBrowse"));
	}
}

void
GuiViewComponent::showErrorPopupMessage(const ray::util::string& message, std::size_t hash) noexcept
{
	if (!_showErrorMessageFirst)
	{
		_errorHash = hash;
		_errorMessage = message;
		_showErrorMessageFirst = true;
	}
}

void
GuiViewComponent::showErrorMessage() noexcept
{
	if (_showErrorMessageFirst)
	{
		_showErrorMessageFirst = false;

		if (_showError[_errorHash])
			return;

		ray::Gui::openPopup(_langs[UILang::Error]);

		_showErrorMessage = true;
	}

	if (!_showErrorMessage)
		return;

	if (ray::Gui::beginPopupModal(_langs[UILang::Error], 0, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysAutoResizeBit))
	{
		ray::Gui::text(_errorMessage.c_str());
		ray::Gui::separator();

		ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarFramePadding, ray::float2(0, 0));
		ray::Gui::checkbox(_langs[UILang::NoShowAgain], &_showError[_errorHash]);
		ray::Gui::popStyleVar();

		if (ray::Gui::button(_langs[UILang::OK], ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); }
		ray::Gui::sameLine();

		if (ray::Gui::button(_langs[UILang::Cancel], ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); }
		ray::Gui::endPopup();
	}
}

void
GuiViewComponent::showProcessMessage() noexcept
{
	if (_showProcessMessageFirst)
	{
		_showProcessMessageFirst = false;

		ray::Gui::openPopup(_langs[UILang::Process]);

		_showProcessMessage = true;
	}

	if (!_showProcessMessage)
		return;

	ray::Gui::pushStyleVar(ray::GuiStyleVar::GuiStyleVarWindowRounding, 0);

	if (ray::Gui::beginPopupModal(_langs[UILang::Process], 0, ray::GuiWindowFlagBits::GuiWindowFlagNoTitleBarBit | ray::GuiWindowFlagBits::GuiWindowFlagNoMoveBit | ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit))
	{
		ray::Gui::setWindowSize(ray::float2(ray::Gui::getDisplaySize().x, 150));
		ray::Gui::text("%.2f", _process);

		ray::Gui::endPopup();
	}

	ray::Gui::popStyleVar();
}

void
GuiViewComponent::showAboutWindow() noexcept
{
	if (_showAboutWindowFirst)
	{
		ray::Gui::openPopup("About");

		_showAboutWindow = true;
		_showAboutWindowFirst = false;
	}

	if (!_showAboutWindow)
		return;

	if (ray::Gui::beginPopupModal("About", 0, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysAutoResizeBit))
	{
		ray::Gui::text("LightMass Ver.0.1");
		ray::Gui::text("Developer by : Rui (https://twitter.com/Rui_cg)");
		ray::Gui::text("Copyright (c) 2017-2018. All rights reserved.");

		ray::Gui::pushStyleColor(ray::GuiCol::GuiColBorder, ray::float4::Zero);
		ray::Gui::separator();
		ray::Gui::separator();
		ray::Gui::popStyleColor();

		ray::Gui::sameLine(ray::Gui::getWindowWidth() - 130);
		if (ray::Gui::button(_langs[UILang::OK], ray::float2(120, 0))) { ray::Gui::closeCurrentPopup(); }

		ray::Gui::endPopup();
	}
}

void
GuiViewComponent::showStyleEditor() noexcept
{
	if (!_showStyleEditor)
		return;

	if (ray::Gui::begin(_langs[UILang::StyleEditor], &_showStyleEditor, ray::float2(550, 500)))
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
			ray::Gui::text("Alignment");
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

			if (ray::Gui::beginChild("#colors", ray::float2(0, 300), true, ray::GuiWindowFlagBits::GuiWindowFlagAlwaysVerticalScrollbarBit))
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

						if (ray::Gui::button(_langs[UILang::Revert]))
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
GuiViewComponent::showLightMass() noexcept
{
	if (!_showLightMassWindow)
		return;

	ray::Gui::setNextWindowPos(ray::float2(10, 80), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);

	if (ray::Gui::begin(_langs[UILang::LightMass], &_showLightMassWindow, ray::float2(260, 700), -1.0, ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit))
	{
		if (ray::Gui::treeNode(_langs[UILang::UvMapper]))
		{
			ray::Gui::text(_langs[UILang::OutputUVSize]);
			ray::Gui::comboWithRevert("##UV size", _langs[UILang::Revert], &_setting.lightmass.imageSize, _default.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));

			ray::Gui::text(_langs[UILang::OutputUVSlot]);
			ray::Gui::comboWithRevert("##Output UV slot", _langs[UILang::Revert], &_setting.uvmapper.slot, _default.uvmapper.slot, itemsUVSlot, sizeof(itemsUVSlot) / sizeof(itemsUVSlot[0]));

			ray::Gui::text(_langs[UILang::UVMargin]);
			ray::Gui::sliderFloatWithRevert("##margin", _langs[UILang::Revert], &_setting.uvmapper.margin, _default.uvmapper.margin, 0.0f, 10.0f);

			ray::Gui::text(_langs[UILang::UVStretch]);
			ray::Gui::sliderFloatWithRevert("##stretch", _langs[UILang::Revert], &_setting.uvmapper.stretch, _default.uvmapper.stretch, 0.0, 1.0, "%.5f", 2.2);

			ray::Gui::text(_langs[UILang::UVChart]);
			ray::Gui::sliderIntWithRevert("##chart", _langs[UILang::Revert], &_setting.uvmapper.chart, _default.uvmapper.chart, 0, 65535);

			if (ray::Gui::button(_langs[UILang::StartUVMapper]))
				this->startUVMapper();

			ray::Gui::treePop();
		}

		if (ray::Gui::treeNode(_langs[UILang::LightMass]))
		{
			ray::Gui::checkbox(_langs[UILang::EnableGI], &_setting.lightmass.enableGI);

			if (_setting.lightmass.enableGI)
				ray::Gui::checkbox(_langs[UILang::EnableIBL], &_setting.lightmass.enableSkyLighting);

			ray::Gui::text(_langs[UILang::OutputImageSize]);
			ray::Gui::comboWithRevert("##Output size", _langs[UILang::Revert], &_setting.lightmass.imageSize, _default.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));

			ray::Gui::text(_langs[UILang::InputUVSlot]);
			ray::Gui::comboWithRevert("##Output UV slot", _langs[UILang::Revert], &_setting.uvmapper.slot, _default.uvmapper.slot, itemsUVSlot, sizeof(itemsUVSlot) / sizeof(itemsUVSlot[0]));

			ray::Gui::text(_langs[UILang::SampleCount]);
			ray::Gui::comboWithRevert("##Sample Count", _langs[UILang::Revert], &_setting.lightmass.sampleCount, _default.lightmass.sampleCount, itemsSampleSize, sizeof(itemsSampleSize) / sizeof(itemsSampleSize[0]));

			ray::Gui::text(_langs[UILang::EnvironmentColor]);
			ray::Gui::colorPicker3WithRevert("##Environment Color", _langs[UILang::Revert], _setting.lightmass.environmentColor.ptr(), _default.lightmass.environmentColor.ptr());

			ray::Gui::text(_langs[UILang::EnvironmentIntensity]);
			ray::Gui::sliderFloatWithRevert("##Environment Intensity", _langs[UILang::Revert], &_setting.lightmass.environmentColor.w, _default.lightmass.environmentColor.w, 0.0f, 10.0f, "%.5f", 2.2);

			ray::Gui::text(_langs[UILang::RayTracingZnear]);
			ray::Gui::sliderFloatWithRevert("##Ray tracing znear", _langs[UILang::Revert], &_setting.lightmass.hemisphereNear, _default.lightmass.hemisphereNear, 0.01f, 1.0, "%.5f", 2.2);

			ray::Gui::text(_langs[UILang::RayTracingZfar]);
			ray::Gui::sliderFloatWithRevert("##Ray tracing zfar", _langs[UILang::Revert], &_setting.lightmass.hemisphereFar, _default.lightmass.hemisphereFar, 10.0f, 1000.0f, "%.5f", 2.2);

			ray::Gui::text(_langs[UILang::InterpolationPasses]);
			ray::Gui::sliderIntWithRevert("##Interpolation Passes", _langs[UILang::Revert], &_setting.lightmass.interpolationPasses, _default.lightmass.interpolationPasses, 1, 5);

			ray::Gui::text(_langs[UILang::InterpolationThreshold]);
			ray::Gui::sliderFloatWithRevert("##Interpolation Threshold", _langs[UILang::Revert], &_setting.lightmass.interpolationThreshold, _default.lightmass.interpolationThreshold, 1e-6f, 1e-2f, "%.6f", 2.2);

			ray::Gui::button(_langs[UILang::StartBaking]);

			ray::Gui::treePop();
		}

		ray::Gui::end();
	}
}

void
GuiViewComponent::switchLangPackage(UILang::Lang type) noexcept
{
	GetLangPackage(type, _langs);
}

void
GuiViewComponent::startUVMapper() noexcept
{
	if (this->_onUVMapperWillStart)
	{
		if (!this->_onUVMapperWillStart(_setting))
		{
			this->showErrorPopupMessage(_langs[UILang::UnsupportModel], std::hash<const char*>{}("UnsupportModel"));
			return;
		}
	}

	if (_showProcessMessage)
	{
		if (this->_onUVMapperStart)
		{
			if (!this->_onUVMapperStart(_setting))
			{
				this->showErrorPopupMessage(_langs[UILang::UnsupportModel], std::hash<const char*>{}("UnsupportModel"));
				return;
			}
		}
	}

	_showProcessMessageFirst = true;
}