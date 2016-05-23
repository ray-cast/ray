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
#include "ogl_pipeline.h"
#include "ogl_descriptor_set.h"
#include "ogl_graphics_data.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_descriptor_set.h"

_NAME_BEGIN

__ImplementSubClass(OGLPipeline, GraphicsPipeline, "OGLPipeline")

OGLPipeline::OGLPipeline() noexcept
{
}

OGLPipeline::~OGLPipeline() noexcept
{
	this->close();
}

bool
OGLPipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<OGLGraphicsState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<OGLProgram>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<OGLInputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<OGLDescriptorSetLayout>());

	auto& layouts = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexLayouts();
	for (auto& it : layouts)
	{
		GLuint attribIndex = GL_INVALID_INDEX;

		auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
		for (auto& attrib : attributes)
		{
			if (attrib->getSemantic() == it.getSemantic() &&
				attrib->getSemanticIndex() == it.getSemanticIndex())
			{
				attribIndex = attrib->downcast<OGLGraphicsAttribute>()->getBindingPoint();
				break;
			}
		}

		if (attribIndex != GL_INVALID_INDEX)
		{
			VertexAttrib attrib;
			attrib.type = OGLTypes::asVertexFormat(it.getVertexFormat());
			attrib.index = attribIndex;
			attrib.count = it.getVertexCount();
			attrib.slot = it.getVertexSlot();
			attrib.offset = it.getVertexOffset();

			_attributes.push_back(attrib);
		}
	}

	auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
	for (auto& it : bindings)
	{
		for (auto& attrib : _attributes)
		{
			if (attrib.slot != it.getVertexSlot())
				continue;

			VertexBinding binding;
			binding.slot = attrib.slot;
			binding.index = attrib.index;
			binding.divisor = it.getVertexDivisor();

			_bindings.push_back(binding);
		}
	}

	_pipelineDesc = pipelineDesc;
	return true;
}

void
OGLPipeline::close() noexcept
{
	_attributes.clear();
}

const GraphicsPipelineDesc&
OGLPipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

void
OGLPipeline::apply() noexcept
{
}

void
OGLPipeline::bindVbo(const OGLGraphicsData& vbo, GLuint slot) noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getInstanceID());

	GLuint stride = vbo.getGraphicsDataDesc().getStride();
	for (auto& it : _attributes)
	{
		if (it.slot != slot)
			continue;

		glEnableVertexAttribArray(it.index);
		glVertexAttribPointer(it.index, it.count, it.type, GL_FALSE, stride, (GLbyte*)nullptr + it.offset);
	}

	for (auto& it : _bindings)
	{
		if (it.slot != slot)
			continue;

		glVertexAttribDivisor(it.index, it.divisor);
	}
}

void
OGLPipeline::bindIbo(const OGLGraphicsData& ibo) noexcept
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.getInstanceID());
}

void
OGLPipeline::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLPipeline::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END