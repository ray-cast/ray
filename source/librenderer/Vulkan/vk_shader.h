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

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setSemantic(const std::string& semantic) noexcept;
	const std::string& getSemantic() const noexcept;

	void setSemanticIndex(std::uint8_t index) noexcept;
	std::uint8_t getSemanticIndex() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

private:
	VulkanGraphicsAttribute(const VulkanGraphicsAttribute&) noexcept = delete;
	VulkanGraphicsAttribute& operator=(const VulkanGraphicsAttribute&) noexcept = delete;

private:
	std::string _name;
	std::string _semantic;
	std::uint8_t _index;
	std::uint32_t _bindingPoint;
	GraphicsUniformType _type;
};

class VulkanGraphicsUniform final : public GraphicsUniform
{
	__DeclareSubClass(VulkanGraphicsUniform, GraphicsUniform)
public:
	VulkanGraphicsUniform() noexcept;
	~VulkanGraphicsUniform() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setOffset(std::uint32_t offset) noexcept;
	std::uint32_t getOffset() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

private:
	VulkanGraphicsUniform(const VulkanGraphicsUniform&) noexcept = delete;
	VulkanGraphicsUniform& operator=(const VulkanGraphicsUniform&) noexcept = delete;

private:
	std::string _name;
	std::uint32_t _offset;
	std::uint32_t _bindingPoint;
	GraphicsUniformType _type;
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

	void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	const GraphicsUniforms& getGraphicsUniforms() const noexcept;

	void setBindingPoint(std::uint32_t bindingPoint) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

private:
	VulkanGraphicsUniformBlock(const VulkanGraphicsUniformBlock&) noexcept = delete;
	VulkanGraphicsUniformBlock& operator=(const VulkanGraphicsUniformBlock&) noexcept = delete;

private:
	std::string _name;
	std::uint32_t _size;
	std::uint32_t _bindingPoint;
	GraphicsUniforms _uniforms;
	GraphicsUniformType _type;
};

class VulkanShader final : public GraphicsShader
{
	__DeclareSubClass(VulkanShader, GraphicsShader)
public:
	VulkanShader() noexcept;
	virtual ~VulkanShader() noexcept;

	bool setup(const GraphicsShaderDesc& desc) noexcept;
	void close() noexcept;

	const std::string& getGlslCodes() const noexcept;

	VkShaderModule getShaderModule() const noexcept;

	const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

private:
	bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanShader(const VulkanShader&) noexcept = delete;
	VulkanShader& operator=(const VulkanShader&) noexcept = delete;

private:
	VkShaderModule _vkShader;
	std::string _glsl;
	GraphicsShaderDesc _shaderDesc;
	GraphicsDeviceWeakPtr _device;
};

class VulkanShaderObject final : public GraphicsProgram
{
	__DeclareSubClass(VulkanShaderObject, GraphicsProgram)
public:
	VulkanShaderObject() noexcept;
	virtual ~VulkanShaderObject() noexcept;

	bool setup(const GraphicsProgramDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsUniforms& getActiveUniforms() const noexcept;
	const GraphicsUniformBlocks& getActiveUniformBlocks() const noexcept;
	const GraphicsAttributes& getActiveAttributes() const noexcept;

	const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

private:
	static GraphicsUniformType toGraphicsUniformType(const std::string& name, int type) noexcept;

private:
	friend class VulkanDevice;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	VulkanShaderObject(const VulkanShaderObject&) noexcept = delete;
	VulkanShaderObject& operator=(const VulkanShaderObject&) noexcept = delete;

private:
	GraphicsUniforms    _activeUniforms;
	GraphicsUniformBlocks _activeUniformBlocks;
	GraphicsAttributes  _activeAttributes;

	GraphicsDeviceWeakPtr _device;
	GraphicsProgramDesc _programDesc;
};

_NAME_END

#endif