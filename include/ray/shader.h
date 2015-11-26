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

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT ShaderVariant
{
public:
	ShaderVariant() noexcept;
	ShaderVariant(const std::string& name, float value) noexcept;
	ShaderVariant(const std::string& name, const float3& value) noexcept;
	ShaderVariant(const std::string& name, const float4& value) noexcept;
	ShaderVariant(const std::string& name, const float4x4& value) noexcept;
	ShaderVariant(const std::string& name, ShaderVariantType type) noexcept;
	~ShaderVariant() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(ShaderVariantType type) noexcept;
	ShaderVariantType getType() const noexcept;

	void addParameter(ShaderVariantPtr arg) noexcept;
	void removeParameter(ShaderVariantPtr arg) noexcept;
	ShaderVariantPtr getParameter(const std::string& name) const noexcept;
	const ShaderVariants& getParameters() const noexcept;

	std::size_t getSize() const noexcept;

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
	void assign(TexturePtr texture, TextureSamplePtr sample = nullptr) noexcept;

	bool getBool() const noexcept;
	int getInt() const noexcept;
	const int2& getInt2() const noexcept;
	float getFloat() const noexcept;
	const float2& getFloat2() const noexcept;
	const float3& getFloat3() const noexcept;
	const float4& getFloat4() const noexcept;
	const float3x3& getFloat3x3() const noexcept;
	const float4x4& getFloat4x4() const noexcept;
	const std::vector<float>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;
	const std::vector<float3>& getFloat3Array() const noexcept;
	const std::vector<float4>& getFloat4Array() const noexcept;
	TexturePtr getTexture() const noexcept;
	TextureSamplePtr getTextureSample() const noexcept;

protected:

	virtual void onChangeBefore() noexcept;
	virtual void onChangeAfter() noexcept;

private:

	std::string _name;

	TexturePtr _texture;
	TextureSamplePtr _textureSample;

	union
	{
		bool b;
		int i[4];
		float f[4];
		float3x3* m3;
		float4x4* m4;
		std::vector<float>* farray;
		std::vector<float2>* farray2;
		std::vector<float3>* farray3;
		std::vector<float4>* farray4;
	} _value;

	ShaderVariantType _type;

	ShaderVariants _params;
};

class EXPORT ShaderParameter
{
public:
	ShaderParameter() noexcept;
	virtual ~ShaderParameter() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setLocation(std::size_t location) noexcept;
	std::size_t getLocation() const noexcept;

	void setBindingPoint(std::size_t unit) noexcept;
	std::size_t getBindingPoint() const noexcept;

	void setBindingProgram(std::size_t program) noexcept;
	std::size_t getBindingProgram() const noexcept;

	void needUpdate(bool update) noexcept;
	bool needUpdate() const noexcept;

private:
	bool _needUpdate;

	std::string _name;
	std::size_t _location;
	std::size_t _bindingPoint;
	std::size_t _bindingProgram;
};

class EXPORT ShaderAttribute final : public ShaderParameter
{
};

class EXPORT ShaderSubroutine final : public ShaderParameter
{
public:
};

class EXPORT ShaderUniform final : public ShaderParameter
{
public:
	ShaderUniform() noexcept;
	~ShaderUniform() noexcept;

	void setType(ShaderVariantType buffer) noexcept;
	ShaderVariantType getType() const noexcept;

	void setValue(ShaderVariantPtr buffer) noexcept;
	ShaderVariantPtr getValue() const noexcept;

public:

	ShaderVariantType _type;
	ShaderVariantPtr _value;
};

class EXPORT Shader
{
public:
	Shader() noexcept;
	Shader(ShaderType type, const std::string& code) noexcept;
	Shader(const std::string& type, const std::string& code) noexcept;
	Shader(const std::wstring& type, const std::string& code) noexcept;
	virtual ~Shader() noexcept;

	virtual bool setup() except = 0;
	virtual void close() noexcept = 0;

	virtual void setType(ShaderType type) noexcept;
	virtual void setType(const std::string& type) noexcept;
	virtual void setType(const std::wstring& type) noexcept;
	virtual ShaderType  getType() const noexcept;

	virtual void setSource(const std::string& source) noexcept;
	virtual const std::string& getSource() const noexcept;

	virtual std::size_t getInstanceID() const noexcept = 0;

private:
	Shader(const Shader&) noexcept = delete;
	Shader& operator=(const Shader&) noexcept = delete;

private:

	ShaderType _type;
	std::string _source;
};

class EXPORT ShaderObject
{
public:
	ShaderObject() noexcept;
	virtual ~ShaderObject() noexcept;

	virtual bool setup() except = 0;
	virtual void close() noexcept = 0;

	virtual void addShader(ShaderPtr shader) except = 0;
	virtual void removeShader(ShaderPtr shader) noexcept = 0;

	virtual Shaders& getShaders() noexcept = 0;

	virtual std::size_t getInstanceID() noexcept = 0;

	virtual ShaderAttributes&  getActiveAttributes() noexcept = 0;
	virtual ShaderUniforms&    getActiveUniforms() noexcept = 0;
	virtual ShaderSubroutines& getActiveSubroutines() noexcept = 0;
};

_NAME_END

#endif