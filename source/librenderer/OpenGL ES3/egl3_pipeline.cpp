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
#include "egl3_pipeline.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_input_layout.h"
#include "egl3_descriptor.h"
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Pipeline, GraphicsPipeline, "EGL3Pipeline")

EGL3Pipeline::EGL3Pipeline() noexcept
	: _vao(GL_NONE)
{
}

EGL3Pipeline::~EGL3Pipeline() noexcept
{
	this->close();
}

bool
EGL3Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(_vao == GL_NONE);
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<EGL3GraphicsState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<EGL3Program>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<EGL3InputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<EGL3DescriptorSetLayout>());

	GL_CHECK(glGenVertexArrays(1, &_vao));
	if (_vao == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateVertexArrays() fail");
		return false;
	}

	GL_CHECK(glBindVertexArray(_vao));

	std::uint32_t offset = 0;

	auto& layouts = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexLayouts();
	for (auto& it : layouts)
	{
		GLenum type = EGL3Types::asVertexFormat(it.getVertexFormat());
		if (type == GL_INVALID_ENUM)
		{
			GL_PLATFORM_LOG("Undefined vertex format.");
			return false;
		}

		GLuint attribIndex = GL_INVALID_INDEX;

		auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
		for (auto& attrib : attributes)
		{
			if (attrib->getSemantic() == it.getSemantic() &&
				attrib->getSemanticIndex() == it.getSemanticIndex())
			{
				attribIndex = attrib->downcast<EGL3GraphicsAttribute>()->getBindingPoint();

				GL_CHECK(glEnableVertexAttribArray(attribIndex));
				GL_CHECK(glVertexAttribBinding(attribIndex, it.getVertexSlot()));
				GL_CHECK(glVertexAttribFormat(attribIndex, it.getVertexCount(), type, EGL3Types::isScaledFormat(it.getVertexFormat()), offset + it.getVertexOffset()));

				break;
			}
		}

		offset += it.getVertexOffset() + it.getVertexSize();
	}

	auto& bindings = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getVertexBindings();
	for (auto& it : bindings)
	{
		for (auto& layout : layouts)
		{
			if (layout.getVertexSlot() != it.getVertexSlot())
				continue;

			auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
			for (auto& attrib : attributes)
			{
				if (attrib->getSemantic() == layout.getSemantic() &&
					attrib->getSemanticIndex() == layout.getSemanticIndex())
				{
					GLuint index = attrib->downcast<EGL3GraphicsAttribute>()->getBindingPoint();

					auto divisor = it.getVertexDivisor();
					if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorVertex)
						GL_CHECK(glVertexAttribDivisor(index, 0));
					else if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorInstance)
						GL_CHECK(glVertexAttribDivisor(index, 1));
				}
			}
		}
	}

	GL_CHECK(glBindVertexArray(GL_NONE));

	_pipelineDesc = pipelineDesc;
	return true;
}

void
EGL3Pipeline::close() noexcept
{
	if (_vao != GL_NONE)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

void
EGL3Pipeline::apply() noexcept
{
	GL_CHECK(glBindVertexArray(_vao));
}

void
EGL3Pipeline::bindVbo(const EGL3GraphicsData& vbo, GLsizei startVertices) noexcept
{
	GLuint stride = vbo.getGraphicsDataDesc().getStride();
	GL_CHECK(glBindVertexBuffer(0, vbo.getInstanceID(), startVertices * stride, stride));
}

void
EGL3Pipeline::bindIbo(const EGL3GraphicsData& ibo) noexcept
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.getInstanceID()));
}

const GraphicsPipelineDesc&
EGL3Pipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

void
EGL3Pipeline::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Pipeline::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END