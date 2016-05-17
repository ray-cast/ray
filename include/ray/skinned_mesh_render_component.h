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
#ifndef _H_SKINNED_MESH_RENDER_COMPONENT_H_
#define _H_SKINNED_MESH_RENDER_COMPONENT_H_

#include <ray/mesh_render_component.h>

_NAME_BEGIN

class SkinnedMeshRenderComponent : public MeshRenderComponent
{
	__DeclareSubClass(SkinnedMeshRenderComponent, RenderComponent)
public:
	SkinnedMeshRenderComponent() noexcept;
	SkinnedMeshRenderComponent(MaterialPtr& material, bool shared = true) noexcept;
	SkinnedMeshRenderComponent(MaterialPtr&& material, bool shared = true) noexcept;
	SkinnedMeshRenderComponent(const Materials& material, bool shared = true) noexcept;
	SkinnedMeshRenderComponent(Materials&& material, bool shared = true) noexcept;
	~SkinnedMeshRenderComponent() noexcept;

	void setTransforms(const GameObjects& transforms) noexcept;
	void setTransforms(GameObjects&& transforms) noexcept;
	const GameObjects& getTransforms() const noexcept;

private:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onAttachComponent(GameComponentPtr& component) noexcept;
	virtual void onDetachComponent(GameComponentPtr& component) noexcept;

	virtual void onMeshChange() noexcept;
	virtual void onMeshWillRender(const class Camera&) noexcept;

	virtual void onFrameEnd() noexcept;

private:
	bool _needUpdate;

	GameObjects _transforms;
	GraphicsDataPtr _jointData;
	Bound _boundingBox;
	MeshPropertyPtr _mesh;

	std::function<void()> _onMeshChange;
	std::function<void(const Camera&)> _onMeshWillRender;
};

_NAME_END

#endif