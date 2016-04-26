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
#ifndef _H_AL_SOUND_SOURCE_H_
#define _H_AL_SOUND_SOURCE_H_

#include "al_sound_types.h"

_NAME_BEGIN

class EXPORT ALSoundSource final : public SoundSource
{
public:
	ALSoundSource() noexcept;
	virtual ~ALSoundSource() noexcept;

	virtual void open() noexcept;
	virtual void close() noexcept;

	virtual void setSoundReader(SoundReaderPtr ptr) noexcept;
	virtual SoundReaderPtr getSoundBuffer() const noexcept;

	virtual void addSoundSourceListener(SoundSourceListener* listener) noexcept;
	virtual void removeSoundSourceListener(SoundSourceListener* listener) noexcept;

	virtual void setVolume(float volume) noexcept;
	virtual void setMinVolume(float volume) noexcept;
	virtual void setMaxVolume(float volume) noexcept;
	virtual void setTranslate(const float3& translate) noexcept;
	virtual void setVelocity(const float3& velocity) noexcept;
	virtual void setOrientation(const float3& forward, const float3& up) noexcept;
	virtual void setPitch(float pitch) noexcept;
	virtual void setMaxDistance(float maxdis) noexcept;
	virtual void setMinDistance(float mindis) noexcept;
	virtual void setSoundClip(const SoundClip& clip) noexcept;

	virtual void getTranslate(float3& translate) noexcept;
	virtual void getVelocity(float3& velocity) noexcept;
	virtual void getOrientation(float3& forward, float3& up) noexcept;
	virtual void getSoundClip(SoundClip& clip) const noexcept;

	virtual float getVolume() const noexcept;
	virtual float getMinVolume() const noexcept;
	virtual float getMaxVolume() const noexcept;
	virtual float getPitch() const noexcept;
	virtual float getMaxDistance() const noexcept;
	virtual float getMinDistance() const noexcept;

	virtual void play(bool play) noexcept;
	virtual void loop(bool loop) noexcept;
	virtual void pause() noexcept;

	virtual bool isPlaying() const noexcept;
	virtual bool isStopped() const noexcept;
	virtual bool isPaused() const noexcept;
	virtual bool isLoop() const noexcept;

private:

	void _playStart() noexcept;
	void _playEnd() noexcept;

	void _initSoundStream() noexcept;
	void _clearSoundQueue() noexcept;
	void _updateSoundQueue() noexcept;

private:

	bool _isLoop;
	bool _isPlaying;
	bool _isPlayEnd;

	ALuint  _alSource;
	ALuint  _alBuffer[2];
	ALsizei _alBufferSize;
	ALenum  _alFormat;

	ALsizei _alSampleLength;
	ALsizei _alSampleLengthTotal;

	std::vector<char> _data;
	std::vector<SoundSourceListener*> _listeners;

	SoundClip _soundClip;
	SoundReaderPtr _soundReader;
};

_NAME_END

#endif
