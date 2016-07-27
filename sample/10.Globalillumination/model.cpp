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
	auto materialTemp = ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml");
	if (!materialTemp)
		return;

	auto white = materialTemp->clone();
	white->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
	white->getParameter("diffuse")->uniform3f(ray::math::pow(ray::float3(0.76, 0.75, 0.5), ray::float3(1.0f / 2.4f)));
	white->getParameter("metalness")->uniform1f(0.1);
	white->getParameter("smoothness")->uniform1f(0.1);

	auto red = materialTemp->clone();
	red->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
	red->getParameter("diffuse")->uniform3f(ray::math::pow(ray::float3(0.63, 0.06, 0.04), ray::float3(1.0f / 2.4f)));
	red->getParameter("metalness")->uniform1f(0.1);
	red->getParameter("smoothness")->uniform1f(0.1);

	auto green = materialTemp->clone();
	green->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
	green->getParameter("diffuse")->uniform3f(ray::math::pow(ray::float3(0.15, 0.48, 0.09), ray::float3(1.0f / 2.4f)));
	green->getParameter("metalness")->uniform1f(0.1);
	green->getParameter("smoothness")->uniform1f(0.1);

	auto planeMesh = std::make_shared<ray::MeshProperty>();
	planeMesh->makePlane(1.0, 1.0);
	auto& normals = planeMesh->getNormalArray();
	for (auto& normal : normals)
		normal = -normal;

	auto cubeMesh = std::make_shared<ray::MeshProperty>();
	cubeMesh->makeCube(1.0, 1.0, 1.0);

	auto nearPlane = std::make_shared<ray::GameObject>();
	nearPlane->setActive(true);
	nearPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	nearPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	nearPlane->setQuaternion(ray::Quaternion(ray::float3::UnitX, 180.0f));
	nearPlane->setScale(ray::float3(5, 5, 5));
	nearPlane->setTranslate(ray::float3(0, 0, -2.5));

	auto farPlane = std::make_shared<ray::GameObject>();
	farPlane->setActive(true);
	farPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	farPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	farPlane->setScale(ray::float3(5, 5, 5));
	farPlane->setTranslate(ray::float3(0, 0, 2.5));

	auto rightPlane = std::make_shared<ray::GameObject>();
	rightPlane->setActive(true);
	rightPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	rightPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(green));
	rightPlane->setQuaternion(ray::Quaternion(ray::float3::UnitY, 90.0f));
	rightPlane->setScale(ray::float3(5, 5, 5));
	rightPlane->setTranslate(ray::float3(2.5, 0, 0.0));

	auto leftPlane = std::make_shared<ray::GameObject>();
	leftPlane->setActive(true);
	leftPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	leftPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(red));
	leftPlane->setQuaternion(ray::Quaternion(ray::float3::UnitY, -90.0f));
	leftPlane->setScale(ray::float3(5, 5, 5));
	leftPlane->setTranslate(ray::float3(-2.5, 0, 0.0));

	auto topPlane = std::make_shared<ray::GameObject>();
	topPlane->setActive(true);
	topPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	topPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	topPlane->setQuaternion(ray::Quaternion(ray::float3::UnitX, -90.0f));
	topPlane->setScale(ray::float3(5, 5, 5));
	topPlane->setTranslate(ray::float3(0.0, 2.5, 0.0));

	auto bottomPlane = std::make_shared<ray::GameObject>();
	bottomPlane->setActive(true);
	bottomPlane->addComponent(std::make_shared<ray::MeshComponent>(planeMesh));
	bottomPlane->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	bottomPlane->setQuaternion(ray::Quaternion(ray::float3::UnitX, 90.0f));
	bottomPlane->setScale(ray::float3(5, 5, 5));
	bottomPlane->setTranslate(ray::float3(0.0, -2.5, 0.0));

	auto largeCube = std::make_shared<ray::GameObject>();
	largeCube->setActive(true);
	largeCube->addComponent(std::make_shared<ray::MeshComponent>(cubeMesh));
	largeCube->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	largeCube->setScale(ray::float3(1.5, 3.4, 1.5));
	largeCube->setQuaternion(ray::Quaternion(ray::float3::UnitY, -20.0f));
	largeCube->setTranslate(ray::float3(-1, -0.8, 1.0));

	auto smallCube = std::make_shared<ray::GameObject>();
	smallCube->setActive(true);
	smallCube->addComponent(std::make_shared<ray::MeshComponent>(cubeMesh));
	smallCube->addComponent(std::make_shared<ray::MeshRenderComponent>(white));
	smallCube->setScale(ray::float3(1.5, 1.5, 1.5));
	smallCube->setQuaternion(ray::Quaternion(ray::float3::UnitY, 20.0f));
	smallCube->setTranslate(ray::float3(1, -1.75, -1));

	_objects.push_back(nearPlane);
	_objects.push_back(farPlane);
	_objects.push_back(rightPlane);
	_objects.push_back(leftPlane);
	_objects.push_back(topPlane);
	_objects.push_back(bottomPlane);
	_objects.push_back(largeCube);
	_objects.push_back(smallCube);
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