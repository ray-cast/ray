// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_MATERIAL_PASS_H_
#define _H_MATERIAL_PASS_H_

#include <ray/material_param.h>

_NAME_BEGIN

class EXPORT MaterialParamBinding final : public MaterialParamListener
{
public:
	MaterialParamBinding() noexcept;
	~MaterialParamBinding() noexcept;

	void setMaterialParam(MaterialParamPtr param) noexcept;
	const MaterialParamPtr& getMaterialParam() const noexcept;

	void setGraphicsUniformSet(GraphicsUniformSetPtr uniformSet) noexcept;
	const GraphicsUniformSetPtr& getGraphicsUniformSet() const noexcept;

	void uniform1b(bool value) noexcept;
	void uniform1i(std::int32_t i1) noexcept;
	void uniform2i(const int2& value) noexcept;
	void uniform2i(std::int32_t i1, std::int32_t i2) noexcept;
	void uniform3i(const int3& value) noexcept;
	void uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept;
	void uniform4i(const int4& value) noexcept;
	void uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept;
	void uniform1ui(std::uint32_t i1) noexcept;
	void uniform2ui(const uint2& value) noexcept;
	void uniform2ui(std::uint32_t i1, std::uint32_t i2) noexcept;
	void uniform3ui(const uint3& value) noexcept;
	void uniform3ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3) noexcept;
	void uniform4ui(const uint4& value) noexcept;
	void uniform4ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) noexcept;
	void uniform1f(float i1) noexcept;
	void uniform2f(const float2& value) noexcept;
	void uniform2f(float i1, float i2) noexcept;
	void uniform3f(const float3& value) noexcept;
	void uniform3f(float i1, float i2, float i3) noexcept;
	void uniform4f(const float4& value) noexcept;
	void uniform4f(float i1, float i2, float i3, float i4) noexcept;
	void uniform2fmat(const float* mat2) noexcept;
	void uniform2fmat(const float2x2& value) noexcept;
	void uniform3fmat(const float* mat3) noexcept;
	void uniform3fmat(const float3x3& value) noexcept;
	void uniform4fmat(const float* mat4) noexcept;
	void uniform4fmat(const float4x4& value) noexcept;
	void uniform1iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform2iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform3iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform4iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform1fv(std::size_t num, const float* str) noexcept;
	void uniform2fv(std::size_t num, const float* str) noexcept;
	void uniform3fv(std::size_t num, const float* str) noexcept;
	void uniform4fv(std::size_t num, const float* str) noexcept;
	void uniform2fmatv(std::size_t num, const float* mat2) noexcept;
	void uniform3fmatv(std::size_t num, const float* mat3) noexcept;
	void uniform4fmatv(std::size_t num, const float* mat4) noexcept;
	void uniform1iv(const std::vector<int1>& value) noexcept;
	void uniform2iv(const std::vector<int2>& value) noexcept;
	void uniform3iv(const std::vector<int3>& value) noexcept;
	void uniform4iv(const std::vector<int4>& value) noexcept;
	void uniform1uiv(const std::vector<uint1>& value) noexcept;
	void uniform2uiv(const std::vector<uint2>& value) noexcept;
	void uniform3uiv(const std::vector<uint3>& value) noexcept;
	void uniform4uiv(const std::vector<uint4>& value) noexcept;
	void uniform1fv(const std::vector<float1>& value) noexcept;
	void uniform2fv(const std::vector<float2>& value) noexcept;
	void uniform3fv(const std::vector<float3>& value) noexcept;
	void uniform4fv(const std::vector<float4>& value) noexcept;
	void uniform2fmatv(const std::vector<float2x2>& value) noexcept;
	void uniform3fmatv(const std::vector<float3x3>& value) noexcept;
	void uniform4fmatv(const std::vector<float4x4>& value) noexcept;
	void uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept;
	void uniformBuffer(GraphicsDataPtr ubo) noexcept;

private:
	MaterialParamBinding(const MaterialParamBinding&) = delete;
	MaterialParamBinding& operator=(const MaterialParamBinding&) = delete;

private:
	MaterialParamPtr _param;
	GraphicsUniformSetPtr _uniformSet;
};

class EXPORT MaterialSemanticBinding final
{
public:
	MaterialSemanticBinding() noexcept;
	~MaterialSemanticBinding() noexcept;

	void setSemanticType(GlobalSemanticType semanticType) noexcept;
	GlobalSemanticType getSemanticType() const noexcept;

	void setGraphicsUniformSet(GraphicsUniformSetPtr uniformSet) noexcept;
	const GraphicsUniformSetPtr& getGraphicsUniformSet() const noexcept;

private:
	GlobalSemanticType _semanticType;
	GraphicsUniformSetPtr _uniformSet;
};

class EXPORT MaterialPass final : public rtti::Interface
{
	__DeclareSubClass(MaterialPass, rtti::Interface)
public:
	MaterialPass() noexcept;
	~MaterialPass() noexcept;

	bool setup(Material& material) noexcept;
	void close() noexcept;

	void setName(const std::string& name) noexcept;
	void setName(std::string&& name) noexcept;
	const std::string& getName() const noexcept;

	void setGraphicsState(GraphicsStatePtr& state) noexcept;
	void setGraphicsProgram(GraphicsProgramPtr& program) noexcept;
	void setGraphicsInputLayout(GraphicsInputLayoutPtr& inputLayout) noexcept;
	void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr& pool) noexcept;
	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;

	void setGraphicsState(GraphicsStatePtr&& state) noexcept;
	void setGraphicsProgram(GraphicsProgramPtr&& program) noexcept;
	void setGraphicsInputLayout(GraphicsInputLayoutPtr&& inputLayout) noexcept;
	void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr&& pool) noexcept;
	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr&& descriptorSetLayout) noexcept;

	const GraphicsStatePtr& getGraphicsState() const noexcept;
	const GraphicsProgramPtr& getGraphicsProgram() const noexcept;
	const GraphicsInputLayoutPtr& getGraphicsInputLayout() const noexcept;
	const GraphicsDescriptorPoolPtr& getGraphicsDescriptorPool() const noexcept;
	const GraphicsDescriptorSetLayoutPtr& getGraphicsDescriptorSetLayout() const noexcept;
	const GraphicsPipelinePtr& getRenderPipeline() const noexcept;
	const GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

	void update(const MaterialSemanticManager& semanticManager) noexcept;

	MaterialPassPtr clone() const noexcept;

private:
	void updateSemantic(GraphicsUniformSet& uniform, const MaterialSemantic& semantic) noexcept;

private:
	MaterialPass(const MaterialPass&) = delete;
	MaterialPass& operator=(const MaterialPass&) = delete;

private:
	typedef std::vector<std::unique_ptr<MaterialParamBinding>> MaterialParamBindings;
	typedef std::vector<MaterialSemanticBinding> MaterialSemanticBindings;

	std::string _name;
	MaterialParamBindings _bindingParams;
	MaterialSemanticBindings _bindingSemantics;

	GraphicsStatePtr _state;
	GraphicsProgramPtr _program;
	GraphicsDescriptorSetPtr _descriptorSet;
	GraphicsDescriptorPoolPtr _descriptorPool;
	GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
	GraphicsInputLayoutPtr _inputLayout;
	GraphicsPipelinePtr _pipeline;
};

_NAME_END

#endif