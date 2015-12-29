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

	GLint _location;

	ShaderVariantType _type;
	ShaderVariants _params;
};

class EGL2ShaderAttribute final : public ShaderAttribute
{
	__DeclareSubClass(OGLShaderAttribute, ShaderAttribute)
public:
	EGL2ShaderAttribute() noexcept;
	~EGL2ShaderAttribute() noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

private:
	GLint  _location;
};

class EGL2ShaderUniform final : public ShaderUniform
{
	__DeclareSubClass(EGL2ShaderUniform, ShaderUniform)
public:
	EGL2ShaderUniform() noexcept;
	~EGL2ShaderUniform() noexcept;

	void setName(const std::string& name) noexcept;
	void setType(ShaderVariantType type) noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

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

	GraphicsTexturePtr getTexture() const noexcept;
	GraphicsSamplerPtr getSampler() const noexcept;

private:
	EGL2ShaderUniform(const EGL2ShaderUniform&) noexcept = delete;
	EGL2ShaderUniform& operator=(const EGL2ShaderUniform&) noexcept = delete;

private:
	EGL2ShaderVariant _value;
};

class EGL2Shader final : public GraphicsShader
{
	__DeclareSubClass(EGL2Shader, GraphicsShader)
public:
	EGL2Shader() noexcept;
	~EGL2Shader() noexcept;

	bool setup(const ShaderDesc& desc) noexcept;
	void close() noexcept;

	GLuint getInstanceID() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2Shader(const EGL2Shader&) noexcept = delete;
	EGL2Shader& operator=(const EGL2Shader&) noexcept = delete;

private:
	GLuint _instance;
	GraphicsDeviceWeakPtr _device;
};

class EGL2ShaderObject final : public GraphicsProgram
{
	__DeclareSubClass(EGL2ShaderObject, GraphicsProgram)
public:
	EGL2ShaderObject() noexcept;
	~EGL2ShaderObject() noexcept;

	bool setup(const ShaderObjectDesc& desc) noexcept;
	void close() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() noexcept;

	GLuint getInstanceID() const noexcept;

	ShaderUniforms& getActiveUniforms() noexcept;
	ShaderAttributes& getActiveAttributes() noexcept;

private:
	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;
	void _updateShaderUniform(ShaderUniformPtr it) noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2ShaderObject(const EGL2ShaderObject&) noexcept = delete;
	EGL2ShaderObject& operator=(const EGL2ShaderObject&) noexcept = delete;

private:

	bool _isActive;

	GLuint _program;

	EGL2Shaders _shaders;

	ShaderUniforms    _activeUniforms;
	ShaderAttributes  _activeAttributes;

	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif