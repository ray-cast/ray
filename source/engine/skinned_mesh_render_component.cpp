// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/skinned_mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/render_system.h>
#include <ray/material.h>
#include <ray/render_mesh.h>
#include <ray/render_feature.h>
#include <ray/game_server.h>
#include <ray/res_manager.h>
#include <ray/graphics_data.h>
#include <ray/geometry.h>

_NAME_BEGIN

__ImplementSubClass(SkinnedMeshRenderComponent, MeshRenderComponent, "SkinnedMeshRender")

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent() noexcept
{
}

SkinnedMeshRenderComponent::~SkinnedMeshRenderComponent() noexcept
{
}

bool
SkinnedMeshRenderComponent::_buildDefaultMaterials(const std::string& filename) noexcept
{
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return false;

	auto model = ResManager::instance()->find<Model>(component->getName());
	if (!model)
		return false;

	auto& materials = model->getMaterialsList();
	if (materials.empty())
		return false;

	for (auto& material : materials)
	{
		float3 specular(0.5f);
		float3 diffuseColor(1.0f);
		float4 quality(0.0f);
		float shininess = 0.0;
		float opacity = 1.0;
		std::string diffuseTexture;

		material->get(MATKEY_OPACITY, opacity);
		material->get(MATKEY_SHININESS, shininess);
		material->get(MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->get(MATKEY_COLOR_SPECULAR, specular);
		material->get(MATKEY_TEXTURE_DIFFUSE(0), diffuseTexture);

		MaterialPtr effect;
		effect = RenderSystem::instance()->createMaterial(filename);
		if (!effect)
			_materials.push_back(nullptr);
		else
		{
			auto luminance = [](const float3& rgb)
			{
				const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
				return math::dot(rgb, lumfact);
			};

			auto texture = RenderSystem::instance()->createTexture(model->getDirectory() + diffuseTexture, GraphicsTextureDim::GraphicsTextureDim2D);
			if (texture)
			{
				quality.x = 1.0f;
				effect->getParameter("texDiffuse")->uniformTexture(texture);
			}

			effect->getParameter("quality")->uniform4f(quality);
			effect->getParameter("diffuse")->uniform3f(diffuseColor);
			effect->getParameter("specular")->uniform1f(luminance(specular));
			effect->getParameter("shininess")->uniform1f(shininess);
			effect->getParameter("joints")->uniformBuffer(_jointData);

			_materials.push_back(effect);
		}
	}

	return true;
}

void 
SkinnedMeshRenderComponent::onActivate() except
{
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return;

	auto mesh = component->getMesh();
	if (!mesh)
		return;

	if (!_jointData)
	{
		std::size_t jointNums = mesh->getBoneArray().size();
		if (jointNums <= 64)
			jointNums = 64;
		else if (jointNums <= 128)
			jointNums = 128;
		else if (jointNums <= 256)
			jointNums = 256;
		else if (jointNums <= 320)
			jointNums = 320;

		GraphicsDataDesc jointDesc;
		jointDesc.setStride(sizeof(float4x4));
		jointDesc.setStreamSize(sizeof(float4x4) * jointNums);
		jointDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
		jointDesc.setType(GraphicsDataType::GraphicsDataTypeUniformBuffer);

		_jointData = RenderSystem::instance()->createGraphicsData(jointDesc);
	}

	if (!this->hasMaterial())
	{
		if (!_material.empty())
			_buildMaterials(_material);
		else
		{
			std::size_t numBone = mesh->getBoneArray().size();

			std::string defaultMaterial;
			if (numBone <= 64)
				defaultMaterial = "sys:fx/opacity_skinning64.fxml";
			else if (numBone <= 128)
				defaultMaterial = "sys:fx/opacity_skinning128.fxml";
			else if (numBone <= 256)
				defaultMaterial = "sys:fx/opacity_skinning256.fxml";
			else if (numBone <= 320)
				defaultMaterial = "sys:fx/opacity_skinning320.fxml";
			else
				defaultMaterial = "sys:fx/opacity_skinning0.fxml";

			_buildDefaultMaterials(defaultMaterial);
		}
	}

	_buildRenderObjects(*mesh, ModelMakerFlagBits::ModelMakerFlagBitALL);
	_attacRenderObjects();
}

void 
SkinnedMeshRenderComponent::onDeactivate() noexcept
{
	MeshRenderComponent::onDeactivate();
}

void
SkinnedMeshRenderComponent::onFrameEnd() noexcept
{
	auto meshComponent = this->getComponent<MeshComponent>();
	if (!meshComponent)
		return;

	auto mesh = meshComponent->getMesh();
	if (!mesh)
		return;

	float4x4* data;
	if (_jointData->map(0, _jointData->getGraphicsDataDesc().getStreamSize(), (void**)&data))
	{
		const auto& bones = mesh->getBoneArray();
		for (auto& bone : bones)
		{
			*data++ = bone.getLocalTransform();
		}
	}

	_jointData->unmap();

	for (auto& renderObject : _renderObjects)
		renderObject->setBoundingBox(mesh->getBoundingBox());
}

_NAME_END