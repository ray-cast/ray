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

_NAME_BEGIN

ALSoundSource::ALSoundSource() noexcept
	: _alSource(AL_NONE)
	, _alFormat(AL_NONE)
	, _isPlaying(false)
	, _isPlayEnd(false)
	, _isLoop(false)
{
	std::memset(_alBuffer, 0, sizeof(_alBuffer));
}

ALSoundSource::~ALSoundSource() noexcept
{
	this->close();
}

void
ALSoundSource::open() noexcept
{
    assert(_alSource == AL_NONE);
    ::alGenSources(1, &_alSource);
	::alSourcei(_alSource, AL_SOURCE_TYPE, AL_STREAMING);
	::alSourcei(_alSource, AL_LOOPING, AL_FALSE);
	::alSourcei(_alSource, AL_SOURCE_RELATIVE, AL_TRUE);

	::alGenBuffers(1, &_alBuffer[0]);
	::alGenBuffers(1, &_alBuffer[1]);
}

void
ALSoundSource::close() noexcept
{
	if (_alSource != AL_NONE)
	{
		this->play(false);

		::alDeleteSources(1, &_alSource);
		_alSource = AL_NONE;
	}

	for (auto& it : _alBuffer)
	{
		if (it != AL_NONE)
		{
			::alDeleteBuffers(1, &it);
			it = AL_NONE;
		}
	}

	_soundReader = nullptr;
}

void 
ALSoundSource::addSoundSourceListener(SoundSourceListener* listener) noexcept
{
	assert(std::find(_listeners.begin(), _listeners.end(), listener) == _listeners.end());
	_listeners.push_back(listener);
}

void 
ALSoundSource::removeSoundSourceListener(SoundSourceListener* listener) noexcept
{
	auto it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it != _listeners.end())
	{
		_listeners.erase(it);
	}
}

void
ALSoundSource::setSoundReader(SoundReaderPtr buffer) noexcept
{
	if (_soundReader != buffer)
	{
		_soundReader = buffer;

		if (buffer)
		{
			auto soundType = buffer->getBufferType();
			_alFormat = AL_NONE;
			if (soundType == SoundFormat::Mono8)
				_alFormat = AL_FORMAT_MONO8;
			else if (soundType == SoundFormat::Mono16)
				_alFormat = AL_FORMAT_MONO16;
			else if (soundType == SoundFormat::Stereo8)
				_alFormat = AL_FORMAT_STEREO8;
			else if (soundType == SoundFormat::Stereo16)
				_alFormat = AL_FORMAT_STEREO16;
			else if (soundType == SoundFormat::Quad16)
				_alFormat = AL_FORMAT_QUAD16;
			else if (soundType == SoundFormat::Chn16)
				_alFormat = AL_FORMAT_51CHN16;

			SoundClip clip;
			clip.length = 0;
			clip.samples = buffer->getBufferTotalSamples();
			clip.channels = buffer->getBufferChannelCount();
			clip.freq = buffer->getBufferFrequency();

			this->setSoundClip(clip);
		}
	}
}

void
ALSoundSource::setPitch(float pitch) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_PITCH, pitch);
}

void
ALSoundSource::setVolume(float volume) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_GAIN, volume);
}

void
ALSoundSource::setMinVolume(float volume) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_MIN_GAIN, volume);
}

void
ALSoundSource::setMaxVolume(float volume) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_MAX_GAIN, volume);
}

void
ALSoundSource::setTranslate(const float3& translate) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat pos[] = { translate.x, translate.y, translate.z };
	alSourcefv(_alSource, AL_POSITION, pos);
}

void
ALSoundSource::setVelocity(const float3& velocity) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
	alSourcefv(_alSource, AL_VELOCITY, vel);
}

void
ALSoundSource::setOrientation(const float3& forward, const float3& up) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
	alSourcefv(_alSource, AL_DIRECTION, dir);
}

void
ALSoundSource::setSoundClip(const SoundClip& clip) noexcept
{
	if (clip.channels == 1)
	{
		_alBufferSize = clip.freq >> 1;
		_alBufferSize -= (_alBufferSize % 2);
	}
	else if (clip.channels == 2)
	{
		_alBufferSize = clip.freq;
		_alBufferSize -= (_alBufferSize % 4);
	}
	else if (clip.channels == 4)
	{
		_alBufferSize = clip.freq * 2;
		_alBufferSize -= (_alBufferSize % 8);
	}
	else if (clip.channels == 6)
	{
		_alBufferSize = clip.freq * 3;
		_alBufferSize -= (_alBufferSize % 12);
	}

	if (_data.size() < _alBufferSize)
		_data.resize(_alBufferSize);

	std::size_t size = _soundReader->size();

	_soundClip = clip;
	_soundClip.length = std::min(clip.length, size);
	_soundClip.samples = std::min(clip.samples, size);

	_soundReader->seekg(_soundClip.length, ios_base::beg);

	_alSampleLength = _soundClip.samples / (_soundReader->size() / _alBufferSize);
	_alSampleLengthTotal = 0;
}

void
ALSoundSource::setMaxDistance(float maxdis) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_MAX_DISTANCE, maxdis);
}

void
ALSoundSource::setMinDistance(float mindis) noexcept
{
	assert(_alSource != AL_NONE);
	alSourcef(_alSource, AL_REFERENCE_DISTANCE, mindis);
}

SoundReaderPtr
ALSoundSource::getSoundBuffer() const noexcept
{
	assert(_alSource != AL_NONE);
	ALint bufferData = 0;
	::alGetSourceiv(_alSource, AL_BUFFER, &bufferData);
	return _soundReader;
}

float
ALSoundSource::getVolume() const noexcept
{
	assert(_alSource != AL_NONE);
	float volume = -1.0f;
	alGetSourcef(_alSource, AL_GAIN, &volume);
	return volume;
}

float
ALSoundSource::getMinVolume() const noexcept
{
	assert(_alSource != AL_NONE);
	float volume = -1.0f;
	alGetSourcef(_alSource, AL_MIN_GAIN, &volume);
	return volume;
}

float
ALSoundSource::getMaxVolume() const noexcept
{
	assert(_alSource != AL_NONE);
	float volume = -1.0f;
	alGetSourcef(_alSource, AL_MAX_GAIN, &volume);
	return volume;
}

void
ALSoundSource::getTranslate(float3& translate) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat pos[3];
	alGetSourcefv(_alSource, AL_POSITION, pos);
	translate.x = pos[0];
	translate.y = pos[1];
	translate.z = pos[2];
}

void
ALSoundSource::getVelocity(float3& velocity) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat pos[3];
	alGetSourcefv(_alSource, AL_VELOCITY, pos);
	velocity.x = pos[0];
	velocity.y = pos[1];
	velocity.z = pos[2];
}

void
ALSoundSource::getOrientation(float3& forward, float3& up) noexcept
{
	assert(_alSource != AL_NONE);
	ALfloat dir[6];
	alGetSourcefv(_alSource, AL_DIRECTION, dir);
	forward.x = dir[0];
	forward.y = dir[1];
	forward.z = dir[2];
	up.x = dir[3];
	up.y = dir[4];
	up.z = dir[5];
}

float
ALSoundSource::getPitch(void) const noexcept
{
	assert(_alSource != AL_NONE);
	float pitch = -1.0f;
	alGetSourcef(_alSource, AL_PITCH, &pitch);
	return pitch;
}

float
ALSoundSource::getMaxDistance() const noexcept
{
	assert(_alSource != AL_NONE);
	float distance = false;
	alGetSourcef(_alSource, AL_MAX_DISTANCE, &distance);
	return distance;
}

float
ALSoundSource::getMinDistance() const noexcept
{
	assert(_alSource != AL_NONE);
	float distance = false;
	alGetSourcef(_alSource, AL_REFERENCE_DISTANCE, &distance);
	return distance;
}

void 
ALSoundSource::getSoundClip(SoundClip& clip) const noexcept
{
	clip = _soundClip;
}

void
ALSoundSource::play(bool play) noexcept
{
	assert(_alSource != AL_NONE);
	assert(_alBuffer != AL_NONE);
	assert(_alFormat != AL_NONE);

	if (play)
	{
		if (_isPlayEnd)
			return;

		this->_updateSoundQueue();

		if (!this->isPlaying())
		{
			if (_isPlaying || _alSampleLengthTotal > _soundClip.samples || _soundReader->eof())
			{
				if (!_isLoop)
				{
					this->_playEnd();
					return;
				}
			}

			this->_playStart();
		}
	}
	else
	{
		::alSourceStop(_alSource);
	}
}

void
ALSoundSource::loop(bool loop) noexcept
{
	_isLoop = loop;
}

void
ALSoundSource::pause() noexcept
{
    assert(_alSource != AL_NONE);
    ::alSourcePause(_alSource);
}

bool
ALSoundSource::isPlaying() const noexcept
{
	if (_alSource != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	return false;
}

bool
ALSoundSource::isStopped() const noexcept
{
	if (_alSource != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED || state == AL_INITIAL;
	}

	return true;
}

bool
ALSoundSource::isPaused() const noexcept
{
	if (_alSource != AL_NONE)
	{
		ALint state = AL_NONE;
		alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
		return state == AL_PAUSED;
	}

	return false;
}

bool 
ALSoundSource::isLoop() const noexcept
{
	return _isLoop;
}

void 
ALSoundSource::_initSoundStream() noexcept
{
	_alSampleLengthTotal = _soundClip.length;
	_soundReader->seekg(_soundClip.length, ios_base::beg);
}

void
ALSoundSource::_playStart() noexcept
{
	this->_initSoundStream();
	this->_clearSoundQueue();

	for (auto it : _alBuffer)
	{
		_soundReader->read(_data.data(), _data.size());
		if (_soundReader->gcount() > 0)
		{
			::alBufferData(it, _alFormat, _data.data(), _soundReader->gcount(), _soundClip.freq);
			::alSourceQueueBuffers(_alSource, 1, &it);

			_alSampleLengthTotal += _alSampleLength;
		}
	}

	::alSourcePlay(_alSource);

	_isPlaying = true;
	_isPlayEnd = false;
}

void
ALSoundSource::_playEnd() noexcept
{
	for (auto& it : _listeners)
		it->onPlayEnd();

	ALint processed = 0;
	::alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed--)
	{
		ALuint buff;
		::alSourceUnqueueBuffers(_alSource, 1, &buff);
	}

	_isPlayEnd = true;
	_isPlaying = false;
}

void 
ALSoundSource::_clearSoundQueue() noexcept
{
	ALint processed = 0;
	::alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed--)
	{
		ALuint buff;
		::alSourceUnqueueBuffers(_alSource, 1, &buff);
	}
}

void
ALSoundSource::_updateSoundQueue() noexcept
{
	ALint processed = 0;
	::alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed--)
	{
		ALuint buff;
		::alSourceUnqueueBuffers(_alSource, 1, &buff);

		if (_alSampleLengthTotal > _soundClip.samples || _soundReader->eof())
			break;

		_soundReader->read(_data.data(), _data.size());
		if (_soundReader->gcount() > 0)
		{
			::alBufferData(buff, _alFormat, _data.data(), _soundReader->gcount(), _soundClip.freq);
			::alSourceQueueBuffers(_alSource, 1, &buff);

			_alSampleLengthTotal += _alSampleLength;
		}
	}
}

_NAME_END