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
#include <ray/al_sound_source.h>
#include <ray/sound_buffer.h>

_NAME_BEGIN

ALSoundSource::ALSoundSource() noexcept
	: _source(AL_NONE)
{
}

ALSoundSource::~ALSoundSource() noexcept
{
	this->close();
}

void
ALSoundSource::open()
{
    assert(_source == AL_NONE);
    ::alGenSources(1, &_source);
}

void
ALSoundSource::close() noexcept
{
	if (_source != AL_NONE)
	{
		::alDeleteSources(1, &_source);
		_source = AL_NONE;
	}
}

void
ALSoundSource::play()
{
    assert(_source != AL_NONE);
    ::alSourcePlay(_source);
}

void
ALSoundSource::stop()
{
    assert(_source != AL_NONE);
    ::alSourceStop(_source);
}

void
ALSoundSource::pause()
{
    assert(_source != AL_NONE);
    ::alSourcePause(_source);
}

void
ALSoundSource::resume()
{
    assert(_source != AL_NONE);
    ::alSourcePlay(_source);
}

void
ALSoundSource::setSoundBuffer(SoundBufferPtr buffer)
{
    assert(_source != AL_NONE);
    if (buffer)
    {
        _buffer = buffer;
        auto bufferData = static_cast<const ALuint*>(_buffer->getBuffer());
        ::alSourceQueueBuffers(_source, 1, bufferData);
    }
}

SoundBufferPtr
ALSoundSource::getSoundBuffer() const noexcept
{
    assert(_source != AL_NONE);
    ALuint bufferData = 0;
    ::alSourceUnqueueBuffers(_source, 1, &bufferData);
    return _buffer;
}

_NAME_END