#ifndef _H_AL_SOUND_SOURCE_H_
#define _H_AL_SOUND_SOURCE_H_

#include "ray/sound_source.h"
#include "al.h"
#include "alc.h"

_NAME_BEGIN

class EXPORT ALSoundSource : public SoundSource
{
public:
    ALSoundSource() noexcept;
    virtual ~ALSoundSource() noexcept;

    virtual void Init();
    virtual void Release();

    virtual void Play();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();

    virtual void SetSoundBuffer(BufferPtr ptr);;
    virtual BufferPtr GetSoundBuffer() const;

private:
    ALuint      _source;
    BufferPtr   _buffer;
};

_NAME_END

#endif
