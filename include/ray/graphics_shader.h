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
#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT ShaderVariant
{
public:
	ShaderVariant() noexcept;
	virtual ~ShaderVariant() noexcept;

	virtual void assign(bool value) noexcept = 0;
	virtual void assign(int value) noexcept = 0;
	virtual void assign(const int2& value) noexcept = 0;
	virtual void assign(float value) noexcept = 0;
	virtual void assign(const float2& value) noexcept = 0;
	virtual void assign(const float3& value) noexcept = 0;
	virtual void assign(const float4& value) noexcept = 0;
	virtual void assign(const float3x3& value) noexcept = 0;
	virtual void assign(const float4x4& value) noexcept = 0;
	virtual void assign(const std::vector<float>& value) noexcept = 0;
	virtual void assign(const std::vector<float2>& value) noexcept = 0;
	virtual void assign(const std::vector<float3>& value) noexcept = 0;
	virtual void assign(const std::vector<float4>& value) noexcept = 0;

private:
	ShaderVariant(const ShaderVariant&) noexcept = delete;
	ShaderVariant& operator=(const ShaderVariant&) noexcept = delete;
};

class EXPORT ShaderParameter
{
public:
	ShaderParameter() noexcept;
	virtual ~ShaderParameter() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void needUpdate(bool update) noexcept;
	bool needUpdate() const noexcept;

private:
	std::string _name;

	bool _needUpdate;
};

class EXPORT ShaderAttribute : public ShaderParameter
{
};

class EXPORT ShaderUniform : public ShaderParameter
{
public:
	ShaderUniform(ShaderVariant* value) noexcept;
	virtual ~ShaderUniform() noexcept;

	ShaderVariantType getType() const noexcept;

	void assign(bool value) noexcept;
	void assign(int value) noexcept;
	void assign(const int2& value) noexcept;
	void assign(float value) noexcept;
	void assign(const float2& value) noexcept;
	void assign(const float3& value) noexcept;
	void assign(const float4& value) noexcept;
	void assign(const float3x3& value) noexcept;
	void assign(const float4x4& value) noexcept;
	void assign(const std::vector<float>& value) noexcept;
	void assign(const std::vector<float2>& value) noexcept;
	void assign(const std::vector<float3>& value) noexcept;
	void assign(const std::vector<float4>& value) noexcept;

protected:
	void setType(ShaderVariantType type) noexcept;
	
private:
	ShaderUniform(const ShaderUniform&) noexcept = delete;
	ShaderUniform& operator=(const ShaderUniform&) noexcept = delete;

private:
	ShaderVariantType _type;
	ShaderVariant* _value;
};

class EXPORT ShaderDesc final
{
public:
	ShaderDesc() noexcept;
	ShaderDesc(ShaderType type, const std::string& code) noexcept;
	virtual ~ShaderDesc() noexcept;

	virtual void setType(ShaderType type) noexcept;
	virtual ShaderType  getType() const noexcept;

	virtual void setSource(const std::string& source) noexcept;
	virtual const std::string& getSource() const noexcept;

private:
	ShaderType _type;
	std::string _source;
};

class EXPORT ShaderObjectDesc final
{
public:
	ShaderObjectDesc() noexcept;
	virtual ~ShaderObjectDesc() noexcept;

	void addShader(const ShaderDesc& shader) noexcept;
	void removeShader(ShaderType type) noexcept;

	const ShadersDesc& getShaders() const noexcept;

private:

	ShadersDesc _shaders;
};

class EXPORT GraphicsShader : public GraphicsChild
{
	__DeclareSubInterface(GraphicsShader, GraphicsChild)
public:
	GraphicsShader() noexcept;
	virtual ~GraphicsShader() noexcept;

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

private:
	GraphicsProgram(const GraphicsProgram&) noexcept = delete;
	GraphicsProgram& operator=(const GraphicsProgram&) noexcept = delete;
};

_NAME_END

#endif