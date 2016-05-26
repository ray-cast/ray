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
#include <ray/render_object_manager.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/geometry.h>

_NAME_BEGIN

DefaultRenderDataManager::DefaultRenderDataManager() noexcept
{
}

DefaultRenderDataManager::~DefaultRenderDataManager() noexcept
{
}

void
DefaultRenderDataManager::addRenderData(RenderQueue queue, RenderObject* object) noexcept
{
	assert(object);
	assert(queue >= RenderQueue::RenderQueueBeginRange && queue <= RenderQueue::RenderQueueEndRange);
	_renderQueue[queue].push_back(object);
}

const RenderObjectRaws&
DefaultRenderDataManager::getRenderData(RenderQueue queue) const noexcept
{
	assert(queue >= RenderQueue::RenderQueueBeginRange && queue <= RenderQueue::RenderQueueEndRange);
	return _renderQueue[queue];
}

void
DefaultRenderDataManager::assginVisiableLight(const Camera& camera) noexcept
{
	_visiable.clear();

	_renderQueue[RenderQueue::RenderQueueShadow].clear();
	_renderQueue[RenderQueue::RenderQueueLighting].clear();

	auto scene = camera.getRenderScene();
	scene->computVisiableLight(camera.getViewProject(), _visiable);

	this->sortDistance(_visiable);

	for (auto& it : _visiable.iter())
	{
		auto object = it.getOcclusionCullNode();
		object->onAddRenderData(*this);
	}
}

void
DefaultRenderDataManager::assginVisiableObject(const Camera& camera) noexcept
{
	_visiable.clear();

	auto scene = camera.getRenderScene();
	scene->computVisiableObject(camera.getViewProject(), _visiable);

	this->sortDistance(_visiable);

	for (auto& it : _visiable.iter())
	{
		auto object = it.getOcclusionCullNode();
		object->onAddRenderData(*this);
	}
}

void
DefaultRenderDataManager::assginVisiable(const Camera& camera) noexcept
{
	_visiable.clear();

	for (std::size_t i = 0; i < RenderQueue::RenderQueueEndRange; i++)
		_renderQueue[i].clear();

	auto cameraOrder = camera.getCameraOrder();
	if (cameraOrder == CameraOrder::CameraOrder3D ||
		cameraOrder == CameraOrder::CameraOrderShadow)
	{
		auto scene = camera.getRenderScene();
		assert(scene);
		scene->computVisiable(camera.getViewProject(), _visiable);

		for (auto& it : _visiable.iter())
		{
			auto object = it.getOcclusionCullNode();

			object->onRenderPre(camera);
			object->onAddRenderData(*this);
		}
	}
}

void
DefaultRenderDataManager::sortMaterial(RenderObjectRaws& list) noexcept
{
	std::sort(list.begin(), list.end(),
		[](RenderObject* lh, RenderObject* rh)
	{
		MaterialPtr m1, m2;
		if (lh->isInstanceOf<Geometry>())
			m1 = lh->downcast<Geometry>()->getMaterial();

		if (rh->isInstanceOf<Geometry>())
			m1 = rh->downcast<Geometry>()->getMaterial();

		return m1 < m2;
	});
}

void
DefaultRenderDataManager::sortDistance(OcclusionCullList& list) noexcept
{
	auto& iter = list.iter();
	if (iter.empty())
		return;

	auto it1 = iter.begin();
	auto it2 = ++iter.begin();

	auto end = iter.end();

	while (it2 != end)
	{
		MaterialPtr m1, m2;

		if (it1->getOcclusionCullNode()->isInstanceOf<Geometry>())
			m1 = it1->getOcclusionCullNode()->downcast<Geometry>()->getMaterial();

		if (it2->getOcclusionCullNode()->isInstanceOf<Geometry>())
			m1 = it2->getOcclusionCullNode()->downcast<Geometry>()->getMaterial();

		while (m1 == m2)
		{
			if ((++it2) == end)
				break;

			m2 = it2->getOcclusionCullNode()->downcast<Geometry>()->getMaterial();
		};

		std::size_t count = it2 - it1;
		if (count > 1)
			list.sort(it1, it2);

		if (it2 != end)
			it1 = it2++;
	}
}

_NAME_END