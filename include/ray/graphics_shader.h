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
#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT ShaderParameter : public rtti::Interface
{
	__DeclareSubInterface(ShaderParameter, rtti::Interface)
public:
	ShaderParameter() noexcept;
	virtual ~ShaderParameter() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

private:
	ShaderParameter(const ShaderParameter&) noexcept = delete;
	ShaderParameter& operator=(const ShaderParameter&) noexcept = delete;

private:
	std::string _name;
};

class EXPORT ShaderAttribute : public ShaderParameter
{
	__DeclareSubInterface(ShaderAttribute, ShaderParameter)
public:
	ShaderAttribute() noexcept;
	virtual ~ShaderAttribute() noexcept;

	void setSemantic(const std::string& semantic) noexcept;
	const std::string& getSemantic() const noexcept;

	void setSemanticIndex(std::uint8_t index) noexcept;
	std::uint8_t getSemanticIndex() const noexcept;

private:
	ShaderAttribute(const ShaderAttribute&) noexcept = delete;
	ShaderAttribute& operator=(const ShaderAttribute&) noexcept = delete;

private:
	std::uint8_t _index;
	std::string _semantic;
};

class EXPORT ShaderUniform : public ShaderParameter
{
	__DeclareSubInterface(ShaderUniform, ShaderParameter)
public:
	ShaderUniform() noexcept;
	virtual ~ShaderUniform() noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

private:
	ShaderUniform(const ShaderUniform&) noexcept = delete;
	ShaderUniform& operator=(const ShaderUniform&) noexcept = delete;

private:
	std::uint32_t _bindingPoint;
	GraphicsUniformType _type;
};

class EXPORT GraphicsShaderDesc final
{
public:
	GraphicsShaderDesc() noexcept;
	GraphicsShaderDesc(GraphicsShaderStage type, const std::vector<char>& code) noexcept;
	virtual ~GraphicsShaderDesc() noexcept;

	void setType(GraphicsShaderStage type) noexcept;
	GraphicsShaderStage getType() const noexcept;

	void setByteCodes(const std::vector<char>& source) noexcept;
	const std::vector<char>& getByteCodes() const noexcept;

private:
	GraphicsShaderStage _type;
	std::vector<char> _bytecodes;
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

	virtual ShaderUniforms& getActiveUniforms() noexcept = 0;
	virtual ShaderAttributes& getActiveAttributes() noexcept = 0;

	virtual const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept = 0;

private:
	GraphicsProgram(const GraphicsProgram&) noexcept = delete;
	GraphicsProgram& operator=(const GraphicsProgram&) noexcept = delete;
};

_NAME_END

#endif