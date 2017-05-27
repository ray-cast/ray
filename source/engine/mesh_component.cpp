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
#include <ray/mesh_component.h>
#include <ray/res_loader.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <stack>

_NAME_BEGIN

__ImplementSubClass(MeshComponent, GameComponent, "Mesh")

MeshComponent::MeshComponent() noexcept
	: _needUpdate(true)
{
}

MeshComponent::MeshComponent(const archivebuf& reader) noexcept
	: MeshComponent()
{
	this->load(reader);
}

MeshComponent::MeshComponent(const MeshPropertyPtr& mesh, bool shared) noexcept
	: MeshComponent()
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
MeshComponent::setMesh(const MeshPropertyPtr& mesh) noexcept
{
	if (_mesh != mesh)
	{
		this->onMeshChangeBefore();

		_mesh = mesh;

		this->onMeshChangeAfter();
	}
}

void
MeshComponent::setSharedMesh(const MeshPropertyPtr& mesh) noexcept
{
	if (_sharedMesh != mesh)
	{
		this->onMeshChangeBefore();

		_sharedMesh = mesh;

		this->onMeshChangeAfter();
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

const BoundingBox&
MeshComponent::getBoundingBox() const noexcept
{
	if (_mesh)
		_mesh->getBoundingBox();
	else if (_sharedMesh)
		_sharedMesh->getBoundingBox();
	return BoundingBox::Empty;
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
	if (this->getActive())
	{
		if (_mesh)
		{
			if (_mesh->getTangentArray().empty())
				_mesh->computeTangents();

			if (_mesh->getMeshSubsets().empty())
				_mesh->setMeshSubsets(MeshSubsets(1, MeshSubset(0, 0, _mesh->getIndicesArray().size(), 0, 0)));

			if (_mesh->getBoundingBox().empty())
				_mesh->computeBoundingBox();
		}

		if (_sharedMesh)
		{
			if (_sharedMesh->getTangentArray().empty())
				_sharedMesh->computeTangents();

			if (_sharedMesh->getMeshSubsets().empty())
				_sharedMesh->setMeshSubsets(MeshSubsets(1, MeshSubset(0, 0, _sharedMesh->getIndicesArray().size(), 0, 0)));

			if (_sharedMesh->getBoundingBox().empty())
				_sharedMesh->computeBoundingBox();
		}

		_onMeshChange.run();
		_needUpdate = false;
	}
	else
	{
		_needUpdate = true;
	}
}

void
MeshComponent::load(const archivebuf& reader) noexcept
{
	GameComponent::load(reader);
}

void
MeshComponent::save(archivebuf& write) noexcept
{
	GameComponent::save(write);
}

GameComponentPtr
MeshComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshComponent>();
	result->setName(this->getName());
	result->setActive(this->getActive());
	result->setSharedMesh(this->getMesh());
	if (_mesh) result->setMesh(this->getMesh()->clone());

	return result;
}

void
MeshComponent::onActivate() noexcept
{
	if (_needUpdate) this->needUpdate();
}

void
MeshComponent::onDeactivate() noexcept
{
	_mesh.reset();
	_sharedMesh.reset();
	_needUpdate = true;
	this->needUpdate();
}

void
MeshComponent::onMeshChangeBefore() noexcept
{
	_needUpdate = true;
}

void
MeshComponent::onMeshChangeAfter() noexcept
{
	if (_needUpdate) this->needUpdate();
}

_NAME_END