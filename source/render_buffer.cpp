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
#include <ray/render_buffer.h>

_NAME_BEGIN

RenderIndirect::RenderIndirect() noexcept
	: startVertice(0)
	, numVertices(0)
	, startIndice(0)
	, numIndices(0)
	, numInstances(0)
{
}

VertexStreams::VertexStreams() noexcept
{
}

VertexStreams::~VertexStreams() noexcept
{
	this->release();
}

void
VertexStreams::resize(std::size_t length) noexcept
{
	_data.resize(length);
}

void
VertexStreams::release() noexcept
{
	_data.clear();
}

char* 
VertexStreams::data() noexcept
{
	return (char*)_data.data();
}

const char*
VertexStreams::data() const noexcept
{
	return _data.data();
}

VertexComponent::VertexComponent() noexcept
	: _attrib(VertexAttrib::GPU_ATTRIB_POSITION)
	, _format(VertexFormat::GPU_VERTEX_FLOAT)
	, _normalize(false)
{
}

VertexComponent::VertexComponent(VertexAttrib attrib, VertexFormat format, bool normalize) noexcept
	: _attrib(attrib)
	, _format(format)
	, _normalize(normalize)
{
}

VertexComponent::~VertexComponent() noexcept
{
}

VertexAttrib
VertexComponent::getVertexAttrib() const noexcept
{
	return _attrib;
}

VertexFormat
VertexComponent::getVertexFormat() const noexcept
{
	return _format;
}

bool
VertexComponent::normalize() const noexcept
{
	return _normalize;
}

int
VertexComponent::getVertexCount() const noexcept
{
	switch (_format)
	{
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE:
		return 1;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE2:
		return 2;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE3:
		return 3;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE4:
		return 4;
	case VertexFormat::GPU_VERTEX_FLOAT:
		return 1;
	case VertexFormat::GPU_VERTEX_FLOAT2:
		return 2;
	case VertexFormat::GPU_VERTEX_FLOAT3:
		return 3;
	case VertexFormat::GPU_VERTEX_FLOAT4:
		return 4;
	}

	assert(false);
	return 0;
}

int
VertexComponent::getVertexSize() const noexcept
{
	switch (_format)
	{
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE:
		return 1;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE2:
		return 2;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE3:
		return 3;
	case VertexFormat::GPU_VERTEX_UNSIGNED_BYTE4:
		return 4;
	case VertexFormat::GPU_VERTEX_FLOAT:
		return 4;
	case VertexFormat::GPU_VERTEX_FLOAT2:
		return 8;
	case VertexFormat::GPU_VERTEX_FLOAT3:
		return 12;
	case VertexFormat::GPU_VERTEX_FLOAT4:
		return 16;
	}

	assert(false);
	return 0;
}

void
VertexLayout::setVertexComponents(const VertexComponents& component) noexcept
{
	_components = component;

	_byteSize = 0;
	for (auto& it : _components)
	{
		_byteSize += it.getVertexSize();
	}
}

const VertexComponents&
VertexLayout::getVertexComponents() const noexcept
{
	return _components;
}

std::size_t
VertexLayout::getVertexSize() const noexcept
{
	return _byteSize;
}

VertexBufferData::VertexBufferData() noexcept
	: _vertexCount(0)
	, _vertexUsage(VertexUsage::GPU_USAGE_STATIC)
{
}

VertexBufferData::~VertexBufferData() noexcept
{
	this->close();
}

void
VertexBufferData::setup(std::size_t count, VertexUsage usage) noexcept
{
	assert(0 != count && this->getVertexSize() > 0);
	_vertexCount = count;
	_vertexUsage = usage;
	_vertexStreams.resize(count * this->getVertexSize());
}

void
VertexBufferData::close() noexcept
{
	_vertexStreams.release();
}

void
VertexBufferData::setVertexComponents(const VertexComponents& components) noexcept
{
	return _vertexLayout.setVertexComponents(components);;
}

const VertexComponents&
VertexBufferData::getVertexComponents() const noexcept
{
	return _vertexLayout.getVertexComponents();
}

void
VertexBufferData::resize(std::size_t count)
{
	assert(0 != count && this->getVertexSize() > 0);
	_vertexCount = count;
	_vertexStreams.resize(count * this->getVertexSize());
}

std::size_t
VertexBufferData::getVertexCount() const noexcept
{
	return _vertexCount;
}

std::size_t
VertexBufferData::getVertexSize() const noexcept
{
	return _vertexLayout.getVertexSize();
}

std::size_t
VertexBufferData::getVertexDataSize() const noexcept
{
	return this->getVertexSize() * this->getVertexCount();
}

VertexUsage
VertexBufferData::getVertexUsage() const noexcept
{
	return _vertexUsage;
}

void*
VertexBufferData::data() noexcept
{
	return _vertexStreams.data();
}

const void*
VertexBufferData::data() const noexcept
{
	return _vertexStreams.data();
}

IndexBufferData::IndexBufferData() noexcept
	: _indexCount(0)
	, _indexType(IndexType::GPU_UINT16)
	, _indexUsage(VertexUsage::GPU_USAGE_STATIC)
{
}

IndexBufferData::~IndexBufferData() noexcept
{
	this->close();
}

void
IndexBufferData::setup(std::size_t num, IndexType type, VertexUsage usage) noexcept
{
	assert(0 != num);

	_indexCount = num;
	_indexType = type;
	_indexUsage = usage;
	if (type == IndexType::GPU_UINT16)
	{
		_indexSize = sizeof(std::int16_t);
		_indexStreams.resize(_indexSize * _indexCount);
	}
	else if (type == IndexType::GPU_UINT32)
	{
		_indexSize = sizeof(std::int32_t);
		_indexStreams.resize(_indexSize * _indexCount);
	}
}

void
IndexBufferData::close() noexcept
{
	_indexStreams.release();
}

std::size_t
IndexBufferData::getIndexCount() const noexcept
{
	return _indexCount;
}

std::size_t
IndexBufferData::getIndexSize() const noexcept
{
	return _indexSize;
}

std::size_t
IndexBufferData::getIndexDataSize() const noexcept
{
	return _indexSize * _indexCount;
}

IndexType
IndexBufferData::getIndexType() const noexcept
{
	return _indexType;
}

VertexUsage
IndexBufferData::getIndexUsage() const noexcept
{
	return _indexUsage;
}

void*
IndexBufferData::data() noexcept
{
	return _indexStreams.data();
}

const void*
IndexBufferData::data() const noexcept
{
	return _indexStreams.data();
}

RenderBuffer::RenderBuffer() noexcept
	: _bufferVertex(nullptr)
	, _bufferIndex(nullptr)
{
}

RenderBuffer::~RenderBuffer() noexcept
{
}

void
RenderBuffer::setVertexBuffer(VertexBufferDataPtr vb) noexcept
{
	_bufferVertex = vb;
}

void
RenderBuffer::setIndexBuffer(IndexBufferDataPtr ib) noexcept
{
	_bufferIndex = ib;
}

VertexBufferDataPtr
RenderBuffer::getVertexBuffer() const noexcept
{
	return _bufferVertex;
}

IndexBufferDataPtr
RenderBuffer::getIndexBuffer() const noexcept
{
	return _bufferIndex;
}

std::size_t
RenderBuffer::getNumVertices() const noexcept
{
	if (_bufferVertex)
		return _bufferVertex->getVertexCount();
	return 0;
}

std::size_t
RenderBuffer::getNumIndices() const noexcept
{
	if (_bufferIndex)
		return _bufferIndex->getIndexCount();
	return 0;
}

bool
RenderBuffer::hasIndices() const noexcept
{
	return nullptr != _bufferIndex;
}

_NAME_END