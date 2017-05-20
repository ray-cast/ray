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
#include "ogl_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(OGLGraphicsData, GraphicsData, "OGLGraphicsData")

OGLGraphicsData::OGLGraphicsData() noexcept
	: _buffer(GL_NONE)
	, _data(nullptr)
{
}

OGLGraphicsData::~OGLGraphicsData() noexcept
{
	this->close();
}

bool
OGLGraphicsData::setup(const GraphicsDataDesc& desc) noexcept
{
	assert(_buffer == GL_NONE);
	assert(desc.getStreamSize() > 0);

	_desc = desc;
	_data = nullptr;

	auto type = desc.getType();
	if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
		_target = GL_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
		_target = GL_ELEMENT_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeUniformBuffer)
		_target = GL_UNIFORM_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageTexelBuffer)
		_target = GL_TEXTURE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
	{
		GL_PLATFORM_LOG("Can't support SSBO.");
		return false;
	}
	else
	{
		GL_PLATFORM_LOG("Unkown data type.");
		return false;
	}

	GLenum flags = GL_STATIC_DRAW;

	auto usage = desc.getUsage();
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
		flags = GL_STATIC_DRAW;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit)
		flags = GL_DYNAMIC_READ;

	glGenBuffers(1, &_buffer);
	glBindBuffer(_target, _buffer);
	glBufferData(_target, desc.getStreamSize(), desc.getStream(), flags);

	return true;
}

bool
OGLGraphicsData::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
OGLGraphicsData::close() noexcept
{
	assert(!_data);

	if (_data)
	{
		this->unmap();
	}

	if (_buffer)
	{
		glDeleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}

bool 
OGLGraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
{
	assert(data);
	assert(!_data);
	glBindBuffer(_target, _buffer);
	_data = *data = glMapBufferRange(_target, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	return _data ? true : false;
}

void
OGLGraphicsData::unmap() noexcept
{
	if (_data)
	{
		glBindBuffer(_target, _buffer);
		glUnmapBuffer(_target);
		_data = nullptr;
	}
}

GLuint
OGLGraphicsData::getInstanceID() const noexcept
{
	return _buffer;
}

const GraphicsDataDesc&
OGLGraphicsData::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
OGLGraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLGraphicsData::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END