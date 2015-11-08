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
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>

#include <ray/render_factory.h>
#include <ray/render_buffer.h>

#include <ray/material_maker.h>
#include <ray/resource.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent)

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

	for (auto& it : this->getSharedMaterials())
	{
		result->addMaterial(it);
		result->addSharedMaterial(it);
	}

	return result;
}

void
MeshRenderComponent::onActivate() except
{
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (component)
	{
		auto mesh = component->getMesh();
		if (!mesh)
			return;

		auto model = ResLoader<Model>::find(component->getName());
		if (!model)
			return;

		buildMaterials(model);
		buildRenderObjects(mesh);

		_attacRenderObjects();
	}
}

void
MeshRenderComponent::buildMaterials(ModelPtr model) except
{
	if (!this->getName().empty())
		return;

	auto materials = model->getMaterialsList();
	for (auto& it : materials)
	{
		std::string diffuseName;
		std::string normalName;
		std::string specularName;
		TexturePtr diffuseTexture;
		TexturePtr normalTexture;
		TexturePtr specularTexture;

		float transparent = 1;
		float shininess = 0;
		Vector3 diffuse = Vector3::Zero;
		Vector3 specular = Vector3::Zero;
		it->get(MATKEY_TEXTURE_DIFFUSE(0), diffuseName);
		it->get(MATKEY_TEXTURE_NORMALS(0), normalName);
		it->get(MATKEY_TEXTURE_SPECULAR(0), specularName);
		it->get(MATKEY_OPACITY, transparent);
		it->get(MATKEY_SHININESS, shininess);
		it->get(MATKEY_COLOR_DIFFUSE, diffuse);
		it->get(MATKEY_COLOR_SPECULAR, specular);

		if (!diffuseName.empty())
		{
			const std::string& directory = model->getDirectory();
			if (!directory.empty())
			{
				diffuseTexture = RenderFactory::createTexture(directory + diffuseName);
				/*if (diffuseTexture->getTexFormat() == PixelFormat::R8G8B8A8)
					transparent = 0;*/
			}
		}

		if (!normalName.empty())
		{
			const std::string& directory = model->getDirectory();
			if (!directory.empty())
			{
				normalTexture = RenderFactory::createTexture(directory + normalName);
			}
		}

		if (!specularName.empty())
		{
			const std::string& directory = model->getDirectory();
			if (!directory.empty())
			{
				specularTexture = RenderFactory::createTexture(directory + specularName);
			}
		}

		MaterialPtr material = nullptr;

		MaterialMaker maker;
		if (normalTexture)
		{
			if (transparent == 1)
				material = maker.load("sys:fx/opacity_normal.glsl");
			else
				material = maker.load("sys:fx/specific.glsl");
		}
		else
		{
			if (transparent == 1)
				material = maker.load("sys:fx/opacity.glsl");
			else
				material = maker.load("sys:fx/specific.glsl");
		}

		if (normalTexture)
		{
			material->getParameter("texNormal")->assign(normalTexture);
		}

		if (diffuseTexture)
		{
			material->getParameter("diffuse")->assign(Vector4(diffuse, 0));
			material->getParameter("texDiffuse")->assign(diffuseTexture);
		}
		else
		{
			material->getParameter("diffuse")->assign(Vector4(diffuse, 1));
		}

		if (material->getParameter("opacity"))
			material->getParameter("opacity")->assign(transparent);

		if (specularTexture)
		{
			material->getParameter("texSpecular")->assign(specularTexture);
			material->getParameter("specular")->assign(Vector4(specular, 0));
		}
		else
		{
			material->getParameter("shininess")->assign(shininess / 8192);
			material->getParameter("specular")->assign(Vector4(specular, 1));
		}

		this->addMaterial(material);
		this->addSharedMaterial(material);
	}
}

void
MeshRenderComponent::buildRenderObjects(MeshPropertyPtr mesh) noexcept
{
	auto children = mesh->getChildren();

	std::vector<MeshPropertyPtr> meshes;
	meshes.push_back(mesh);
	meshes.insert(meshes.end(), children.begin(), children.end());

	auto renderBuffer = RenderFactory::createRenderBuffer(meshes);
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
	auto material = this->getMaterial(mesh->getMaterialID());
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