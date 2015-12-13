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
#ifndef _H_RENDER_BUFFER_H_
#define _H_RENDER_BUFFER_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT RenderIndirect final
{
public:
	std::size_t startVertice;
	std::size_t startIndice;
	std::size_t startInstances;
	std::size_t numVertices;
	std::size_t numIndices;
	std::size_t numInstances;

	RenderIndirect() noexcept;
};

class EXPORT RenderBufferData
{
public:
	RenderBufferData() noexcept;
	virtual ~RenderBufferData() noexcept;

	virtual bool is_open() const noexcept = 0;

	virtual streamsize size() const noexcept = 0;

	virtual int flush() noexcept = 0;

	virtual void resize(const char* data, streamsize count) noexcept = 0;

	virtual streamsize read(char* data, streamsize cnt) noexcept = 0;
	virtual streamsize write(const char* data, streamsize cnt) noexcept = 0;

	virtual streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept = 0;
	virtual streamoff tellg() noexcept = 0;

	virtual const char* map(std::uint32_t access) noexcept = 0;
	virtual void unmap() noexcept = 0;
	virtual bool isMapping() const noexcept = 0;

private:
	RenderBufferData(const RenderBufferData&) = delete;
	RenderBufferData& operator=(const RenderBufferData&) = delete;
};

class EXPORT RenderBufferStream
{
public:
	RenderBufferStream(RenderBufferData* data) noexcept;
	virtual ~RenderBufferStream() noexcept;

	bool is_open() const noexcept;

	streamsize size() noexcept;

	void resize(const char* data, std::size_t cnt) noexcept;

	int flush() noexcept;

	streamsize read(char* data, streamsize cnt) noexcept;
	streamsize write(const char* data, streamsize cnt) noexcept;

	streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
	streamoff tellg() noexcept;

	const char* map(std::uint32_t access) noexcept;
	void unmap() noexcept;
	bool isMapping() const noexcept;

private:
	RenderBufferStream(const RenderBufferStream&) = delete;
	RenderBufferStream& operator=(const RenderBufferStream&) = delete;

private:
	RenderBufferData* _data;
};

class EXPORT VertexStreams final
{
public:
	VertexStreams() noexcept;
	~VertexStreams() noexcept;

	void resize(std::size_t length) noexcept;
	void release() noexcept;

	char* data() noexcept;
	const char* data() const noexcept;

private:
	std::vector<char> _data;
};

class EXPORT VertexComponent final
{
public:
	VertexComponent() noexcept;
	VertexComponent(VertexAttrib attrib, VertexFormat format) noexcept;
	~VertexComponent() noexcept;

	void setVertexAttrib(VertexAttrib attrib) noexcept;
	void setVertexFormat(VertexFormat attrib) noexcept;

	VertexAttrib getVertexAttrib() const noexcept;
	VertexFormat getVertexFormat() const noexcept;

	int getVertexCount() const noexcept;
	int getVertexSize() const noexcept;

public:

	VertexAttrib _attrib;
	VertexFormat _format;
};

class EXPORT VertexLayout final
{
public:
	void setVertexComponents(const VertexComponents& component) noexcept;
	const VertexComponents& getVertexComponents() const noexcept;

	void addComponent(const VertexComponent& compoent) noexcept;
	void removeComponent(const VertexComponent& compoent) noexcept;

	std::uint32_t getVertexSize() const noexcept;

private:
	VertexComponents _components;
};

class EXPORT VertexBufferData : public RenderBufferStream
{
public:
	VertexBufferData(RenderBufferData* data) noexcept;
	virtual ~VertexBufferData() noexcept;

	virtual void open(const VertexLayout& layout, std::uint32_t usage, const char* data, streamsize datasize) noexcept = 0;
	virtual void close() noexcept = 0;

	virtual const VertexComponents& getVertexComponents() const noexcept = 0;

	virtual std::size_t getVertexCount() const noexcept = 0;
	virtual std::size_t getVertexDataSize() const noexcept = 0;
	virtual std::uint32_t getVertexSize() const noexcept = 0;

private:
	VertexBufferData(const VertexBufferData&) = delete;
	VertexBufferData& operator=(const VertexBufferData&) = delete;
};

class EXPORT IndexBufferData : public RenderBufferStream
{
public:
	IndexBufferData(RenderBufferData* data) noexcept;
	virtual ~IndexBufferData() noexcept;

	virtual void open(IndexType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept = 0;
	virtual void close() noexcept = 0;

	virtual std::size_t getIndexCount() const noexcept = 0;
	virtual std::size_t getIndexDataSize() const noexcept = 0;
	virtual std::uint32_t getIndexSize() const noexcept = 0;
	virtual IndexType getIndexType() const noexcept = 0;

private:
	IndexBufferData(const IndexBufferData&) = delete;
	IndexBufferData& operator=(const IndexBufferData&) = delete;
};

class EXPORT RenderBuffer
{
public:
	RenderBuffer() noexcept;
	virtual ~RenderBuffer() noexcept;

	virtual void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except = 0;
	virtual void close() noexcept = 0;

	virtual void apply() noexcept = 0;

	std::size_t getNumVertices() const noexcept;
	std::size_t getNumIndices() const noexcept;

	void setVertexBuffer(VertexBufferDataPtr vb) noexcept;
	void setIndexBuffer(IndexBufferDataPtr ib) noexcept;

	VertexBufferDataPtr getVertexBuffer() const noexcept;
	IndexBufferDataPtr getIndexBuffer() const noexcept;

private:
	RenderBuffer(const RenderBuffer& copy) noexcept = delete;
	RenderBuffer& operator=(const RenderBuffer&) noexcept = delete;

private:

	VertexBufferDataPtr _bufferVertex;
	IndexBufferDataPtr  _bufferIndex;
};

_NAME_END

#endif