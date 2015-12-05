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
#include <ray/al_sound_device.h>
#include <ray/al_sound_buffer.h>
#include <ray/al_sound_source.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>
#include <codec.h>

_NAME_BEGIN

ov_callbacks ALSoundDevice::OV_CALLBACKS_IOSERVER = {
    (size_t(*)(void *, size_t, size_t, void *))  ALSoundDevice::_ioserverRead,
    (int(*)(void *, ogg_int64_t, int))           ALSoundDevice::_ioserverSeek,
    (int(*)(void *))                             ALSoundDevice::_ioserverClose,
    (long(*)(void *))                            ALSoundDevice::_ioserverTell
};

ov_callbacks ALSoundDevice::OV_CALLBACKS_MEMORYSTREAM = {
    (size_t(*)(void *, size_t, size_t, void *))  ALSoundDevice::_memoryStreamRead,
    (int(*)(void *, ogg_int64_t, int))           ALSoundDevice::_memoryStreamSeek,
    (int(*)(void *))                             ALSoundDevice::_memoryStreamClose,
    (long(*)(void *))                            ALSoundDevice::_memoryStreamTell,
};

ALSoundDevice::ALSoundDevice() noexcept
    : _device(nullptr)
	, _context(nullptr)
{
}

ALSoundDevice::~ALSoundDevice() noexcept
{
	this->close();
}

bool 
ALSoundDevice::open()
{
    _device = ::alcOpenDevice(nullptr);
    if (!_device)
        return false;

	_context = ::alcCreateContext(_device, nullptr);
	if (::alcMakeContextCurrent(_context))
		return true;

    return true;
}

void 
ALSoundDevice::close() noexcept
{
	if (_context)
	{
		::alcDestroyContext(_context);
		_context = nullptr;
	}
		
	if (_device)
	{
		::alcCloseDevice(_device);
		_device = nullptr;
	}
}

bool
ALSoundDevice::isOpen() const noexcept
{
    return _device ? true : false;
}

SoundBufferPtr
ALSoundDevice::createSoundBuffer()
{
	return std::make_shared<ALSoundBuffer>();
}

SoundSourcePtr
ALSoundDevice::createSoundSource()
{
	return std::make_shared<ALSoundSource>();
}

SoundBufferPtr 
ALSoundDevice::load(const std::string& filename)
{
    MemoryReader reader;
    IoServer::instance()->openFile(filename, reader, ios_base::in | ios_base::binary);
    return this->load(reader);
}

SoundBufferPtr 
ALSoundDevice::load(MemoryReader& reader)
{
    OggVorbis_File oggfile;
    auto err = ::ov_open_callbacks(&reader, &oggfile, nullptr, 0, OV_CALLBACKS_MEMORYSTREAM);
    if (err < 0)
    {
        return nullptr;
    }

    return LoadFile(&oggfile);
}

SoundBufferPtr 
ALSoundDevice::LoadFile(OggVorbis_File* file)
{
    auto info = ::ov_info(file, -1);
    if (info == nullptr)
    {
        return nullptr;
    }

    auto format = (info->channels == 1) ? SoundFormat::Mono16 : SoundFormat::Stereo16;
    auto freq   = info->rate;

    long bytes      = 0;
    int endian      = 0;
    int bitstream   = 0;

    const int file_buffer_size = 4096;
    char file_buff[file_buffer_size];

    std::vector<char> fileData;
    do
    {
        bytes = ::ov_read(file, file_buff, file_buffer_size, endian, 2, 1, &bitstream);
        fileData.insert(fileData.end(), file_buff, file_buff + bytes);

    } while (bytes != 0);

    ::ov_clear(file);

    auto result = std::make_shared<ALSoundBuffer>();
    result->open();
    result->setBufferType(format);
    result->setBufferFrequency(freq);
    result->writeBufferData(fileData.data(), fileData.size());

    return result;
}

std::size_t
ALSoundDevice::_ioserverRead(void* ptr, size_t elementSize, size_t count, void* data)
{
    if (data != nullptr)
    {
        auto input = static_cast<MemoryReader*>(data);
        if (input->is_open())
        {
            input->read((char*)(ptr), elementSize * count);
            return count;
        }
    }
    return 0;
}

int 
ALSoundDevice::_ioserverSeek(void* data, ogg_int64_t pos, int whence)
{
    if(data != nullptr)
    {
        auto input = static_cast<ray::istream*>(data);
        {
			switch (whence)
			{
			case SEEK_SET:
				input->seekg(pos, ios_base::beg);
				break;
			case SEEK_CUR:
				input->seekg(pos, ios_base::cur);
				break;
			case SEEK_END:
				input->seekg(pos, ios_base::end);
				break;
			}            
            return 0;
        }
    }
    return -1;
}

int 
ALSoundDevice::_ioserverClose(void *data)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::istream*>(data);
        {
            if (input->is_open())
            {
                input->close();
                if (!input->is_open())
                {
                    return 0;
                }
            }
        }
    }
    return -1;
}

long 
ALSoundDevice::_ioserverTell(void *data)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::istream*>(data);
        {
            return input->tellg();
        }
    }
    return 0;
}

std::size_t 
ALSoundDevice::_memoryStreamRead(void* ptr, size_t elementSize, size_t count, void* data)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::MemoryReader*>(data);
        if (input->is_open())
        {
            std::memcpy(ptr, input->map(), elementSize * count);
            return count;
        }
    }
    return 0;
}

int
ALSoundDevice::_memoryStreamSeek(void* data, ogg_int64_t pos, int whence)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::MemoryReader*>(data);
        {
            input->seekg(pos, whence);
            return 0;
        }
    }
    return -1;
}

int 
ALSoundDevice::_memoryStreamClose(void *data)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::MemoryReader*>(data);
        {
            if (input->is_open())
            {
                input->close();
                if (!input->is_open())
                {
                    return 0;
                }
            }
        }
    }
    return -1;
}

long
ALSoundDevice::_memoryStreamTell(void *data)
{
    if (data != nullptr)
    {
        auto input = static_cast<ray::MemoryReader*>(data);
        {
            return input->tellg();
        }
    }
    return 0;
}

_NAME_END