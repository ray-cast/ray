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
#ifndef _H_MESH_RENDER_COMPONENT_H_
#define _H_MESH_RENDER_COMPONENT_H_

#include <ray/mesh_component.h>
#include <ray/render_component.h>
#include <ray/res_manager.h>

_NAME_BEGIN

class EXPORT MeshRenderComponent : public RenderComponent
{
	__DeclareSubClass(MeshRenderComponent, RenderComponent)
public:
	MeshRenderComponent() noexcept;
	MeshRenderComponent(MaterialPtr& material, bool shared = true) noexcept;
	MeshRenderComponent(MaterialPtr&& material, bool shared = true) noexcept;
	MeshRenderComponent(const Materials& materials, bool shared = true) noexcept;
	MeshRenderComponent(Materials&& materials, bool shared = true) noexcept;
	MeshRenderComponent(const archivebuf& reader) noexcept;
	~MeshRenderComponent() noexcept;

	void setCastShadow(bool value) noexcept;
	bool getCastShadow() const noexcept;

	void setReceiveShadow(bool value) noexcept;
	bool getReceiveShadow() const noexcept;

	void setMaterial(MaterialPtr& material) noexcept;
	void setSharedMaterial(MaterialPtr& material) noexcept;

	void setMaterial(MaterialPtr&& material) noexcept;
	void setSharedMaterial(MaterialPtr&& material) noexcept;

	MaterialPtr getMaterial() const noexcept;
	MaterialPtr getSharedMaterial() const noexcept;

	void setMaterials(const Materials& materials) noexcept;
	void setSharedMaterials(const Materials& materials) noexcept;

	void setMaterials(Materials&& materials) noexcept;
	void setSharedMaterials(Materials&& materials) noexcept;

	const Materials& getMaterials() const noexcept;
	const Materials& getSharedMaterials() const noexcept;

	bool hasMaterial() const noexcept;
	bool hasSharedMaterial() const noexcept;

	void load(const archivebuf& reader) noexcept;
	void save(archivebuf& write) noexcept;

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
	void _attacRenderObject(GeometryPtr object) noexcept;
	void _attacRenderObjects() noexcept;

	void _destroyRenderhObject(GeometryPtr object) noexcept;
	void _destroyRenderhObjects() noexcept;

	void _destroyMaterials() noexcept;

	bool _buildMaterials(const util::string& filename) noexcept;

	bool _buildRenderObjects(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;

	void _updateMaterial(std::size_t n) noexcept;
	void _updateMaterials() noexcept;

private:
	MeshRenderComponent(const MeshRenderComponent&) noexcept = delete;
	MeshRenderComponent& operator=(const MeshRenderComponent&) noexcept = delete;

protected:
	bool _isCastShadow;
	bool _isReceiveShadow;

	Materials _materials;
	Materials _sharedMaterials;

	GraphicsDataPtr _renderMeshVbo;
	GraphicsDataPtr _renderMeshIbo;

	Geometryes _renderObjects;

	std::function<void()> _onMeshChange;

	util::string _material;
};

_NAME_END

#endif