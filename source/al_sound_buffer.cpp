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
#include <ray/al_sound_buffer.h>

_NAME_BEGIN

ALSoundBuffer::ALSoundBuffer() noexcept
	: _buffer(AL_NONE)
	, _soundFreqency(0)
	, _soundType(SoundFormat::None)
{
}

ALSoundBuffer::~ALSoundBuffer() noexcept
{
	this->close();
}

bool
ALSoundBuffer::open()
{
	assert(_buffer == AL_NONE);
    ::alGenBuffers(1, &_buffer);
    return true;
}

void
ALSoundBuffer::close() noexcept
{
	if (_buffer != AL_NONE)
	{
		::alDeleteBuffers(1, &_buffer);
		_buffer = AL_NONE;
	}
}

bool
ALSoundBuffer::isOpen() const noexcept
{
    return _buffer ? true : false;
}

void
ALSoundBuffer::setBufferType(const SoundFormat type) noexcept
{
    _soundType = type;
}

void
ALSoundBuffer::setBufferFrequency(const SoundFrequency freq) noexcept
{
    _soundFreqency = freq;
}

const void*
ALSoundBuffer::getBuffer() const noexcept
{
	return &_buffer;
}

void
ALSoundBuffer::setBuffer(void* buffer)
{
	assert(_buffer == AL_NONE);
	_buffer = *static_cast<ALuint*>(buffer);
}

void
ALSoundBuffer::readtBufferData(void* dest, std::size_t size) const
{
	assert(_buffer != AL_NONE);
	assert(dest != nullptr && size != 0 && _data.size() > size);

	std::memcpy(dest, _data.data(), size);
}

void
ALSoundBuffer::writeBufferData(const void* src, std::size_t size)
{
	assert(_buffer != AL_NONE);
	assert(src != nullptr && size != 0);

    if (_data.size() != size)
        _data.resize(size);

    std::memcpy(&_data[0], src, size);

    alBufferData(_buffer, static_cast<ALenum>(_soundType), _data.data(),  static_cast<ALsizei>(_data.size()), _soundFreqency);
}

_NAME_END