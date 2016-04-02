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

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#include <ray/except.h>
#include <ray/xmlreader.h>
#include <ray/ioserver.h>

struct ShaderIncludePath : public ID3DInclude
{
public:
	ShaderIncludePath(const ray::util::string& shader_path)
	{
		_path = shader_path;
		auto begin = _path.rbegin();
		auto end = _path.rend();

		for (auto& it : _path)
		{
			if (it == '\\')
				it = '/';
		}

		std::size_t length = _path.find_last_of('/') + 1;
		_filename = _path.substr(length, _path.size() - length);
		_filename = _filename.substr(0, _filename.find('.'));
		_path = _path.substr(0, length);
	}

	HRESULT STDMETHODCALLTYPE Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		std::string filename;

		if (IncludeType == D3D_INCLUDE_LOCAL)
		{
			filename = _path;
			filename += pFileName;
		}
		else if (IncludeType == D3D_INCLUDE_SYSTEM)
		{
			filename = "shaders/";
			filename += pFileName;
		}
		else // invalid type passed ???
		{
			*ppData = NULL;
			*pBytes = 0;
			return S_OK;
		}

		// open file
		char* data;
		DWORD size, read;

		ray::StreamReaderPtr stream;
		if (ray::IoServer::instance()->openFile(stream, filename))
		{
			std::size_t size = stream->size();
			if (size > 0)
			{
				data = new char[size];
				if (stream->read(data, size))
				{
					*ppData = data;
					*pBytes = size;
					return S_OK;
				}

				delete[] data;
			}
		}

		*ppData = NULL;
		*pBytes = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Close(LPCVOID pData)
	{
		delete[](std::uint8_t*)pData;
		return S_OK;
	}

	ray::util::string path() const
	{
		return _path;
	}

	ray::util::string filename() const
	{
		return _filename;
	}

private:
	ray::util::string _path;
	ray::util::string _filename;
};

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
		else if (name == "parameter")
		{
			Parameter parameter;
			parameter.name = reader.getValue<std::string>("name");
			parameter.type = reader.getValue<std::string>("type");
			parameter.semantic = reader.getValue<std::string>("semantic");
			parameter.value = reader.getValue<std::string>("value");

			_hlslCodes += "uniform " + parameter.type + " " + parameter.name + ";\n";

			_parameters.push_back(parameter);
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
						if (it.value == "point")
							_hlslCodes += "Filter = MIN_MAG_MIP_POINT;";
						else if (it.value == "linear")
							_hlslCodes += "Filter = MIN_MAG_MIP_LINEAR;";
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

	ShaderIncludePath sh_include(options.complieIn);

	D3D_SHADER_MACRO macro;
	macro.Name = options.defineKey.empty() ? NULL : options.defineKey.c_str();
	macro.Definition = options.defineValue.empty() ? NULL : options.defineValue.c_str();

	HRESULT hr = D3DCompile(
		options.hlslCode.data(),
		options.hlslCode.size(),
		options.complieIn.c_str(),
		&macro,
		&sh_include,
		options.main.c_str(),
		options.hlsl.c_str(),
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
		0,
		&binary,
		&error
		);

	if (hr != S_OK)
	{
		std::string line;
		std::size_t index = 1;
		std::ostringstream ostream;
		std::istringstream istream(options.hlslCode);

		ostream << (const char*)error->GetBufferPointer() << std::endl;
		while (std::getline(istream, line))
		{
			ostream << index << '\t' << line << std::endl;
			index++;
		}

		throw std::exception(ostream.str().c_str());
	}
	else
	{
		ray::util::hex2str(options.convCode, (const char*)binary->GetBufferPointer(), binary->GetBufferSize(), 16);

		if (!options.conv.empty() && options.conv != "bytecodes")
		{
			GLSLShader shader;
			GLSLCrossDependencyData dependency;

			std::uint32_t flags = HLSLCC_FLAG_GS_ENABLED | HLSLCC_FLAG_TESS_ENABLED | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
			if (TranslateHLSLFromMem((const char*)binary->GetBufferPointer(), flags, ConvGLLang(options.conv), NULL, &dependency, &shader))
				options.convCode = shader.sourceCode;

			FreeGLSLShader(&shader);
		}
	}

	if (binary)
		binary->Release();

	if (error)
		error->Release();
}