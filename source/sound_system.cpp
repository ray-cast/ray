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
#include <ray/sound_system.h>
#include <ray/sound_source.h>
#include <ray/sound_buffer_all.h>

#include <ray/al_sound_device.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>

_NAME_BEGIN

__ImplementSingleton(SoundSystem)

SoundSystem::SoundSystem() noexcept
{
}

SoundSystem::~SoundSystem() noexcept
{
	this->close();
}

bool
SoundSystem::open() noexcept
{
	assert(!this->isOpened());

	_soundDevice = std::make_shared<ALSoundDevice>();
	return _soundDevice->open();
}

void 
SoundSystem::close() noexcept
{
	if (!_soundReaders.empty())
	{
		for (auto& it : _soundReaders)
		{
			it.second.reset();
		}

		_soundReaders.clear();
	}

	if (_soundDevice)
	{
		_soundDevice.reset();
		_soundDevice = nullptr;
	}
}

bool
SoundSystem::isOpened() noexcept
{
	return (_soundDevice != nullptr) ? true : false;
}

void 
SoundSystem::setDistanceModel(bool enable) noexcept
{
	assert(_soundDevice);
	_soundDevice->setDistanceModel(enable);
}

bool 
SoundSystem::getDistanceModel() const noexcept
{
	assert(_soundDevice);
	return _soundDevice->getDistanceModel();
}

SoundListenerPtr
SoundSystem::createSoundListener() noexcept
{
	assert(_soundDevice);
	return _soundDevice->createSoundListener();
}

SoundSourcePtr
SoundSystem::createSoundSource() except
{
	assert(_soundDevice);
	return _soundDevice->createSoundSource();
}

SoundSourcePtr
SoundSystem::createSoundSource(const std::string& filename, SoundFile::Type type) except
{
	assert(this->isOpened());

	auto soundBuffer = this->createSoundReader(filename, type);
	if (soundBuffer)
	{
		auto sound = this->createSoundSource();
		if (sound)
		{
			sound->open();
			sound->setSoundReader(soundBuffer);
			return sound;
		}
	}

	return nullptr;
}

SoundReaderPtr
SoundSystem::createSoundReader(const std::string& filename, SoundFile::Type type) noexcept
{
	assert(this->isOpened());

	auto soundReader = _soundReaders[filename];
	if (!soundReader)
	{
		StreamReaderPtr stream;
		if (IoServer::instance()->openFile(stream, filename))
			soundReader = createSoundReader(*stream, type);

		if (soundReader)
			_soundReaders[filename] = soundReader;
	}

	return soundReader;
}

SoundReaderPtr
SoundSystem::createSoundReader(StreamReader& stream, SoundFile::Type type) noexcept
{
	assert(this->isOpened());

	if (this->emptyHandler())
		GetSoundInstanceList(*this);

	SoundReaderPtr impl;
	
	if (this->find(stream, type, impl))
	{
		auto reader = std::shared_ptr<SoundReader>(static_cast<SoundReader*>(impl->clone()));
		if (reader)
		{
			reader->open(stream);
			return reader;
		}
	}

	return nullptr;
}

bool
SoundSystem::emptyHandler() const noexcept
{
	return _handlers.empty();
}

bool
SoundSystem::add(SoundReaderPtr handler) noexcept
{
	assert(handler);

	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it == _handlers.end())
	{
		_handlers.push_back(handler);
		return true;
	}

	return false;
}

bool
SoundSystem::remove(SoundReaderPtr handler) noexcept
{
	assert(handler);

	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it != _handlers.end())
	{
		_handlers.erase(it);
		return true;
	}

	return false;
}

bool
SoundSystem::find(StreamReader& stream, SoundReaderPtr& out) const noexcept
{
	if (!stream.good())
		return false;

	for (auto it : _handlers)
	{
		stream.seekg(0, std::ios_base::beg);

		if (it->access(stream))
		{
			stream.seekg(0, std::ios_base::beg);

			out = it;
			return true;
		}
	}

	return false;
}

bool
SoundSystem::find(SoundFile::Type type, SoundReaderPtr& out) const noexcept
{
	std::size_t index = (std::size_t)type;
	if (_handlers.size() < index)
	{
		out = _handlers[index];
		return true;
	}

	return false;
}

bool
SoundSystem::find(StreamReader& stream, SoundFile::Type type, SoundReaderPtr& out) const noexcept
{
	if (type != SoundFile::Type::Unknown)
	{
		if (this->find(type, out))
		{
			return true;
		}
	}

	return this->find(stream, out);
}

_NAME_END