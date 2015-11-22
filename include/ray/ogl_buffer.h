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
#ifndef _H_OGL_BUFFER_H_
#define _H_OGL_BUFFER_H_

#include <ray/ogl_canvas.h>

_NAME_BEGIN

class OGLVertexBuffer final
{
public:
	OGLVertexBuffer() noexcept;
	~OGLVertexBuffer() noexcept;

	void setup(VertexBufferDataPtr vb) except;
	void close() noexcept;
	
	void update() noexcept;

	GLuint getInstanceID() noexcept;
	GLuint64 getInstanceAddr() noexcept;

	VertexBufferDataPtr getVertexBufferData() const noexcept;

private:

	GLuint _vbo;
	GLuint64 _bindlessVbo;

	GLuint _dataSize;

	VertexBufferDataPtr _vb;
};

class OGLIndexBuffer final
{
public:
	OGLIndexBuffer() noexcept;
	~OGLIndexBuffer() noexcept;

	void setup(IndexBufferDataPtr ib) noexcept;
	void close() noexcept;

	void update() noexcept;

	GLuint getInstanceID() noexcept;
	GLuint64 getInstanceAddr() noexcept;

	IndexBufferDataPtr getIndexBufferData() const noexcept;

private:

	GLuint _ibo;
	GLuint64 _bindlessIbo;

	IndexBufferDataPtr _ib;
};

class OGLRenderBuffer final : public RenderBuffer
{
public:
	OGLRenderBuffer() noexcept;
	~OGLRenderBuffer() noexcept;

	void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except;
	void close() noexcept;

	void update() noexcept;

	GLuint getInstanceID() noexcept;

	void apply() noexcept;

private:

	GLuint _vao;

	std::shared_ptr<OGLVertexBuffer> _vb;
	std::shared_ptr<OGLIndexBuffer> _ib;
};

_NAME_END

#endif