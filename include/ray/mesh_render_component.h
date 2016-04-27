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
#ifndef _H_MESH_RENDER_COMPONENT_H_
#define _H_MESH_RENDER_COMPONENT_H_

#include <ray/render_component.h>

_NAME_BEGIN

class EXPORT MeshRenderComponent : public RenderComponent
{
	__DeclareSubClass(MeshRenderComponent, RenderComponent)
public:
	MeshRenderComponent() noexcept;
	~MeshRenderComponent() noexcept;

	void setMaterial(MaterialPtr material) noexcept;
	void setSharedMaterial(MaterialPtr material) noexcept;

	MaterialPtr getMaterial() const noexcept;
	MaterialPtr getSharedMaterial() const noexcept;

	void setMaterials(const Materials& material) noexcept;
	void setSharedMaterials(const Materials& material) noexcept;

	const Materials& getMaterials() const noexcept;
	const Materials& getSharedMaterials() const noexcept;

	bool hasMaterial() const noexcept;
	bool hasSharedMaterial() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

protected:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onMeshChange() except;

	virtual void onMoveAfter() noexcept;
	virtual void onLayerChangeAfter() noexcept;

	virtual void onAttachComponent(GameComponentPtr& component) noexcept;
	virtual void onDetachComponent(GameComponentPtr& component) noexcept;

protected:
	void _attacRenderObjects() noexcept;

	void _cleanupRenderhObjects() noexcept;
	void _cleanupMaterials() noexcept;

	bool _buildMaterials(const std::string& filename) noexcept;
	bool _buildDefaultMaterials(const std::string& filename) noexcept;
	bool _buildRenderObjects(MeshPropertyPtr mesh, ModelMakerFlags flags) noexcept;

	void _updateMaterial(std::size_t n) noexcept;
	void _updateMaterials() noexcept;

	void _setModelMakerFlags(ModelMakerFlags flags) noexcept;
	ModelMakerFlags _getModelMakerFlags() const noexcept;

	void _setDefaultMaterial(const std::string& material) noexcept;
	const std::string& _getDefaultMaterial() const noexcept;

private:
	bool _buildRenderObject(MeshPropertyPtr mesh, std::size_t& startVertice, std::size_t& startIndice) noexcept;
	bool _buildRenderObject(GeometryPtr geometry, MeshPropertyPtr mesh, RenderMeshPtr buffer) noexcept;

private:
	MeshRenderComponent(const MeshRenderComponent&) noexcept = delete;
	MeshRenderComponent& operator=(const MeshRenderComponent&) noexcept = delete;

private:

	Materials _materials;
	Materials _sharedMaterials;

	RenderMeshPtr _renderMesh;
	Geometryes _renderObjects;

	ModelMakerFlags _flags;

	std::string _material;
	std::string _defaultMaterial;
};

_NAME_END

#endif