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
#include <ray/skinned_joint_render_component.h>
#include <ray/render_system.h>
#include <ray/material.h>
#include <ray/geometry.h>
#include <ray/render_feature.h>
#include <ray/game_server.h>
#include <ray/graphics_data.h>

_NAME_BEGIN

__ImplementSubClass(SkinnedJointRenderComponent, MeshRenderComponent, "SkinnedJointRender")

SkinnedJointRenderComponent::SkinnedJointRenderComponent() noexcept
{
}

SkinnedJointRenderComponent::~SkinnedJointRenderComponent() noexcept
{
}

bool
SkinnedJointRenderComponent::_buildJointObject() noexcept
{
	MeshProperty meshes;
	auto& vertices = meshes.getVertexArray();

	for (auto& transfrom : _trasnforms)
	{
		if (transfrom->getParent())
		{
			vertices.push_back(transfrom->getParent()->getTranslate());
			vertices.push_back(transfrom->getTranslate());
		}
	}

	meshes.computeBoundingBox();

	_renderMeshVbo = ResManager::instance()->createVertexBuffer(meshes, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_renderMeshVbo)
		return false;

	_geometry = std::make_shared<Geometry>();
	_geometry->setMaterial(RenderSystem::instance()->createMaterial("sys:fx/debug.fxml"));
	_geometry->setRenderScene(GameServer::instance()->getFeature<RenderFeature>()->getRenderScene());
	_geometry->setVertexBuffer(_renderMeshVbo, 0);
	_geometry->setIndexBuffer(_renderMeshIbo, 0, GraphicsIndexType::GraphicsIndexTypeUInt32);
	_geometry->setBoundingBox(meshes.getBoundingBox());
	_geometry->setOwnerListener(this);
	_geometry->setCastShadow(this->getCastShadow());
	_geometry->setReceiveShadow(this->getReceiveShadow());
	_geometry->setLayer(this->getGameObject()->getLayer());
	_geometry->setTransform(this->getGameObject()->getWorldTransform());
	_geometry->setGraphicsIndirect(std::make_shared<GraphicsIndirect>(meshes.getNumVertices(), meshes.getNumIndices()));

	return true;
}

void
SkinnedJointRenderComponent::onActivate() except
{
	_buildJointObject();
}

void
SkinnedJointRenderComponent::onDeactivate() noexcept
{
}

void
SkinnedJointRenderComponent::onFrameEnd() noexcept
{
	float3* data;
	if (_renderMeshVbo->map(0, _renderMeshVbo->getGraphicsDataDesc().getStreamSize(), (void**)&data))
	{
		for (auto& transform : _trasnforms)
		{
			if (transform->getParent())
			{
				*data++ = transform->getParent()->getTranslate();
				*data++ = transform->getTranslate();
			}
		}

		_renderMeshVbo->unmap();
	}
}

_NAME_END