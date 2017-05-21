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
#ifndef _H_GRAPHICS_SHADER_H_
#define _H_GRAPHICS_SHADER_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsShaderDesc final
{
public:
	GraphicsShaderDesc() noexcept;
	GraphicsShaderDesc(GraphicsShaderLang lang, GraphicsShaderModel model, GraphicsShaderStageFlagBits stage, const std::string& code) noexcept;
	GraphicsShaderDesc(GraphicsShaderLang lang, GraphicsShaderModel model, GraphicsShaderStageFlagBits stage, const std::vector<char>& code) noexcept;
	virtual ~GraphicsShaderDesc() noexcept;

	void setLanguage(GraphicsShaderLang lang) noexcept;
	GraphicsShaderLang getLanguage() const noexcept;

	void setShaderModel(GraphicsShaderModel model) noexcept;
	GraphicsShaderModel getShaderModel() const noexcept;

	void setStage(GraphicsShaderStageFlagBits type) noexcept;
	GraphicsShaderStageFlagBits getStage() const noexcept;

	void setByteCodes(const std::string& codes) noexcept;
	void setByteCodes(std::string&& codes) noexcept;
	const std::string& getByteCodes() const noexcept;

	void setEntryPoint(const std::string& codes) noexcept;
	void setEntryPoint(std::string&& codes) noexcept;
	const std::string& getEntryPoint() const noexcept;

private:
	std::string _main;
	std::string _bytecodes;
	GraphicsShaderLang _lang;
	GraphicsShaderModel _model;
	GraphicsShaderStageFlagBits _stage;
};

class EXPORT GraphicsProgramDesc final
{
public:
	GraphicsProgramDesc() noexcept;
	virtual ~GraphicsProgramDesc() noexcept;

	bool addShader(GraphicsShaderPtr shader) noexcept;
	void removeShader(GraphicsShaderPtr shader) noexcept;

	const GraphicsShaders& getShaders() const noexcept;

private:
	GraphicsShaders _shaders;
};

class EXPORT GraphicsAttribute : public rtti::Interface
{
	__DeclareSubInterface(GraphicsAttribute, GraphicsParam)
public:
	GraphicsAttribute() noexcept;
	virtual ~GraphicsAttribute() noexcept;

	virtual GraphicsFormat getType() const noexcept = 0;
	virtual const std::string& getSemantic() const noexcept = 0;
	virtual std::uint32_t getSemanticIndex() const noexcept = 0;

private:
	GraphicsAttribute(const GraphicsAttribute&) noexcept = delete;
	GraphicsAttribute& operator=(const GraphicsAttribute&) noexcept = delete;
};

class EXPORT GraphicsParam : public rtti::Interface
{
	__DeclareSubInterface(GraphicsParam, rtti::Interface)
public:
	GraphicsParam() noexcept;
	virtual ~GraphicsParam() noexcept;

	virtual const std::string& getName() const noexcept = 0;
	virtual GraphicsUniformType getType() const noexcept = 0;
	virtual GraphicsShaderStageFlags getShaderStageFlags() const noexcept = 0;
	virtual std::uint32_t getBindingPoint() const noexcept = 0;

private:
	GraphicsParam(const GraphicsParam&) noexcept = delete;
	GraphicsParam& operator=(const GraphicsParam&) noexcept = delete;
};

class EXPORT GraphicsUniform : public GraphicsParam
{
	__DeclareSubInterface(GraphicsUniform, GraphicsParam)
public:
	GraphicsUniform() noexcept;
	virtual ~GraphicsUniform() noexcept;

	virtual std::uint32_t getOffset() const noexcept = 0;
	virtual const std::string& getSamplerName() const noexcept = 0;

private:
	GraphicsUniform(const GraphicsUniform&) noexcept = delete;
	GraphicsUniform& operator=(const GraphicsUniform&) noexcept = delete;
};

class EXPORT GraphicsUniformBlock : public GraphicsParam
{
	__DeclareSubInterface(GraphicsUniformBlock, GraphicsParam)
public:
	GraphicsUniformBlock() noexcept;
	virtual ~GraphicsUniformBlock() noexcept;

	virtual std::uint32_t getBlockSize() const noexcept = 0;
	virtual const GraphicsUniforms& getGraphicsUniforms() const noexcept = 0;

private:
	GraphicsUniformBlock(const GraphicsUniformBlock&) noexcept = delete;
	GraphicsUniformBlock& operator=(const GraphicsUniformBlock&) noexcept = delete;
};

class EXPORT GraphicsShader : public GraphicsChild
{
	__DeclareSubInterface(GraphicsShader, GraphicsChild)
public:
	GraphicsShader() noexcept;
	virtual ~GraphicsShader() noexcept;

	virtual const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept = 0;

private:
	GraphicsShader(const GraphicsShader&) noexcept = delete;
	GraphicsShader& operator=(const GraphicsShader&) noexcept = delete;
};

class EXPORT GraphicsProgram : public GraphicsChild
{
	__DeclareSubInterface(GraphicsProgram, GraphicsChild)
public:
	GraphicsProgram() noexcept;
	virtual ~GraphicsProgram() noexcept;

	virtual const GraphicsParams& getActiveParams() const noexcept = 0;
	virtual const GraphicsAttributes& getActiveAttributes() const noexcept = 0;

	virtual const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept = 0;

private:
	GraphicsProgram(const GraphicsProgram&) noexcept = delete;
	GraphicsProgram& operator=(const GraphicsProgram&) noexcept = delete;
};

_NAME_END

#endif