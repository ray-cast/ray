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
#ifndef _H_EGL2_SHADER_H_
#define _H_EGL2_SHADER_H_

#include "egl2_canvas.h"

_NAME_BEGIN

class EGL2ShaderVariant final : public ShaderVariant
{
public:
	EGL2ShaderVariant() noexcept;
	virtual ~EGL2ShaderVariant() noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;

	void setType(ShaderVariantType type) noexcept;
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

	bool getBool() const noexcept;
	int getInt() const noexcept;
	float getFloat() const noexcept;
	const int2& getInt2() const noexcept;
	const float2& getFloat2() const noexcept;
	const float3& getFloat3() const noexcept;
	const float4& getFloat4() const noexcept;
	const float3x3& getFloat3x3() const noexcept;
	const float4x4& getFloat4x4() const noexcept;
	const std::vector<float>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;
	
private:
	EGL2ShaderVariant(const EGL2ShaderVariant&) noexcept = delete;
	EGL2ShaderVariant& operator=(const EGL2ShaderVariant&) noexcept = delete;

private:

	std::string _name;

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

	GLint _location;
	GLuint _bindingProgram;
};

class EGLShaderUniform final : public ShaderUniform
{
public:
	EGLShaderUniform() noexcept;
	~EGLShaderUniform() noexcept;

	void setName(const std::string& name) noexcept;
	void setType(ShaderVariantType type) noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;

	bool getBool() const noexcept;
	int getInt() const noexcept;
	float getFloat() const noexcept;
	const int2& getInt2() const noexcept;
	const float2& getFloat2() const noexcept;
	const float3& getFloat3() const noexcept;
	const float4& getFloat4() const noexcept;
	const float3x3& getFloat3x3() const noexcept;
	const float4x4& getFloat4x4() const noexcept;
	const std::vector<float>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;

	TexturePtr getTexture() const noexcept;
	SamplerObjectPtr getSampler() const noexcept;

private:
	EGLShaderUniform(const EGLShaderUniform&) noexcept = delete;
	EGLShaderUniform& operator=(const EGLShaderUniform&) noexcept = delete;

private:
	EGL2ShaderVariant _value;
};

class EGL2Shader final : public Shader
{
public:
	EGL2Shader() noexcept;
	~EGL2Shader() noexcept;

	virtual bool setup() except;
	virtual void close() noexcept;

	virtual std::size_t getInstanceID() const noexcept;

private:
	GLuint _instance;
};

class EGL2ShaderObject final : public ShaderObject
{
public:
	EGL2ShaderObject() noexcept;
	~EGL2ShaderObject() noexcept;

	bool setup() except;
	void close() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() noexcept;

	void addShader(ShaderPtr shader) noexcept;
	void removeShader(ShaderPtr shader) noexcept;

	const Shaders& getShaders() const noexcept;

	std::size_t getInstanceID() noexcept;

	ShaderUniforms& getActiveUniforms() noexcept;
	ShaderAttributes& getActiveAttributes() noexcept;

private:
	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;

	void _updateShaderUniform(ShaderUniformPtr uniform) noexcept;

private:

	bool _isActive;

	GLuint _program;

	Shaders _shaders;

	ShaderUniforms    _activeUniforms;
	ShaderAttributes  _activeAttributes;
};

_NAME_END

#endif