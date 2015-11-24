#ifndef _H_AL_SOUND_SYSTEM_H_
#define _H_AL_SOUND_SYSTEM_H_

#include "ray/sound_system.h"
#include "al.h"
#include "alc.h"
#include "vorbisfile.h"

_NAME_BEGIN

class EXPORT ALSoundSystem : public SoundSystem
{
public:
    ALSoundSystem() noexcept;
    virtual ~ALSoundSystem() noexcept;

    virtual bool Init();
    virtual void Destory();
    virtual bool IsInvalid() const noexcept;

    virtual BufferPtr Open(const std::string& filename);
    virtual BufferPtr Open(MemoryReader& reader);

private:
    void InitContext();
    BufferPtr LoadFile(OggVorbis_File* file);

public:
    static ov_callbacks OV_CALLBACKS_IOSERVER;
    static ov_callbacks OV_CALLBACKS_MEMORYSTREAM;

    static size_t _ioserverRead(void*, size_t, size_t, void*);
    static int _ioserverSeek(void*, ogg_int64_t, int);
    static int _ioserverClose(void *data);
    static long _ioserverTell(void *data);

    static size_t _memoryStreamRead(void*, size_t, size_t, void*);
    static int _memoryStreamSeek(void*, ogg_int64_t, int);
    static int _memoryStreamClose(void *data);
    static long _memoryStreamTell(void *data);

private:
    ALCdevice*      _device;
    ALCcontext*     _context;
};

_NAME_END

#endif 
