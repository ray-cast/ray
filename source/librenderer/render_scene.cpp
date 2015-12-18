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

_NAME_BEGIN

__ImplementSubClass(RenderScene, rtti::Interface, "RenderScene")

OcclusionCullNode::OcclusionCullNode() noexcept
	: _item(nullptr)
	, _distanceSqrt(0)
{
}

OcclusionCullNode::OcclusionCullNode(RenderObjectPtr item, float distSqrt) noexcept
	: _item(item)
	, _distanceSqrt(distSqrt)
{
}

OcclusionCullNode::~OcclusionCullNode() noexcept
{
}

void 
OcclusionCullNode::setOcclusionCullNode(RenderObjectPtr node) noexcept
{
	_item = node;
}

RenderObjectPtr 
OcclusionCullNode::getOcclusionCullNode() noexcept
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
OcclusionCullList::insert(RenderObjectPtr item, float distanceSqrt) noexcept
{
	_iter.push_back(OcclusionCullNode(item, distanceSqrt));
}

void 
OcclusionCullList::sort() noexcept
{
	std::sort(_iter.begin(), _iter.end(),
		[](const OcclusionCullNode& lhs, const OcclusionCullNode& rhs)
	{
		return lhs.getDistanceSqrt() < rhs.getDistanceSqrt();
	}
	);
}

RenderScene::RenderScene() noexcept
{
}

RenderScene::~RenderScene() noexcept
{
}

void
RenderScene::addCamera(CameraPtr camera) noexcept
{
	auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
	if (it == _cameraList.end())
	{
		_cameraList.push_back(camera);
		this->sortCamera();
	}
}

void
RenderScene::removeCamera(CameraPtr camera) noexcept
{
	auto it = std::find(_cameraList.begin(), _cameraList.end(), camera);
	if (it != _cameraList.end())
	{
		_cameraList.erase(it);
	}
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
RenderScene::sortCamera() noexcept
{
	std::sort(_cameraList.begin(), _cameraList.end(),
		[](CameraPtr lhs, CameraPtr rhs)
	{
		return lhs->getCameraOrder() < rhs->getCameraOrder();
	}
	);
}

void
RenderScene::addLight(LightPtr light) noexcept
{
	assert(_lightList.end() == std::find(_lightList.begin(), _lightList.end(), light));

	_lightList.push_back(light);
}

void
RenderScene::removeLight(LightPtr light) noexcept
{
	auto it = std::find(_lightList.begin(), _lightList.end(), light);
	if (it != _lightList.end())
	{
		_lightList.erase(it);
	}
}

Lights&
RenderScene::getLightList() noexcept
{
	return _lightList;
}

void
RenderScene::sortLight() noexcept
{
}

void
RenderScene::addRenderObject(RenderObjectPtr object) noexcept
{
	assert(std::find(_renderObjectList.begin(), _renderObjectList.end(), object) == _renderObjectList.end());
	_renderObjectList.push_back(object);
}

void
RenderScene::removeRenderObject(RenderObjectPtr object) noexcept
{
	auto it = std::find(_renderObjectList.begin(), _renderObjectList.end(), object);
	if (it != _renderObjectList.end())
	{
		_renderObjectList.erase(it);
	}
}

void
RenderScene::computVisiable(const Matrix4x4& viewProject, OcclusionCullList& list) noexcept
{
	Frustum fru;
	fru.extract(viewProject);

	auto eyePosition = viewProject.getTranslate();

	for (auto& it : _renderObjectList)
	{
		if (fru.contains(it->getBoundingBoxInWorld().aabb()))
		{
			list.insert(it, eyePosition.sqrDistance(it->getBoundingBoxInWorld().center()));

			for (auto& child : it->getSubeRenderObjects())
			{
				if (fru.contains(child->getBoundingBoxInWorld().aabb()))
				{
					list.insert(it, eyePosition.sqrDistance(it->getBoundingBoxInWorld().center()));
				}
			}
		}
	}

	list.sort();
}

void
RenderScene::computVisiableLight(const Matrix4x4& viewProject, OcclusionCullList& list) noexcept
{
	Frustum fru;
	fru.extract(viewProject);

	auto eyePosition = viewProject.getTranslate();

	for (auto& light : _lightList)
	{
		Bound bound;

		LightType type = light->getLightType();

		auto range = light->getRange();

		if (type == LightType::LT_SUN ||
			type == LightType::LT_AMBIENT)
		{
			float infinity = std::numeric_limits<float>::max();

			Vector3 min(-infinity, -infinity, -infinity);
			Vector3 max(infinity, infinity, infinity);

			bound.encapsulate(min);
			bound.encapsulate(max);
		}
		else if (type == LightType::LT_POINT)
		{
			Vector3 min(-range, -range, -range);
			Vector3 max(range, range, range);

			bound.encapsulate(min);
			bound.encapsulate(max);
		}
		else if (type == LightType::LT_AREA ||
			type == LightType::LT_SPOT ||
			type == LightType::LT_HEMI_SPHERE)
		{
			Vector3 min(-range, -range, -range);
			Vector3 max(range, range, range);

			bound.encapsulate(min);
			bound.encapsulate(max);
		}

		light->setBoundingBox(bound);

		if (fru.contains(light->getBoundingBoxInWorld().aabb()))
		{
			list.insert(light, eyePosition.sqrDistance(light->getBoundingBoxInWorld().center()));
		}
	}
}

_NAME_END