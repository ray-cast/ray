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
#include <ray/gui_buffer.h>
#include <ray/render_buffer.h>
#include <GL/glew.h>

_NAME_BEGIN

using namespace Gui;

GuiVertexBuffer::GuiVertexBuffer() noexcept
	: _bufferID(0)
	, _vao(0)
	, _needVertexCount(0)
	, _sizeInBytes(0)
{
}

GuiVertexBuffer::~GuiVertexBuffer() noexcept
{
	destroy();
}

void
GuiVertexBuffer::setVertexCount(size_t _count)
{
	if (_count != _needVertexCount)
	{
		_needVertexCount = _count;
		destroy();
		create();
	}
}

std::size_t 
GuiVertexBuffer::getVertexCount() noexcept
{
	return _needVertexCount;
}

MyGUI::Vertex* 
GuiVertexBuffer::lock() noexcept
{
	_stream.resize(_sizeInBytes);
	return (MyGUI::Vertex*)_stream.map();
}

void 
GuiVertexBuffer::unlock() noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
	glBufferData(GL_ARRAY_BUFFER, _sizeInBytes, _stream.map(), GL_STREAM_DRAW);	
}

void 
GuiVertexBuffer::destroy()
{
	if (_bufferID != 0)
	{
		glDeleteBuffers(1, &_bufferID);
		_bufferID = 0;
	}

	if (_vao != 0)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}
}

void 
GuiVertexBuffer::create()
{
	MYGUI_PLATFORM_ASSERT(!_bufferID, "Vertex buffer already exist");

	_sizeInBytes = _needVertexCount * sizeof(MyGUI::Vertex);
	void* data = 0;

	glGenBuffers(1, &_bufferID);
	glGenVertexArrays(1, &_vao);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
	glBufferData(GL_ARRAY_BUFFER, _sizeInBytes, data, GL_STREAM_DRAW);

	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if (_sizeInBytes != (size_t)bufferSize)
	{
		destroy();
		MYGUI_PLATFORM_EXCEPT("Data size is mismatch with input array");
	}

	glEnableVertexAttribArray(GPU_ATTRIB_POSITION);
	glEnableVertexAttribArray(GPU_ATTRIB_DIFFUSE);
	glEnableVertexAttribArray(GPU_ATTRIB_TEXCOORD);
	glVertexAttribPointer(GPU_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(MyGUI::Vertex), (GLubyte *)NULL);
	glVertexAttribPointer(GPU_ATTRIB_DIFFUSE, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(MyGUI::Vertex), (GLubyte *)offsetof(struct MyGUI::Vertex, colour));
	glVertexAttribPointer(GPU_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(MyGUI::Vertex), (GLubyte *)offsetof(struct MyGUI::Vertex, u));
}

_NAME_END