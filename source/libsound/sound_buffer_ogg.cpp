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
#include <ray/sound_buffer_ogg.h>
#include <vorbisfile.h>

_NAME_BEGIN

std::size_t _istreamRead(void* ptr, std::size_t elementSize, std::size_t count, void* data)
{
	assert(data != nullptr);

	auto input = static_cast<StreamReader*>(data);
	input->read((char*)(ptr), elementSize * count);
	return input->gcount();
}

int _istreamSeek(void* data, ogg_int64_t pos, int whence)
{
	assert(data != nullptr);

	auto input = static_cast<ray::StreamReader*>(data);
	switch (whence)
	{
	case SEEK_SET:
		input->seekg(pos, ios_base::beg);
		break;
	case SEEK_CUR:
		input->seekg(pos, ios_base::cur);
		break;
	case SEEK_END:
		input->seekg(pos, ios_base::end);
		break;
	}
	return 0;
}

int _istreamClose(void *data)
{
	return 0;
}

long _istreamTell(void *data)
{
	assert(data != nullptr);

	auto input = static_cast<ray::StreamReader*>(data);
	return input->tellg();
}

OggStreamBuffer::OggStreamBuffer() noexcept
	: _oggVorbisFile(nullptr)
	, _next(0)
{
}

OggStreamBuffer::~OggStreamBuffer() noexcept
{
	this->close();
}

bool
OggStreamBuffer::access(StreamReader& stream) const noexcept
{
	OggVorbis_File oggfile;

	ov_callbacks callbacks;
	callbacks.read_func = &_istreamRead;
	callbacks.seek_func = &_istreamSeek;
	callbacks.tell_func = &_istreamTell;
	callbacks.close_func = &_istreamClose;

	auto err = ::ov_test_callbacks(&stream, &oggfile, nullptr, 0, callbacks);
	::ov_clear(&oggfile);

	return (err < 0) ? false : true;
}

bool
OggStreamBuffer::open(StreamReaderPtr stream) noexcept
{
	assert(!_oggVorbisFile);

	OggVorbis_File ogg;

	ov_callbacks callbacks;
	callbacks.read_func = &_istreamRead;
	callbacks.seek_func = &_istreamSeek;
	callbacks.tell_func = &_istreamTell;
	callbacks.close_func = &_istreamClose;

	_next = 0;
	_stream = stream;

	auto err = ::ov_open_callbacks(_stream.get(), &ogg, nullptr, 0, callbacks);
	if (err < 0)
	{
		::ov_clear(&ogg);
		return false;
	}

	_oggVorbisFile = new OggVorbis_File;
	std::memcpy(_oggVorbisFile, &ogg, sizeof(OggVorbis_File));

	return true;
}

streamsize 
OggStreamBuffer::read(char* str, std::streamsize cnt) noexcept
{
	assert(_oggVorbisFile);

	int bitstream = 0;
	streamsize bytes = 0;
	std::size_t offset = 0;

	do
	{
		bytes = ::ov_read(_oggVorbisFile, str + offset, cnt - offset, 0, 2, 1, &bitstream);
		offset += bytes;
	} while (bytes && offset < cnt);

	if (bytes > 0)
	{
		auto info = ::ov_info(_oggVorbisFile, -1);
		if (info->channels == 6)
		{
			short* samples = (short*)str;
			for (std::size_t i = 0; i < (bytes >> 1); i += 6)
			{
				std::swap(samples[i + 1], samples[i + 2]);
				std::swap(samples[i + 3], samples[i + 5]);
				std::swap(samples[i + 4], samples[i + 5]);
			}
		}
	}

	return offset;
}

streamsize 
OggStreamBuffer::write(const char* str, std::streamsize cnt) noexcept
{
	assert(false);
	return 0;
}

streamoff 
OggStreamBuffer::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
	assert(dir == ios_base::beg || dir == ios_base::cur || dir == ios_base::end);

	if (dir == ios_base::beg)
	{
		_next = pos;

		::ov_pcm_seek(_oggVorbisFile, pos);
		return pos;
	}
	else if (dir == ios_base::cur)
	{
		_next = _next + pos;
		if (_next > this->size())
		{
			pos = this->size() - _next;
			_next = this->size();
		}

		::ov_pcm_seek(_oggVorbisFile, pos);
		return pos;
	}
	else if (dir == ios_base::end)
	{
		std::size_t size = this->size();
		pos = size + pos;
		if (pos > size)
			_next = size;
		else
			_next = pos;

		::ov_pcm_seek(_oggVorbisFile, pos);
		return pos;
	}

	return 0;
}

streamoff 
OggStreamBuffer::tellg() noexcept
{
	assert(this->is_open());
	return ::ov_pcm_tell(_oggVorbisFile);
}

streamsize 
OggStreamBuffer::size() const noexcept
{
	assert(this->is_open());
	auto info = ::ov_info(_oggVorbisFile, -1);
	return ::ov_pcm_total(_oggVorbisFile, -1) * info->channels * 2;
}

bool 
OggStreamBuffer::is_open() const noexcept
{
	return _oggVorbisFile ? true : false;
}

int 
OggStreamBuffer::flush() noexcept
{
	if (_stream->flush())
		return 0;
	return -1;
}

bool
OggStreamBuffer::close() noexcept
{
	if (_oggVorbisFile)
	{
		::ov_clear(_oggVorbisFile);

		delete _oggVorbisFile;
		_oggVorbisFile = nullptr;

		return true;
	}

	return false;
}

std::uint8_t
OggStreamBuffer::getBufferChannelCount() const noexcept
{
	assert(_oggVorbisFile);
	auto info = ::ov_info(_oggVorbisFile, -1);
	return info->channels;
}

std::size_t
OggStreamBuffer::getBufferTotalSamples() const noexcept
{
	assert(_oggVorbisFile);
	return ::ov_pcm_total(_oggVorbisFile, -1);
}

SoundFormat
OggStreamBuffer::getBufferType() const noexcept
{
	assert(_oggVorbisFile);

	auto info = ::ov_info(_oggVorbisFile, -1);

	if (info->channels == 1)
		return SoundFormat::Mono16;
	else if (info->channels == 2)
		return SoundFormat::Stereo16;
	else if (info->channels == 4)
		return SoundFormat::Quad16;
	else if (info->channels == 6)
		return SoundFormat::Chn16;
	else
		return SoundFormat::None;
}

SoundFrequency
OggStreamBuffer::getBufferFrequency() const noexcept
{
	assert(_oggVorbisFile);
	auto info = ::ov_info(_oggVorbisFile, -1);
	return info->rate;
}

OggSoundReader::OggSoundReader() noexcept
	: SoundReader(&_oggVorbisFile)
{
}

OggSoundReader::~OggSoundReader() noexcept
{
	_oggVorbisFile.close();
}

SoundReaderPtr
OggSoundReader::clone() const noexcept
{
	return std::make_shared<OggSoundReader>();
}

_NAME_END