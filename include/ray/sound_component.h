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
#ifndef _H_SOUND_COMPONENT_H_
#define _H_SOUND_COMPONENT_H_

#include <ray/game_component.h>
#include <ray/sound_source.h>

_NAME_BEGIN

class SoundComponent final : public GameComponent, public SoundSourceListener
{
	__DeclareSubClass(SoundComponent, GameComponent)
public:
	SoundComponent() noexcept;
	~SoundComponent() noexcept;

    void setSoundBuffer(const std::string& name) noexcept;
	const std::string& getSoundBuffer() const noexcept;

    void setVolume(float volume) noexcept;
    void setMinVolume(float volume) noexcept;
    void setMaxVolume(float volume) noexcept;
    void setPitch(float pitch) noexcept;
    void setMaxDistance(float maxdis) noexcept;
    void setMinDistance(float mindis) noexcept;

    float getVolume() const noexcept;
    float getMinVolume() const noexcept;
    float getMaxVolume() const noexcept;
    float getPitch() const noexcept;
    float getMaxDistance() const noexcept;
    float getMinDistance() const noexcept;

	void setSoundClip(const SoundClip& clip) noexcept;
	void getSoundClip(SoundClip& clip) const noexcept;

    void play() noexcept;
    void stop() noexcept;
    void pause() noexcept;
    void loop(bool loop) noexcept;

    bool isPlaying() const noexcept;
    bool isStopped() const noexcept;
    bool isPaused() const noexcept;
    bool isLooping() const noexcept;

	void addPlayEndListener(std::function<void()> func) noexcept;
	void removePlayEndListener(std::function<void()> func) noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:

	virtual void onFrameBegin() noexcept;

	virtual void onActivate() except;
	virtual void onDeactivate() except;

	virtual void onMoveAfter() noexcept;

	virtual void onPlayEnd() noexcept;

private:

	std::string _sourceName;

	SoundClip _clip;
	SoundSourcePtr _sound;

	float _volume;
	float _volumeMin;
	float _volumeMax;

	float _distanceMin;
	float _distanceMax;

	float _pitch;

	bool _isPlayOnActivate;
	bool _isPause;
	bool _isLoop;

	delegate<void()> _onPlayEnd;
};

_NAME_END

#endif