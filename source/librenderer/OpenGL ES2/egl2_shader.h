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
#ifndef _H_EGL2_SHADER_H_
#define _H_EGL2_SHADER_H_

#include "egl2_types.h"

_NAME_BEGIN

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

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;

private:
	EGL2ShaderUniform(const EGL2ShaderUniform&) noexcept = delete;
	EGL2ShaderUniform& operator=(const EGL2ShaderUniform&) noexcept = delete;

private:
	GLuint _program;
};

class EGL2Shader final : public GraphicsShader
{
	__DeclareSubClass(EGL2Shader, GraphicsShader)
public:
	EGL2Shader() noexcept;
	~EGL2Shader() noexcept;

	bool setup(const GraphicsShaderDesc& desc) noexcept;
	void close() noexcept;

	GLuint getInstanceID() const noexcept;

	const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

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
	GraphicsShaderDesc _shaderDesc;
};

class EGL2ShaderObject final : public GraphicsProgram
{
	__DeclareSubClass(EGL2ShaderObject, GraphicsProgram)
public:
	EGL2ShaderObject() noexcept;
	~EGL2ShaderObject() noexcept;

	bool setup(const GraphicsProgramDesc& desc) noexcept;
	void close() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() noexcept;

	GLuint getInstanceID() const noexcept;

	ShaderUniforms& getActiveUniforms() noexcept;
	ShaderAttributes& getActiveAttributes() noexcept;

	const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

private:
	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;

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
	GraphicsProgramDesc _programDesc;
};

_NAME_END

#endif