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
#include <ray/render_data_manager.h>
#include <ray/render_post_process.h>

_NAME_BEGIN

void 
DefaultRenderDataManager::open() noexcept
{
}

void
DefaultRenderDataManager::close() noexcept
{
}

void
DefaultRenderDataManager::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	assert(object);
	assert(queue == RenderQueue::RQ_OPAQUE || queue == RenderQueue::RQ_TRANSPARENT || queue == RenderQueue::RQ_LIGHTING);

	_renderQueue[queue][pass].push_back(object);
}

RenderObjects&
DefaultRenderDataManager::getRenderData(RenderQueue queue, RenderPass pass) noexcept
{
	assert(queue == RenderQueue::RQ_OPAQUE || queue == RenderQueue::RQ_TRANSPARENT || queue == RenderQueue::RQ_LIGHTING);
	return _renderQueue[queue][pass];
}

void 
DefaultRenderDataManager::assginVisiable(CameraPtr camera) noexcept
{
	assert(camera);

	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_OPAQUES].clear();
	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_SPECIFIC].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_TRANSPARENT].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_SPECIFIC].clear();
	_renderQueue[RenderQueue::RQ_LIGHTING][RenderPass::RP_LIGHTS].clear();

	_visiable.clear();

	auto scene = camera->getRenderScene();
	scene->computVisiable(camera->getViewProject(), _visiable);

	for (auto& it : _visiable.iter())
	{
		if (CameraOrder::CO_SHADOW == camera->getCameraOrder())
		{
			if (!it.getOcclusionCullNode()->getCastShadow())
				return;
		}

		auto material = it.getOcclusionCullNode()->getMaterial();
		if (material)
		{
			auto& techiniques = material->getTechs();
			for (auto& technique : techiniques)
			{
				auto queue = technique->getRenderQueue();
				for (auto& pass : technique->getPassList())
				{
					auto listener = it.getOcclusionCullNode()->getOwnerListener();
					if (listener)
						listener->onWillRenderObject(*camera);

					this->addRenderData(queue, pass->getRenderPass(), it.getOcclusionCullNode());
				}
			}
		}
	}

	_visiable.clear();

	scene->computVisiableLight(camera->getViewProject(), _visiable);

	for (auto& it : _visiable.iter())
	{
		auto listener = it.getOcclusionCullNode()->getOwnerListener();
		if (listener)
			listener->onWillRenderObject(*camera);

		this->addRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS, it.getOcclusionCullNode());
	}
}

_NAME_END