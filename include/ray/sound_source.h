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
#ifndef _H_SOUND_SOURCE_H_
#define _H_SOUND_SOURCE_H_

#include <ray/sound_listener.h>

_NAME_BEGIN

class EXPORT SoundSource
{
public:
    SoundSource() noexcept;
    virtual ~SoundSource() noexcept;

    virtual void open() = 0;
    virtual void close() noexcept = 0;

	virtual void setSoundBuffer(SoundBufferPtr ptr) = 0;
	virtual SoundBufferPtr getSoundBuffer() const noexcept = 0;

	virtual void setVolume(float volume) noexcept = 0;
	virtual float getVolume() const noexcept = 0;

	virtual void setPitch(float pitch) noexcept = 0;
	virtual float getPitch(void) const noexcept = 0;

	virtual void setLoop(bool loop) noexcept = 0;
	virtual bool getLoop(void) const noexcept = 0;

	virtual void setMaxDistance(float maxdis) noexcept = 0;
	virtual float getMaxDistance() const noexcept = 0;

	virtual void setMinDistance(float mindis) noexcept = 0;
	virtual float getMinDistance() const noexcept = 0;

	virtual void setTransform3D(const float3& position, const float3& velocity, const float3& forward, const float3& up) noexcept = 0;
	virtual void getTransform3D(float3& position, float3& velocity, float3& forward, float3& up) noexcept = 0;

    virtual void play() noexcept = 0;
	virtual bool isPlaying() const noexcept = 0;

    virtual void stop() noexcept = 0;
	virtual bool isStopped() const noexcept = 0;

    virtual void pause() noexcept = 0;
	virtual bool isPaused() const noexcept = 0;
};

_NAME_END

#endif
