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
#ifndef _H_COMPILE_H_
#define _H_COMPILE_H_

#include <ray/iarchive.h>

struct FxmlOptions
{
	std::string complieIn;
	std::string complieOut;

	std::string compilePath;
	std::string compileFilename;

	std::string defineKey;
	std::string defineValue;

	std::string main;

	std::string hlsl;
	std::string conv;

	std::string hlslCode;
	std::string convCode;
};

class FxmlCompile final
{
public:
	FxmlCompile() noexcept;
	~FxmlCompile() noexcept;

	bool setConvLang(const std::string& str) noexcept;

	void clear() noexcept;

	void load(const ray::util::string& filename) except;
	void save(const ray::util::string& filename) except;

	void load(ray::StreamReader& filename) except;
	void save(ray::StreamWrite& filename) except;

	void load(ray::iarchive& reader) except;
	void save(ray::oarchive& reader) except;

	void compile() except;
	void compile(FxmlOptions& options) except;

private:
	struct State
	{
		std::string name;
		std::string value;
	};

	struct Parameter
	{
		std::string name;
		std::string type;
		std::string semantic;
		std::string value;
	};

	struct Sampler
	{
		std::string name;
		std::vector<State> state;
	};

	struct Shader
	{
		std::string name;
		std::string value;
	};

	struct Pass
	{
		std::string name;
		std::vector<State> state;
	};

	struct Technique
	{
		std::string name;
		std::vector<Pass> pass;
	};

	struct InputLayoutChild
	{
		std::string name;
		std::string format;
	};

	struct InputLayout
	{
		std::string name;
		std::vector<InputLayoutChild> layouts;
	};

	std::string _conv;
	std::string _hlslCodes;
	
	std::map<std::string, bool> _onceInclude;
	std::map<std::string, FxmlOptions> _shaders;

	std::vector<Shader> _shader;
	std::vector<Parameter> _macros;
	std::vector<Parameter> _parameters;
	std::vector<Sampler> _samplers;
	std::vector<Technique> _techniques;
	std::vector<InputLayout> _inputLayouts;
};

#endif