// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#if defined(_BUILD_SOUND)
#include <ray/sound_component.h>
#include <ray/sound_system.h>

_NAME_BEGIN

__ImplementSubClass(SoundComponent, GameComponent, "Sound")

SoundComponent::SoundComponent() noexcept
	: _volume(1.0)
	, _volumeMin(0.0)
	, _volumeMax(1.0)
	, _pitch(1.0)
	, _distanceMin(1.0)
	, _distanceMax(5.0)
	, _isLoop(false)
	, _isPlayOnActivate(false)
	, _isPause(false)
{
}

SoundComponent::~SoundComponent() noexcept
{
	this->stop();
}

void
SoundComponent::setSoundBuffer(const util::string& name) noexcept
{
	if (_sourceName != name)
	{
		if (_sound)
		{
			auto soundReader = SoundSystem::instance()->createSoundReader(name);
			if (soundReader)
				_sound->setSoundReader(soundReader);
			else
				_sound->setSoundReader(nullptr);
		}

		_sourceName = name;
	}
}

const util::string&
SoundComponent::getSoundBuffer() const noexcept
{
	return _sourceName;
}

void
SoundComponent::setVolume(float volume) noexcept
{
	if (_volume != volume)
	{
		if (_sound)
			_sound->setVolume(volume);
		_volume = volume;
	}
}

void
SoundComponent::setMinVolume(float volume) noexcept
{
	if (_volumeMin != volume)
	{
		if (_sound)
			_sound->setMinVolume(volume);
		_volumeMin = volume;
	}
}

void
SoundComponent::setMaxVolume(float volume) noexcept
{
	if (_volumeMax != volume)
	{
		if (_sound)
			_sound->setMaxVolume(volume);
		_volumeMax = volume;
	}
}

void
SoundComponent::setPitch(float pitch) noexcept
{
	if (_pitch != pitch)
	{
		if (_sound)
			_sound->setPitch(pitch);
		_pitch = pitch;
	}
}

void
SoundComponent::setMaxDistance(float maxdis) noexcept
{
	if (_distanceMax != maxdis)
	{
		if (_sound)
			_sound->setMaxDistance(maxdis);
		_distanceMax = maxdis;
	}
}

void
SoundComponent::setMinDistance(float mindis) noexcept
{
	if (_distanceMin != mindis)
	{
		if (_sound)
			_sound->setMaxDistance(mindis);
		_distanceMin = mindis;
	}
}

float
SoundComponent::getVolume() const noexcept
{
	return _volume;
}

float
SoundComponent::getMinVolume() const noexcept
{
	return _volumeMin;
}

float
SoundComponent::getMaxVolume() const noexcept
{
	return _volumeMax;
}

float
SoundComponent::getPitch() const noexcept
{
	return _pitch;
}

float
SoundComponent::getMaxDistance() const noexcept
{
	return _distanceMax;
}

float
SoundComponent::getMinDistance() const noexcept
{
	return _distanceMin;
}

void
SoundComponent::setSoundClip(const SoundClip& clip) noexcept
{
	if (_sound)
		_sound->setSoundClip(clip);
	_clip = clip;
}

void
SoundComponent::getSoundClip(SoundClip& clip) const noexcept
{
	clip = _clip;
}

void
SoundComponent::play() noexcept
{
	if (this->getActive())
	{
		if (!_sound)
		{
			auto soundReader = SoundSystem::instance()->createSoundReader(_sourceName);
			if (soundReader)
			{
				auto actor = this->getGameObject();
				assert(actor);

				_sound = SoundSystem::instance()->createSoundSource();
				_sound->open();

				_sound->setSoundReader(soundReader);

				_sound->setVolume(_volume);
				_sound->setMinVolume(_volumeMin);
				_sound->setMaxVolume(_volumeMax);

				_sound->setPitch(_pitch);

				_sound->setMinDistance(_distanceMin);
				_sound->setMinDistance(_distanceMax);

				_sound->setTranslate(actor->getTranslate());
				_sound->setOrientation(actor->getForward(), actor->getUpVector());

				_sound->loop(_isLoop);

				_sound->addSoundSourceListener(this);
			}
		}
	}

	_isPlayOnActivate = true;
}

void
SoundComponent::stop() noexcept
{
	if (_sound)
	{
		_sound->close();
		_sound = nullptr;
	}

	_isPlayOnActivate = false;
}

void
SoundComponent::pause() noexcept
{
	if (_sound)
		_sound->pause();
	_isPause = true;
}

void
SoundComponent::loop(bool loop) noexcept
{
	if (this->getActive())
		return _sound->loop(loop);
	_isLoop = loop;
}

bool
SoundComponent::isPlaying() const noexcept
{
	if (this->getActive())
		return _sound->isPlaying();
	return false;
}

bool
SoundComponent::isStopped() const noexcept
{
	if (this->getActive())
		return _sound->isStopped();
	return true;
}

bool
SoundComponent::isPaused() const noexcept
{
	if (this->getActive())
		return _sound->isPaused();
	return false;
}

bool
SoundComponent::isLooping() const noexcept
{
	if (this->getActive())
		return _sound->isLoop();
	return _isLoop;
}

void
SoundComponent::addPlayEndListener(std::function<void()>* func) noexcept
{
	assert(!_onPlayEnd.find(func));
	_onPlayEnd.attach(func);
}

void
SoundComponent::removePlayEndListener(std::function<void()>* func) noexcept
{
	assert(_onPlayEnd.find(func));
	_onPlayEnd.remove(func);
}

void
SoundComponent::load(iarchive& reader) noexcept
{
	reader >> make_archive(_volume, "volume");
	reader >> make_archive(_volumeMin, "volumeMin");
	reader >> make_archive(_volumeMax, "volumeMax");

	reader >> make_archive(_pitch, "pitch");

	reader >> make_archive(_distanceMin, "_distanceMin");
	reader >> make_archive(_distanceMax, "_distanceMax");

	reader >> make_archive(_isPlayOnActivate, "play");
	reader >> make_archive(_isLoop, "loop");

	reader >> make_archive(_sourceName, "source");
}

void
SoundComponent::save(oarchive& write) noexcept
{
	write << make_archive(_volume, "volume");
	write << make_archive(_volumeMin, "volumeMin");
	write << make_archive(_volumeMax, "volumeMax");

	write << make_archive(_pitch, "pitch");

	write << make_archive(_distanceMin, "_distanceMin");
	write << make_archive(_distanceMax, "_distanceMax");

	write << make_archive(_isPlayOnActivate, "play");
	write << make_archive(_isLoop, "loop");

	write << make_archive(_sourceName, "source");
}

GameComponentPtr
SoundComponent::clone() const noexcept
{
	auto component = std::make_shared<SoundComponent>();
	component->_volume = this->_volume;
	component->_volumeMin = this->_volumeMin;
	component->_volumeMax = this->_volumeMax;
	component->_distanceMin = this->_distanceMin;
	component->_distanceMax = this->_distanceMax;
	component->_pitch = this->_pitch;
	component->_sourceName = this->_sourceName;
	return component;
}

void
SoundComponent::onMoveAfter() noexcept
{
	auto actor = this->getGameObject();
	assert(actor);

	_sound->setTranslate(actor->getTranslate());
	_sound->setOrientation(actor->getForward(), actor->getUpVector());
}

void
SoundComponent::onPlayEnd() noexcept
{
	_onPlayEnd.run();
}

void
SoundComponent::onActivate() except
{
	this->play();
}

void
SoundComponent::onDeactivate() noexcept
{
	this->stop();
}

void
SoundComponent::onFrameBegin() noexcept
{
	_sound->play(_isPlayOnActivate);
}

_NAME_END

#endif