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
#ifndef _H_EGL3_BUFFER_H_
#define _H_EGL3_BUFFER_H_

#include "egl_canvas.h"

_NAME_BEGIN

class EGL3BufferData : public RenderBufferData
{
public:
	EGL3BufferData() noexcept;
	~EGL3BufferData() noexcept;

	bool open(StreamType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept;
	void close() noexcept;

	bool is_open() const noexcept;

	streamsize size() const noexcept;

	void resize(const char* data, streamsize datasize) noexcept;

	int flush() noexcept;

	streamsize read(char* data, streamsize cnt) noexcept;
	streamsize write(const char* data, streamsize cnt) noexcept;

	streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
	streamoff tellg() noexcept;

	const char* map(std::uint32_t access) noexcept;
	void unmap() noexcept;
	bool isMapping() const noexcept;

	GLuint getInstanceID() noexcept;

	void bind() noexcept;

private:
	GLuint _buffer;
	GLenum _target;
	GLsizei _dataSize;
	GLsizei _dataOffset;
	GLsizei _dataTell;

	bool _isMapping;
	std::uint32_t _usage;
};

class EGL3VertexBuffer final : public VertexBufferData
{
public:
	EGL3VertexBuffer() noexcept;
	~EGL3VertexBuffer() noexcept;

	void open(const VertexLayout& layout, std::uint32_t usage, const char* data, streamsize count) noexcept;
	void close() noexcept;

	const VertexComponents& getVertexComponents() const noexcept;

	std::size_t getVertexCount() const noexcept;
	std::size_t getVertexDataSize() const noexcept;
	std::uint32_t getVertexSize() const noexcept;

	void bind() noexcept;
	void bindLayout() noexcept;

private:
	VertexLayout  _vertexLayout;
	EGL3BufferData _vertexData;
};

class EGL3IndexBuffer final : public IndexBufferData
{
public:
	EGL3IndexBuffer() noexcept;
	~EGL3IndexBuffer() noexcept;

	void open(IndexType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept;
	void close() noexcept;

	std::size_t getIndexCount() const noexcept;
	std::size_t getIndexDataSize() const noexcept;
	std::uint32_t getIndexSize() const noexcept;
	IndexType getIndexType() const noexcept;

	GLuint getInstanceID() noexcept;

	void bind() noexcept;

private:
	IndexType     _indexType;
	std::uint8_t  _indexSize;
	EGL3BufferData _indexData;
};

class EGL3RenderBuffer final : public RenderBuffer
{
public:
	EGL3RenderBuffer() noexcept;
	~EGL3RenderBuffer() noexcept;

	void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except;
	void close() noexcept;

	void apply() noexcept;

private:

	std::shared_ptr<EGL3VertexBuffer> _vb;
	std::shared_ptr<EGL3IndexBuffer> _ib;
};

_NAME_END

#endif