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
#ifndef _H_EGL3_SHADER_H_
#define _H_EGL3_SHADER_H_

#include "egl3_canvas.h"

_NAME_BEGIN

class EGL3ShaderVariant final : public ShaderVariant
{
public:
	EGL3ShaderVariant() noexcept;
	virtual ~EGL3ShaderVariant() noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;

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

private:
	EGL3ShaderVariant(const EGL3ShaderVariant&) noexcept = delete;
	EGL3ShaderVariant& operator=(const EGL3ShaderVariant&) noexcept = delete;

private:
	GLint _location;
	GLuint _bindingProgram;
};

class EGL3ShaderAttribute final : public ShaderAttribute
{
	__DeclareSubClass(OGLShaderAttribute, ShaderAttribute)
public:
	EGL3ShaderAttribute() noexcept;
	~EGL3ShaderAttribute() noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

private:
	GLint  _location;
};

class EGL3ShaderUniform final : public ShaderUniform
{
	__DeclareSubClass(EGL3ShaderUniform, ShaderUniform)
public:
	EGL3ShaderUniform() noexcept;
	~EGL3ShaderUniform() noexcept;

	void setName(const std::string& name) noexcept;
	void setType(ShaderVariantType type) noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;
private:
	EGL3ShaderUniform(const EGL3ShaderUniform&) noexcept = delete;
	EGL3ShaderUniform& operator=(const EGL3ShaderUniform&) noexcept = delete;

private:
	EGL3ShaderVariant _value;
};

class EGL3Shader final : public GraphicsShader
{
	__DeclareSubClass(EGL3Shader, GraphicsShader)
public:
	EGL3Shader() noexcept;
	~EGL3Shader() noexcept;

	bool setup(const ShaderDesc& desc) noexcept;
	void close() noexcept;

	GLuint getInstanceID() const noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3Shader(const EGL3Shader&) noexcept = delete;
	EGL3Shader& operator=(const EGL3Shader&) noexcept = delete;

private:
	GLuint _instance;
	GraphicsDeviceWeakPtr _device;
};

class EGL3ShaderObject final : public GraphicsProgram
{
	__DeclareSubClass(EGL3ShaderObject, GraphicsProgram)
public:
	EGL3ShaderObject() noexcept;
	~EGL3ShaderObject() noexcept;

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
	void _initActiveUniformBlock() noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3ShaderObject(const EGL3ShaderObject&) noexcept = delete;
	EGL3ShaderObject& operator=(const EGL3ShaderObject&) noexcept = delete;

private:

	bool _isActive;

	GLuint _program;

	EGL3Shaders _shaders;

	ShaderUniforms    _activeUniforms;
	ShaderAttributes  _activeAttributes;

	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif