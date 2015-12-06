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
ALSoundSource::setSoundBuffer(SoundBufferPtr buffer)
{
	assert(_source != AL_NONE);
	if (buffer)
	{
		auto bufferData = static_cast<const ALuint*>(buffer->getBuffer());
		::alSourceQueueBuffers(_source, 1, bufferData);
		_buffer = buffer;
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

void
ALSoundSource::setVolume(float volume) noexcept
{
	assert(_source != AL_NONE);
	alSourcef(_source, AL_GAIN, volume);
}

float
ALSoundSource::getVolume() const noexcept
{
	assert(_source != AL_NONE);
	float volume = -1.0f;
	alGetSourcef(_source, AL_GAIN, &volume);
	return volume;
}

void
ALSoundSource::setPitch(float pitch) noexcept
{
	assert(_source != AL_NONE);
	alSourcef(_source, AL_PITCH, pitch);
}

float
ALSoundSource::getPitch(void) const noexcept
{
	assert(_source != AL_NONE);
	float pitch = -1.0f;
	alGetSourcef(_source, AL_PITCH, &pitch);
	return pitch;
}

void
ALSoundSource::setLoop(bool loop) noexcept
{
	assert(_source != AL_NONE);
	alSourcei(_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

bool
ALSoundSource::getLoop(void) const noexcept
{
	assert(_source != AL_NONE);
	int loop = false;
	alGetSourcei(_source, AL_LOOPING, &loop);
	return loop == AL_FALSE ? false : true;
}

void
ALSoundSource::setMaxDistance(float maxdis) noexcept
{
	assert(_source != AL_NONE);
	alSourcef(_source, AL_MAX_DISTANCE, maxdis);
}

float
ALSoundSource::getMaxDistance() const noexcept
{
	assert(_source != AL_NONE);
	float distance = false;
	alGetSourcef(_source, AL_MAX_DISTANCE, &distance);
	return distance;
}

void
ALSoundSource::setMinDistance(float mindis) noexcept
{
	assert(_source != AL_NONE);
	alSourcef(_source, AL_REFERENCE_DISTANCE, mindis);
}

float
ALSoundSource::getMinDistance() const noexcept
{
	assert(_source != AL_NONE);
	float distance = false;
	alGetSourcef(_source, AL_REFERENCE_DISTANCE, &distance);
	return distance;
}

void 
ALSoundSource::setTransform3D(const float3& position, const float3& velocity, const float3& forward, const float3& up) noexcept
{
	assert(_source != AL_NONE);

	ALfloat pos[] = { position.x, position.y, position.z };
	ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
	ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z};

	alSourcefv(_source, AL_POSITION, pos);
	alSourcefv(_source, AL_VELOCITY, vel);
	alSourcefv(_source, AL_DIRECTION, dir);
}

void 
ALSoundSource::getTransform3D(float3& position, float3& velocity, float3& forward, float3& up) noexcept
{
	assert(_source != AL_NONE);

	ALfloat pos[3];
	ALfloat vel[3];
	ALfloat dir[6];
	alGetSourcefv(_source, AL_POSITION, pos);
	alGetSourcefv(_source, AL_VELOCITY, vel);
	alGetSourcefv(_source, AL_DIRECTION, dir);

	position.set(pos[0], pos[1], pos[2]);
	velocity.set(vel[0], vel[1], vel[2]);
	forward.set(dir[0], dir[1], dir[2]);
	up.set(dir[3], dir[4], dir[5]);
}

void
ALSoundSource::play() noexcept
{
    assert(_source != AL_NONE);
    ::alSourcePlay(_source);
}

void
ALSoundSource::stop() noexcept
{
    assert(_source != AL_NONE);
    ::alSourceStop(_source);
}

void
ALSoundSource::pause() noexcept
{
    assert(_source != AL_NONE);
    ::alSourcePause(_source);
}

bool 
ALSoundSource::isPlaying() const noexcept
{
	if (_source != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_source, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	return false;
}

bool 
ALSoundSource::isStopped() const noexcept
{
	if (_source != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_source, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED;
	}

	return true;
}

bool
ALSoundSource::isPaused() const noexcept
{
	if (_source != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_source, AL_SOURCE_STATE, &state);
		return state == AL_PAUSED;
	}

	return false;
}

_NAME_END