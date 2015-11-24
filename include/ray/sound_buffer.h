#ifndef _H_SOUND_BUFFER_H_
#define _H_SOUND_BUFFER_H_

#include "ray/platform.h"
#include "ray/sound_types.h"

_NAME_BEGIN

class EXPORT SoundBuffer
{
public:
    SoundBuffer() noexcept;
    virtual ~SoundBuffer() noexcept;

    virtual bool Init()                     = 0;
    virtual void Destory()                  = 0;
    virtual bool IsInvalid() const noexcept = 0;

    virtual void SetBuffer(void* buff) = 0;
    virtual void* GetBuffer() const = 0;

    virtual void ReadtBufferData(void* dest, size_t size) const = 0;
    virtual void WriteBufferData(const void* src, size_t size)  = 0;


    virtual void SetBufferType(const SOUND_FORMAT_ENUM type) noexcept           = 0;
    virtual void SetBufferFrequency(const SOUND_FREQUENCY_TYPE freq) noexcept   = 0;
};

_NAME_END

#endif 

