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
#include <ray/mesh_component.h>
#include <ray/res_loader.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <stack>

_NAME_BEGIN

__ImplementSubClass(MeshComponent, GameComponent, "Mesh")

MeshComponent::MeshComponent() noexcept
{
}

MeshComponent::MeshComponent(MeshPropertyPtr mesh, bool shared) noexcept
{
	if (shared)
		this->setSharedMesh(mesh);
	else
		this->setMesh(mesh);
}

MeshComponent::~MeshComponent() noexcept
{
}

void
MeshComponent::setMesh(MeshPropertyPtr& mesh) noexcept
{
	if (_mesh != mesh)
	{
		this->_onSetMesh(mesh);
		_mesh = mesh;
		this->needUpdate();
	}
}

void
MeshComponent::setSharedMesh(MeshPropertyPtr& mesh) noexcept
{
	if (_sharedMesh != mesh)
	{
		this->_onSetMesh(mesh);
		_sharedMesh = mesh;
		this->needUpdate();
	}
}

void
MeshComponent::setMesh(MeshPropertyPtr&& mesh) noexcept
{
	this->setMesh(mesh);
}

void
MeshComponent::setSharedMesh(MeshPropertyPtr&& mesh) noexcept
{
	this->setSharedMesh(mesh);
}

MeshPropertyPtr
MeshComponent::getMesh() const noexcept
{
	if (_mesh)
		return _mesh;
	else if (_sharedMesh)
		return _sharedMesh;
	return nullptr;
}

MeshPropertyPtr
MeshComponent::getSharedMesh() const noexcept
{
	return _sharedMesh;
}

std::size_t
MeshComponent::getNumVertices() const noexcept
{
	return _mesh->getNumVertices();
}

std::size_t
MeshComponent::getNumIndices() const noexcept
{
	return _mesh->getNumIndices();
}

Bound
MeshComponent::getBoundingBox() const noexcept
{
	if (_mesh)
		_mesh->getBoundingBox();
	else if (_sharedMesh)
		_sharedMesh->getBoundingBox();
	return Bound::Empty;
}

Bound
MeshComponent::getBoundingBoxDownwards() const noexcept
{
	if (_mesh)
		_mesh->getBoundingBoxDownwards();
	else if (_sharedMesh)
		_sharedMesh->getBoundingBoxDownwards();
	return Bound::Empty;
}

void
MeshComponent::addMeshChangeListener(std::function<void()>* func) noexcept
{
	assert(!_onMeshChange.find(func));
	_onMeshChange.attach(func);
}

void
MeshComponent::removeMeshChangeListener(std::function<void()>* func) noexcept
{
	assert(_onMeshChange.find(func));
	_onMeshChange.remove(func);
}

void
MeshComponent::needUpdate() noexcept
{
	_onMeshChange.run();
}

void
MeshComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	ResLoader<Model> model;

	model.load(this->getName(),
		[&](ray::ModelPtr model, const std::string& filename)
	{
		StreamReaderPtr stream;
		if (IoServer::instance()->openFile(stream, filename))
			return model->load(*stream);
		return false;
	});

	if (model.isLoaded())
	{
		auto instance = model.getInstance();
		if (instance->hasMeshes())
		{
			instance->setDirectory(util::directory(this->getName()));

			auto& meshes = instance->getMeshsList();

			MeshPropertyPtr root = meshes.front();

			if (root)
			{
				for (auto& it : meshes)
				{
					if (it->getTangentArray().empty())
						it->computeTangents();

					if (it->getTangentQuatArray().empty())
						it->computeTangentQuats();

					it->computeBoundingBox();
					if (it !=  root)
						root->addChild(it);
				}

				this->setSharedMesh(root);
			}
		}
	}
}

void
MeshComponent::save(oarchive& write) noexcept
{
}

GameComponentPtr
MeshComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshComponent>();
	result->setName(this->getName());
	result->setActive(this->getActive());
	result->setSharedMesh(this->getSharedMesh());

	if (this->getMesh())
		result->setMesh(this->getMesh()->clone());

	return result;
}

void
MeshComponent::_onSetMesh(MeshPropertyPtr& mesh) noexcept
{
	std::stack<MeshPropertyPtr> meshes;
	meshes.push(mesh);

	while (!meshes.empty())
	{
		auto it = meshes.top();
		meshes.pop();

		if (it->getTangentArray().empty())
			it->computeTangents();

		if (it->getTangentQuatArray().empty())
			it->computeTangentQuats();

		if (it->getBoundingBox().empty())
			it->computeBoundingBox();

		for (auto& child : it->getChildren())
			meshes.push(child);
	}
}

void
MeshComponent::onActivate() noexcept
{
}

void
MeshComponent::onDeactivate() noexcept
{
	_mesh.reset();
	_sharedMesh.reset();
}

_NAME_END