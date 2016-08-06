// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/res_manager.h>
#include <ray/render_system.h>
#include <ray/game_object.h>
#include <ray/mesh_component.h>
#include <ray/mesh_render_component.h>
#include <ray/skinned_mesh_render_component.h>

#if _BUILD_PHYSIC
#include <ray/physics_body_component.h>
#include <ray/physics_box_component.h>
#include <ray/physics_sphere_component.h>
#include <ray/physics_capsule_component.h>
#include <ray/physics_joint_configurable_component.h>
#endif

#include <ray/ik_solver_component.h>
#include <ray/material.h>
#include <ray/anim_component.h>

_NAME_BEGIN

__ImplementSingleton(ResManager)

ResManager::ResManager() noexcept
{
}

ResManager::~ResManager() noexcept
{
}

MaterialPtr
ResManager::createMaterial(const std::string& name) noexcept
{
	auto material = RenderSystem::instance()->createMaterial(name);
	if (!material)
		return nullptr;
	return material;
}

GraphicsTexturePtr
ResManager::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter) noexcept
{
	auto texture = RenderSystem::instance()->createTexture(name, dim, filter);
	if (!texture)
		return nullptr;
	return texture;
}

GameObjectPtr
ResManager::createGameObject(const std::string& name, const std::string& anim) noexcept
{
	ResLoader<Model> resource;
	if (!this->loadModel(name, resource))
		return nullptr;

	auto model = resource.getInstance();
	if (!model->hasMeshes())
		return nullptr;

	auto gameObject = std::make_shared<GameObject>();
	this->createMeshes(*model, gameObject);

	GameObjects bones;
	this->createBones(*model, bones);

	this->createAnimation(*model, gameObject, bones, anim);

	GameObjects rigidbodys;
	this->createRigidbodyToBone(*model, rigidbodys, bones);

	GameObjects joints;
	this->createJoints(*model, rigidbodys, joints);

	Materials materials;
	this->createMaterials(*model, materials);

	if (materials.size() > 0)
	{
		if (bones.empty())
			gameObject->addComponent(std::make_shared<MeshRenderComponent>(std::move(materials)));
		else
		{
			auto smr = std::make_shared<SkinnedMeshRenderComponent>();
			smr->setMaterials(std::move(materials));
			smr->setTransforms(bones);

			gameObject->addComponent(smr);
		}
	}

	return gameObject;
}

bool
ResManager::loadModel(const std::string& filename, ResLoader<Model>& model) noexcept
{
	model.load(filename,
		[&](ray::ModelPtr model, const std::string& filename)
	{
		model->setDirectory(util::directory(filename));

		StreamReaderPtr stream;
		if (IoServer::instance()->openFile(stream, filename))
			return model->load(*stream);
		return false;
	});

	if (!model.isLoaded())
		return false;

	return true;
}

void
ResManager::createMeshes(const Model& model, GameObjectPtr& object) noexcept
{
	CombineMeshes combineMeshes;
	for (auto& meshProp : model.getMeshsList())
	{
		if (meshProp->getTangentArray().empty())
			meshProp->computeTangents();
		if (meshProp->getTangentQuatArray().empty())
			meshProp->computeTangentQuats();
		combineMeshes.push_back(CombineMesh(std::move(meshProp)));
	}

	if (combineMeshes.size() > 0)
	{
		auto mesh = std::make_shared<MeshProperty>();
		mesh->combineMeshes(combineMeshes, false);

		if (model.getBonesList().size() > 0)
		{
			std::vector<float4x4> bindposes;
			for (auto& bone : model.getBonesList())
			{
				float4x4 bindpose;
				bindpose.makeTranslate(-bone->getPosition());
				bindposes.push_back(bindpose);
			}

			mesh->setBindposes(std::move(bindposes));
		}

		object->addComponent(std::make_shared<MeshComponent>(mesh));
	}
}

void
ResManager::createBones(const Model& model, GameObjects& bones) noexcept
{
	for (auto& it : model.getBonesList())
	{
		auto bone = std::make_shared<GameObject>();
		bone->setName(it->getName());
		bone->setWorldTranslate(it->getPosition());
		bone->setActive(true);

		bones.push_back(std::move(bone));
	}

	std::size_t index = 0;
	for (auto& it : model.getBonesList())
	{
		auto parent = it->getParent();
		if (parent >= 0 && (std::size_t)parent < bones.size())
			bones[index]->setParent(bones[parent]);
		++index;
	}

	for (auto& it : model.getIKList())
	{
		auto iksolver = std::make_shared<IKSolverComponent>();
		iksolver->setTargetBone(bones[it->targetBoneIndex]);
		iksolver->setIterations(it->iterations);
		iksolver->setChainLength(it->chainLength);

		for (auto& child : it->child)
		{
			auto bone = std::make_shared<IKSolverBone>();
			bone->bone = bones[child.boneIndex];
			bone->angleWeight = child.angleWeight;
			bone->rotateLimited = child.rotateLimited;
			bone->minimumDegrees = child.minimumDegrees;
			bone->maximumDegrees = child.maximumDegrees;

			iksolver->addBone(bone);
		}

		bones[it->boneIndex]->addComponent(std::move(iksolver));
	}
}

void 
ResManager::createAnimation(const Model& model, GameObjectPtr& gameObject, GameObjects& bones, const std::string& file) noexcept
{
	if (!file.empty())
	{
		auto animtor = std::make_shared<ray::AnimationComponent>();
		animtor->setTransforms(bones);
		animtor->play(file);

		gameObject->addComponent(animtor);
	}
}

void
ResManager::createMaterials(const Model& model, Materials& materials) noexcept
{
	std::size_t numBones = model.getBonesList().size();

	for (auto& materialProp : model.getMaterialsList())
	{
		float opacity = 1.0;
		std::string defaultMaterial;

		materialProp->get(MATKEY_OPACITY, opacity);
		materialProp->get(MATKEY_EFFECT, defaultMaterial);

		if (defaultMaterial.empty())
		{
			if (opacity == 1.0)
			{
				if (numBones == 0)
					defaultMaterial = "sys:fx/opacity_skinning0.fxml";
				else if (numBones <= 64)
					defaultMaterial = "sys:fx/opacity_skinning64.fxml";
				else if (numBones <= 128)
					defaultMaterial = "sys:fx/opacity_skinning128.fxml";
				else if (numBones <= 256)
					defaultMaterial = "sys:fx/opacity_skinning256.fxml";
				else
					defaultMaterial = "sys:fx/opacity_skinning0.fxml";
			}
			else
			{
				if (numBones == 0)
					defaultMaterial = "sys:fx/transparent_skinning0.fxml";
				else if (numBones <= 64)
					defaultMaterial = "sys:fx/transparent_skinning64.fxml";
				else if (numBones <= 128)
					defaultMaterial = "sys:fx/transparent_skinning128.fxml";
				else if (numBones <= 256)
					defaultMaterial = "sys:fx/transparent_skinning256.fxml";
				else
					defaultMaterial = "sys:fx/transparent_skinning0.fxml";
			}
		}

		MaterialPtr material;
		material = _buildDefaultMaterials(*materialProp, defaultMaterial, model.getDirectory());
		if (!material)
			continue;

		materials.push_back(std::move(material));
	}
}

void
ResManager::createRigidbodys(const Model& model, GameObjects& rigidbodys) noexcept
{
#if defined(_BUILD_PHYSIC)
	for (auto& it : model.getRigidbodyList())
	{
		auto gameObject = std::make_shared<GameObject>();
		gameObject->setName(it->name);
		gameObject->setLayer(it->group);
		gameObject->setQuaternion(Quaternion(RAD_TO_DEG(it->rotation)));
		gameObject->setWorldTranslate(it->position);
		
		if (it->shape == ShapeType::ShapeTypeCircle)
			gameObject->addComponent(std::make_shared<PhysicsSphereComponent>(it->scale.x));
		else if (it->shape == ShapeType::ShapeTypeSquare)
			gameObject->addComponent(std::make_shared<PhysicsBoxComponent>(it->scale));
		else if (it->shape == ShapeType::ShapeTypeCapsule)
			gameObject->addComponent(std::make_shared<PhysicsCapsuleComponent>(it->scale.x, it->scale.y));

		auto component = std::make_shared<PhysicsBodyComponent>();
		component->setName(it->name);
		component->setMass(it->mass);
		component->setRestitution(it->elasticity);
		component->setFriction(it->friction);
		component->setLinearDamping(it->movementDecay);
		component->setAngularDamping(it->rotationDecay);

		if (it->physicsOperation == 0)
			component->isKinematic(true);

		gameObject->addComponent(component);

		rigidbodys.push_back(std::move(gameObject));
	}
#endif
}

void
ResManager::createRigidbodyToBone(const Model& model, GameObjects& rigidbodys, GameObjects& bones)
{
#if defined(_BUILD_PHYSIC)
	if (bones.empty())
		return;

	for (auto& it : model.getRigidbodyList())
	{
		if (bones.size() < it->bone)
			continue;

		auto gameObject = std::make_shared<GameObject>();
		gameObject->setActive(true);
		gameObject->setParent(bones[it->bone]);
		gameObject->setName(it->name);
		gameObject->setLayer(it->group);
		gameObject->setQuaternion(Quaternion(RAD_TO_DEG(it->rotation)));
		gameObject->setWorldTranslate(it->position);

		if (it->shape == ShapeType::ShapeTypeCircle)
			gameObject->addComponent(std::make_shared<PhysicsSphereComponent>(it->scale.x));
		else if (it->shape == ShapeType::ShapeTypeSquare)
			gameObject->addComponent(std::make_shared<PhysicsBoxComponent>(it->scale));
		else if (it->shape == ShapeType::ShapeTypeCapsule)
			gameObject->addComponent(std::make_shared<PhysicsCapsuleComponent>(it->scale.x, it->scale.y));

		auto component = std::make_shared<PhysicsBodyComponent>();
		component->setName(it->name);
		component->setCollisionMask(it->groupMask);
		component->setMass(it->mass);
		component->setRestitution(it->elasticity);
		component->setFriction(it->friction);
		component->setLinearDamping(it->movementDecay);
		component->setAngularDamping(it->rotationDecay);

		if (it->physicsOperation == 0)
			component->isKinematic(true);

		gameObject->addComponent(component);

		rigidbodys.push_back(std::move(gameObject));
	}
#endif
}

void
ResManager::createJoints(const Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept
{
#if defined(_BUILD_PHYSIC)
	for (auto& it : model.getJointList())
	{
		auto transformA = rigidbodys[it->bodyIndexA];
		auto trasnformB = rigidbodys[it->bodyIndexB];

		auto bodyA = transformA->getComponent<PhysicsBodyComponent>();
		auto bodyB = trasnformB->getComponent<PhysicsBodyComponent>();

		if (!bodyA)
			bodyA = transformA->getParent()->getComponent<PhysicsBodyComponent>();

		if (!bodyB)
			bodyB = trasnformB->getParent()->getComponent<PhysicsBodyComponent>();

		if (bodyA != bodyB)
		{
			auto joint = std::make_shared<PhysicsJointConfigurableComponent>();
			joint->setActive(true);
			joint->setLinearSpring(it->position);
			joint->setAngularSprint(Quaternion(RAD_TO_DEG(it->rotation)));
			joint->setLinearLowerLimit(it->movementLowerLimit);
			joint->setLinearHighLimit(it->movementUpperLimit);
			joint->setAngularLowerLimit(it->rotationLowerLimit);
			joint->setAngularHighLimit(it->rotationUpperLimit);
			joint->setMovementConstant(it->springMovementConstant);
			joint->setRotationConstant(it->springRotationConstant);
			joint->setConnectRigidbody(bodyB);

			bodyA->getGameObject()->addComponent(joint);

			joints.push_back(joint->getGameObject());
		}
	}
#endif
}

MaterialPtr
ResManager::_buildDefaultMaterials(const MaterialProperty& material, const std::string& file, const std::string& directory) noexcept
{
	float3 metalness(0.5f);
	float3 diffuseColor(1.0f);
	float3 translucency;
	float4 quality(0.0f);
	float smoothness = 0.0;
	float opacity = 1.0;
	std::string diffuseTexture;
	std::string normalTexture;

	material.get(MATKEY_OPACITY, opacity);
	material.get(MATKEY_SHININESS, smoothness);
	material.get(MATKEY_COLOR_DIFFUSE, diffuseColor);
	material.get(MATKEY_COLOR_SPECULAR, metalness);
	material.get(MATKEY_COLOR_AMBIENT, translucency);
	material.get(MATKEY_TEXTURE_DIFFUSE(0), diffuseTexture);
	material.get(MATKEY_TEXTURE_NORMALS(0), normalTexture);

	MaterialPtr effect;
	effect = RenderSystem::instance()->createMaterial(file);
	if (!effect)
		return nullptr;

	auto luminance = [](const float3& rgb)
	{
		const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
		return math::dot(rgb, lumfact);
	};

	if (!diffuseTexture.empty())
	{
		auto texture = RenderSystem::instance()->createTexture(directory + diffuseTexture, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
		if (texture)
		{
			quality.x = 1.0f;
			effect->getParameter("texDiffuse")->uniformTexture(texture);
		}
	}

	if (!normalTexture.empty())
	{
		auto texture = RenderSystem::instance()->createTexture(directory + normalTexture, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
		if (texture)
		{
			quality.y = 1.0f;
			effect->getParameter("texNormal")->uniformTexture(texture);
		}
	}

	effect->getParameter("quality")->uniform4f(quality);
	effect->getParameter("diffuse")->uniform3f(diffuseColor);

	if (effect->getParameter("metalness"))
		effect->getParameter("metalness")->uniform1f(metalness.x);
	else if (effect->getParameter("specular"))
		effect->getParameter("specular")->uniform3f(metalness);

	effect->getParameter("smoothness")->uniform1f(smoothness);

	auto transmittance = effect->getParameter("transmittance"); 
	if (transmittance)
		transmittance->uniform3f(translucency);

	if (opacity < 1.0f)
	{
		if (effect->getParameter("opacity"))
			effect->getParameter("opacity")->uniform1f(opacity);
	}

	return effect;
}

_NAME_END