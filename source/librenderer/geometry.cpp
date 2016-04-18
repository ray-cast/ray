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
#include <ray/geometry.h>
#include <ray/render_pipeline.h>
#include <ray/render_object_manager.h>
#include <ray/material.h>

_NAME_BEGIN

__ImplementSubClass(Geometry, RenderObject, "Geometry")

Geometry::Geometry() noexcept
{
}

Geometry::~Geometry() noexcept
{
}

void
Geometry::setMaterial(MaterialPtr material) noexcept
{
	if (_material != material)
	{
		if (material)
		{
			const auto& techs = material->getTechs();
			for (auto& tech : techs)
			{
				RenderQueue queue = stringToRenderQueue(tech->getName());
				if (queue == RenderQueue::RenderQueueMaxEnum)
					continue;

				_techniques[queue] = tech;
			}
		}
		else
		{
			for (std::size_t i = 0; i < RenderQueue::RenderQueueRangeSize; i++)
				_techniques[i] = nullptr;
		}

		_material = material;
	}
}

MaterialPtr
Geometry::getMaterial() noexcept
{
	return _material;
}

void
Geometry::setRenderMesh(RenderMeshPtr mesh) noexcept
{
	_mesh = mesh;
}

RenderMeshPtr
Geometry::getRenderMesh() noexcept
{
	return _mesh;
}

void 
Geometry::setGraphicsIndirect(GraphicsIndirectPtr renderable) noexcept
{
	_renderable = renderable;
}

GraphicsIndirectPtr
Geometry::getGraphicsIndirect() noexcept
{
	return _renderable;
}

void
Geometry::onAddRenderData(RenderDataManager& manager) noexcept
{
	if (_material)
	{
		if (_techniques[RenderQueue::RenderQueueShadow] && this->getCastShadow())
			manager.addRenderData(RenderQueue::RenderQueueShadow, this->upcast<RenderObject>());

		if (_techniques[RenderQueue::RenderQueueOpaque])
			manager.addRenderData(RenderQueue::RenderQueueOpaque, this->upcast<RenderObject>());

		if (_techniques[RenderQueue::RenderQueueOpaqueSpecific])
			manager.addRenderData(RenderQueue::RenderQueueOpaqueSpecific, this->upcast<RenderObject>());

		if (_techniques[RenderQueue::RenderQueueTransparent])
			manager.addRenderData(RenderQueue::RenderQueueTransparent, this->upcast<RenderObject>());

		if (_techniques[RenderQueue::RenderQueueTransparentSpecific])
			manager.addRenderData(RenderQueue::RenderQueueTransparentSpecific, this->upcast<RenderObject>());
	}
}

void 
Geometry::onRenderObject(RenderPipeline& pipelineContext, RenderQueue queue, MaterialTechPtr _tech) noexcept
{
	auto tech = _tech ? _tech : _techniques[queue];
	if (tech)
	{
		pipelineContext.setTransform(this->getTransform());
		pipelineContext.setTransformInverse(this->getTransformInverse());
		pipelineContext.setTransformInverseTranspose(this->getTransformInverseTranspose());

		pipelineContext.drawMesh(tech, this->getRenderMesh(), *_renderable);
	}
}

RenderQueue
Geometry::stringToRenderQueue(const std::string& techName) noexcept
{
	if (techName == "custom")			return RenderQueue::RenderQueueCustom;
	if (techName == "opaque")			return RenderQueue::RenderQueueOpaque;
	if (techName == "opaquespecific")   return RenderQueue::RenderQueueOpaqueSpecific;
	if (techName == "transparent")		return RenderQueue::RenderQueueTransparent;
	if (techName == "lighting")			return RenderQueue::RenderQueueLighting;
	if (techName == "postprocess")		return RenderQueue::RenderQueuePostprocess;

	return RenderQueue::RenderQueueMaxEnum;
}

_NAME_END