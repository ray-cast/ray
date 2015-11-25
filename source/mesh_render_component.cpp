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
#if defined(_BUILD_RENDERER)
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>

#include <ray/render_system.h>
#include <ray/render_buffer.h>

#include <ray/resource.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent, "MeshRender")

MeshRenderComponent::MeshRenderComponent() noexcept
{
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshRenderComponent>();
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());
	result->setName(this->getName());
	result->setMaterial(this->getSharedMaterial());
	result->setSharedMaterial(this->getSharedMaterial());
	return result;
}

void 
MeshRenderComponent::onAttachComponent() except
{
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (component)
	{
		component->addMeshListener(this);
	}
}

void 
MeshRenderComponent::onDetachComponent() except
{
}

void
MeshRenderComponent::onActivate() except
{
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (component)
	{
		component->addMeshListener(this);

		auto mesh = component->getMesh();
		if (!mesh)
			return;

		auto model = ResLoader<Model>::find(component->getName());
		if (!model)
			return;

		buildMaterials();
		buildRenderObjects(mesh);

		_attacRenderObjects();
	}
}

void
MeshRenderComponent::onDeactivate() except
{
	this->_dettachRenderhObjects();
}

void 
MeshRenderComponent::onMeshChangeAfter() except
{
	if (this->getActive())
	{
		auto component = this->getGameObject()->getComponent<MeshComponent>();
		if (component)
		{
			auto mesh = component->getMesh();
			if (!mesh)
				return;

			buildRenderObjects(mesh);
		}
	}
}

void
MeshRenderComponent::buildMaterials() except
{
	if (!this->hasSharedMaterial())
	{
		if (!this->getName().empty())
		{
			auto material = RenderSystem::instance()->createMaterial(this->getName());
			if (material)
			{
				this->setMaterial(material);
				this->setSharedMaterial(material);
			}
		}
	}

	this->_attacRenderObjects();
}

void
MeshRenderComponent::buildRenderObjects(MeshPropertyPtr mesh) noexcept
{
	auto children = mesh->getChildren();

	std::vector<MeshPropertyPtr> meshes;
	meshes.push_back(mesh);
	meshes.insert(meshes.end(), children.begin(), children.end());

	auto renderBuffer = RenderSystem::instance()->createRenderBuffer(meshes);
	if (children.empty())
	{
		auto renderObject = this->buildRenderObject(mesh, renderBuffer);
		if (renderObject)
		{
			this->_setRenderObject(renderObject);
		}
	}
	else
	{
		std::size_t startVertice = 0;
		std::size_t startIndice = 0;

		RenderObjectPtr root = std::make_shared<RenderMesh>();
		root->setBoundingBox(mesh->getBoundingBoxDownwards());

		for (auto& it : meshes)
		{
			auto submesh = this->buildRenderObject(it, renderBuffer);
			if (submesh)
			{
				auto renderable = submesh->getRenderIndirect();

#if !defined(EGLAPI)
				renderable->startVertice = startVertice;
#else
				renderable->startVertice = 0;
#endif
				renderable->startIndice = startIndice;
				renderable->numVertices = it->getNumVertices();
				renderable->numIndices = it->getNumIndices();

				startVertice += it->getNumVertices();
				startIndice += it->getNumIndices();

				root->addSubRenderObject(submesh);
			}
		};

		this->_setRenderObject(root);
	}
}

RenderObjectPtr
MeshRenderComponent::buildRenderObject(MeshPropertyPtr mesh, RenderBufferPtr buffer) noexcept
{
	auto material = this->getMaterial();
	if (material)
	{
		auto renderObject = std::make_shared<RenderMesh>();
		renderObject->setRenderBuffer(buffer);
		renderObject->setBoundingBox(mesh->getBoundingBox());
		renderObject->setRenderListener(this);

		renderObject->setMaterial(material);

		renderObject->setCastShadow(this->getCastShadow());
		renderObject->setReceiveShadow(this->getReceiveShadow());

		renderObject->setLayer(this->getGameObject()->getLayer());

		renderObject->setTransform(this->getGameObject()->getTransform());
		renderObject->setTransformInverse(this->getGameObject()->getTransformInverse());
		renderObject->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());

		return renderObject;
	}

	return nullptr;
}

_NAME_END
#endif