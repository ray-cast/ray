#ifndef _H_SOUND_SOURCE_H_
#define _H_SOUND_SOURCE_H_

#include "ray/platform.h"
#include "ray/sound_types.h"

_NAME_BEGIN

class EXPORT SoundSource
{
public:
    SoundSource() noexcept;
    virtual ~SoundSource() noexcept;

    virtual void Init()     = 0;
    virtual void Release()  = 0;

    virtual void Play()     = 0;
    virtual void Stop()     = 0;
    virtual void Pause()    = 0;
    virtual void Resume()   = 0;

    virtual void SetSoundBuffer(BufferPtr ptr)  = 0;
    virtual BufferPtr GetSoundBuffer() const    = 0;
};

_NAME_END

#endif
