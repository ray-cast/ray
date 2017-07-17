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
#include <ray/render_scene.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/geometry.h>

_NAME_BEGIN

__ImplementSubClass(RenderScene, rtti::Interface, "RenderScene")

RenderScenes RenderScene::_sceneList;

OcclusionCullNode::OcclusionCullNode() noexcept
	: _distanceSqrt(0)
	, _item(nullptr)
{
}

OcclusionCullNode::OcclusionCullNode(RenderObject* item, float distSqrt) noexcept
	: _distanceSqrt(distSqrt)
	, _item(item)
{
}

OcclusionCullNode::~OcclusionCullNode() noexcept
{
}

void
OcclusionCullNode::setOcclusionCullNode(RenderObject* node) noexcept
{
	_item = node;
}

RenderObject*
OcclusionCullNode::getOcclusionCullNode() const noexcept
{
	return _item;
}

void
OcclusionCullNode::setDistanceSqrt(float distSq) noexcept
{
	_distanceSqrt = distSq;
}

float
OcclusionCullNode::getDistanceSqrt() const noexcept
{
	return _distanceSqrt;
}

OcclusionCullList::OcclusionCullList() noexcept
{
}

OcclusionCullList::~OcclusionCullList() noexcept
{
}

void
OcclusionCullList::clear() noexcept
{
	_iter.clear();
}

OcclusionCullList::OcclusionCullNodes&
OcclusionCullList::iter() noexcept
{
	return _iter;
}

const OcclusionCullList::OcclusionCullNodes&
OcclusionCullList::iter() const noexcept
{
	return _iter;
}

void
OcclusionCullList::insert(RenderObject* item, float distanceSqrt) noexcept
{
	_iter.push_back(OcclusionCullNode(item, distanceSqrt));
}

void
OcclusionCullList::sort() noexcept
{
	this->sort(_iter.begin(), _iter.end());
}

void
OcclusionCullList::sort(iterator begin, iterator end) noexcept
{
	std::sort(begin, end,
		[](const OcclusionCullNode& lhs, const OcclusionCullNode& rhs)
	{
		return lhs.getDistanceSqrt() < rhs.getDistanceSqrt();
	}
	);
}

RenderScene::RenderScene()
{
	this->addRenderScene(this);
}

RenderScene::~RenderScene() noexcept
{
	this->removeRenderScene(this);
}

void
RenderScene::addCamera(const CameraPtr& camera) noexcept
{
	assert(!camera->getRenderScene());

	auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
	if (it == _cameraList.end())
	{
		_cameraList.push_back(camera);

		std::sort(_cameraList.begin(), _cameraList.end(),
			[](const CameraPtr& lhs, const CameraPtr& rhs)
		{
			return lhs->getCameraOrder() < rhs->getCameraOrder();
		}
		);
	}
}

void
RenderScene::removeCamera(const CameraPtr& camera) noexcept
{
	assert(camera->getRenderScene() == this->cast_pointer<RenderScene>());

	auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
	if (it != _cameraList.end())
		_cameraList.erase(it);
}

Cameras&
RenderScene::getCameraList() noexcept
{
	return _cameraList;
}

const Cameras&
RenderScene::getCameraList() const noexcept
{
	return _cameraList;
}

void
RenderScene::addRenderObject(RenderObject* object) noexcept
{
	assert(!object->getRenderScene());

	if (object->isInstanceOf<Camera>())
		this->addCamera(object->downcast_pointer<Camera>());
	else
		_renderObjectList.push_back(object);
}

void
RenderScene::removeRenderObject(RenderObject* object) noexcept
{
	if (object->isInstanceOf<Camera>())
	{
		auto it = std::find(_cameraList.begin(), _cameraList.end(), object->downcast_pointer<Camera>());
		if (it != _cameraList.end())
			_cameraList.erase(it);
	}
	else
	{
		auto it = std::find(_renderObjectList.begin(), _renderObjectList.end(), object);
		if (it != _renderObjectList.end())
			_renderObjectList.erase(it);
	}
}

void
RenderScene::computVisiable(const float3& eye, const float4x4& viewProject, OcclusionCullList& list) noexcept
{
	Frustum fru;
	fru.extract(viewProject);

	for (auto& it : _renderObjectList)
	{
		if (!it->getVisible())
			continue;

		if (it->onVisiableTest(fru))
			list.insert(it, math::sqrDistance(eye, it->getTransform().getTranslate()));
	}
}

void
RenderScene::computVisiableLight(const float3& eye, const float4x4& viewProject, OcclusionCullList& list) noexcept
{
	Frustum fru;
	fru.extract(viewProject);

	const auto& eyePosition = viewProject.getTranslate();

	for (auto& it : _renderObjectList)
	{
		if (!it->isInstanceOf<Light>())
			continue;

		if (!it->getVisible())
			continue;

		if (it->onVisiableTest(fru))
			list.insert(it, math::sqrDistance(eye, it->getTransform().getTranslate()));
	}
}

const RenderScenes&
RenderScene::getSceneAll() noexcept
{
	return _sceneList;
}

void
RenderScene::addRenderScene(RenderScene* _this)
{
	_sceneList.push_back(_this);
}

void
RenderScene::removeRenderScene(RenderScene* _this) noexcept
{
	auto it = std::find(_sceneList.begin(), _sceneList.end(), _this);
	if (it != _sceneList.end())
		_sceneList.erase(it);
}

_NAME_END