// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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

	virtual void onRenderObjectPre(RenderPipeline& pipeline) noexcept = 0;
	virtual void onRenderObjectPost(RenderPipeline& pipeline) noexcept = 0;
};

class EXPORT RenderObject : public rtti::Interface
{
	__DeclareSubInterface(RenderObject, rtti::Interface)
public:
	RenderObject() noexcept;
	virtual ~RenderObject() noexcept;

	void setLayer(std::uint8_t layer) noexcept;
	std::uint8_t getLayer() const noexcept;

	void setCastShadow(bool enable) noexcept;
	bool getCastShadow() const noexcept;

	void setReceiveShadow(bool enable) noexcept;
	bool getReceiveShadow() const noexcept;

	void setOwnerListener(RenderListener* listener) noexcept;
	RenderListener* getOwnerListener() noexcept;

	void setBoundingBox(const Bound& bound) noexcept;
	const Bound& getBoundingBox() const noexcept;
	const Bound& getBoundingBoxInWorld() const noexcept;

	void setRenderScene(RenderScenePtr scene) noexcept;
	RenderScenePtr getRenderScene() const noexcept;

	void setTransform(const Matrix4x4& transform, const Matrix4x4& inverse, const Matrix4x4& inverseTranspose) noexcept;
	const Matrix4x4& getTransform() const noexcept;
	const Matrix4x4& getTransformInverse() const noexcept;
	const Matrix4x4& getTransformInverseTranspose() const noexcept;

	Vector3 getTranslate() const noexcept;

	Vector3 getRight() const noexcept;
	Vector3 getUpVector() const noexcept;
	Vector3 getForward() const noexcept;

	void addSubRenderObject(RenderObjectPtr object) noexcept;
	void removeSubRenderObject(RenderObjectPtr object) noexcept;
	RenderObjects& getSubeRenderObjects() noexcept;

	void render(RenderPipeline& pipelineContext, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) noexcept;

protected:
	virtual void onMoveBefor() noexcept;
	virtual void onMoveAfter() noexcept;

	virtual void onSceneChangeBefor() noexcept;
	virtual void onSceneChangeAfter() noexcept;

	virtual void onRenderObject(RenderPipeline& pipelineContext, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) noexcept;

private:
	std::uint8_t _layer;

	bool _isCastShadow;
	bool _isReceiveShadow;

	Bound _boundingBox;
	Bound _worldBoundingxBox;

	Matrix4x4 _transform;
	Matrix4x4 _transformInverse;
	Matrix4x4 _transformInverseTranspose;

	RenderObjects _renderObjects;
	RenderListener* _renderListener;
	RenderSceneWeakPtr  _renderScene;
};

_NAME_END

#endif