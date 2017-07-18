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
#include <ray/graphics_data.h>
#include <ray/camera.h>

_NAME_BEGIN

__ImplementSubClass(Geometry, RenderObject, "Geometry")

GraphicsIndirect::GraphicsIndirect() noexcept
	: startVertice(0)
	, startIndice(0)
	, startInstances(0)
	, numVertices(0)
	, numIndices(0)
	, numInstances(1)
{
}

GraphicsIndirect::GraphicsIndirect(std::uint32_t _numVertices, std::uint32_t _numIndices, std::uint32_t _numInstance, std::uint32_t _startVertice, std::uint32_t _startIndice, std::uint32_t _startInstance) noexcept
	: startVertice(_startVertice)
	, startIndice(_startIndice)
	, startInstances(_startInstance)
	, numVertices(_numVertices)
	, numIndices(_numIndices)
	, numInstances(_numInstance)
{
}

Geometry::Geometry() noexcept
	: _isCastShadow(true)
	, _isReceiveShadow(true)
	, _indexType(GraphicsIndexType::GraphicsIndexTypeUInt32)
	, _vertexOffset(0)
	, _indexOffset(0)
{
}

Geometry::~Geometry() noexcept
{
}

void
Geometry::setReceiveShadow(bool enable) noexcept
{
	_isReceiveShadow = enable;
}

bool
Geometry::getReceiveShadow() const noexcept
{
	return _isReceiveShadow;
}

void
Geometry::setCastShadow(bool value) noexcept
{
	_isCastShadow = value;
}

bool
Geometry::getCastShadow()  const noexcept
{
	return _isCastShadow;
}

void
Geometry::setMaterial(const MaterialPtr& material) noexcept
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

const MaterialPtr&
Geometry::getMaterial() noexcept
{
	return _material;
}

void
Geometry::setVertexBuffer(const GraphicsDataPtr& data, std::intptr_t offset) noexcept
{
	assert(!data || (data && data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer));
	_vbo = data;
	_vertexOffset = offset;
}

const GraphicsDataPtr&
Geometry::getVertexBuffer() const noexcept
{
	return _vbo;
}

void
Geometry::setIndexBuffer(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(!data || (data && data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer));
	assert(indexType == GraphicsIndexType::GraphicsIndexTypeUInt16 || indexType == GraphicsIndexType::GraphicsIndexTypeUInt32);
	_ibo = data;
	_indexType = indexType;
}

const GraphicsDataPtr&
Geometry::getIndexBuffer() const noexcept
{
	return _ibo;
}

void
Geometry::setGraphicsIndirect(GraphicsIndirectPtr&& renderable) noexcept
{
	_renderable = std::move(renderable);
}

void
Geometry::setGraphicsIndirect(const GraphicsIndirectPtr& renderable) noexcept
{
	_renderable = renderable;
}

GraphicsIndirectPtr
Geometry::getGraphicsIndirect() noexcept
{
	return _renderable;
}

bool
Geometry::onVisiableTest(const Camera& camera, const Frustum& fru) noexcept
{
	if (camera.getCameraOrder() == CameraOrder::CameraOrderShadow)
	{
		if (!this->getCastShadow())
			return false;
	}

	if (camera.getCameraType() == CameraType::CameraTypeCube)
		return math::sqrDistance(camera.getTranslate(), this->getTranslate()) < (camera.getFar() * camera.getFar());
	else
		return fru.contains(this->getBoundingBoxInWorld().aabb());
}

void
Geometry::onAddRenderData(RenderDataManager& manager) noexcept
{
	if (this->getCastShadow())
	{
		if (_techniques[RenderQueue::RenderQueueShadow])
			manager.addRenderData(RenderQueue::RenderQueueShadow, this);

		if (_techniques[RenderQueue::RenderQueueReflectiveShadow])
			manager.addRenderData(RenderQueue::RenderQueueReflectiveShadow, this);
	}

	for (std::size_t i = 0; i < RenderQueue::RenderQueueRangeSize; i++)
	{
		if (i != RenderQueue::RenderQueueShadow && i != RenderQueue::RenderQueueReflectiveShadow)
		{
			if (_techniques[i])
				manager.addRenderData((RenderQueue)i, this);
		}
	}
}

void
Geometry::onRenderObject(RenderPipeline& pipeline, RenderQueue queue, MaterialTech* tech) noexcept
{
	if (_techniques[queue] || tech)
	{
		pipeline.setTransform(this->getTransform());
		pipeline.setTransformInverse(this->getTransformInverse());

		if (_vbo)
			pipeline.setVertexBuffer(0, _vbo, _vertexOffset);

		if (_ibo)
			pipeline.setIndexBuffer(_ibo, _indexOffset, _indexType);

		if (_techniques[queue])
		{
			auto& passList = _techniques[queue]->getPassList();
			for (auto& pass : passList)
			{
				pipeline.setMaterialPass(pass);
				pipeline.drawIndexedLayer(_renderable->numIndices, _renderable->numInstances, _renderable->startIndice, _renderable->startVertice, _renderable->startInstances, this->getLayer());
			}
		}
		else if (tech)
		{
			auto& passList = tech->getPassList();
			for (auto& pass : passList)
			{
				pipeline.setMaterialPass(pass);
				pipeline.drawIndexedLayer(_renderable->numIndices, _renderable->numInstances, _renderable->startIndice, _renderable->startVertice, _renderable->startInstances, this->getLayer());
			}
		}
	}
}

RenderQueue
Geometry::stringToRenderQueue(const std::string& techName) noexcept
{
	if (techName == "Custom")					return RenderQueue::RenderQueueCustom;
	if (techName == "Shadow")					return RenderQueue::RenderQueueShadow;
	if (techName == "ReflectiveShadow")			return RenderQueue::RenderQueueReflectiveShadow;
	if (techName == "Opaque")					return RenderQueue::RenderQueueOpaque;
	if (techName == "OpaqueBatch")				return RenderQueue::RenderQueueOpaqueBatch;
	if (techName == "OpaqueSpecific")			return RenderQueue::RenderQueueOpaqueSpecific;
	if (techName == "OpaqueShading")			return RenderQueue::RenderQueueOpaqueShading;
	if (techName == "TransparentBack")			return RenderQueue::RenderQueueTransparentBack;
	if (techName == "TransparentBatchBack")     return RenderQueue::RenderQueueTransparentBatchBack;
	if (techName == "TransparentShadingBack")	return RenderQueue::RenderQueueTransparentShadingBack;
	if (techName == "TransparentSpecificBack")	return RenderQueue::RenderQueueTransparentSpecificBack;
	if (techName == "TransparentFront")			return RenderQueue::RenderQueueTransparentFront;
	if (techName == "TransparentBatchFront")    return RenderQueue::RenderQueueTransparentBatchFront;
	if (techName == "TransparentShadingFront")	return RenderQueue::RenderQueueTransparentShadingFront;
	if (techName == "TransparentSpecificFront")	return RenderQueue::RenderQueueTransparentSpecificFront;
	if (techName == "Lighting")					return RenderQueue::RenderQueueLights;
	if (techName == "Postprocess")				return RenderQueue::RenderQueuePostprocess;

	return RenderQueue::RenderQueueMaxEnum;
}

_NAME_END