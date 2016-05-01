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
#ifndef _H_EGL3_SHADER_H_
#define _H_EGL3_SHADER_H_

#include "egl3_types.h"

_NAME_BEGIN

class EGL3GraphicsAttribute final : public GraphicsAttribute
{
	__DeclareSubClass(EGL3GraphicsAttribute, GraphicsAttribute)
public:
	EGL3GraphicsAttribute() noexcept;
	~EGL3GraphicsAttribute() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsFormat type) noexcept;
	GraphicsFormat getType() const noexcept;

	void setSemantic(const std::string& semantic) noexcept;
	const std::string& getSemantic() const noexcept;

	void setSemanticIndex(std::uint8_t index) noexcept;
	std::uint8_t getSemanticIndex() const noexcept;

	void setBindingPoint(GLuint bindingPoint) noexcept;
	GLuint getBindingPoint() const noexcept;

private:
	EGL3GraphicsAttribute(const EGL3GraphicsAttribute&) noexcept = delete;
	EGL3GraphicsAttribute& operator=(const EGL3GraphicsAttribute&) noexcept = delete;

private:
	std::string _name;
	std::string _semantic;
	std::uint8_t _index;
	GLuint _bindingPoint;
	GraphicsFormat _type;
};

class EGL3GraphicsUniform final : public GraphicsUniform
{
	__DeclareSubClass(EGL3GraphicsUniform, GraphicsUniform)
public:
	EGL3GraphicsUniform() noexcept;
	~EGL3GraphicsUniform() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setOffset(std::uint32_t offset) noexcept;
	std::uint32_t getOffset() const noexcept;

	void setBindingPoint(GLuint bindingPoint) noexcept;
	GLuint getBindingPoint() const noexcept;

private:
	EGL3GraphicsUniform(const EGL3GraphicsUniform&) noexcept = delete;
	EGL3GraphicsUniform& operator=(const EGL3GraphicsUniform&) noexcept = delete;

private:
	std::string _name;
	std::uint32_t _offset;
	GLuint _bindingPoint;
	GraphicsUniformType _type;
};

class EGL3GraphicsUniformBlock final : public GraphicsUniformBlock
{
	__DeclareSubClass(EGL3GraphicsUniformBlock, GraphicsUniformBlock)
public:
	EGL3GraphicsUniformBlock() noexcept;
	~EGL3GraphicsUniformBlock() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setBlockSize(std::uint32_t size) noexcept;
	std::uint32_t getBlockSize() const noexcept;

	void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	const GraphicsUniforms& getGraphicsUniforms() const noexcept;

	void setBindingPoint(GLuint bindingPoint) noexcept;
	GLuint getBindingPoint() const noexcept;

private:
	EGL3GraphicsUniformBlock(const EGL3GraphicsUniformBlock&) noexcept = delete;
	EGL3GraphicsUniformBlock& operator=(const EGL3GraphicsUniformBlock&) noexcept = delete;

private:
	std::string _name;
	std::uint32_t _size;
	GLuint _bindingPoint;
	GraphicsUniforms _uniforms;
	GraphicsUniformType _type;
};

class EGL3Shader final : public GraphicsShader
{
	__DeclareSubClass(EGL3Shader, GraphicsShader)
public:
	EGL3Shader() noexcept;
	~EGL3Shader() noexcept;

	bool setup(const GraphicsShaderDesc& desc) noexcept;
	void close() noexcept;

	GLuint getInstanceID() const noexcept;

	const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

private:
	static bool HlslByteCodes2GLSL(GraphicsShaderStage stage, const char* codes, std::string& out);

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3Shader(const EGL3Shader&) noexcept = delete;
	EGL3Shader& operator=(const EGL3Shader&) noexcept = delete;

private:
	GLuint _instance;
	GraphicsShaderDesc _shaderDesc;
	GraphicsDeviceWeakPtr _device;
};

class EGL3Program final : public GraphicsProgram
{
	__DeclareSubClass(EGL3Program, GraphicsProgram)
public:
	EGL3Program() noexcept;
	~EGL3Program() noexcept;

	bool setup(const GraphicsProgramDesc& desc) noexcept;
	void close() noexcept;

	void apply() noexcept;

	GLuint getInstanceID() const noexcept;

	const GraphicsUniforms& getActiveUniforms() const noexcept;
	const GraphicsUniformBlocks& getActiveUniformBlocks() const noexcept;
	const GraphicsAttributes& getActiveAttributes() const noexcept;

	const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

private:
	void _initActiveAttribute() noexcept;
	void _initActiveUniform() noexcept;
	void _initActiveUniformBlock() noexcept;

private:
	static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
	static GraphicsUniformType toGraphicsUniformType(const std::string& name, GLenum type) noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3Program(const EGL3Program&) noexcept = delete;
	EGL3Program& operator=(const EGL3Program&) noexcept = delete;

private:

	bool _isActive;

	GLuint _program;

	GraphicsUniforms    _activeUniforms;
	GraphicsUniformBlocks _activeUniformBlocks;
	GraphicsAttributes  _activeAttributes;

	GraphicsDeviceWeakPtr _device;
	GraphicsProgramDesc _programDesc;
};

_NAME_END

#endif