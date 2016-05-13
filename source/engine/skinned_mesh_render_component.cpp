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
#include <ray/graphics_data.h>
#include <ray/geometry.h>
#include <ray/render_system.h>
#include <ray/mesh_component.h>
#include <ray/material.h>

_NAME_BEGIN

__ImplementSubClass(SkinnedMeshRenderComponent, MeshRenderComponent, "SkinnedMeshRender")

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent() noexcept
	: _onMeshChange(std::bind(&SkinnedMeshRenderComponent::onMeshChange, this))
	, _onMeshWillRender(std::bind(&SkinnedMeshRenderComponent::onMeshWillRender, this, std::placeholders::_1))
{
}

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent(MaterialPtr& material, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterial(material);
	else
		this->setMaterial(material);
}

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent(MaterialPtr&& material, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterial(material);
	else
		this->setMaterial(material);
}

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent(const Materials& materials, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterials(materials);
	else
		this->setMaterials(materials);
}

SkinnedMeshRenderComponent::SkinnedMeshRenderComponent(Materials&& materials, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterials(materials);
	else
		this->setMaterials(materials);
}

SkinnedMeshRenderComponent::~SkinnedMeshRenderComponent() noexcept
{
}

void 
SkinnedMeshRenderComponent::setTransforms(const GameObjects& transforms) noexcept
{
	_transforms = transforms;
}

void 
SkinnedMeshRenderComponent::setTransforms(GameObjects&& transforms) noexcept
{
	_transforms = std::move(transforms);
}

const GameObjects&
SkinnedMeshRenderComponent::getTransforms() const noexcept
{
	return _transforms;
}

void 
SkinnedMeshRenderComponent::onActivate() except
{
	if (_transforms.empty())
		return;

	if (!_jointData)
	{
		std::size_t jointNums = _transforms.size();
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

	if (this->hasMaterial())
	{
		auto& materials = this->getMaterials();
		for (auto& it : materials)
		{
			auto param = it->getParameter("joints");
			if (param)
				param->uniformBuffer(_jointData);
		}
	}

	auto meshComponent = this->getComponent<MeshComponent>();
	if (meshComponent)
		_mesh = meshComponent->getMesh();

	this->addPreRenderListener(&_onMeshWillRender);

	this->addComponentDispatch(GameDispatchType::GameDispatchTypeFrameEnd, this);

	MeshRenderComponent::onActivate();
}

void 
SkinnedMeshRenderComponent::onDeactivate() noexcept
{
	MeshRenderComponent::onDeactivate();

	this->removePreRenderListener(&_onMeshWillRender);

	_mesh.reset();
	_jointData.reset();

	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeFrameEnd, this);
}

void 
SkinnedMeshRenderComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
	{
		component->downcast<MeshComponent>()->addMeshChangeListener(&_onMeshChange);
		_mesh = component->downcast<MeshComponent>()->getMesh();
	}
}

void 
SkinnedMeshRenderComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
	{
		component->downcast<MeshComponent>()->removeMeshChangeListener(&_onMeshChange);
		_mesh = nullptr;
	}		
}

void
SkinnedMeshRenderComponent::onMeshChange() noexcept
{
	_mesh = this->getComponent<MeshComponent>()->getMesh();
}

void 
SkinnedMeshRenderComponent::onMeshWillRender(class RenderPipeline&) noexcept
{
	if (!_mesh)
		return;

	if (_needUpdate)
	{
		float4x4* data;
		if (_jointData->map(0, _jointData->getGraphicsDataDesc().getStreamSize(), (void**)&data))
		{
			auto& bindposes = _mesh->getBindposes();
			if (bindposes.size() != _transforms.size())
			{
				std::size_t size = _transforms.size();
				for (std::size_t i = 0; i < size; ++i)
					*data++ = float4x4::One;
			}
			else
			{
				std::size_t index = 0;
				for (auto& transform : _transforms)
					*data++ = math::transformMultiply(transform->getWorldTransform(), bindposes[index++]);
			}
		}

		_jointData->unmap();

		_needUpdate = false;
	}
}

void
SkinnedMeshRenderComponent::onFrameEnd() noexcept
{
	_needUpdate = true;

	AABB aabb;
	for (auto& transform : _transforms)
		aabb.encapsulate(transform->getWorldTranslate());

	_boundingBox.set(aabb);

	for (auto& renderObject : _renderObjects)
		renderObject->setBoundingBox(_boundingBox);
}

_NAME_END