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
#include <ray/sound_buffer.h>

_NAME_BEGIN

SoundBuffer::SoundBuffer() noexcept
{
}

SoundBuffer::~SoundBuffer() noexcept
{
}

SoundReader::SoundReader(SoundBuffer* buf) noexcept
	: StreamReader(buf)
	, _buf(buf)
{
}

SoundReader::~SoundReader() noexcept
{
}

bool
SoundReader::open(StreamReaderPtr stream) noexcept
{
	return this->_buf->open(stream);
}

bool
SoundReader::access(StreamReader& stream) const noexcept
{
	return this->_buf->access(stream);
}

std::uint8_t
SoundReader::getBufferChannelCount() const noexcept
{
	return this->_buf->getBufferChannelCount();
}

std::size_t
SoundReader::getBufferTotalSamples() const noexcept
{
	return this->_buf->getBufferTotalSamples();
}

SoundFormat
SoundReader::getBufferType() const noexcept
{
	return this->_buf->getBufferType();
}

SoundFrequency
SoundReader::getBufferFrequency() const noexcept
{
	return this->_buf->getBufferFrequency();
}

_NAME_END