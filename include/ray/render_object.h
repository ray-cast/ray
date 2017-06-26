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
#ifndef _H_RENDER_OBJECT_H_
#define _H_RENDER_OBJECT_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT RenderListener
{
public:
	RenderListener() noexcept;
	virtual ~RenderListener() noexcept;

	virtual void onRenderObjectPre(const Camera& camera) noexcept = 0;
	virtual void onRenderObjectPost(const Camera& camera) noexcept = 0;
};

class EXPORT RenderObject : public rtti::Interface
{
	__DeclareSubInterface(RenderObject, rtti::Interface)
public:
	RenderObject() noexcept;
	virtual ~RenderObject() noexcept;

	void setLayer(std::uint8_t layer) noexcept;
	std::uint8_t getLayer() const noexcept;

	void setOwnerListener(RenderListener* listener) noexcept;
	RenderListener* getOwnerListener() noexcept;

	void setBoundingBox(const BoundingBox& bound) noexcept;
	const BoundingBox& getBoundingBox() const noexcept;
	const BoundingBox& getBoundingBoxInWorld() const noexcept;

	void setRenderScene(RenderScenePtr scene) noexcept;
	const RenderScenePtr& getRenderScene() const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	const float4x4& getTransform() const noexcept;
	const float4x4& getTransformInverse() const noexcept;

	const Vector3& getRight() const noexcept;
	const Vector3& getUpVector() const noexcept;
	const Vector3& getForward() const noexcept;
	const Vector3& getTranslate() const noexcept;

public:
	virtual void onMoveBefor() noexcept;
	virtual void onMoveAfter() noexcept;

	virtual void onSceneChangeBefor() noexcept;
	virtual void onSceneChangeAfter() noexcept;

	virtual bool onVisiableTest(const Frustum& fru) noexcept;
	virtual void onAddRenderData(RenderDataManager& manager) noexcept;

	virtual void onRenderPre(const Camera& camera) noexcept;
	virtual void onRenderPost(const Camera& camera) noexcept;
	virtual void onRenderObject(RenderPipeline& pipeline, RenderQueue queue, MaterialTech* tech) noexcept;

protected:
	std::uint8_t _layer;

	BoundingBox _boundingBox;
	BoundingBox _worldBoundingxBox;

	float4x4 _transform;
	float4x4 _transformInverse;

	RenderListener* _renderListener;
	RenderScenePtr  _renderScene;
};

_NAME_END

#endif