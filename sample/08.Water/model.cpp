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
#include "model.h"
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/res_manager.h>
#include <ray/material.h>

__ImplementSubClass(ModelComponent, GameComponent, "Model")

ModelComponent::ModelComponent() noexcept
{
}

ModelComponent::~ModelComponent() noexcept
{
}

void
ModelComponent::onActivate() noexcept
{
	auto terrainMap = ray::ResManager::instance()->createTexture("dlc:common/textures/terrain.dds", ray::GraphicsTextureDim::GraphicsTextureDim2D);
	if (!terrainMap)
		return;

	auto waveMask = ray::ResManager::instance()->createTexture("dlc:common/textures/wave_mask.dds", ray::GraphicsTextureDim::GraphicsTextureDim2D);
	if (!waveMask)
		return;

	auto normalMap = ray::ResManager::instance()->createTexture("dlc:common/textures/wavesbump.dds", ray::GraphicsTextureDim::GraphicsTextureDim2D);
	if (!normalMap)
		return;

	auto waterMaterial = ray::ResManager::instance()->createMaterial("sys:fx/water.fxml");
	if (!waterMaterial)
		return;

	auto terrainMaterial = ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml");
	if (!terrainMaterial)
		return;

	terrainMaterial->getParameter("quality")->uniform4f(ray::float4(1.0, 0.0, 0.0, 0.0));
	terrainMaterial->getParameter("diffuse")->uniform3f(1.0, 1.0, 1.0);
	terrainMaterial->getParameter("metalness")->uniform1f(0.0f);
	terrainMaterial->getParameter("smoothness")->uniform1f(0.0f);
	terrainMaterial->getParameter("texDiffuse")->uniformTexture(terrainMap);
	terrainMaterial->getParameter("texNormal")->uniformTexture(normalMap);

	waterMaterial->getParameter("quality")->uniform4f(ray::float4(0.0, 1.0, 0.0, 0.0));
	waterMaterial->getParameter("diffuse")->uniform3f(0, 0.0, 0.0);
	waterMaterial->getParameter("transmittance")->uniform3f(0, 0.1, 0.15);
	waterMaterial->getParameter("specular")->uniform3f(0.0196, 0.0196, 0.0196);
	waterMaterial->getParameter("smoothness")->uniform1f(0.90);
	waterMaterial->getParameter("texDiffuse")->uniformTexture(waveMask);
	waterMaterial->getParameter("texNormal")->uniformTexture(normalMap);

	auto planeMesh = std::make_shared<ray::MeshProperty>();
	planeMesh->makeFloor(1.0, 1.0, 10, 10);

	auto terrainObject = std::make_shared<ray::GameObject>();
	terrainObject->setActive(true);
	terrainObject->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	terrainObject->addComponent(std::make_shared<ray::MeshRenderComponent>(terrainMaterial));
	terrainObject->setScale(ray::float3(80.0f));
	terrainObject->setTranslate(ray::float3(0.0, 1.0, 0.0));

	auto waveObject = std::make_shared<ray::GameObject>();
	waveObject->setActive(true);
	waveObject->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	waveObject->addComponent(std::make_shared<ray::MeshRenderComponent>(waterMaterial));
	waveObject->setScale(ray::float3(80.0f));
	waveObject->setTranslate(ray::float3(0.0, 2.0, 0.0));

	_objects.push_back(terrainObject);
	_objects.push_back(waveObject);
}

void
ModelComponent::onDeactivate() noexcept
{
	_objects.clear();
}

ray::GameComponentPtr
ModelComponent::clone() const noexcept
{
	return std::make_shared<ModelComponent>();
}