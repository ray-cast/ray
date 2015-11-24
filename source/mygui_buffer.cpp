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
#include <ray/mygui_buffer.h>

_NAME_BEGIN

MyGuiVertexBuffer::MyGuiVertexBuffer() noexcept
	: _needVertexCount(0)
	, _sizeInBytes(0)
{
}

MyGuiVertexBuffer::~MyGuiVertexBuffer() noexcept
{
}

void
MyGuiVertexBuffer::setVertexCount(size_t _count)
{
	_needVertexCount = _count;
}

std::size_t 
MyGuiVertexBuffer::getVertexCount() noexcept
{
	return _needVertexCount;
}

MyGUI::Vertex* 
MyGuiVertexBuffer::lock() noexcept
{
	if (!_vb || _needVertexCount != _vb->getVertexCount())
	{
		VertexComponents components;
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_UNSIGNED_BYTE4, true));
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));

		if (!_vb)
			_vb = RenderFactory::createVertexBuffer();

		_vb->setVertexComponents(components);
		_vb->setup(_needVertexCount, VertexUsage::GPU_MAP_READ_BIT | VertexUsage::GPU_MAP_WRITE_BIT);
	}

	return (MyGUI::Vertex*)_vb->data();
}

void 
MyGuiVertexBuffer::unlock() noexcept
{
	if (!_buffer)
	{
		_buffer = RenderFactory::createRenderBuffer();
		_buffer->setup(_vb, nullptr);
	}
	else
	{
		_buffer->update();
	}
}

RenderBufferPtr 
MyGuiVertexBuffer::getBuffer() const
{
	return _buffer;
}

_NAME_END