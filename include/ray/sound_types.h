#ifndef _H_SOUND_TYPES_H_
#define _H_SOUND_TYPES_H_

#include "ray/platform.h"

_NAME_BEGIN

typedef int SOUND_FORMAT_TYPE;
typedef int SOUND_FREQUENCY_TYPE;

enum class SOUND_FORMAT_ENUM : SOUND_FORMAT_TYPE
{
    Invalid     = 0,
    MONO8       = 0x1100,
    MONO16      = 0x1101,
    STEREO8     = 0x1102,
    STEREO16    = 0x1103,
};

typedef std::shared_ptr<class SoundBuffer> BufferPtr;
typedef std::shared_ptr<class SoundSource> SourcePtr;

_NAME_END

#endif 
