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
#include "compile.h"

#include <d3dcompiler.h>
#include <sstream>
#include <regex>

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#include <ray/except.h>
#include <ray/xmlreader.h>
#include <ray/ioserver.h>

GLLang ConvGLLang(const std::string& str)
{
	if (str == "default") return GLLang::LANG_DEFAULT;
	if (str == "glsl120") return GLLang::LANG_120;
	if (str == "glsl130") return GLLang::LANG_130;
	if (str == "glsl140") return GLLang::LANG_140;
	if (str == "glsl150") return GLLang::LANG_150;
	if (str == "glsl330") return GLLang::LANG_330;
	if (str == "glsl400") return GLLang::LANG_400;
	if (str == "glsl410") return GLLang::LANG_410;
	if (str == "glsl420") return GLLang::LANG_420;
	if (str == "glsl430") return GLLang::LANG_430;
	if (str == "glsl440") return GLLang::LANG_440;
	if (str == "gles100") return GLLang::LANG_ES_100;
	if (str == "gles300") return GLLang::LANG_ES_300;
	if (str == "gles310") return GLLang::LANG_ES_310;

	return GLLang::LANG_DEFAULT;
}

FxmlCompile::FxmlCompile() noexcept
	: _conv("bytecodes")
{
}

FxmlCompile::~FxmlCompile() noexcept
{
}

bool
FxmlCompile::setConvLang(const std::string& str) noexcept
{
	if (str == "glsl120") { _conv = str; return true; }
	if (str == "glsl130") { _conv = str; return true; }
	if (str == "glsl140") { _conv = str; return true; }
	if (str == "glsl150") { _conv = str; return true; }
	if (str == "glsl330") { _conv = str; return true; }
	if (str == "glsl400") { _conv = str; return true; }
	if (str == "glsl410") { _conv = str; return true; }
	if (str == "glsl420") { _conv = str; return true; }
	if (str == "glsl430") { _conv = str; return true; }
	if (str == "glsl440") { _conv = str; return true; }
	if (str == "gles100") { _conv = str; return true; }
	if (str == "gles300") { _conv = str; return true; }
	if (str == "gles310") { _conv = str; return true; }
	if (str == "bytecodes") { _conv = str; return true; }

	_conv = "bytecodes";
	return false;
}

void
FxmlCompile::clear() noexcept
{
	_shader.clear();
	_parameters.clear();
	_samplers.clear();
	_techniques.clear();
	_hlslCodes.clear();
	_onceInclude.clear();
	_shaders.clear();
}

void
FxmlCompile::load(const ray::util::string& filename) except
{
	try
	{
		ray::StreamReaderPtr stream;
		if (!ray::IoServer::instance()->openFile(stream, filename, ray::ios_base::in))
			throw ray::failure(__TEXT("Opening file fail:") + filename);

		this->load(*stream);
	}
	catch (const ray::failure& e)
	{
		throw ray::failure(__TEXT("in ") + filename + __TEXT(" ") + e.message(), e.stack());
	}
}

void
FxmlCompile::save(const ray::util::string& filename) except
{
	try
	{
		ray::StreamWritePtr stream;
		if (!ray::IoServer::instance()->openFile(stream, filename, ray::ios_base::out))
			throw ray::failure(__TEXT("Opening file fail:") + filename);

		this->save(*stream);
	}
	catch (const ray::failure& e)
	{
		throw ray::failure(__TEXT("in ") + filename + __TEXT(" ") + e.message(), e.stack());
	}
}

void
FxmlCompile::load(ray::StreamReader& stream) except
{
	ray::XMLReader reader;
	if (reader.open(stream))
	{
		reader.setToFirstChild();
		this->load(reader);
	}
}

void
FxmlCompile::save(ray::StreamWrite& stream) except
{
	ray::XMLWrite reader;
	if (reader.open("1.0", "utf-8", ""))
	{
		reader.setToFirstChild();
		this->save(reader);

		reader.setToRoot();
		reader.save(stream);
	}
}

void
FxmlCompile::load(ray::iarchive& reader) except
{
	auto language = reader.getValue<std::string>("language");

	if (!reader.setToFirstChild())
		throw ray::failure(__TEXT("The file has been damaged and can't be recovered, so I can't open it"));

	do
	{
		std::string name = reader.getCurrentNodeName();
		if (name == "include")
		{
			auto path = reader.getValue<std::string>("name");
			if (!_onceInclude[path])
			{
				if (!path.empty())
					this->load(path);
				_onceInclude[path] = true;
			}
		}
		else if (name == "inputlayout")
		{
			InputLayout inputLayout;
			inputLayout.name = reader.getValue<std::string>("name");
			if (reader.setToFirstChild())
			{
				do
				{
					InputLayoutChild child;
					child.name = reader.getValue<std::string>("name");
					child.format = reader.getValue<std::string>("format");
					inputLayout.layouts.push_back(child);
				} while (reader.setToNextChild());
			}
			_inputLayouts.push_back(inputLayout);
		}
		else if (name == "parameter")
		{
			Parameter parameter;
			parameter.name = reader.getValue<std::string>("name");
			parameter.type = reader.getValue<std::string>("type");
			parameter.semantic = reader.getValue<std::string>("semantic");
			parameter.value = reader.getValue<std::string>("value");

			auto type = parameter.type.substr(0, parameter.type.find_first_of('['));
			_hlslCodes += "uniform " + type + " " + parameter.name + ";\n";

			_parameters.push_back(parameter);
		}
		else if (name == "buffer")
		{
			cbuffer buffer;

			buffer.name = reader.getValue<std::string>("name");

			if (!reader.setToFirstChild())
				continue;

			_hlslCodes += "cbuffer " + buffer.name + " {\n";

			do
			{
				auto nodename = reader.getCurrentNodeName();
				if (nodename == "parameter")
				{
					Parameter parameter;
					parameter.name = reader.getValue<std::string>("name");
					parameter.type = reader.getValue<std::string>("type");
					parameter.semantic = reader.getValue<std::string>("semantic");
					parameter.value = reader.getValue<std::string>("value");

					buffer.params.push_back(parameter);

					if (parameter.name.empty())
						continue;
					if (parameter.name.empty())
						continue;

					parameter.type = parameter.type.substr(0, parameter.type.find_first_of('['));
					_hlslCodes += "uniform " + parameter.type + " " + parameter.name + ";\n";
				}

			} while (reader.setToNextChild());

			_hlslCodes += "}\n";

			_buffers.push_back(buffer);
		}
		else if (name == "macro")
		{
			Parameter macro;
			macro.name = reader.getValue<std::string>("name");
			macro.type = reader.getValue<std::string>("type");
			macro.semantic = reader.getValue<std::string>("semantic");
			macro.value = reader.getValue<std::string>("value");

			_hlslCodes += "#define " + macro.name + " " + macro.value + "\n";

			_macros.push_back(macro);
		}
		else if (name == "sampler")
		{
			Sampler sampler;
			sampler.name = reader.getValue<std::string>("name");
			if (reader.setToFirstChild())
			{
				do
				{
					State state;
					state.name = reader.getValue<std::string>("name");
					state.value = reader.getValue<std::string>("value");
					sampler.state.push_back(state);
				} while (reader.setToNextChild());
			}

			if (!sampler.name.empty() && !sampler.state.empty())
			{
				_hlslCodes += "SamplerState " + sampler.name + "{";
				for (auto& it : sampler.state)
				{
					if (it.name == "filter")
					{
						_hlslCodes += "filter = " + it.value + ";";
					}
					else if (it.name == "wrap")
					{
						if (it.value == "clamp")
							_hlslCodes += "AddressU = Clamp; AddressV = Clamp;";
						else if (it.value == "mirror")
							_hlslCodes += "AddressU = Mirror; AddressV = Mirror;";
						else if (it.value == "repeat")
							_hlslCodes += "AddressU = Wrap; AddressV = Wrap;";
					}
				}

				_hlslCodes += "};\n";
				_samplers.push_back(sampler);
			}
		}
		else if (name == "shader")
		{
			_hlslCodes += reader.getText();
		}
		else if (name == "technique")
		{
			Technique technique;
			technique.name = reader.getValue<std::string>("name");

			if (reader.setToFirstChild())
			{
				do
				{
					if (reader.getCurrentNodeName() == "pass")
					{
						Pass pass;
						pass.name = reader.getValue<std::string>("name");
						if (reader.setToFirstChild())
						{
							do
							{
								State state;
								state.name = reader.getValue<std::string>("name");
								state.value = reader.getValue<std::string>("value");
								pass.state.push_back(state);
							} while (reader.setToNextChild());
						}

						technique.pass.push_back(pass);
					}

				} while (reader.setToNextChild());
			}

			_techniques.push_back(technique);
		}
	} while (reader.setToNextChild());
}

void
FxmlCompile::save(ray::oarchive& reader) except
{
	reader.addNode("effect");
	reader.addAttribute("language", _conv);

	for (auto& parameter : _parameters)
	{
		reader.addSubNode("parameter");
		if (!parameter.name.empty())
			reader.addAttribute("name", parameter.name);
		if (!parameter.type.empty())
			reader.addAttribute("type", parameter.type);
		if (!parameter.semantic.empty())
			reader.addAttribute("semantic", parameter.semantic);
		if (!parameter.value.empty())
			reader.addAttribute("value", parameter.value);
		reader.setToParent();
	}

	for (auto& macro : _macros)
	{
		reader.addSubNode("macro");
		if (!macro.name.empty())
			reader.addAttribute("name", macro.name);
		if (!macro.type.empty())
			reader.addAttribute("type", macro.type);
		if (!macro.semantic.empty())
			reader.addAttribute("semantic", macro.semantic);
		if (!macro.value.empty())
			reader.addAttribute("value", macro.value);
		reader.setToParent();
	}

	for (auto& sampler : _samplers)
	{
		reader.addSubNode("sampler");
		reader.addAttribute("name", sampler.name);

		for (auto& state : sampler.state)
		{
			reader.addSubNode("state");
			reader.addAttribute("name", state.name);
			reader.addAttribute("value", state.value);
			reader.setToParent();
		}

		reader.setToParent();
	}

	for (auto& buffer : _buffers)
	{
		reader.addSubNode("buffer");
		reader.addAttribute("name", buffer.name);

		for (auto& parameter : buffer.params)
		{
			reader.addSubNode("parameter");
			if (!parameter.name.empty())
				reader.addAttribute("name", parameter.name);
			if (!parameter.type.empty())
				reader.addAttribute("type", parameter.type);
			if (!parameter.semantic.empty())
				reader.addAttribute("semantic", parameter.semantic);
			if (!parameter.value.empty())
				reader.addAttribute("value", parameter.value);
			reader.setToParent();
		}

		reader.setToParent();
	}

	for (auto& inputLayout : _inputLayouts)
	{
		reader.addSubNode("inputlayout");
		reader.addAttribute("name", inputLayout.name);

		for (auto& layout : inputLayout.layouts)
		{
			reader.addSubNode("layout");
			reader.addAttribute("name", layout.name);
			reader.addAttribute("format", layout.format);
			reader.setToParent();
		}

		reader.setToParent();
	}

	for (auto& it : _shaders)
	{
		reader.addSubNode("shader");
		reader.addAttribute("name", it.first);
		reader.addAttribute("", "\n" + it.second.convCode);
		reader.setToParent();
	}

	for (auto& it : _techniques)
	{
		reader.addSubNode("technique");
		if (!it.name.empty())
			reader.addAttribute("name", it.name);

		for (auto& pass : it.pass)
		{
			reader.addSubNode("pass");
			reader.addAttribute("name", pass.name);

			for (auto& state : pass.state)
			{
				reader.addSubNode("state");
				reader.addAttribute("name", state.name);
				reader.addAttribute("value", state.value);
				reader.setToParent();
			}

			reader.setToParent();
		}

		reader.setToParent();
	}
}

void
FxmlCompile::compile() except
{
	for (auto& it : _techniques)
	{
		for (auto& pass : it.pass)
		{
			for (auto& state : pass.state)
			{
				if (state.name == "vertex")
				{
					auto& options = _shaders[state.value];
					if (options.hlslCode.empty())
					{
						options.hlsl = "vs_4_0";
						options.hlslCode = _hlslCodes;
						options.main = state.value;
						options.conv = _conv;

						this->compile(options);
					}
				}
				else if (state.name == "fragment")
				{
					auto& options = _shaders[state.value];
					if (options.hlslCode.empty())
					{
						options.hlsl = "ps_4_0";
						options.hlslCode = _hlslCodes;
						options.main = state.value;
						options.conv = _conv;

						this->compile(options);
					}
				}
			}
		}
	}
}

void
FxmlCompile::compile(FxmlOptions& options) except
{
	ID3DBlob* binary = nullptr;
	ID3DBlob* error = nullptr;

	D3DCreateBlob(4096, &binary);
	D3DCreateBlob(4096, &error);

	D3D_SHADER_MACRO macro;
	macro.Name = options.defineKey.empty() ? NULL : options.defineKey.c_str();
	macro.Definition = options.defineValue.empty() ? NULL : options.defineValue.c_str();

	HRESULT hr = D3DCompile(
		options.hlslCode.data(),
		options.hlslCode.size(),
		options.complieIn.c_str(),
		&macro,
		nullptr,
		options.main.c_str(),
		options.hlsl.c_str(),
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
		0,
		&binary,
		&error
		);

	if (hr != S_OK)
	{
		std::regex pattern("\\((.*?,.*?)\\): error");
		std::match_results<std::string::const_iterator> result;
		std::string errorString((char*)error->GetBufferPointer(), error->GetBufferSize());

		if (std::regex_search(errorString, result, pattern))
		{
			std::istringstream patternResult(result.str());

			std::size_t row;
			patternResult.ignore(1);
			patternResult >> row;

			std::string line;
			std::ostringstream ostream;
			std::istringstream istream(_hlslCodes);
			ostream << (const char*)error->GetBufferPointer() << std::endl;

			std::size_t start = std::max<std::size_t>(0, row - 10);

			for (std::size_t i = 0; i < row + 10; i++)
			{
				if (!std::getline(istream, line))
					break;

				if (i >= start)
					ostream << i << '\t' << line << std::endl;
			}

			if (binary)
				binary->Release();

			if (error)
				error->Release();

			throw ray::failure(ostream.str().c_str());
		}
	}
	else
	{
		ray::util::hex2str(options.convCode, (const char*)binary->GetBufferPointer(), binary->GetBufferSize(), 16);

		if (!options.conv.empty() && options.conv != "bytecodes")
		{
			GLSLShader shader;
			GLSLCrossDependencyData dependency;

			std::uint32_t flags = HLSLCC_FLAG_GS_ENABLED | HLSLCC_FLAG_TESS_ENABLED | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
			if (TranslateHLSLFromMem((const char*)binary->GetBufferPointer(), flags, ConvGLLang(options.conv), 0, NULL, &dependency, &shader))
				options.convCode = shader.sourceCode;

			FreeGLSLShader(&shader);
		}
	}

	if (binary)
		binary->Release();

	if (error)
		error->Release();
}