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
#include <ray/sound_handler_ogg.h>
#include <ray/sound_buffer.h>
#include <codec.h>
#include <vorbisfile.h>

_NAME_BEGIN

std::size_t _istreamRead(void* ptr, size_t elementSize, size_t count, void* data)
{
	if (data != nullptr)
	{
		auto input = static_cast<istream*>(data);
		if (input->is_open())
		{
			input->read((char*)(ptr), elementSize * count);
			return count;
		}
	}
	return 0;
}

int _istreamSeek(void* data, ogg_int64_t pos, int whence)
{
	if (data != nullptr)
	{
		auto input = static_cast<ray::istream*>(data);
		{
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
	}
	return -1;
}

int _istreamClose(void *data)
{
	if (data != nullptr)
	{
		auto input = static_cast<ray::istream*>(data);
		{
			if (input->is_open())
			{
				input->close();
				if (!input->is_open())
				{
					return 0;
				}
			}
		}
	}
	return -1;
}

long _istreamTell(void *data)
{
	if (data != nullptr)
	{
		auto input = static_cast<ray::istream*>(data);
		{
			return input->tellg();
		}
	}
	return 0;
}

bool 
OggSoundHandler::doCanRead(istream& stream) const noexcept
{
	OggVorbis_File oggfile;

	ov_callbacks callbacks;
	callbacks.read_func = &_istreamRead;
	callbacks.seek_func = &_istreamSeek;
	callbacks.tell_func = &_istreamTell;
	callbacks.close_func = &_istreamClose;

	auto err = ::ov_test_callbacks(&stream, &oggfile, nullptr, 0, callbacks);
	if (err < 0)
		return false;
	return true;
}

bool 
OggSoundHandler::doLoad(SoundBuffer& buffer, istream& stream) except
{
	OggVorbis_File oggfile;

	ov_callbacks callbacks;
	callbacks.read_func = &_istreamRead;
	callbacks.seek_func = &_istreamSeek;
	callbacks.tell_func = &_istreamTell;
	callbacks.close_func = &_istreamClose;

	auto err = ::ov_open_callbacks(&stream, &oggfile, nullptr, 0, callbacks);
	if (err < 0)
	{
		::ov_clear(&oggfile);
		return false;
	}

	auto info = ::ov_info(&oggfile, -1);
	if (info == nullptr)
	{
		::ov_clear(&oggfile);
		return false;
	}

	auto format = (info->channels == 1) ? SoundFormat::Mono16 : SoundFormat::Stereo16;
	auto freq = info->rate;

	long bytes = 0;
	int endian = 0;
	int bitstream = 0;

	const int file_buffer_size = 4096;
	char file_buff[file_buffer_size];

	std::vector<char> fileData;
	do
	{
		bytes = ::ov_read(&oggfile, file_buff, file_buffer_size, endian, 2, 1, &bitstream);
		fileData.insert(fileData.end(), file_buff, file_buff + bytes);

	} while (bytes != 0);

	buffer.setBufferType(format);
	buffer.setBufferFrequency(freq);
	buffer.writeBufferData(fileData.data(), fileData.size());

	::ov_clear(&oggfile);

	return true;
}

bool 
OggSoundHandler::doSave(SoundBuffer& buffer, ostream& stream) except
{
	assert(false);
	return false;
}

_NAME_END