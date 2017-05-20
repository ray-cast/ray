// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/graphics_shader.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsParam, rtti::Interface, "GraphicsParam")
__ImplementSubInterface(GraphicsAttribute, rtti::Interface, "GraphicsAttribute")
__ImplementSubInterface(GraphicsUniform, GraphicsParam, "GraphicsUniform")
__ImplementSubInterface(GraphicsUniformBlock, GraphicsParam, "GraphicsUniformBlock")
__ImplementSubInterface(GraphicsShader, GraphicsChild, "GraphicsShader")
__ImplementSubInterface(GraphicsProgram, GraphicsChild, "GraphicsProgram")

GraphicsShaderDesc::GraphicsShaderDesc() noexcept
	: _lang(GraphicsShaderLang::GraphicsShaderLangNone)
	, _model(GraphicsShaderModel::GraphicsShaderModel4X)
	, _stage(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit)
{
}

GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderLang lang, GraphicsShaderModel model, GraphicsShaderStageFlagBits stage, const std::string& code) noexcept
	: _bytecodes(code)
	, _lang(lang)
	, _model(model)
	, _stage(stage)
{
}

GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderLang lang, GraphicsShaderModel model, GraphicsShaderStageFlagBits stage, const std::vector<char>& code) noexcept
	: _lang(lang)
	, _model(model)
	, _stage(stage)
{
	_bytecodes.insert(_bytecodes.begin(), code.begin(), code.end());
}

GraphicsShaderDesc::~GraphicsShaderDesc() noexcept
{
}

void
GraphicsShaderDesc::setLanguage(GraphicsShaderLang lang) noexcept
{
	_lang = lang;
}

GraphicsShaderLang
GraphicsShaderDesc::getLanguage() const noexcept
{
	return _lang;
}

void 
GraphicsShaderDesc::setShaderModel(GraphicsShaderModel model) noexcept
{
	_model = model;
}

GraphicsShaderModel 
GraphicsShaderDesc::getShaderModel() const noexcept
{
	return _model;
}

void
GraphicsShaderDesc::setStage(GraphicsShaderStageFlagBits stage) noexcept
{
	_stage = stage;
}

GraphicsShaderStageFlagBits
GraphicsShaderDesc::getStage()const noexcept
{
	return _stage;
}

void
GraphicsShaderDesc::setByteCodes(const std::string& codes) noexcept
{
	_bytecodes = codes;
}

void 
GraphicsShaderDesc::setByteCodes(std::string&& codes) noexcept
{
	_bytecodes = std::move(codes);
}

const std::string&
GraphicsShaderDesc::getByteCodes() const noexcept
{
	return _bytecodes;
}

void
GraphicsShaderDesc::setEntryPoint(const std::string& main) noexcept
{
	_main = main;
}

void
GraphicsShaderDesc::setEntryPoint(std::string&& main) noexcept
{
	_main = std::move(main);
}

const std::string&
GraphicsShaderDesc::getEntryPoint() const noexcept
{
	return _main;
}

GraphicsProgramDesc::GraphicsProgramDesc() noexcept
{
}

GraphicsProgramDesc::~GraphicsProgramDesc() noexcept
{
}

bool
GraphicsProgramDesc::addShader(GraphicsShaderPtr shader) noexcept
{
	assert(shader);

	auto comp = [&](const GraphicsShaderPtr& it)
	{
		return it->getGraphicsShaderDesc().getStage() == shader->getGraphicsShaderDesc().getStage();
	};

	auto it = std::find_if(_shaders.begin(), _shaders.end(), comp);
	if (it != _shaders.end())
		return false;

	_shaders.push_back(shader);
	return true;
}

void
GraphicsProgramDesc::removeShader(GraphicsShaderPtr shader) noexcept
{
	auto it = std::find(_shaders.begin(), _shaders.end(), shader);
	if (it != _shaders.end())
		_shaders.erase(it);
}

const GraphicsShaders&
GraphicsProgramDesc::getShaders() const noexcept
{
	return _shaders;
}

GraphicsParam::GraphicsParam() noexcept
{
}

GraphicsParam::~GraphicsParam() noexcept
{
}

GraphicsAttribute::GraphicsAttribute() noexcept
{
}

GraphicsAttribute::~GraphicsAttribute() noexcept
{
}

GraphicsUniform::GraphicsUniform() noexcept
{
}

GraphicsUniform::~GraphicsUniform() noexcept
{
}

GraphicsUniformBlock::GraphicsUniformBlock() noexcept
{
}

GraphicsUniformBlock::~GraphicsUniformBlock() noexcept
{
}

GraphicsShader::GraphicsShader() noexcept
{
}

GraphicsShader::~GraphicsShader() noexcept
{
}

GraphicsProgram::GraphicsProgram() noexcept
{
}

GraphicsProgram::~GraphicsProgram() noexcept
{
}

_NAME_END