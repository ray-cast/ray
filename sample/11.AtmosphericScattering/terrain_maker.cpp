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
#include <ray/game_object_manager.h>

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
	/*auto materialTemp = ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml");
	if (!materialTemp)
		return;

	auto terrainMesh = std::make_shared<ray::MeshProperty>();
	terrainMesh->makeNoise(2048, 2048, 400, 400);

	auto gameObject = std::make_shared<ray::GameObject>();
	gameObject->setActive(true);
	gameObject->addComponent(std::make_shared<ray::MeshComponent>(terrainMesh));
	gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(materialTemp->clone()));
	gameObject->setScale(ray::float3(100, 50, 100));
	gameObject->setTranslate(ray::float3(0, 8000, 0));

	auto material = gameObject->getComponent<ray::MeshRenderComponent>()->getMaterial();
	material->getParameter("quality")->uniform4f(0.0, 0.0, 0.0, 0.0);
	material->getParameter("diffuse")->uniform3f(ray::math::pow(ray::float3(0.15, 0.48, 0.09), ray::float3(1.0f / 2.4f)));
	material->getParameter("specular")->uniform1f(0.1);
	material->getParameter("shininess")->uniform1f(0.1);

	_objects.push_back(gameObject);*/

	auto materialTemp = ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml");
	if (!materialTemp)
		return;

	auto sphereMesh = std::make_shared<ray::MeshProperty>();
	sphereMesh->makeSphere(1.0, 64, 48);

	for (std::size_t i = 0; i < 10; i++)
	{
		for (std::size_t j = 0; j < 10; j++)
		{
			auto gameObject = std::make_shared<ray::GameObject>();
			gameObject->setActive(true);
			gameObject->addComponent(std::make_shared<ray::MeshComponent>(sphereMesh));
			gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(materialTemp->clone()));
			gameObject->setScale(ray::float3(0.9));
			gameObject->setTranslate(ray::float3(-10.0f + i * 2.0f, 8000, j * 2.0f));

			auto material = gameObject->getComponent<ray::MeshRenderComponent>()->getMaterial();
			material->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
			material->getParameter("diffuse")->uniform3f(0.5,0.5,0.5);
			material->getParameter("specular")->uniform1f(0.1);
			material->getParameter("shininess")->uniform1f(0.1);

			_objects.push_back(gameObject);
		}
	}

	this->addComponentDispatch(ray::GameDispatchType::GameDispatchTypeFrame, this);
}

void
TerrainMakerComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(ray::GameDispatchType::GameDispatchTypeFrame, this);
}

void 
TerrainMakerComponent::onFrame() noexcept
{
	auto sunObject = ray::GameObjectManager::instance()->findObject("sun");
	if (sunObject)
	{
		ray::Quaternion rotate;
		rotate.makeRotate(ray::float3::UnitX, 0.03);
		sunObject->setQuaternionAccum(rotate);
	}
}

ray::GameComponentPtr
TerrainMakerComponent::clone() const noexcept
{
	return std::make_shared<TerrainMakerComponent>();
}