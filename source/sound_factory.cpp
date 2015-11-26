#include "ray/sound_factory.h"
#include "ray/al_sound_buffer.h"
#include "ray/al_sound_source.h"
#include "ray/al_sound_system.h"

_NAME_BEGIN

BufferPtr SoundFactory::CreateBuffer(const std::string& name)
{
    if (name == "AlSoundBuffer")
    {
        return std::make_shared<ALSoundBuffer>();
    }
    else
    {
        return nullptr;
    }
}

SourcePtr SoundFactory::CreateSource(const std::string& name)
{
    if (name == "AlSoundSource")
    {
        return std::make_shared<ALSoundSource>();
    }
    else
    {
        return nullptr;
    }
}

SystemPtr SoundFactory::CreateSystem(const std::string& name)
{
    if (name == "ALSoundSystem")
    {
        return std::make_shared<ALSoundSystem>();
    }
    else
    {
        return nullptr;
    }
}

_NAME_END