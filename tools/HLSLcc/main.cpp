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
#include <d3dcompiler.h>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <ray/fcntl.h>
#include <ray/string.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/except.h>

#include "compile.h"

class Options
{
public:
	std::string cmd;

	FxmlOptions options;
	FxmlCompile compile;
};

typedef void(CommandLineCallback(Options&));
typedef std::pair<std::string, CommandLineCallback*> CommandLine;
typedef std::vector<CommandLine> CommandLines;

void InitOptions(Options* psOptions, int argc, char** argv)
{
	psOptions->options.hlsl = "fxml";
	psOptions->options.conv = "bytecodes";
	psOptions->options.main = "main";

	if (argc == 0)
		return;

	if (ray::fcntl::access(argv[0], 0) == 0)
	{
		auto workDir = ray::util::directory(argv[0]);

		auto ioServer = ray::IoServer::instance();
		ioServer->addAssign(ray::IoAssign("sys", workDir + "..\\..\\engine\\"));
		ioServer->addAssign(ray::IoAssign("dlc", workDir + "..\\..\\dlc\\"));
		ioServer->mountArchives();
	}
}

bool checkLang(const std::string& str)
{
	if (str == "fxml") return true;

	if (str == "cs_5_0") return true;
	if (str == "ds_5_0") return true;
	if (str == "gs_5_0") return true;
	if (str == "hs_5_0") return true;
	if (str == "ps_5_0") return true;
	if (str == "vs_5_0") return true;

	if (str == "cs_4_1") return true;
	if (str == "gs_4_1") return true;
	if (str == "ps_4_1") return true;
	if (str == "vs_4_1") return true;

	if (str == "cs_4_0") return true;
	if (str == "gs_4_0") return true;
	if (str == "ps_4_0") return true;
	if (str == "vs_4_0") return true;

	if (str == "ps_3_0") return true;
	if (str == "ps_3_sw") return true;
	if (str == "vs_3_0") return true;
	if (str == "vs_3_sw") return true;

	if (str == "ps_2_0") return true;
	if (str == "ps_2_a") return true;
	if (str == "ps_2_b") return true;
	if (str == "ps_2_sw") return true;
	if (str == "vs_2_0") return true;
	if (str == "vs_2_a") return true;
	if (str == "vs_2_b") return true;
	if (str == "vs_2_sw") return true;

	return false;
}

void HelpCommand(Options& option)
{
	std::cout << "Command line options:" << std::endl;
	std::cout << "\t-q quit." << std::endl;
	std::cout << "\t-in=X Shader file to compile." << std::endl;
	std::cout << "\t-out=X File to write the compiled shader from -in to." << std::endl;
	std::cout << "\t-lang=X HLSL language to use. (example, vs_4_0, ps_4_0)." << std::endl;
	std::cout << "\t-main=X The name of the shader entry point function where shader execution begins." << std::endl;
	std::cout << "\t-conv convert glsl lang (example, glsl330, gles300)." << std::endl;
	std::cout << "\t-show show compiled shader code." << std::endl;
	std::cout << "\t-linkin=X Semicolon-separated shader list. Max one per stage. Pixel shader should be given first. Hull (if present) should come before domain." << std::endl;
	std::cout << "\t-linkout=X Semicolon-separated list of output files to write the compilition result of -linkin shaders to." << std::endl;
	std::cout << "\t-hashout=[dir/]out-file-name \t Output file name is a hash of 'out-file-name', put in the directory 'dir'." << std::endl;
	std::cout << "\t-hashlinkout=X Semicolon-separated list of output files to write the compilition result of -linkin shaders to. The basepath is a hash of all the file names given." << std::endl;
	std::cout << std::endl;
}

void QuitCommand(Options& option)
{
	::exit(0);
}

void SetInputFileCommand(Options& options)
{
	options.options.complieIn = options.cmd;
	options.options.complieIn = ray::util::trim(options.options.complieIn, ' ');
	options.options.complieIn = ray::util::trim(options.options.complieIn, '"');

	std::replace_if(options.options.complieIn.begin(), options.options.complieIn.end(), [](char it) { return it == '\\';}, '/');

	options.options.compilePath = options.options.complieIn;
	options.options.compilePath = options.options.compilePath.substr(0, options.cmd.find_last_of('/') + 1);

	options.options.compileFilename = options.options.complieIn.substr(options.options.compilePath.size(), options.options.complieIn.size() - options.options.compilePath.size());
	options.options.compileFilename = options.options.compileFilename.substr(0, options.options.compileFilename.find('.'));
}

void SetOutputFileCommand(Options& option)
{
	option.options.complieOut = option.cmd;
	option.options.complieOut = ray::util::trim(option.options.complieOut, '"');
}

void SetHLSLCommand(Options& option)
{
	option.options.hlsl = option.cmd;
	option.options.hlsl = ray::util::trim(option.options.hlsl, '"');
	option.options.hlsl = ray::util::trim(option.options.hlsl, ' ');

	if (!checkLang(option.options.hlsl))
	{
		option.options.hlsl.clear();
		std::cout << "Invlid HLSL lang: " << option.options.hlsl << std::endl;
	}
}

void SetGLSLCommand(Options& option)
{
	option.options.conv = option.cmd;
	option.options.conv = ray::util::trim(option.options.conv, '"');
	option.options.conv = ray::util::trim(option.options.conv, ' ');

	if (!option.compile.setConvLang(option.options.conv))
		std::cout << "Invlid GLSL lang: " << option.options.conv << std::endl;
}

void SetEntryPointCommand(Options& option)
{
	option.options.main = option.cmd;
	option.options.main = ray::util::trim(option.options.main, '"');
}

void CompileCommand(Options& options)
{
	try
	{
		options.compile.clear();
		options.compile.load(options.options.complieIn);
		options.compile.compile();

		if (!options.options.complieOut.empty())
			options.compile.save(options.options.complieOut);
		else
			options.compile.save(options.options.complieIn + ".o");

		std::cout << "compile " << options.options.hlsl << " success." << std::endl;
	}
	catch (const ray::failure& e)
	{
		std::cout << "compile " << options.options.hlsl << " fail." << std::endl;
		std::cout << e.what();
	}
	catch (const std::exception& e)
	{
		std::cout << "compile " << options.options.hlsl << " fail." << std::endl;
		std::cout << e.what();
	}
}

void ShowCommand(Options& options)
{
	ray::MemoryWrite stream;
	options.compile.save(stream);
	if (stream)
	{
		std::string string(stream.map(), stream.size());
		std::cout << string;
	}
}

bool GetOptions(std::vector<std::string>& result, std::string args, const std::string& delims)
{
	std::size_t start = args.find_first_not_of(delims);
	while (start != args.npos)
	{
		std::size_t end = args.find_first_of(delims, start);
		if (end != args.npos)
			result.push_back(delims + args.substr(start, end - start));
		else
		{
			result.push_back(delims + args.substr(start));
			break;
		}

		start = args.find_first_not_of(delims, end + 1);
	}

	if (result.empty())
		return false;

	return true;
}

int main(int argc, char** argv)
{
	Options options;
	InitOptions(&options, argc, argv);

	CommandLines commandlist;
	commandlist.push_back(std::make_pair("-help", &HelpCommand));
	commandlist.push_back(std::make_pair("-q", &QuitCommand));
	commandlist.push_back(std::make_pair("-in=", &SetInputFileCommand));
	commandlist.push_back(std::make_pair("-out=", &SetOutputFileCommand));
	commandlist.push_back(std::make_pair("-lang=", &SetHLSLCommand));
	commandlist.push_back(std::make_pair("-conv=", &SetGLSLCommand));
	commandlist.push_back(std::make_pair("-main=", &SetEntryPointCommand));
	commandlist.push_back(std::make_pair("-compile", &CompileCommand));
	commandlist.push_back(std::make_pair("-show", &ShowCommand));

	for (;;)
	{
		std::string args;
		std::cout << ">>";
		std::getline(std::cin, args);

		std::vector<std::string> cmds;
		if (GetOptions(cmds, args, "-"))
		{
			for (auto& arg : cmds)
			{
				auto it = commandlist.begin();
				auto end = commandlist.end();

				for (;it != end; ++it)
				{
					if (arg.compare(0, (*it).first.size(), (*it).first) == 0)
					{
						options.cmd = arg.substr((*it).first.size());
						(*it).second(options);
						break;
					}
				}

				if (it == end)
				{
					std::cout << "Unknown command: " << arg << std::endl;
				}
			}
		}
	}

	return 0;
}