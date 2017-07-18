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
#include <ray/light_probe.h>
#include <ray/lightprobe_render_framebuffer.h>
#include <ray/render_object_manager.h>
#include <ray/camera.h>

_NAME_BEGIN

LightProbe::LightProbe() noexcept
	: _sh(0.0f)
{
}

LightProbe::~LightProbe() noexcept
{
}

void
LightProbe::setSH9(const SH9& sh) noexcept
{
	_sh = sh;
}

const SH9&
LightProbe::getSH9() const noexcept
{
	return _sh;
}

const CameraPtr&
LightProbe::getCamera() const noexcept
{
	return _camera;
}

bool
LightProbe::setupProbeCamera() noexcept
{
	auto framebuffer = std::make_shared<ray::LightProbeRenderFramebuffer>();
	if (!framebuffer->setup())
		return false;

	_camera = std::make_shared<Camera>();
	_camera->setOwnerListener(this);
	_camera->setCameraOrder(CameraOrder::CameraOrderShadow);
	_camera->setCameraRenderFlags(CameraRenderFlagBits::CameraRenderTextureBit);
	_camera->setAperture(90.0f);
	_camera->setNear(0.1f);
	_camera->setRatio(1.0f);
	_camera->setRenderPipelineFramebuffer(framebuffer);

	return true;
}

void
LightProbe::_updateTransform() noexcept
{
	if (_camera)
		_camera->setTransform(this->getTransform(), this->getTransformInverse());
}

void
LightProbe::_updateBoundingBox() noexcept
{
	BoundingBox boundingBox;

	if (!_camera)
	{
		Vector3 min(-5, -5, -5);
		Vector3 max(5, 5, 5);

		BoundingBox bound;
		bound.encapsulate(min);
		bound.encapsulate(max);

		boundingBox.encapsulate(bound);
	}

	this->setBoundingBox(boundingBox);
}

void
LightProbe::onMoveAfter() noexcept
{
	this->_updateTransform();
}

void
LightProbe::onSceneChangeBefore() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		if (_camera)
			_camera->setRenderScene(nullptr);
	}
}

void
LightProbe::onSceneChangeAfter() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		if (_camera)
			_camera->setRenderScene(renderScene);
	}
}

bool
LightProbe::onVisiableTest(const Camera& camera, const Frustum& fru) noexcept
{
	if (camera.getCameraOrder() == CameraOrder::CameraOrderShadow)
		return false;

	if (camera.getCameraOrder() == CameraOrder::CameraOrderLightProbe)
		return false;

	return fru.contains(this->getBoundingBoxInWorld().aabb());
}

void
LightProbe::onAddRenderData(RenderDataManager& manager) noexcept
{
	manager.addRenderData(RenderQueue::RenderQueueLightProbes, this);
}

void
LightProbe::onGenProbeBefore(const Camera& camera) noexcept
{
	if (_camera) this->setupProbeCamera();
}

void
LightProbe::onGenProbeAfter(const Camera& camera) noexcept
{
}

void
LightProbe::onRenderObjectPre(const Camera& camera) noexcept
{
}

void
LightProbe::onRenderObjectPost(const Camera& camera) noexcept
{
}

_NAME_END