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
#include "al_sound_listener.h"

_NAME_BEGIN

ALSoundListener::ALSoundListener() noexcept
{
}

ALSoundListener::~ALSoundListener() noexcept
{
	this->close();
}

bool 
ALSoundListener::open() noexcept
{
	return true;
}

void 
ALSoundListener::close() noexcept
{
}

void
ALSoundListener::setVolume(float volume) noexcept
{
	alListenerf(AL_GAIN, volume);
}

float
ALSoundListener::getVolume() const noexcept
{
	float volume = -1.0f;
	alGetListenerf(AL_GAIN, &volume);
	return volume;
}

void 
ALSoundListener::setTranslate(const float3& translate) noexcept
{
	ALfloat pos[] = { translate.x, translate.y, translate.z };
	alListenerfv(AL_POSITION, pos);
}

void 
ALSoundListener::setVelocity(const float3& velocity) noexcept
{
	ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
	alListenerfv(AL_VELOCITY, vel);
}

void 
ALSoundListener::setOrientation(const float3& forward, const float3& up) noexcept
{
	ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
	alListenerfv(AL_ORIENTATION, dir);
}

void 
ALSoundListener::getTranslate(float3& translate) noexcept
{
	ALfloat pos[3];
	alGetListenerfv(AL_POSITION, pos);
	translate.x = pos[0];
	translate.y = pos[1];
	translate.z = pos[2];
}

void 
ALSoundListener::getVelocity(float3& velocity) noexcept
{
	ALfloat pos[3];
	alGetListenerfv(AL_VELOCITY, pos);
	velocity.x = pos[0];
	velocity.y = pos[1];
	velocity.z = pos[2];
}

void 
ALSoundListener::getOrientation(float3& forward, float3& up) noexcept
{
	ALfloat dir[6];
	alGetListenerfv(AL_ORIENTATION, dir);
	forward.x = dir[0];
	forward.y = dir[1];
	forward.z = dir[2];
	up.x = dir[3];
	up.y = dir[4];
	up.z = dir[5];
}

_NAME_END