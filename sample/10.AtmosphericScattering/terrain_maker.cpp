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
#include "terrain_maker.h"
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/material.h>
#include <ray/res_manager.h>
#include <ray/game_server.h>
#include <ray/game_object_manager.h>
#include <ray/gui_feature.h>
#include <ray/input_feature.h>

using namespace ray;

__ImplementSubClass(TerrainMakerComponent, GameComponent, "TerrainMaker")

TerrainMakerComponent::TerrainMakerComponent() noexcept
{
}

TerrainMakerComponent::~TerrainMakerComponent() noexcept
{
}

void
TerrainMakerComponent::onActivate() except
{
	auto materialTemp = ResManager::instance()->createMaterial("sys:fx/opacity.fxml");
	if (!materialTemp)
		return;

	auto sphereMesh = std::make_shared<MeshProperty>();
	sphereMesh->makeSphere(1.0, 64, 48);

	for (std::size_t i = 0; i < 10; i++)
	{
		for (std::size_t j = 0; j < 10; j++)
		{
			auto gameObject = std::make_shared<GameObject>();
			gameObject->setActive(true);
			gameObject->addComponent(std::make_shared<MeshComponent>(sphereMesh));
			gameObject->addComponent(std::make_shared<MeshRenderComponent>(materialTemp->clone()));
			gameObject->setScale(float3(0.9));
			gameObject->setTranslate(float3(-10.0f + i * 2.0f, 0, j * 2.0f));

			auto material = gameObject->getComponent<MeshRenderComponent>()->getMaterial();
			material->getParameter("quality")->uniform4f(float4(0.0, 0.0, 0.0, 0.0));
			material->getParameter("diffuse")->uniform3f(0.5,0.5,0.5);
			material->getParameter("specular")->uniform1f(0.1);
			material->getParameter("shininess")->uniform1f(0.1);

			_objects.push_back(gameObject);
		}
	}

	this->addComponentDispatch(GameDispatchType::GameDispatchTypeFrame, this);
}

void
TerrainMakerComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeFrame, this);
}

void 
TerrainMakerComponent::onFrame() noexcept
{
	auto sunObject = GameObjectManager::instance()->findObject("sun");
	if (sunObject)
	{
		Quaternion rotate;
		rotate.makeRotate(float3::UnitX, 0.03);
		sunObject->setQuaternionAccum(rotate);
	}

	auto moonObject = GameObjectManager::instance()->findObject("moon");
	if (moonObject)
	{
		Quaternion rotate;
		rotate.makeRotate(float3::UnitX, 0.03);
		moonObject->setQuaternionAccum(rotate);
	}
}

void
TerrainMakerComponent::onMessage(const MessagePtr& message) noexcept
{
	static bool _showTestWindow = true;

	auto inputFeature = GameServer::instance()->getFeature<InputFeature>();
	if (inputFeature)
	{
		if (inputFeature->getInput()->isLockedCursor())
			return;
	}

	/*if (message->isInstanceOf<GuiMessage>())
	{
		float delta = GameServer::instance()->getTimer()->delta();
		float fps = GameServer::instance()->getTimer()->averageFps();

		auto& Gui = message->downcast<GuiMessage>()->getGui();

		Gui.setNextWindowPos(float2(0, 0), GuiSetCondFlagBits::GuiSetCondFlagAlwaysBit);
		Gui.setNextWindowSize(float2(300, 768), GuiSetCondFlagBits::GuiSetCondFlagAlwaysBit);
		if (!Gui.begin("Setting"))
		{
			Gui.end();
			return;
		}

		if (Gui.collapsingHeader("Abount"))
		{
		}

		Gui.separator();

		if (Gui.collapsingHeader("Rendering"))
		{
		}

		Gui.separator();

		if (Gui.collapsingHeader("Environment"))
		{
		}

		Gui.separator();

		if (Gui.collapsingHeader("HDR"))
		{
		}

		Gui.separator();

		if (Gui.collapsingHeader("Anti-aliasing"))
		{
		}

		Gui.separator();
		
		if (Gui.collapsingHeader("Style"))
		{
			Gui.pushStyleColor(ray::GuiColText, float4(0, 0, 0));
			Gui.showStyleEditor();
			Gui.popStyleColor(1);
		}
		
		Gui.separator();

		if (Gui.collapsingHeader("Debug"))
		{
			Gui.pushStyleColor(ray::GuiColText, float4(0, 0, 0));

			if (Gui.button("Test Window")) _showTestWindow ^= 1;
			Gui.text("Application average %f ms/frame (%f FPS)", delta, fps);

			Gui.popStyleColor(1);
		}

		Gui.end();

		if (_showTestWindow)
		{
			Gui.setNextWindowPos(float2(650, 20), GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);
			Gui.showTestWindow(&_showTestWindow);
		}
	}*/
}

GameComponentPtr
TerrainMakerComponent::clone() const noexcept
{
	return std::make_shared<TerrainMakerComponent>();
}