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

	bool hasMaterial() const noexcept;
	bool hasSharedMaterial() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

	void buildMaterials() except;
	void buildRenderObjects(MeshPropertyPtr mesh) noexcept;

protected:

	virtual void onActivate() except;
	virtual void onDeactivate() except;

	virtual void onMeshChange() except;

	virtual void onMoveAfter() noexcept;
	virtual void onLayerChangeAfter() noexcept;

	virtual void onAttachComponent(GameComponentPtr& component) except;
	virtual void onDetachComponent(GameComponentPtr& component) except;

protected:

	void _attacRenderObjects() noexcept;
	void _dettachRenderhObjects() noexcept;

	void _setRenderObject(RenderObjectPtr object) noexcept;
	RenderObjectPtr _getRenderObject() noexcept;

	RenderObjectPtr buildRenderObject(MeshPropertyPtr mesh, RenderBufferPtr buffer) noexcept;

private:
	MeshRenderComponent(const MeshRenderComponent&) noexcept = delete;
	MeshRenderComponent& operator=(const MeshRenderComponent&) noexcept = delete;

private:

	MaterialPtr _material;
	MaterialPtr _sharedMaterial;

	RenderObjectPtr _renderObject;
};

_NAME_END

#endif