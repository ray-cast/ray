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
#if defined(_BUILD_MYGUI)
#include "mygui_buffer.h"

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
MyGuiVertexBuffer::setVertexCount(std::size_t _count)
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
		if (!_vb)
		{
			VertexLayout layout;
			layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
			layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_UNSIGNED_BYTE4));
			layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));

			auto usage = VertexUsage::MAP_READ_BIT | VertexUsage::MAP_WRITE_BIT;

			_vb = RenderSystem::instance()->createVertexBufferData();
			_vb->open(layout, usage, nullptr, layout.getVertexSize() * _needVertexCount);

			_buffer = RenderSystem::instance()->createRenderBuffer();
			_buffer->setup(_vb, nullptr);
		}
		else
		{
			_vb->resize(nullptr, _needVertexCount * _vb->getVertexSize());
		}
	}

	return (MyGUI::Vertex*)_vb->map(AccessMapping::MAP_READ_BIT | AccessMapping::MAP_WRITE_BIT);
}

void 
MyGuiVertexBuffer::unlock() noexcept
{
	_vb->unmap();
}

RenderBufferPtr 
MyGuiVertexBuffer::getBuffer() const
{
	return _buffer;
}

_NAME_END
#endif