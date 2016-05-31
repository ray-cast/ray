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
#ifndef _H_VK_SHADER_H_
#define _H_VK_SHADER_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanGraphicsAttribute final : public GraphicsAttribute
{
	__DeclareSubClass(VulkanGraphicsAttribute, GraphicsAttribute)
public:
	VulkanGraphicsAttribute() noexcept;
	~VulkanGraphicsAttribute() noexcept;

	void setSemantic(const std::string& semantic) noexcept;
	const std::string& getSemantic() const noexcept;

	void setSemanticIndex(std::uint32_t index) noexcept;
	std::uint32_t getSemanticIndex() const noexcept;

	void setType(GraphicsFormat type) noexcept;
	GraphicsFormat getType() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

private:
	VulkanGraphicsAttribute(const VulkanGraphicsAttribute&) noexcept = delete;
	VulkanGraphicsAttribute& operator=(const VulkanGraphicsAttribute&) noexcept = delete;

private:
	std::string _semantic;
	std::uint32_t _semanticIndex;
	std::uint32_t _bindingPoint;
	GraphicsFormat _type;
};

class VulkanGraphicsUniform final : public GraphicsUniform
{
	__DeclareSubClass(VulkanGraphicsUniform, GraphicsUniform)
public:
	VulkanGraphicsUniform() noexcept;
	~VulkanGraphicsUniform() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setSamplerName(const std::string& name) noexcept;
	const std::string& getSamplerName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setOffset(std::uint32_t offset) noexcept;
	std::uint32_t getOffset() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

	void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
	GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

private:
	VulkanGraphicsUniform(const VulkanGraphicsUniform&) noexcept = delete;
	VulkanGraphicsUniform& operator=(const VulkanGraphicsUniform&) noexcept = delete;

private:
	std::string _name;
	std::string _samplerName;
	std::uint32_t _offset;
	std::uint32_t _bindingPoint;
	GraphicsUniformType _type;
	GraphicsShaderStageFlags _stageFlags;
};

class VulkanGraphicsUniformBlock final : public GraphicsUniformBlock
{
	__DeclareSubClass(VulkanGraphicsUniformBlock, GraphicsUniformBlock)
public:
	VulkanGraphicsUniformBlock() noexcept;
	~VulkanGraphicsUniformBlock() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setBlockSize(std::uint32_t size) noexcept;
	std::uint32_t getBlockSize() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

	void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
	GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

	void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	const GraphicsUniforms& getGraphicsUniforms() const noexcept;

private:
	VulkanGraphicsUniformBlock(const VulkanGraphicsUniformBlock&) noexcept = delete;
	VulkanGraphicsUniformBlock& operator=(const VulkanGraphicsUniformBlock&) noexcept = delete;

private:
	std::string _name;
	std::uint32_t _size;
	std::uint32_t _bindingPoint;
	GraphicsUniforms _uniforms;
	GraphicsUniformType _type;
	GraphicsShaderStageFlags _stageFlags;
};

class VulkanShader final : public GraphicsShader
{
	__DeclareSubClass(VulkanShader, GraphicsShader)
public:
	VulkanShader() noexcept;
	virtual ~VulkanShader() noexcept;

	bool setup(const GraphicsShaderDesc& desc) noexcept;
	void close() noexcept;

	VkShaderModule getShaderModule() const noexcept;

	const GraphicsParams& getParams() const noexcept;
	const GraphicsAttributes& getAttributes() const noexcept;

	const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

private:
	bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out);
	bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);
	bool GLSLtoSPV(VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanShader(const VulkanShader&) noexcept = delete;
	VulkanShader& operator=(const VulkanShader&) noexcept = delete;

private:
	VkShaderModule _vkShader;
	GraphicsShaderDesc _shaderDesc;
	GraphicsParams _parameters;
	GraphicsAttributes _attributes;
	GraphicsDeviceWeakPtr _device;
};

class VulkanProgram final : public GraphicsProgram
{
	__DeclareSubClass(VulkanProgram, GraphicsProgram)
public:
	VulkanProgram() noexcept;
	virtual ~VulkanProgram() noexcept;

	bool setup(const GraphicsProgramDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsParams& getActiveParams() const noexcept;
	const GraphicsAttributes& getActiveAttributes() const noexcept;

	const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

private:
	void _initActiveAttribute(glslang::TProgram& program, const GraphicsProgramDesc& programDesc) noexcept;
	void _initActiveUniform(glslang::TProgram& program) noexcept;
	void _initActiveUniformBlock(glslang::TProgram& program, const GraphicsProgramDesc& programDesc) noexcept;

private:
	static GraphicsFormat toGraphicsFormat(int type) noexcept;
	static GraphicsUniformType toGraphicsUniformType(const std::string& name, int type, bool isArray) noexcept;

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanProgram(const VulkanProgram&) noexcept = delete;
	VulkanProgram& operator=(const VulkanProgram&) noexcept = delete;

private:
	GraphicsParams    _activeParams;
	GraphicsAttributes  _activeAttributes;

	GraphicsDeviceWeakPtr _device;
	GraphicsProgramDesc _programDesc;
};

_NAME_END

#endif