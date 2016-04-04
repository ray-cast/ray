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
#include <ray/render_pipeline_context.h>
#include <ray/render_pipeline.h>
#include <ray/material.h>
#include <ray/graphics_context.h>
#include <ray/render_buffer.h>

_NAME_BEGIN

RenderPipelineContext::RenderPipelineContext(RenderPipeline& pipeline) noexcept
	: _pipeline(pipeline)
{
}

RenderPipelineContext::~RenderPipelineContext() noexcept
{
}

void
RenderPipelineContext::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	assert(_dataManager);
	_dataManager->addRenderData(queue, pass, object);
}

RenderObjects&
RenderPipelineContext::getRenderData(RenderQueue queue, RenderPass pass) noexcept
{
	assert(_dataManager);
	return _dataManager->getRenderData(queue, pass);
}

void
RenderPipelineContext::renderBegin() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->renderBegin();
}

void
RenderPipelineContext::renderEnd() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->renderEnd();
	_graphicsContext->present();
}

void
RenderPipelineContext::setViewport(const Viewport& view, std::size_t i) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setViewport(view, i);
}

const Viewport&
RenderPipelineContext::getViewport(std::size_t i) const noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->getViewport(i);
}

bool
RenderPipelineContext::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->updateBuffer(data, str, cnt);
}

void*
RenderPipelineContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->mapBuffer(data, access);
}

void
RenderPipelineContext::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_graphicsContext);
	return _graphicsContext->unmapBuffer(data);
}

void
RenderPipelineContext::setRenderTexture(GraphicsFramebufferPtr target) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setRenderTexture(target);
}

void
RenderPipelineContext::clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipelineContext::clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->clearRenderTexture(flags, color, depth, stencil, i);
}

void
RenderPipelineContext::discradRenderTexture() noexcept
{
	assert(_graphicsContext);
	_graphicsContext->discardRenderTexture();
}

void
RenderPipelineContext::readRenderTexture(GraphicsFramebufferPtr texture, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipelineContext::blitRenderTexture(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->blitRenderTexture(srcTarget, src, destTarget, dest);
}

GraphicsTexturePtr
RenderPipelineContext::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	return _pipeline.createTexture(desc);
}

void
RenderPipelineContext::setMaterialPass(MaterialPassPtr pass) noexcept
{
	auto& semantics = pass->getParameters();
	for (auto& it : semantics)
	{
		auto semantic = it->getSemantic();
		if (semantic)
		{
			auto type = semantic->getType();
			switch (type)
			{
			case GraphicsUniformType::GraphicsUniformTypeBool:
				it->assign(semantic->getBool());
				break;
			case GraphicsUniformType::GraphicsUniformTypeInt:
				it->assign(semantic->getInt());
				break;
			case GraphicsUniformType::GraphicsUniformTypeInt2:
				it->assign(semantic->getInt2());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat:
				it->assign(semantic->getFloat());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat2:
				it->assign(semantic->getFloat2());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat3:
				it->assign(semantic->getFloat3());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat4:
				it->assign(semantic->getFloat4());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
				it->assign(semantic->getFloat3x3());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
				it->assign(semantic->getFloat4x4());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloatArray:
				it->assign(semantic->getFloatArray());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
				it->assign(semantic->getFloat2Array());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
				it->assign(semantic->getFloat3Array());
				break;
			case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
				it->assign(semantic->getFloat4Array());
				break;
			case GraphicsUniformType::GraphicsUniformTypeStorageImage:
				it->assign(semantic->getTexture());
				break;
			default:
				break;
			}
		}
	}

	_graphicsContext->setRenderPipeline(pass->getRenderPipeline());
	_graphicsContext->setDescriptorSet(pass->getDescriptorSet());
}

MaterialPassPtr
RenderPipelineContext::getMaterialPass() noexcept
{
	return _materialPass;
}

void
RenderPipelineContext::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->setInputLayout(buffer->getInputLayout());
	_graphicsContext->setVertexBufferData(buffer->getVertexBuffer());
	_graphicsContext->setIndexBufferData(buffer->getIndexBuffer());
}

void
RenderPipelineContext::drawRenderBuffer(const GraphicsIndirect& renderable) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->drawRenderBuffer(renderable);
}

void
RenderPipelineContext::drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_graphicsContext);
	_graphicsContext->drawRenderBuffer(renderable, first, count);
}

void
RenderPipelineContext::drawSphere(MaterialPassPtr pass, const float4x4& transform) noexcept
{
	//_materialMatModel->assign(transform);
	//this->drawMesh(pass, _renderSphere, _renderSphereIndirect);
}

void
RenderPipelineContext::drawCone(MaterialPassPtr pass, const float4x4& transform) noexcept
{
	//_materialMatModel->assign(transform);
	//this->drawMesh(pass, _renderCone, _renderConeIndirect);
}

void
RenderPipelineContext::drawMesh(MaterialPassPtr pass, RenderBufferPtr buffer, const GraphicsIndirect& renderable) noexcept
{
	this->setMaterialPass(pass);
	this->setRenderBuffer(buffer);
	this->drawRenderBuffer(renderable);
}

void
RenderPipelineContext::drawScreenQuad(MaterialPassPtr pass) noexcept
{
	assert(pass);
	//this->drawMesh(pass, _renderScreenQuad, _renderScreenQuadIndirect);
}

void
RenderPipelineContext::drawRenderQueue(RenderQueue queue, RenderPass pass, MaterialPassPtr material, GraphicsFramebufferPtr target) noexcept
{
	auto& renderable = this->getRenderData(queue, pass);
	for (auto& it : renderable)
	{
		this->onRenderObjectPre(*it, queue, pass, material);
		this->onRenderObject(*it, queue, pass, material);
		this->onRenderObjectPost(*it, queue, pass, material);
	}
}

void
RenderPipelineContext::addPostProcess(RenderPostProcessPtr postprocess) noexcept
{
}

void
RenderPipelineContext::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
}

void
RenderPipelineContext::drawPostProcess(RenderQueue queue, GraphicsTexturePtr source, GraphicsFramebufferPtr swap, GraphicsFramebufferPtr dest) noexcept
{
}

void
RenderPipelineContext::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept
{
}

void
RenderPipelineContext::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept
{
}

void
RenderPipelineContext::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) noexcept
{
}

_NAME_END