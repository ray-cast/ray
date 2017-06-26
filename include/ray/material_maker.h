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
#ifndef _H_MATERIAL_MAKETER_H_
#define _H_MATERIAL_MAKETER_H_

#include <ray/material_loader.h>
#include <ray/xmlreader.h>

_NAME_BEGIN

class EXPORT MaterialMaker final : public MaterialLoader
{
public:
	MaterialMaker() noexcept;
	~MaterialMaker() noexcept;

	bool doCanRead(StreamReader& stream) const noexcept;

	bool load(MaterialManager& manager, Material& material, const std::string& filename) noexcept;
	bool load(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	bool load(MaterialManager& manager, Material& material, StreamReader& stream) noexcept;

private:
	bool loadMaterial(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	bool loadEffect(MaterialManager& manager, Material& material, ixmlarchive& reader) except;

private:
	bool instanceInclude(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instancePass(MaterialManager& manager, Material& material, MaterialTechPtr& tech, ixmlarchive& reader) except;
	void instanceTech(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instanceSampler(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instanceParameter(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instanceMacro(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instanceBuffer(MaterialManager& manager, Material& material, ixmlarchive& reader) except;
	void instanceCodes(MaterialManager& manager, ixmlarchive& reader) except;
	void instanceShader(MaterialManager& manager, Material& material, GraphicsProgramDesc& programDesc, ixmlarchive& reader) except;
	void instanceInputLayout(MaterialManager& manager, Material& material, ixmlarchive& reader) except;

	static bool GetShaderStage(const std::string& string, GraphicsShaderStageFlagBits& flags) noexcept;
	static bool GetUniformType(const std::string& string, GraphicsUniformType& type) noexcept;
	static bool GetPrimitive(const std::string& string, GraphicsVertexType& primitive) noexcept;
	static bool GetCullMode(const std::string& string, GraphicsCullMode& cullmode) noexcept;
	static bool GetFillMode(const std::string& string, GraphicsPolygonMode& fillmode) noexcept;
	static bool GetBlendOperation(const std::string& string, GraphicsBlendOp& blendop) noexcept;
	static bool GetBlendFactor(const std::string& string, GraphicsBlendFactor& factor) noexcept;
	static bool GetColorMask(const std::string& string, GraphicsColorMaskFlags& mask) noexcept;
	static bool GetCompareFunc(const std::string& string, GraphicsCompareFunc& func) noexcept;
	static bool GetStencilOp(const std::string& string, GraphicsStencilOp& stencilop) noexcept;
	static bool GetFormat(const std::string& string, GraphicsFormat& format) noexcept;
	static bool GetSemanticType(const std::string& string, GlobalSemanticType& type) noexcept;

private:
	MaterialMaker(const MaterialMaker&) noexcept = delete;
	MaterialMaker& operator=(const MaterialMaker&) noexcept = delete;

private:
	bool _isHlsl;
	std::string _hlslCodes;
	std::map<std::string, bool> _onceInclude;
	std::map<std::string, std::vector<char>> _shaderCodes;
};

_NAME_END

#endif