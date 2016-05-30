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
#include "ogl_core_pipeline.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_graphics_data.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_descriptor_set.h"

_NAME_BEGIN

__ImplementSubClass(OGLCorePipeline, GraphicsPipeline, "OGLCorePipeline")

OGLCorePipeline::OGLCorePipeline() noexcept
	: _vao(GL_NONE)
	, _vbo(GL_NONE)
	, _ibo(GL_NONE)
{
}

OGLCorePipeline::~OGLCorePipeline() noexcept
{
	this->close();
}

bool
OGLCorePipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(_vao == GL_NONE);
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<OGLGraphicsState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<OGLProgram>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<OGLInputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<OGLDescriptorSetLayout>());

	glCreateVertexArrays(1, &_vao);
	if (_vao == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateVertexArrays() fail");
		return false;
	}

	std::uint16_t offset = 0;

	auto& layouts = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexLayouts();
	for (auto& it : layouts)
	{
		GLuint attribIndex = GL_INVALID_INDEX;
		GLenum type = OGLTypes::asVertexFormat(it.getVertexFormat());
		if (type == GL_INVALID_ENUM)
		{
			GL_PLATFORM_LOG("Undefined vertex format.");
			return false;
		}

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
			glEnableVertexArrayAttrib(_vao, attribIndex);
			glVertexArrayAttribBinding(_vao, attribIndex, it.getVertexSlot());
			glVertexArrayAttribFormat(_vao, attribIndex, it.getVertexCount(), type, OGLTypes::isNormFormat(it.getVertexFormat()), offset + it.getVertexOffset());
		}

		offset += it.getVertexOffset() + it.getVertexSize();
	}

	auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
	for (auto& it : bindings)
	{
		auto divisor = it.getVertexDivisor();
		if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorVertex)
			glVertexArrayBindingDivisor(_vao, it.getVertexSlot(), 0);
		else if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorInstance)
			glVertexArrayBindingDivisor(_vao, it.getVertexSlot(), 1);
	}

	_pipelineDesc = pipelineDesc;
	return true;
}

void
OGLCorePipeline::close() noexcept
{
	if (_vao != GL_NONE)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

const GraphicsPipelineDesc&
OGLCorePipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

void
OGLCorePipeline::apply() noexcept
{
	glBindVertexArray(_vao);
}

void
OGLCorePipeline::bindVbo(const OGLCoreGraphicsData& vbo, GLuint slot) noexcept
{
	GLuint stride = vbo.getGraphicsDataDesc().getStride();
	glVertexArrayVertexBuffer(_vao, slot, vbo.getInstanceID(), 0, stride);
}

void
OGLCorePipeline::bindIbo(const OGLCoreGraphicsData& ibo) noexcept
{
	glVertexArrayElementBuffer(_vao, ibo.getInstanceID());
}

void
OGLCorePipeline::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCorePipeline::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END