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

class EXPORT GraphicsData
{
public:
	GraphicsData() noexcept;
	virtual ~GraphicsData() noexcept;

	virtual bool is_open() const noexcept = 0;

	virtual streamsize size() const noexcept = 0;

	virtual int flush() noexcept = 0;
	virtual int flush(ios_base::off_type offset, streamsize cnt) noexcept = 0;

	virtual void resize(const char* data, streamsize count) noexcept = 0;

	virtual streamsize read(char* data, streamsize cnt) noexcept = 0;
	virtual streamsize write(const char* data, streamsize cnt) noexcept = 0;

	virtual streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept = 0;
	virtual streamoff tellg() noexcept = 0;

	virtual const char* map(std::uint32_t access) noexcept = 0;
	virtual const char* map(ios_base::off_type offset, streamsize cnt, std::uint32_t access) noexcept = 0;
	virtual void unmap() noexcept = 0;
	virtual bool isMapping() const noexcept = 0;

private:
	GraphicsData(const GraphicsData&) = delete;
	GraphicsData& operator=(const GraphicsData&) = delete;
};

class EXPORT GraphicsStream
{
public:
	GraphicsStream(GraphicsData* data) noexcept;
	virtual ~GraphicsStream() noexcept;

	bool is_open() const noexcept;

	streamsize size() noexcept;

	void resize(const char* data, std::size_t cnt) noexcept;

	int flush() noexcept;
	int flush(ios_base::off_type offset, streamsize cnt) noexcept;

	streamsize read(char* data, streamsize cnt) noexcept;
	streamsize write(const char* data, streamsize cnt) noexcept;

	streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
	streamoff tellg() noexcept;

	const char* map(std::uint32_t access) noexcept;
	const char* map(ios_base::off_type offset, streamsize cnt, std::uint32_t access) noexcept;
	void unmap() noexcept;
	bool isMapping() const noexcept;

private:
	GraphicsStream(const GraphicsStream&) = delete;
	GraphicsStream& operator=(const GraphicsStream&) = delete;

private:
	GraphicsData* _data;
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

private:
	VertexAttrib _attrib;
	VertexFormat _format;
};

class EXPORT VertexLayout final
{
public:
	VertexLayout() noexcept;
	~VertexLayout() noexcept;

	void setVertexComponents(const VertexComponents& component) noexcept;
	const VertexComponents& getVertexComponents() const noexcept;

	void addComponent(const VertexComponent& compoent) noexcept;
	void removeComponent(const VertexComponent& compoent) noexcept;

	std::uint32_t getVertexSize() const noexcept;

private:
	VertexComponents _components;
};

class EXPORT VertexBufferData : public GraphicsStream
{
public:
	VertexBufferData(GraphicsData* data) noexcept;
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

class EXPORT IndexBufferData : public GraphicsStream
{
public:
	IndexBufferData(GraphicsData* data) noexcept;
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

class EXPORT RenderIndirect final
{
public:
	std::int32_t startVertice;
	std::int32_t startIndice;
	std::int32_t startInstances;
	std::int32_t numVertices;
	std::int32_t numIndices;
	std::int32_t numInstances;

	RenderIndirect() noexcept;
};

class EXPORT RenderBuffer
{
public:
	RenderBuffer() noexcept;
	virtual ~RenderBuffer() noexcept;

	virtual void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except = 0;
	virtual void close() noexcept = 0;

	virtual void apply() noexcept = 0;

	virtual std::size_t getNumVertices() const noexcept = 0;
	virtual std::size_t getNumIndices() const noexcept = 0;

	virtual VertexBufferDataPtr getVertexBuffer() const noexcept = 0;
	virtual IndexBufferDataPtr getIndexBuffer() const noexcept = 0;

private:
	RenderBuffer(const RenderBuffer& copy) noexcept = delete;
	RenderBuffer& operator=(const RenderBuffer&) noexcept = delete;
};

_NAME_END

#endif