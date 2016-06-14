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
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3GraphicsData, GraphicsData, "EGL3GraphicsData")

EGL3GraphicsData::EGL3GraphicsData() noexcept
	: _buffer(GL_NONE)
{
}

EGL3GraphicsData::~EGL3GraphicsData() noexcept
{
	this->close();
}

bool
EGL3GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
{
	assert(!_buffer);
	assert(desc.getStride() > 0);
	assert(desc.getStreamSize() > 0);

	_desc = desc;

	auto type = desc.getType();
	if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
		_target = GL_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
		_target = GL_ELEMENT_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeUniformBuffer)
		_target = GL_UNIFORM_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageTexelBuffer)
		_target = GL_TEXTURE_BUFFER_EXT;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_SHADER_STORAGE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_DRAW_INDIRECT_BUFFER;
	else
		return false;

	GLenum flags = GL_STATIC_DRAW;

	auto usage = desc.getUsage();
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
		flags = GL_STATIC_DRAW;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit)
		flags = GL_DYNAMIC_READ;

	GL_CHECK(glGenBuffers(1, &_buffer));
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags));
	
	return true;
}

void
EGL3GraphicsData::close() noexcept
{
	if (_buffer != GL_NONE)
	{
		glDeleteBuffers(1, &_buffer);
		_buffer = GL_NONE;
	}
}

int
EGL3GraphicsData::flush() noexcept
{
	return this->flush(0, _desc.getStreamSize());
}

int
EGL3GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glFlushMappedBufferRange(_buffer, offset, cnt));
	return cnt;
}

bool
EGL3GraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
{
	assert(data);
	GL_CHECK(glBindBuffer(_target, _buffer));
	*data = glMapBufferRange(_target, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	return *data ? true : false;
}

void
EGL3GraphicsData::unmap() noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glUnmapBuffer(_target));
}

GLuint
EGL3GraphicsData::getInstanceID() const noexcept
{
	return _buffer;
}

const GraphicsDataDesc&
EGL3GraphicsData::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
EGL3GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3GraphicsData::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END