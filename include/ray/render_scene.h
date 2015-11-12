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

#include <ray/camera.h>
#include <ray/light.h>
#include <ray/kdtree.h>

_NAME_BEGIN

class EXPORT OcclusionCullNode
{
public:
	OcclusionCullNode() noexcept;
	OcclusionCullNode(RenderObjectPtr item, float distSqrt) noexcept;
	~OcclusionCullNode() noexcept;

	void setOcclusionCullNode(RenderObjectPtr node) noexcept;
	RenderObjectPtr getOcclusionCullNode() noexcept;
	void setDistanceSqrt(float distSq) noexcept;

	float getDistanceSqrt() const noexcept;

private:

	float _distanceSqrt;
	RenderObjectPtr _item;
};

class EXPORT OcclusionCullList
{
public:
	typedef std::vector<OcclusionCullNode> OcclusionCullNodes;

public:
	OcclusionCullList() noexcept;
	~OcclusionCullList() noexcept;
	void clear() noexcept;

	OcclusionCullNodes& iter() noexcept;
	const OcclusionCullNodes& iter() const noexcept;

	void insert(RenderObjectPtr item, float distanceSqrt) noexcept;
	void sort() noexcept;

private:

	OcclusionCullNodes _iter;
};

class EXPORT RenderScene : public Reference<RenderScene>
{
public:
	RenderScene() noexcept;
	virtual ~RenderScene() noexcept;

	void addCamera(CameraPtr camera) noexcept;
	void removeCamera(CameraPtr camera) noexcept;
	void sortCamera() noexcept;

	Cameras& getCameraList() noexcept;
	const Cameras& getCameraList() const noexcept;

	void addLight(LightPtr light) noexcept;
	void removeLight(LightPtr light) noexcept;
	void sortLight() noexcept;

	Lights& getLightList() noexcept;

	void addRenderObject(RenderObjectPtr object) noexcept;
	void removeRenderObject(RenderObjectPtr object) noexcept;

	void computVisiableLight(const Matrix4x4& viewProject, OcclusionCullList& list) noexcept;
	void computVisiable(const Matrix4x4& viewProject, OcclusionCullList& list) noexcept;

private:

	Cameras _cameraList;
	Lights _lightList;
	RenderObjects _renderObjectList;
};

_NAME_END

#endif