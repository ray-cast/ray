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

#include <ray/graphics_data.h>

_NAME_BEGIN

MyGuiVertexBuffer::MyGuiVertexBuffer() noexcept
	: _vertexCount(0)
	, _needVertexCount(0)
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
	if (!_vb || _needVertexCount != _vertexCount)
	{
		std::uint32_t inputSize = 0;
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
		inputSize += GraphicsVertexLayout::getVertexSize(GraphicsFormat::GraphicsFormatR32G32SFloat);
		
		if (!_vb)
		{
			GraphicsDataDesc vb;
			vb.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit | GraphicsUsageFlags::GraphicsUsageFlagsWriteBit);
			vb.setStride(inputSize);
			vb.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
			vb.setStreamSize(inputSize * _needVertexCount);

			_vb = RenderSystem::instance()->createGraphicsData(vb);

			_buffer = RenderSystem::instance()->createRenderMesh(_vb, nullptr);

			_vertexCount = _needVertexCount;
		}
		else
		{
			RenderSystem::instance()->updateBuffer(_vb, nullptr, inputSize * _needVertexCount);
		}
	}

	return (MyGUI::Vertex*)RenderSystem::instance()->mapBuffer(_vb, GraphicsAccessFlagsBits::GraphicsAccessFlagsMapReadBit | GraphicsAccessFlagsBits::GraphicsAccessFlagsMapWriteBit);
}

void 
MyGuiVertexBuffer::unlock() noexcept
{
	RenderSystem::instance()->unmapBuffer(_vb);
}

RenderMeshPtr
MyGuiVertexBuffer::getBuffer() const
{
	return _buffer;
}

_NAME_END

#endif