#include "ray/al_sound_system.h"
#include "ray/al_sound_buffer.h"
#include "codec.h"
#include "ray/mstream.h"
#include "ray/ioserver.h"

_NAME_BEGIN

ov_callbacks ALSoundSystem::OV_CALLBACKS_IOSERVER = {
    (size_t(*)(void *, size_t, size_t, void *))  ALSoundSystem::_ioserverRead,
    (int(*)(void *, ogg_int64_t, int))           ALSoundSystem::_ioserverSeek,
    (int(*)(void *))                             ALSoundSystem::_ioserverClose,
    (long(*)(void *))                            ALSoundSystem::_ioserverTell
};

ov_callbacks ALSoundSystem::OV_CALLBACKS_MEMORYSTREAM = {
    (size_t(*)(void *, size_t, size_t, void *))  ALSoundSystem::_memoryStreamRead,
    (int(*)(void *, ogg_int64_t, int))           ALSoundSystem::_memoryStreamSeek,
    (int(*)(void *))                             ALSoundSystem::_memoryStreamClose,
    (long(*)(void *))                            ALSoundSystem::_memoryStreamTell,
};

ALSoundSystem::ALSoundSystem() noexcept
    :_device(nullptr), _context(nullptr)
{

}

ALSoundSystem::~ALSoundSystem() noexcept
{
    if (!IsInvalid())
    {
        Destory();
    }
}

bool ALSoundSystem::Init()
{
    _device = ::alcOpenDevice(nullptr);
    if (!_device)
    {
        return false;
    }

    InitContext();
    return true;
}

void ALSoundSystem::InitContext()
{
    _context = ::alcCreateContext(_device, nullptr);
    ::alcMakeContextCurrent(_context);

    // 以下是我瞎设的
    ::alListener3f(AL_POSITION, 0, 0, 0);
    ::alListener3f(AL_VELOCITY, 0, 0, 0);
    ::alListener3f(AL_ORIENTATION, 0, 0, -1);
}

void ALSoundSystem::Destory()
{
    ::alcDestroyContext(_context);
    ::alcCloseDevice(_device);
}

bool ALSoundSystem::IsInvalid() const noexcept
{
    return _device == nullptr;
}

BufferPtr ALSoundSystem::Open(const std::string& filename)
{
    MemoryReader* reader = new MemoryReader;
    IoServer::instance()->openFile(filename, *reader, ios_base::binary | ios_base::in);

    OggVorbis_File* oggfile = new OggVorbis_File;
    auto err = ::ov_open_callbacks(reader, oggfile, nullptr, 0, OV_CALLBACKS_IOSERVER);
    if (err < 0)
    {
        return nullptr;
    }

    return LoadFile(oggfile);
}

BufferPtr ALSoundSystem::Open(MemoryReader& reader)
{
    OggVorbis_File oggfile;
    auto err = ::ov_open_callbacks(&reader, &oggfile, nullptr, 0, OV_CALLBACKS_MEMORYSTREAM);
    if (err < 0)
    {
        return nullptr;
    }
    return LoadFile(&oggfile);
}

BufferPtr ALSoundSystem::LoadFile(OggVorbis_File* file)
{
    auto info = ::ov_info(file, -1);
    if (info == nullptr)
    {
        return nullptr;
    }

    auto format = (info->channels == 1) ? SOUND_FORMAT_ENUM::MONO16 : SOUND_FORMAT_ENUM::STEREO16;
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
    result->Init();
    result->SetBufferType(format);
    result->SetBufferFrequency(freq);
    result->WriteBufferData(fileData.data(), fileData.size());

    return result;
}

size_t ALSoundSystem::_ioserverRead(void* ptr, size_t elementSize, size_t count, void* data)
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

int ALSoundSystem::_ioserverSeek(void* data, ogg_int64_t pos, int whence)
{
    if(data != nullptr)
    {
        auto input = reinterpret_cast<ray::istream*>(data);
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

int ALSoundSystem::_ioserverClose(void *data)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::istream*>(data);
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

long ALSoundSystem::_ioserverTell(void *data)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::istream*>(data);
        {
            return input->tellg();
        }
    }
    return 0;
}

size_t ALSoundSystem::_memoryStreamRead(void* ptr, size_t elementSize, size_t count, void* data)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::MemoryReader*>(data);
        if (input->is_open())
        {
            std::memcpy(ptr, input->map(), elementSize * count);
            return count;
        }
    }
    return 0;
}

int ALSoundSystem::_memoryStreamSeek(void* data, ogg_int64_t pos, int whence)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::MemoryReader*>(data);
        {
            input->seekg(pos, whence);
            return 0;
        }
    }
    return -1;
}

int ALSoundSystem::_memoryStreamClose(void *data)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::MemoryReader*>(data);
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

long ALSoundSystem::_memoryStreamTell(void *data)
{
    if (data != nullptr)
    {
        auto input = reinterpret_cast<ray::MemoryReader*>(data);
        {
            return input->tellg();
        }
    }
    return 0;
}

_NAME_END