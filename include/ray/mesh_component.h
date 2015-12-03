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
#ifndef _H_MESH_COMPONENT_H_
#define _H_MESH_COMPONENT_H_

#include <ray/game_component.h>
#include <ray/model.h>

_NAME_BEGIN

class EXPORT MeshComponent final : public GameComponent
{
	__DeclareSubClass(MeshComponent, GameComponent)
public:
	MeshComponent() noexcept;
	~MeshComponent() noexcept;

	void setMesh(MeshPropertyPtr mesh) noexcept;
	void setSharedMesh(MeshPropertyPtr mesh) noexcept;
	void setCombieInstnace(const CombineInstance& instances) noexcept;

	MeshPropertyPtr getMesh() const noexcept;
	MeshPropertyPtr getSharedMesh() const noexcept;

	std::size_t getNumVertices() const noexcept;
	std::size_t getNumIndices() const noexcept;

	const Bound& getBoundingBox() const noexcept;
	const Bound& getBoundingBoxDownwards() const noexcept;

	void addMeshChangeListener(std::function<void()> func) noexcept;
	void removeMeshChangeListener(std::function<void()> func) noexcept;

	void clear() noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:

	void onActivate() noexcept;
	void onDeactivate() noexcept;

private:
	MeshComponent(const MeshComponent&) noexcept = delete;
	MeshComponent& operator=(const MeshComponent&) noexcept = delete;

private:

	MeshPropertyPtr _mesh;
	MeshPropertyPtr _sharedMesh;

	delegate<void()> _onMeshChange;
};

_NAME_END

#endif