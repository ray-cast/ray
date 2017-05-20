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
#ifndef _H_EGL3_RENDER_PIPELINE_H_
#define _H_EGL3_RENDER_PIPELINE_H_

#include "egl3_types.h"

_NAME_BEGIN

class EGL3Pipeline final : public GraphicsPipeline
{
	__DeclareSubClass(EGL3Pipeline, GraphicsPipeline)
public:
	EGL3Pipeline() noexcept;
	virtual ~EGL3Pipeline() noexcept;

	bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
	void close() noexcept;

	void bindVertexBuffers(EGL3VertexBuffers& vbos, bool forceUpdate) noexcept;

	void apply() noexcept;

	const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3Pipeline(const EGL3Pipeline&) noexcept = delete;
	EGL3Pipeline& operator=(const EGL3Pipeline&) noexcept = delete;

private:
	struct VertexAttrib
	{
		GLenum type;
		GLuint index;
		GLuint slot;
		GLuint count;
		GLboolean normalize;
		GLintptr offset;
	};

	struct VertexBinding
	{
		GLuint slot;
		GLuint divisor;
		GLuint stride;
	};

	std::vector<VertexAttrib> _attributes;
	std::vector<VertexBinding> _bindings;

	GraphicsPipelineDesc _pipelineDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif