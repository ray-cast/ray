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
#include "ogl_core_pipeline.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_graphics_data.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_descriptor_set.h"
#include "ogl_device.h"

_NAME_BEGIN

__ImplementSubClass(OGLCorePipeline, GraphicsPipeline, "OGLCorePipeline")

OGLCorePipeline::OGLCorePipeline() noexcept
{
}

OGLCorePipeline::~OGLCorePipeline() noexcept
{
	this->close();
}

bool
OGLCorePipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<OGLGraphicsState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<OGLProgram>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<OGLInputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<OGLDescriptorSetLayout>());

	std::uint16_t offset = 0;

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
			GLenum type = OGLTypes::asVertexFormat(it.getVertexFormat());
			if (type == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Undefined vertex format.");
				return false;
			}

			VertexAttrib attrib;
			attrib.type = type;
			attrib.index = attribIndex;
			attrib.slot = it.getVertexSlot();
			attrib.count = it.getVertexCount();
			attrib.offset = offset + it.getVertexOffset();
			attrib.normalize = OGLTypes::isNormFormat(it.getVertexFormat());

			_attributes.push_back(attrib);
		}

		offset += it.getVertexOffset() + it.getVertexSize();
	}

	auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
	for (auto& it : bindings)
	{
		VertexBinding binding;
		binding.slot = it.getVertexSlot();
		binding.stride = it.getVertexSize();

		auto divisor = it.getVertexDivisor();
		if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorVertex)
			binding.divisor = 0;
		else if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorInstance)
			binding.divisor = 1;
		else
		{
			assert(false);
			binding.divisor = 0;
		}

		_bindings.push_back(binding);
	}

	_pipelineDesc = pipelineDesc;
	return true;
}

void
OGLCorePipeline::close() noexcept
{
}

const GraphicsPipelineDesc&
OGLCorePipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

void
OGLCorePipeline::apply() noexcept
{
	for (auto& it : _attributes)
	{
		glEnableVertexAttribArray(it.index);
		glVertexAttribBinding(it.index, it.slot);
		glVertexAttribFormat(it.index, it.count, it.type, it.normalize, it.offset);
	}

	if (GLEW_NV_vertex_buffer_unified_memory)
	{
		for (auto& it : _bindings)
		{
			glVertexBindingDivisor(it.slot, it.divisor);
			glBindVertexBuffer(it.slot, 0, 0, _bindings[it.slot].stride);
		}
	}
	else
	{
		for (auto& it : _bindings)
		{
			glVertexBindingDivisor(it.slot, it.divisor);
		}
	}
}

void
OGLCorePipeline::bindVertexBuffers(OGLCoreVertexBuffers& vbos, bool forceUpdate) noexcept
{
	if (GLEW_NV_vertex_buffer_unified_memory)
	{
		for (auto& it : _bindings)
		{
			if (!vbos[it.slot].vbo)
				continue;

			if (vbos[it.slot].needUpdate)
			{
				GLuint64 addr = vbos[it.slot].vbo->getInstanceAddr() + vbos[it.slot].offset;
				GLsizeiptr size = vbos[it.slot].vbo->getGraphicsDataDesc().getStreamSize() - vbos[it.slot].offset;
				glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.slot, addr, size);
				vbos[it.slot].needUpdate = false;
			}
		}
	}
	else
	{
		for (auto& it : _bindings)
		{
			if (!vbos[it.slot].vbo)
				continue;

			if (vbos[it.slot].needUpdate || forceUpdate)
			{
				glBindVertexBuffer(it.slot, vbos[it.slot].vbo->getInstanceID(), vbos[it.slot].offset, _bindings[it.slot].stride);
				vbos[it.slot].needUpdate = false;
			}
		}
	}
}

void
OGLCorePipeline::setDevice(const GraphicsDevicePtr& device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCorePipeline::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END