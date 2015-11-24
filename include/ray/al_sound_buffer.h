#ifndef _H_AL_SOUND_BUFFER_H_
#define _H_AL_SOUND_BUFFER_H_

#include "ray/sound_buffer.h"
#include "al.h"
#include "alc.h"

_NAME_BEGIN

class EXPORT ALSoundBuffer : public SoundBuffer
{
public:
    ALSoundBuffer() noexcept;
    virtual ~ALSoundBuffer() noexcept;

    virtual bool Init();

    virtual void Destory();
    virtual bool IsInvalid() const noexcept;

    virtual void SetBuffer(void* buff);
    virtual void* GetBuffer() const;

    virtual void ReadtBufferData(void* dest, size_t size) const;
    virtual void WriteBufferData(const void* src, size_t size);

    virtual void SetBufferType(const SOUND_FORMAT_ENUM type) noexcept;
    virtual void SetBufferFrequency(const SOUND_FREQUENCY_TYPE freq) noexcept;

private:
    std::vector<char>           _data;
    SOUND_FORMAT_ENUM           _soundType;
    SOUND_FREQUENCY_TYPE        _soundFreqency;
    ALuint                      _buffer;
    bool                        _isInvalid;
};

_NAME_END

#endif 
