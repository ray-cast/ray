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

#include "egl_canvas.h"

_NAME_BEGIN

class EGL3Shader final : public Shader
{
public:
	EGL3Shader() noexcept;
	~EGL3Shader() noexcept;

	virtual bool setup() except;
	virtual void close() noexcept;

	virtual std::size_t getInstanceID() const noexcept;

private:

	GLuint _instance;
};

class EGL3ShaderObject final : public ShaderObject
{
public:
	EGL3ShaderObject() noexcept;
	~EGL3ShaderObject() noexcept;

	virtual bool setup() except;
	virtual void close() noexcept;

	virtual void addShader(ShaderPtr shader) noexcept;
	virtual void removeShader(ShaderPtr shader) noexcept;

	virtual Shaders& getShaders() noexcept;

	virtual std::size_t getInstanceID() noexcept;

	virtual ShaderAttributes&  getActiveAttributes() noexcept;
	virtual ShaderUniforms&    getActiveUniforms() noexcept;

private:

	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;
	void _initActiveUniformBlock() noexcept;

private:

	GLuint _program;

	Shaders _shaders;

	ShaderUniforms    _activeUniforms;
	ShaderAttributes  _activeAttributes;
};

_NAME_END

#endif