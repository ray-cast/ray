#include "ray/al_sound_buffer.h"

_NAME_BEGIN

ALSoundBuffer::ALSoundBuffer() noexcept
    :_isInvalid(true), _buffer(0), _soundFreqency(0), _soundType(SOUND_FORMAT_ENUM::Invalid)
{

}

ALSoundBuffer::~ALSoundBuffer() noexcept
{
    if (!IsInvalid())
    {
        Destory();
    }
}

bool ALSoundBuffer::Init()
{
    ::alGenBuffers(1, &_buffer);
    _isInvalid = true;
    return true;
}

void ALSoundBuffer::Destory()
{
    ::alDeleteBuffers(1, &_buffer);
    _isInvalid = false;
}

bool ALSoundBuffer::IsInvalid() const noexcept
{
    return _isInvalid;
}

void ALSoundBuffer::SetBufferType(const SOUND_FORMAT_ENUM type) noexcept
{
    _soundType = type;
}

void ALSoundBuffer::SetBufferFrequency(const SOUND_FREQUENCY_TYPE freq) noexcept
{
    _soundFreqency = freq;
}

void* ALSoundBuffer::GetBuffer() const
{
    if (!IsInvalid())
    {
        return reinterpret_cast<void*>(_buffer);
    }
    return nullptr;
}

void ALSoundBuffer::SetBuffer(void* buffer)
{
    if (!IsInvalid())
    {
        _buffer = reinterpret_cast<ALuint>(buffer);
    }
}

void ALSoundBuffer::ReadtBufferData(void* dest, size_t size) const
{
    if (!IsInvalid() && dest != nullptr && size != 0 && _data.size() > size)
    {
        std::memcpy(dest, _data.data(), size);
    }
}

void ALSoundBuffer::WriteBufferData(const void* src, size_t size)
{
    if (!IsInvalid() && src != nullptr && size != 0)
    {
        if (_data.size() != size)
        {
            _data.resize(size);
        }

        std::memcpy(&_data[0], src, size);

        alBufferData(_buffer, 
            static_cast<ALenum>(_soundType),
            _data.data(), 
            static_cast<ALsizei>(_data.size()), 
            _soundFreqency);
    }
}

_NAME_END