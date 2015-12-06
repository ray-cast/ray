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
#include <ray/sound_handler_all.h>

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

void 
SoundSystem::open() noexcept
{
	assert(!this->isOpened());

	_soundDevice = std::make_shared<ALSoundDevice>();
}

void 
SoundSystem::close() noexcept
{
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

SoundSourcePtr
SoundSystem::createSoundSource(const std::string& filename, SoundFile::Type type) except
{
	assert(this->isOpened());

	auto soundBuffer = this->load(filename, type);
	if (soundBuffer)
	{
		auto sound = _soundDevice->createSoundSource();
		if (sound)
		{
			sound->open();
			sound->setSoundBuffer(soundBuffer);
			return sound;
		}
	}

	return nullptr;
}

SoundBufferPtr
SoundSystem::load(const std::string& filename, SoundFile::Type type) noexcept
{
	assert(this->isOpened());

	auto soundBuffer = _soundBuffers[filename];
	if (!soundBuffer)
	{
		MemoryStream stream;
		if (IoServer::instance()->openFile(filename, stream))
			soundBuffer = load(stream, type);

		if (soundBuffer)
			_soundBuffers[filename] = soundBuffer;
	}

	return soundBuffer;
}

SoundBufferPtr
SoundSystem::load(istream& stream, SoundFile::Type type) noexcept
{
	assert(this->isOpened());

	if (this->emptyHandler())
		GetSoundInstanceList(*this);

	SoundHandlerPtr impl;

	if (this->find(stream, type, impl))
	{
		auto buffer = _soundDevice->createSoundBuffer();
		if (buffer)
		{
			if (impl->doLoad(*buffer, stream))
			{
				return buffer;
			}
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
SoundSystem::add(SoundHandlerPtr handler) noexcept
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
SoundSystem::remove(SoundHandlerPtr handler) noexcept
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
SoundSystem::find(istream& stream, SoundHandlerPtr& out) const noexcept
{
	if (!stream.is_open())
		return false;

	for (auto it : _handlers)
	{
		stream.seekg(0, std::ios_base::beg);

		if (it->doCanRead(stream))
		{
			stream.seekg(0, std::ios_base::beg);

			out = it;
			return true;
		}
	}

	return false;
}

bool
SoundSystem::find(SoundFile::Type type, SoundHandlerPtr& out) const noexcept
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
SoundSystem::find(istream& stream, SoundFile::Type type, SoundHandlerPtr& out) const noexcept
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