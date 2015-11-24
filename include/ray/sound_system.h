#ifndef _H_SOUND_SYSTEM_H_
#define _H_SOUND_SYSTEM_H_

#include "ray/platform.h"
#include "ray/sound_types.h"

_NAME_BEGIN

class MemoryReader;

class EXPORT SoundSystem
{
public:
    SoundSystem() noexcept;
    virtual ~SoundSystem() noexcept;

    virtual bool Init()     = 0;
    virtual void Destory()  = 0;
    virtual bool IsInvalid() const noexcept = 0;

    virtual BufferPtr Open(const std::string& filename) = 0;
    virtual BufferPtr Open(MemoryReader& reader)        = 0;
};

_NAME_END

#endif
