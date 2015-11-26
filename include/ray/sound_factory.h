#ifndef _H_SOUND_FACTORY_H_
#define _H_SOUND_FACTORY_H_

#include "ray/sound_types.h"

_NAME_BEGIN

class EXPORT SoundFactory
{
    SoundFactory() = delete;
public:
    static BufferPtr CreateBuffer(const std::string& name);
    static SourcePtr CreateSource(const std::string& name);
    static SystemPtr CreateSystem(const std::string& name);
};

_NAME_END

#endif