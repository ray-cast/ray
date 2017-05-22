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
#include "UIController.h"
#include <ray/gui.h>
#include <ray/gui_message.h>
#include <ray/game_server.h>

__ImplementSubClass(GuiControllerComponent, ray::GameComponent, "GuiController")

const char* itemsImageSize[] = { "512", "1024", "2048", "4096", "8192" };
const char* itemsSampleSize[] = { "32", "64", "128", "256", "512" };

GuiControllerComponent::GuiControllerComponent() noexcept
	: _clearColor(ray::float4(114.f / 255.f, 144.f / 255.f, 154.f / 255.f))
{
	_fps = 0.0f;
	_showTestWindow = true;
	_showLightMassWindow = true;
}

GuiControllerComponent::~GuiControllerComponent() noexcept
{
}

ray::GameComponentPtr
GuiControllerComponent::clone() const noexcept
{
	return std::make_shared<GuiControllerComponent>();
}

void
GuiControllerComponent::onMessage(const ray::MessagePtr& message) noexcept
{
	if (!message->isInstanceOf<ray::GuiMessage>())
		return;

	float delta = ray::GameServer::instance()->getTimer()->delta();
	float fps = ray::GameServer::instance()->getTimer()->averageFps();

	static bool show_app_main_menu_bar = false;

	if (ray::Gui::beginMainMenuBar())
	{
		ray::Gui::pushStyleColor(ray::GuiCol::GuiColBorder, ray::float4::Zero);

		if (ray::Gui::beginMenu("File"))
		{
			if (ray::Gui::menuItem("Open", "CTRL+O")) {}
			if (ray::Gui::menuItem("Save", "CTRL+S")) {}
			if (ray::Gui::menuItem("Save As", "CTRL+SHIFT+S")) {}
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu("Edit"))
		{
			if (ray::Gui::menuItem("Undo", "CTRL+Z")) {}
			if (ray::Gui::menuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ray::Gui::separator();
			if (ray::Gui::menuItem("Cut", "CTRL+X")) {}
			if (ray::Gui::menuItem("Copy", "CTRL+C")) {}
			if (ray::Gui::menuItem("Paste", "CTRL+V")) {}
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu("Environment"))
		{
			if (ray::Gui::menuItem("Load HDRi")) {}
			if (ray::Gui::menuItem("Save HDRi")) {}
			ray::Gui::endMenu();
		}

		if (ray::Gui::beginMenu("About"))
		{
			ray::Gui::endMenu();
		}

		ray::Gui::popStyleColor();

		ray::Gui::endMainMenuBar();
	}

	ray::Gui::setNextWindowPos(ray::float2(0, 0), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);
	ray::Gui::setNextWindowSize(ray::float2(310, 700), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);

	if (ray::Gui::begin("Light Mass", &_showLightMassWindow, ray::GuiWindowFlagBits::GuiWindowFlagNoTitleBarBit | ray::GuiWindowFlagBits::GuiWindowFlagNoResizeBit))
	{
		ray::Gui::text("Light Mass");

		if (ray::Gui::collapsingHeader("About"))
		{
			ray::Gui::text("LightMass Ver.0.1");
			ray::Gui::text("Developer by : Rui (2017)");
		}

		if (ray::Gui::collapsingHeader("Rendering"))
		{
		}

		if (ray::Gui::collapsingHeader("Uvmapper"))
		{
			ray::Gui::text("Output Size");
			ray::Gui::combo("##Output size", &_setting.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));
			if (_setting.lightmass.imageSize != _default.lightmass.imageSize)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.imageSize = _default.lightmass.imageSize;
			}

			ray::Gui::text("margin:");
			ray::Gui::sliderFloat("##Margin", &_setting.uvmapper.margin, 0.0f, 10.0f);
			if (_setting.uvmapper.margin != _default.uvmapper.margin)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.uvmapper.margin = _default.uvmapper.margin;
			}

			ray::Gui::text("stretch:");
			ray::Gui::sliderFloat("##stretch", &_setting.uvmapper.stretch, 0.0, 1.0, "%.5f", 2.2);
			if (_setting.uvmapper.stretch != _default.uvmapper.stretch)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.uvmapper.stretch = _default.uvmapper.stretch;
			}

			ray::Gui::text("chart:");
			ray::Gui::sliderInt("##chart", &_setting.uvmapper.chart, 0, 65535);
			if (_setting.uvmapper.chart != _default.uvmapper.chart)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.uvmapper.chart = _default.uvmapper.chart;
			}
		}

		if (ray::Gui::collapsingHeader("Light Mass"))
		{
			ray::Gui::checkbox("Enable Global Illumination", &_setting.lightmass.enableGI);

			ray::Gui::text("Output Size");
			ray::Gui::combo("##Output size", &_setting.lightmass.imageSize, itemsImageSize, sizeof(itemsImageSize) / sizeof(itemsImageSize[0]));
			if (_setting.lightmass.imageSize != _default.lightmass.imageSize)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.imageSize = _default.lightmass.imageSize;
			}

			ray::Gui::text("Sample Count");
			ray::Gui::combo("##Sample Count", &_setting.lightmass.sampleCount, itemsSampleSize, sizeof(itemsSampleSize) / sizeof(itemsSampleSize[0]));
			if (_setting.lightmass.sampleCount != _default.lightmass.sampleCount)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.sampleCount = _default.lightmass.sampleCount;
			}

			ray::Gui::text("Environment Color:");
			ray::Gui::colorEdit3("##Environment Color", _setting.lightmass.environmentColor.ptr());
			if (_setting.lightmass.environmentColor.xyz() != _default.lightmass.environmentColor.xyz())
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.environmentColor.set(_default.lightmass.environmentColor.xyz());
			}

			ray::Gui::text("Environment Intensity:");
			ray::Gui::sliderFloat("##Environment Intensity", &_setting.lightmass.environmentColor.w, 0.0f, 10.0f, "%.5f", 2.2);
			if (_setting.lightmass.environmentColor.w != _default.lightmass.environmentColor.w)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.environmentColor.w = _default.lightmass.environmentColor.w;
			}

			ray::Gui::text("Hemisphere znear:");
			ray::Gui::sliderFloat("##Hemisphere znear", &_setting.lightmass.hemisphereNear, 0.01f, 1.0, "%.5f", 2.2);
			if (_setting.lightmass.hemisphereNear != _default.lightmass.hemisphereNear)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.hemisphereNear = _default.lightmass.hemisphereNear;
			}

			ray::Gui::text("Hemisphere zfar:");
			ray::Gui::sliderFloat("##Hemisphere zfar", &_setting.lightmass.hemisphereFar, 10.0f, 1000.0f, "%.5f", 2.2);
			if (_setting.lightmass.hemisphereFar != _default.lightmass.hemisphereFar) {
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.hemisphereFar = _default.lightmass.hemisphereFar;
			}

			ray::Gui::text("Interpolation Passes");
			ray::Gui::sliderInt("##Interpolation Passes", &_setting.lightmass.interpolationPasses, 1, 5);
			if (_setting.lightmass.interpolationPasses != _default.lightmass.interpolationPasses)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.interpolationPasses = _default.lightmass.interpolationPasses;
			}

			ray::Gui::text("Interpolation Threshold");
			ray::Gui::sliderFloat("##Interpolation Threshold", &_setting.lightmass.interpolationThreshold, 1e-6, 1e-2, "%.5f", 2.2);
			if (_setting.lightmass.interpolationThreshold != _default.lightmass.interpolationThreshold)
			{
				ray::Gui::sameLine();
				if (ray::Gui::button("Revert")) _setting.lightmass.interpolationThreshold = _default.lightmass.interpolationThreshold;
			}
		}

		ray::Gui::end();
	}

	if (_showTestWindow)
	{
		ray::Gui::setNextWindowPos(ray::float2(650, 20), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);
		ray::Gui::showTestWindow(&_showTestWindow);
	}
}