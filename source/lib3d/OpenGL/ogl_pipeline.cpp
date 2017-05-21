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
				GL_PLATFORM_LOG("Undefined vertex format.");
				return false;
			}

			VertexAttrib attrib;
			attrib.type = type;
			attrib.index = attribIndex;
			attrib.count = it.getVertexCount();
			attrib.stride = 0;
			attrib.offset = offset + it.getVertexOffset();
			attrib.normalize = OGLTypes::isNormFormat(it.getVertexFormat());

			if (it.getVertexSlot() <= _attributes.size())
				_attributes.resize(it.getVertexSlot() + 1);

			_attributes[it.getVertexSlot()].push_back(attrib);
		}

		offset += it.getVertexOffset() + it.getVertexSize();
	}

	auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
	for (auto& it : bindings)
	{
		for (auto& attrib : _attributes[it.getVertexSlot()])
		{
			attrib.stride = it.getVertexSize();

			VertexBinding binding;
			binding.index = attrib.index;
			binding.slot = it.getVertexSlot();

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
	for (auto& it : _bindings)
	{
		glVertexAttribDivisor(it.index, it.divisor);
	}
}

void
OGLPipeline::bindVertexBuffers(OGLVertexBuffers& vbos, bool forceUpdate) noexcept
{
	for (std::size_t slot = 0; slot < _attributes.size(); slot++)
	{
		if (!vbos[slot].vbo)
			continue;

		if (vbos[slot].needUpdate || forceUpdate)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbos[slot].vbo->getInstanceID());

			for (auto& it : _attributes[slot])
			{
				glEnableVertexAttribArray(it.index);
				glVertexAttribPointer(it.index, it.count, it.type, it.normalize, it.stride, (GLbyte*)nullptr + vbos[slot].offset + it.offset);
			}

			vbos[slot].needUpdate = false;
		}
	}
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