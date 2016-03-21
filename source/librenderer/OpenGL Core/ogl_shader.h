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
#ifndef _H_OGL_SHADER_H_
#define _H_OGL_SHADER_H_

#include "ogl_canvas.h"

_NAME_BEGIN

class OGLShaderAttribute final : public ShaderAttribute
{
	__DeclareSubClass(OGLShaderAttribute, ShaderAttribute)
public:
	OGLShaderAttribute() noexcept;
	~OGLShaderAttribute() noexcept;

	void setLocation(GLint location) noexcept;
	GLint getLocation() const noexcept;

private:
	GLint  _location;
};

class OGLShaderUniform final : public ShaderUniform
{
	__DeclareSubClass(OGLShaderUniform, ShaderUniform)
public:
	OGLShaderUniform() noexcept;
	~OGLShaderUniform() noexcept;

	void setBindingProgram(GLuint program) noexcept;
	GLuint getBindingProgram() const noexcept;

private:
	OGLShaderUniform(const OGLShaderUniform&) noexcept = delete;
	OGLShaderUniform& operator=(const OGLShaderUniform&) noexcept = delete;

private:
	GLuint _program;
};

class OGLShader final : public GraphicsShader
{
	__DeclareSubClass(OGLShader, GraphicsShader)
public:
	OGLShader() noexcept;
	~OGLShader() noexcept;

	bool setup(const GraphicsShaderDesc& shader) noexcept;
	void close() noexcept;

	GLuint getInstanceID() const noexcept;

	const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

private:
	friend class OGLDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	OGLShader(const OGLShader&) noexcept = delete;
	OGLShader& operator=(const OGLShader&) noexcept = delete;

private:
	GLuint _instance;

	GraphicsShaderDesc _shaderDesc;
	GraphicsDeviceWeakPtr _device;
};

class OGLShaderObject final : public GraphicsProgram
{
	__DeclareSubClass(OGLShaderObject, GraphicsProgram)
public:
	OGLShaderObject() noexcept;
	~OGLShaderObject() noexcept;

	bool setup(const GraphicsProgramDesc& program) noexcept;
	void close() noexcept;

	GLuint getInstanceID() noexcept;

	ShaderUniforms& getActiveUniforms() noexcept;
	ShaderAttributes& getActiveAttributes() noexcept;

	const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

private:
	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;
	void _initActiveUniformBlock() noexcept;

private:
	friend class OGLDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	OGLShaderObject(const OGLShaderObject&) noexcept = delete;
	OGLShaderObject& operator=(const OGLShaderObject&) noexcept = delete;

private:
	GLuint _program;
	ShaderUniforms _activeUniforms;
	ShaderAttributes  _activeAttributes;
	GraphicsProgramDesc _programDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif