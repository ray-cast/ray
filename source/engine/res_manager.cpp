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
#include <ray/res_manager.h>
#include <ray/render_system.h>
#include <ray/game_object.h>
#include <ray/mesh_component.h>
#include <ray/mesh_render_component.h>
#include <ray/skinned_mesh_render_component.h>

#include <ray/graphics_data.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_input_layout.h>

#if _BUILD_PHYSIC
#include <ray/physics_body_component.h>
#include <ray/physics_box_component.h>
#include <ray/physics_sphere_component.h>
#include <ray/physics_capsule_component.h>
#include <ray/physics_joint_configurable_component.h>
#endif

#include <ray/ik_solver_component.h>
#include <ray/image.h>
#include <ray/material.h>
#include <ray/anim_component.h>

_NAME_BEGIN

__ImplementSingleton(ResManager)

ResManager::ResManager() noexcept
{
}

ResManager::~ResManager() noexcept
{
	_textures.clear();
}

bool
ResManager::createMaterial(const util::string& name, MaterialPtr& material) noexcept
{
	material = RenderSystem::instance()->createMaterial(name);
	if (!material)
		return false;
	return true;
}

bool
ResManager::createTexture(const util::string& name, GraphicsTexturePtr& _texture, GraphicsTextureDim dim, GraphicsSamplerFilter filter, GraphicsSamplerWrap warp) noexcept
{
	if (name.empty())
		return false;

	auto texture = _textures[name];
	if (texture)
	{
		_texture = texture;
		return true;
	}

	StreamReaderPtr stream;
	if (!IoServer::instance()->openFile(stream, name))
		return false;

	image::Image image;
	if (!image.load(*stream))
		return false;

	auto imageFormat = image.format();
	GraphicsFormat format = GraphicsFormat::GraphicsFormatUndefined;
	if (imageFormat == image::format_t::BC1RGBUNormBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBUNormBlock;
	else if (imageFormat == image::format_t::BC1RGBAUNormBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock;
	else if (imageFormat == image::format_t::BC1RGBSRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock;
	else if (imageFormat == image::format_t::BC1RGBASRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock;
	else if (imageFormat == image::format_t::BC3UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC3UNormBlock;
	else if (imageFormat == image::format_t::BC3SRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC3SRGBBlock;
	else if (imageFormat == image::format_t::BC4UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC4UNormBlock;
	else if (imageFormat == image::format_t::BC4SNormBlock)
		format = GraphicsFormat::GraphicsFormatBC4SNormBlock;
	else if (imageFormat == image::format_t::BC5UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC5UNormBlock;
	else if (imageFormat == image::format_t::BC5SNormBlock)
		format = GraphicsFormat::GraphicsFormatBC5SNormBlock;
	else if (imageFormat == image::format_t::BC6HUFloatBlock)
		format = GraphicsFormat::GraphicsFormatBC6HUFloatBlock;
	else if (imageFormat == image::format_t::BC6HSFloatBlock)
		format = GraphicsFormat::GraphicsFormatBC6HSFloatBlock;
	else if (imageFormat == image::format_t::BC7UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC7UNormBlock;
	else if (imageFormat == image::format_t::BC7SRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC7SRGBBlock;
	else if (imageFormat == image::format_t::R8G8B8UNorm || imageFormat == image::format_t::R8G8B8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	else if (imageFormat == image::format_t::R8G8B8A8UNorm || imageFormat == image::format_t::R8G8B8A8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else if (imageFormat == image::format_t::B8G8R8UNorm || imageFormat == image::format_t::B8G8R8SRGB)
		format = GraphicsFormat::GraphicsFormatB8G8R8UNorm;
	else if (imageFormat == image::format_t::B8G8R8A8UNorm || imageFormat == image::format_t::B8G8R8A8SRGB)
		format = GraphicsFormat::GraphicsFormatB8G8R8A8UNorm;
	else if (imageFormat == image::format_t::R8UNorm || imageFormat == image::format_t::R8SRGB)
		format = GraphicsFormat::GraphicsFormatR8UNorm;
	else if (imageFormat == image::format_t::R8G8UNorm || imageFormat == image::format_t::R8G8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8UNorm;
	else if (imageFormat == image::format_t::R16SFloat)
		format = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (imageFormat == image::format_t::R16G16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16SFloat;
	else if (imageFormat == image::format_t::R16G16B16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16B16SFloat;
	else if (imageFormat == image::format_t::R16G16B16A16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (imageFormat == image::format_t::R32SFloat)
		format = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (imageFormat == image::format_t::R32G32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32SFloat;
	else if (imageFormat == image::format_t::R32G32B32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32B32SFloat;
	else if (imageFormat == image::format_t::R32G32B32A32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;

	if (format == GraphicsFormat::GraphicsFormatUndefined)
		return nullptr;

	GraphicsTextureDesc textureDesc;
	textureDesc.setSize(image.width(), image.height(), image.depth());
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	textureDesc.setStream(image.data());
	textureDesc.setStreamSize(image.size());
	textureDesc.setMipBase(image.mipBase());
	textureDesc.setMipLevel(image.mipLevel());
	textureDesc.setLayerBase(image.layerBase());
	textureDesc.setLayerNums(image.layerLevel());
	textureDesc.setSamplerFilter(filter);
	textureDesc.setSamplerWrap(warp);

	texture = RenderSystem::instance()->createTexture(textureDesc);
	if (!texture)
		return false;

	_texture = _textures[name] = texture;
	return true;
}

void
ResManager::destroyTexture(GraphicsTexturePtr texture) noexcept
{
	assert(texture);

	for (auto& it : _textures)
	{
		if (it.second == texture)
		{
			it.second = nullptr;
		}
	}
}

void
ResManager::destroyTexture(const util::string& name) noexcept
{
	assert(name.empty());
	_textures[name] = nullptr;
}

GraphicsTexturePtr
ResManager::getTexture(const util::string& name) noexcept
{
	assert(!name.empty());
	return _textures[name];
}

bool
ResManager::createModel(const util::string& filename, ModelPtr& model) noexcept
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, filename))
	{
		if (!model)
			model = std::make_shared<Model>();

		return model->load(*stream);
	}

	return false;
}

bool
ResManager::createGameObject(const Model& model, GameObjectPtr& gameObject) noexcept
{
	if (!gameObject)
		gameObject = std::make_shared<GameObject>();

	if (!this->createMeshes(model, gameObject))
		return false;

	GameObjects bones;
	if (!this->createBones(model, bones))
		return false;

	GameObjects rigidbodys;
	if (!this->createRigidbodyToBone(model, bones, rigidbodys))
		return false;

	GameObjects joints;
	if (!this->createJoints(model, rigidbodys, joints))
		return false;

	Materials materials;
	if (!this->createMaterials(model, materials))
		return false;

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

	return true;
}

bool
ResManager::createMeshes(const Model& model, GameObjectPtr& object) noexcept
{
	CombineMeshes combineMeshes;
	for (auto& meshProp : model.getMeshsList())
	{
		if (meshProp->getTexcoordArray().empty())
			return false;

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

	return true;
}

GraphicsDataPtr
ResManager::createIndexBuffer(const MeshProperty& mesh) noexcept
{
	std::size_t numIndices = mesh.getNumIndices();
	for (auto& it : mesh.getChildren())
		numIndices += it->getNumIndices();

	if (numIndices == 0)
		return nullptr;

	auto _buildIndiceBuffer = [&](const MeshProperty& mesh, std::size_t& offsetIndices, std::vector<std::uint8_t>& _ibo)
	{
		auto& array = mesh.getFaceArray();
		if (!array.empty())
		{
			char* indices = (char*)_ibo.data() + offsetIndices;
			std::memcpy(indices, array.data(), array.size() * sizeof(std::uint32_t));
			offsetIndices += mesh.getFaceArray().size() * sizeof(std::uint32_t);
		}
	};

	std::vector<std::uint8_t> faces(numIndices * sizeof(std::uint32_t));
	std::size_t offsetIndices = 0;

	_buildIndiceBuffer(mesh, offsetIndices, faces);

	auto subMeshCount = mesh.getChildCount();
	for (std::size_t i = 0; i < subMeshCount; i++)
	{
		auto submesh = mesh.getChildren()[i];
		_buildIndiceBuffer(*submesh, offsetIndices, faces);
	}

	GraphicsDataDesc _ib;
	_ib.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	_ib.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_ib.setStream((std::uint8_t*)faces.data());
	_ib.setStreamSize(faces.size());

	return RenderSystem::instance()->createGraphicsData(_ib);
}

GraphicsDataPtr
ResManager::createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	std::size_t numVertex = mesh.getNumVertices();
	for (auto& it : mesh.getChildren())
		numVertex += it->getNumVertices();

	if (numVertex == 0)
		return nullptr;

	std::uint32_t inputSize = 0;
	if (!mesh.getVertexArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	if (!mesh.getTangentQuatArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!mesh.getColorArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	if (!mesh.getTexcoordArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);
	if (!mesh.getWeightArray().empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
	{
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UScaled);
	}

	auto _buildVertexBuffer = [&](const MeshProperty& mesh, std::size_t& offsetVertices, std::vector<std::uint8_t>& _vbo)
	{
		auto& vertices = mesh.getVertexArray();
		auto& tangentQuats = mesh.getTangentQuatArray();
		auto& colors = mesh.getColorArray();
		auto& texcoords = mesh.getTexcoordArray();
		auto& weight = mesh.getWeightArray();

		std::size_t offset1 = 0;
		std::uint8_t* mapBuffer = (std::uint8_t*)_vbo.data();

		if (!vertices.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitVertex)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float3);
		}

		if (!tangentQuats.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTangent)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : tangentQuats)
			{
				float4 unorm = math::snorm2unorm(it);

				data[0] = math::fpToInt8UNORM(unorm.x);
				data[1] = math::fpToInt8UNORM(unorm.y);
				data[2] = math::fpToInt8UNORM(unorm.z);
				data[3] = math::fpToInt8UNORM(unorm.w);

				data += inputSize;
			}

			offset1 += 4;
		}

		if (!colors.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitColor)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float4);
		}

		if (!texcoords.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitTexcoord)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += inputSize;
			}

			offset1 += sizeof(float2);
		}

		if (!weight.empty() && flags & ModelMakerFlagBits::ModelMakerFlagBitWeight)
		{
			std::uint8_t* data = mapBuffer + offset1 + offsetVertices;
			for (auto& it : weight)
			{
				data[0] = math::fpToInt8UNORM(it.weight1);
				data[1] = math::fpToInt8UNORM(it.weight2);
				data[2] = math::fpToInt8UNORM(it.weight3);
				data[3] = math::fpToInt8UNORM(it.weight4);
				data[4] = it.bone1;
				data[5] = it.bone2;
				data[6] = it.bone3;
				data[7] = it.bone4;

				data += inputSize;
			}

			offset1 += 8;
		}

		offsetVertices += mesh.getNumVertices() * inputSize;
	};

	std::vector<std::uint8_t> _data(numVertex * inputSize);
	std::size_t offsetVertices = 0;

	_buildVertexBuffer(mesh, offsetVertices, _data);

	auto subMeshCount = mesh.getChildCount();
	for (std::size_t i = 0; i < subMeshCount; i++)
	{
		auto submesh = mesh.getChildren()[i];
		_buildVertexBuffer(*submesh, offsetVertices, _data);
	}

	GraphicsDataDesc _vb;
	_vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	_vb.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit);
	_vb.setStream((std::uint8_t*)_data.data());
	_vb.setStreamSize(_data.size());

	return RenderSystem::instance()->createGraphicsData(_vb);
}

bool
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

	return true;
}

bool
ResManager::createAnimation(const util::string& file, const GameObjects& bones, GameComponentPtr& animation) noexcept
{
	if (!file.empty())
	{
		auto animtor = std::make_shared<ray::AnimationComponent>();
		animtor->setTransforms(bones);
		animtor->play(file);

		animation = std::move(animtor);
	}

	return true;
}

bool
ResManager::createMaterials(const Model& model, Materials& materials, bool skinned) noexcept
{
	std::size_t numBones = model.getBonesList().size();

	for (auto& materialProp : model.getMaterialsList())
	{
		float opacity = 1.0;
		util::string defaultMaterial;

		materialProp->get(MATKEY_OPACITY, opacity);
		materialProp->get(MATKEY_EFFECT, defaultMaterial);

		if (defaultMaterial.empty())
		{
			if (opacity == 1.0)
			{
				if (numBones == 0 || !skinned)
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
				if (numBones == 0 || !skinned)
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

	return true;
}

bool
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

	return true;
#else
	return false;
#endif
}

bool
ResManager::createRigidbodyToBone(const Model& model, const GameObjects& bones, GameObjects& rigidbodys)
{
#if defined(_BUILD_PHYSIC)
	if (bones.empty())
		return false;

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

	return true;
#else
	return false;
#endif
}

bool
ResManager::createJoints(const Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept
{
#if defined(_BUILD_PHYSIC)
	for (auto& it : model.getJointList())
	{
		if (rigidbodys.size() <= it->bodyIndexA || rigidbodys.size() <= it->bodyIndexB)
			continue;

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

	return true;
#else
	return false;
#endif
}

MaterialPtr
ResManager::_buildDefaultMaterials(const MaterialProperty& material, const util::string& file, const util::string& directory) noexcept
{
	float3 metalness(0.5f);
	float3 diffuseColor(1.0f);
	float3 translucency;
	float4 quality(0.0f);
	float smoothness = 0.0;
	float opacity = 1.0;
	util::string diffuseTexture;
	util::string normalTexture;

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
		GraphicsTexturePtr texture;
		if (this->createTexture(directory + diffuseTexture, texture, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter::GraphicsSamplerFilterLinear))
		{
			quality.x = 1.0f;
			effect->getParameter("texDiffuse")->uniformTexture(texture);
		}
	}

	if (!normalTexture.empty())
	{
		GraphicsTexturePtr texture;
		if (this->createTexture(directory + normalTexture, texture, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter::GraphicsSamplerFilterNearest))
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

	effect->getParameter("smoothness")->uniform1f(0.2);

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