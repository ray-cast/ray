// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_AL_SOUND_DEVICE_H_
#define _H_AL_SOUND_DEVICE_H_

#include <ray/sound_device.h>
#include <al.h>
#include <alc.h>
#include <vorbisfile.h>

_NAME_BEGIN

class EXPORT ALSoundDevice : public SoundDevice
{
public:
    ALSoundDevice() noexcept;
    virtual ~ALSoundDevice() noexcept;

    virtual bool open();
    virtual void close() noexcept;

    virtual bool isOpen() const noexcept;

	virtual SoundBufferPtr createSoundBuffer();
	virtual SoundSourcePtr createSoundSource();

    virtual SoundBufferPtr load(const std::string& filename);
    virtual SoundBufferPtr load(MemoryReader& reader);

private:
    SoundBufferPtr LoadFile(OggVorbis_File* file);

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
