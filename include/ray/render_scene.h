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
#ifndef _H_RENDER_SCENE_H_
#define _H_RENDER_SCENE_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT OcclusionCullNode
{
public:
	OcclusionCullNode() noexcept;
	OcclusionCullNode(RenderObject* item, float distSqrt) noexcept;
	~OcclusionCullNode() noexcept;

	void setOcclusionCullNode(RenderObject* node) noexcept;
	RenderObject* getOcclusionCullNode() const noexcept;
	void setDistanceSqrt(float distSq) noexcept;

	float getDistanceSqrt() const noexcept;

private:

	float _distanceSqrt;
	RenderObject* _item;
};

class EXPORT OcclusionCullList
{
public:
	typedef std::vector<OcclusionCullNode> OcclusionCullNodes;
	typedef std::vector<OcclusionCullNode>::iterator iterator;
	typedef std::vector<OcclusionCullNode>::const_iterator const_iterator;

public:
	OcclusionCullList() noexcept;
	~OcclusionCullList() noexcept;
	void clear() noexcept;

	OcclusionCullNodes& iter() noexcept;
	const OcclusionCullNodes& iter() const noexcept;

	void insert(RenderObject* item, float distanceSqrt) noexcept;
	void sort() noexcept;
	void sort(iterator begin, iterator end) noexcept;

private:

	OcclusionCullNodes _iter;
};

class EXPORT RenderScene final : public rtti::Interface
{
	__DeclareSubClass(RenderScene, rtti::Interface)
public:
	RenderScene();
	~RenderScene() noexcept;

	void addCamera(CameraPtr camera) noexcept;
	void removeCamera(CameraPtr camera) noexcept;
	Cameras& getCameraList() noexcept;
	const Cameras& getCameraList() const noexcept;

	void addRenderObject(RenderObject* object) noexcept;
	void removeRenderObject(RenderObject* object) noexcept;

	void computVisiable(const float3& eye, const float4x4& viewProject, OcclusionCullList& list) noexcept;
	void computVisiableLight(const float4x4& viewProject, OcclusionCullList& list) noexcept;

	static const RenderScenes& getSceneAll() noexcept;

protected:
	void addRenderScene(RenderScene* _this);
	void removeRenderScene(RenderScene* _this) noexcept;

private:
	Cameras _cameraList;
	RenderObjectRaws _renderObjectList;

	static RenderScenes _sceneList;
};

_NAME_END

#endif