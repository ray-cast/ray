#include "ray/al_sound_source.h"
#include "ray/sound_buffer.h"

_NAME_BEGIN

ALSoundSource::ALSoundSource() noexcept
{

}

ALSoundSource::~ALSoundSource() noexcept
{

}

void ALSoundSource::Init()
{
    ::alGenSources(1, &_source);
}

void ALSoundSource::Release()
{
    ::alDeleteSources(1, &_source);
}

void ALSoundSource::Play()
{
    ::alSourcePlay(_source);
}

void ALSoundSource::Stop()
{
    ::alSourceStop(_source);
}

void ALSoundSource::Pause()
{
    ::alSourcePause(_source);
}

void ALSoundSource::Resume()
{
    ::alSourcePlay(_source);
}

void ALSoundSource::SetSoundBuffer(BufferPtr ptr)
{
    if (ptr != nullptr)
    {
        _buffer         = ptr;
        auto bufferData = reinterpret_cast<ALuint>(_buffer->GetBuffer());
        ::alSourceQueueBuffers(_source, 1, &bufferData);
    }
}

BufferPtr ALSoundSource::GetSoundBuffer() const
{
    ALuint bufferData = 0;
    ::alSourceUnqueueBuffers(_source, 1, &bufferData);
    return _buffer;
}

_NAME_END