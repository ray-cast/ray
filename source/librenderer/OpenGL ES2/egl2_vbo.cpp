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
#include "egl2_vbo.h"
#include "egl2_layout.h"

_NAME_BEGIN

__ImplementSubClass(EGL2VertexBuffer, EGL2GraphicsData, "EGL2VertexBuffer")

EGL2VertexBuffer::EGL2VertexBuffer() noexcept
	: EGL2GraphicsData(&_vertexData)
{
}

EGL2VertexBuffer::EGL2VertexBuffer(const GraphicsDataDesc& desc) noexcept
	: EGL2GraphicsData(&_vertexData)
{
	this->open(desc);
}

EGL2VertexBuffer::~EGL2VertexBuffer() noexcept
{
	this->close();
}

void
EGL2VertexBuffer::open(const GraphicsDataDesc& desc) noexcept
{
	assert(desc.getType() == GraphicsStream::VBO);
	assert(desc.getStreamSize() > 0);
	_vertexData.open(desc);
}

void
EGL2VertexBuffer::close() noexcept
{
	_vertexData.close();
}

bool
EGL2VertexBuffer::is_open() const noexcept
{
	return _vertexData.is_open();
}

void
EGL2VertexBuffer::bind(const EGL2GraphicsLayoutPtr& layout) noexcept
{
	GLuint offset = 0;
	GLsizei vertexByteSize = layout->getVertexSize();
	//glBindVertexBuffer(0, _vertexData.getInstanceID(), 0, vertexByteSize);
}

_NAME_END