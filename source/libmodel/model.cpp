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
#include <ray/model.h>
#include "modall.h"

_NAME_BEGIN

Model::Model() noexcept
{
}

Model::~Model() noexcept
{
	this->clear();
}

bool
Model::load(StreamReader& stream, ModelType type) noexcept
{
	if (emptyHandler())
		GetModelInstanceList(*this);

	_Myhandler impl;
	if (this->find(stream, type, impl))
		return impl->doLoad(*this, stream);

	return false;
}

bool
Model::save(StreamWrite& stream, ModelType type) noexcept
{
	return false;
}

void
Model::clear() noexcept
{
	_name.clear();
	_meshes.clear();
	_textures.clear();
	_materials.clear();
	_animations.clear();
	_lights.clear();
	_cameras.clear();
}

void
Model::addMesh(MeshPropertyPtr& mesh) noexcept
{
	_meshes.push_back(mesh);
}

void
Model::addBone(BonePtr& bone) noexcept
{
	_bones.push_back(bone);
}

void
Model::addIK(IKAttrPtr& ik) noexcept
{
	_iks.push_back(ik);
}

void 
Model::addRigidbody(RigidbodyPropertyPtr& body) noexcept
{
	_rigidbodys.push_back(body);
}

void 
Model::addJoint(JointPropertyPtr& joint) noexcept
{
	_joints.push_back(joint);
}

void
Model::addTexture(TexturePropertyPtr& texture) noexcept
{
	_textures.push_back(texture);
}

void
Model::addMaterial(MaterialPropertyPtr& mat) noexcept
{
	_materials.push_back(mat);
}

void
Model::addAnimtion(AnimationPropertyPtr& anim) noexcept
{
	_animations.push_back(anim);
}

void
Model::addLight(LightPropertyPtr& light) noexcept
{
	_lights.push_back(light);
}

void
Model::addCamera(CameraPropertyPtr& camera) noexcept
{
	_cameras.push_back(camera);
}

void
Model::addMesh(MeshPropertyPtr&& mesh) noexcept
{
	_meshes.push_back(std::move(mesh));
}

void
Model::addBone(BonePtr&& bone) noexcept
{
	_bones.push_back(std::move(bone));
}

void
Model::addIK(IKAttrPtr&& ik) noexcept
{
	_iks.push_back(std::move(ik));
}

void
Model::addRigidbody(RigidbodyPropertyPtr&& body) noexcept
{
	_rigidbodys.push_back(std::move(body));
}

void
Model::addJoint(JointPropertyPtr&& joint) noexcept
{
	_joints.push_back(std::move(joint));
}

void
Model::addTexture(TexturePropertyPtr&& texture) noexcept
{
	_textures.push_back(std::move(texture));
}

void
Model::addMaterial(MaterialPropertyPtr&& mat) noexcept
{
	_materials.push_back(std::move(mat));
}

void
Model::addAnimtion(AnimationPropertyPtr&& anim) noexcept
{
	_animations.push_back(std::move(anim));
}

void
Model::addLight(LightPropertyPtr&& light) noexcept
{
	_lights.push_back(std::move(light));
}

void
Model::addCamera(CameraPropertyPtr&& camera) noexcept
{
	_cameras.push_back(std::move(camera));
}

Model::MeshList&
Model::getMeshsList() noexcept
{
	return _meshes;
}

Model::BoneList&
Model::getBonesList() noexcept
{
	return _bones;
}

Model::IKList&
Model::getIKList() noexcept
{
	return _iks;
}

Model::RigidbodyList&
Model::getRigidbodyList() noexcept
{
	return _rigidbodys;
}

Model::JointList&
Model::getJointList() noexcept
{
	return _joints;
}

Model::TextureList&
Model::getTexturesList() noexcept
{
	return _textures;
}

Model::MaterialList&
Model::getMaterialsList() noexcept
{
	return _materials;
}

Model::AnimList&
Model::getAnimationList() noexcept
{
	return _animations;
}

Model::LightList&
Model::getLightList() noexcept
{
	return _lights;
}

Model::CameraList&
Model::getCameraList() noexcept
{
	return _cameras;
}

const std::string&
Model::getName() const noexcept
{
	return _name;
}

void 
Model::setDirectory(const std::string& name) noexcept
{
	_directory = name;
}

const std::string&
Model::getDirectory() const noexcept
{
	return _directory;
}

const Model::MeshList&
Model::getMeshsList() const noexcept
{
	return _meshes;
}

const Model::BoneList&
Model::getBonesList() const noexcept
{
	return _bones;
}

const Model::IKList&
Model::getIKList() const noexcept
{
	return _iks;
}

const Model::RigidbodyList&
Model::getRigidbodyList() const noexcept
{
	return _rigidbodys;
}

const Model::JointList&
Model::getJointList() const noexcept
{
	return _joints;
}

const Model::TextureList&
Model::getTexturesList() const noexcept
{
	return _textures;
}

const Model::MaterialList&
Model::getMaterialsList() const noexcept
{
	return _materials;
}

const Model::AnimList&
Model::getAnimationList() const noexcept
{
	return _animations;
}

const Model::LightList&
Model::getLightList() const noexcept
{
	return _lights;
}

const Model::CameraList&
Model::getCameraList() const noexcept
{
	return _cameras;
}

bool
Model::hasMeshes() const noexcept
{
	return !_meshes.empty();
}

bool 
Model::hasBones() const noexcept
{
	return !_bones.empty();
}

bool
Model::hasIKs() const noexcept
{
	return !_iks.empty();
}

bool
Model::hasMaterials() const noexcept
{
	return !_materials.empty();
}

bool
Model::hasLights() const noexcept
{
	return !_lights.empty();
}

bool
Model::hasTextures() const noexcept
{
	return !_textures.empty();
}

bool
Model::hasCameras() const noexcept
{
	return !_cameras.empty();
}

bool
Model::hasAnimations() const noexcept
{
	return !_animations.empty();
}

void
Model::applyProcess(int) noexcept
{
}

bool
Model::emptyHandler() const noexcept
{
	return _handlers.empty();
}

bool
Model::addHandler(_Myhandler handler) noexcept
{
	assert(handler);

	std::string baked;

	for (auto it : _handlers)
	{
		if (it == handler)
		{
			return false;
		}
	}

	_handlers.push_back(handler);

	return true;
}

bool
Model::removeHandler(_Myhandler handler) noexcept
{
	assert(handler);

	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it != _handlers.end())
	{
		if (*it == handler)
		{
			_handlers.erase(it);
			return true;
		}
	}

	return false;
}

bool
Model::find(ModelType type, _Myhandler& out) const noexcept
{
	std::size_t index = (std::size_t)type;
	if (_handlers.size() < index)
	{
		out = _handlers[index];
		return true;
	}

	return false;
}

bool
Model::find(StreamReader& stream, _Myhandler& out) const noexcept
{
	if (!stream.good())
		return false;

	for (auto& it : _handlers)
	{
		stream.seekg(0, std::ios_base::beg);

		if (it->doCanRead(stream))
		{
			stream.seekg(0, std::ios_base::beg);

			out = it;
			return true;
		}
	}

	return false;
}

bool
Model::find(StreamReader& stream, ModelType type, _Myhandler& out) const noexcept
{
	if (type != MT_UNKNOWN)
	{
		if (this->find(type, out))
		{
			return true;
		}
	}

	return this->find(stream, out);
}

_NAME_END