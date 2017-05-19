// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <io.h>
#include <fstream>
#include <iostream>

#include <ray/image.h>
#include "ies_Loader.h"

bool IES2HDR(const std::string& path, const std::string& outpath, IESFileInfo& info)
{
	std::fstream stream(path, std::ios_base::binary | std::ios_base::in);
	if (!stream.is_open())
	{
		std::cout << "IES2HDR Error: Failed to open file :" << path << std::endl;
		return false;
	}

	stream.seekg(0, std::ios_base::end);
	std::streamsize streamSize = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	std::vector<std::uint8_t> IESBuffer;
	IESBuffer.resize(streamSize + 1);
	stream.read((char*)IESBuffer.data(), IESBuffer.size());

	IESLoadHelper IESLoader;
	if (!IESLoader.load((char*)IESBuffer.data(), streamSize, info))
		return false;

	ray::image::Image image;
	if (!image.create(256, 1, ray::image::format_t::R32G32B32SFloat))
	{
		std::cout << "IES2HDR Error: Failed to create image : " << outpath << path << std::endl;;
		return false;
	}

	if (!IESLoader.saveAs1D(info, (float*)image.data(), image.width(), image.channel()))
	{
		std::cout << "IES2HDR Error: Failed to save image : " << outpath << path << std::endl;;
		return false;
	}

	image.save(outpath, "hdr");

	return true;
}

bool IES2HDR(const std::string& path, IESFileInfo& info)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	std::memset(drive, 0, sizeof(drive));
	std::memset(dir, 0, sizeof(dir));
	std::memset(fname, 0, sizeof(fname));
	std::memset(ext, 0, sizeof(ext));

	_splitpath(path.c_str(), drive, dir, fname, ext);

	std::string out = drive;
	out += dir;
	out += fname;
	out += ".hdr";

	return IES2HDR(path, out, info);
}

int main(int argc, const char* argv[])
{
	try
	{
		if (argc <= 1)
		{
			std::cout << "IES2HDR Info: ERROR! Please enter a path to a IES file/directory." << std::endl;
			std::system("pause");
			return EXIT_FAILURE;
		}

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		std::memset(drive, 0, sizeof(drive));
		std::memset(dir, 0, sizeof(dir));
		std::memset(fname, 0, sizeof(fname));
		std::memset(ext, 0, sizeof(ext));

		_splitpath(argv[1], drive, dir, fname, ext);

		if (_stricmp(ext, ".ies") == 0)
		{
			IESFileInfo info;
			if (!IES2HDR(argv[1], info))
				std::cout << "IES2HDR Error: " << info.error() << std::endl;
		}
		else
		{
			std::string path = drive;
			path += dir;
			path += fname;
			path += "/*.ies";

			_finddata_t fileinfo;
			std::memset(&fileinfo, 0, sizeof(fileinfo));

			auto handle = _findfirst(path.data(), &fileinfo);
			if (handle == -1)
				return false;

			try
			{
				do
				{
					if (!(fileinfo.attrib & _A_SUBDIR))
					{
						std::string filename;
						filename = drive;
						filename += dir;
						filename += fname;
						filename += "/";
						filename += fileinfo.name;

						std::cout << "IES2HDR Info: Converting IES to HDR :" << filename << std::endl;

						IESFileInfo info;
						if (!IES2HDR(filename, info))
							std::cout << "IES2HDR Error: " << info.error() << std::endl;
					}
				} while (_findnext(handle, &fileinfo) == 0);

				_findclose(handle);
			}
			catch (const std::exception& e)
			{
				_findclose(handle);
				throw e;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::system("pause");
	return 0;
}