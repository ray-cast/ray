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
#ifndef _H_SOUND_SYSTEM_H_
#define _H_SOUND_SYSTEM_H_

#include <ray/sound_types.h>

_NAME_BEGIN

class EXPORT SoundSystem final
{
	__DeclareSingleton(SoundSystem)
public:
    SoundSystem() noexcept;
    ~SoundSystem() noexcept;

	void open() noexcept;
	void close() noexcept;

	bool isOpened() noexcept;

	SoundSourcePtr createSoundSource(const std::string& filename, SoundFile::Type type = SoundFile::Unknown) except;

	bool emptyHandler() const noexcept;
	bool add(SoundHandlerPtr handler) noexcept;
	bool remove(SoundHandlerPtr handler) noexcept;
	bool find(istream& stream, SoundHandlerPtr& handler) const noexcept;
	bool find(SoundFile::Type type, SoundHandlerPtr& handler) const noexcept;
	bool find(istream& stream, SoundFile::Type type, SoundHandlerPtr& handler) const noexcept;

private:

	SoundBufferPtr load(const std::string& filename, SoundFile::Type type) noexcept;
	SoundBufferPtr load(istream& stream, SoundFile::Type type = SoundFile::Unknown) noexcept;

private:

	SoundDevicePtr _soundDevice;
	SoundBufferMaps _soundBuffers;

	SoundHandlers _handlers;
};

_NAME_END

#endif