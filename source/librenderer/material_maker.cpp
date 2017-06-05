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
#include <ray/material_maker.h>
#include <ray/material_manager.h>
#include <ray/material_pass.h>
#include <ray/material_tech.h>
#include <ray/material_param.h>

#include <ray/graphics_state.h>
#include <ray/graphics_device.h>
#include <ray/graphics_shader.h>
#include <ray/graphics_pipeline.h>
#include <ray/graphics_sampler.h>
#include <ray/graphics_input_layout.h>

#include <ray/render_system.h>
#include <ray/ioserver.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>
#include <ray/except.h>

_NAME_BEGIN

MaterialMaker::MaterialMaker() noexcept
	: _isHlsl(false)
{
}

MaterialMaker::~MaterialMaker() noexcept
{
}

bool
MaterialMaker::doCanRead(StreamReader& stream) const noexcept
{
	try
	{
		XMLReader reader;
		if (!reader.open(stream))
			return false;

		reader.setToFirstChild();

		std::string nodeName;
		nodeName = reader.getCurrentNodeName();
		if (nodeName != "material" && nodeName != "effect")
			return false;

		std::string language = reader.getValue<std::string>("language");
		if (language == "bytecodes")
			return false;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void
MaterialMaker::instanceInputLayout(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	GraphicsInputLayoutDesc inputLayoutDesc;

	std::string inputLayoutName = reader.getValue<std::string>("name");

	auto inputLayout = manager.getInputLayout(inputLayoutName);
	if (inputLayout)
		return;

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	do
	{
		std::string name = reader.getCurrentNodeName();
		if (name == "layout")
		{
			std::string layoutName = reader.getValue<std::string>("name");
			if (layoutName.empty())
				throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

			std::string layoutFormat = reader.getValue<std::string>("format");

			GraphicsFormat format;
			if (!GetFormat(layoutFormat, format))
				throw failure(__TEXT("Undefined format : ") + reader.getCurrentNodePath());

			std::uint32_t index = 0;
			reader.getValue("index", index);

			std::uint32_t slot = 0;
			reader.getValue("slot", slot);

			std::uint32_t offset = 0;
			reader.getValue("offset", offset);

			inputLayoutDesc.addVertexLayout(GraphicsVertexLayout(slot, layoutName, index, format, offset));
		}
	} while (reader.setToNextChild());

	inputLayoutDesc.addVertexBinding(GraphicsVertexBinding(0, inputLayoutDesc.getVertexSize(0)));
	inputLayout = manager.createInputLayout(inputLayoutName, inputLayoutDesc);
	if (!inputLayout)
		throw failure(__TEXT("Can't create input layout") + reader.getCurrentNodeName());
}

void
MaterialMaker::instanceCodes(MaterialManager& manager, ixmlarchive& reader) except
{
	std::string name = reader.getValue<std::string>("name");

	if (!_isHlsl && name.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	std::string codes;
	if (!reader.getValue("", codes))
		throw failure(__TEXT("Failed to fetch codes: ") + reader.getCurrentNodePath());

	if (!_isHlsl)
		util::str2hex(_shaderCodes[name], codes.c_str(), codes.size());
	else
		_hlslCodes.append(codes.begin(), codes.end());
}

void
MaterialMaker::instanceShader(MaterialManager& manager, Material& material, GraphicsProgramDesc& programDesc, ixmlarchive& reader) except
{
	std::string type = reader.getValue<std::string>("name");
	std::string value = reader.getValue<std::string>("value");

	if (type.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	if (value.empty())
		throw failure(__TEXT("Empty shader entrypoint : ") + reader.getCurrentNodePath());

	GraphicsShaderStageFlagBits shaderStage;
	if (!GetShaderStage(type, shaderStage))
		throw failure(__TEXT("Unknown shader type : ") + type + reader.getCurrentNodePath());

	GraphicsShaderDesc shaderDesc;
	if (_isHlsl)
	{
		shaderDesc.setStage(shaderStage);
		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangHLSL);
		shaderDesc.setEntryPoint(std::move(value));
		shaderDesc.setByteCodes(_hlslCodes);
	}
	else
	{
		std::vector<char> codes = _shaderCodes[value];
		if (codes.empty())
			throw failure(__TEXT("Empty shader code : ") + value + reader.getCurrentNodePath());

		shaderDesc.setStage(shaderStage);
		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangHLSLbytecodes);
		shaderDesc.setByteCodes(std::string(codes.data(), codes.size()));
	}

	auto shaderModule = manager.createShader(shaderDesc);
	if (!shaderModule)
		throw failure(__TEXT("Can't create shader : ") + reader.getCurrentNodePath());

	programDesc.addShader(std::move(shaderModule));
}

void
MaterialMaker::instancePass(MaterialManager& manager, Material& material, MaterialTechPtr& tech, ixmlarchive& reader) except
{
	std::string passName = reader.getValue<std::string>("name");
	if (passName.empty())
		throw failure(__TEXT("Empty pass name : ") + reader.getCurrentNodePath());

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	std::string name;

	GraphicsStateDesc stateDesc;
	GraphicsProgramDesc programDesc;
	GraphicsInputLayoutPtr inputLayout;

	GraphicsColorBlends blends;

	do
	{
		auto nodeName = reader.getCurrentNodeName();
		if (nodeName != "state")
			throw failure(__TEXT("Unkonwn node name : ") + nodeName + reader.getCurrentNodePath());

		if (!reader.getValue("name", name))
			throw failure(__TEXT("Empty state name : ") + reader.getCurrentNodePath());

		if (name == "vertex")
			this->instanceShader(manager, material, programDesc, reader);
		else if (name == "fragment")
			this->instanceShader(manager, material, programDesc, reader);
		else if (name == "inputlayout")
			inputLayout = manager.getInputLayout(reader.getValue<std::string>("value"));
		else if (name == "cullmode")
		{
			GraphicsCullMode cullMode;
			if (GetCullMode(reader.getValue<std::string>("value"), cullMode))
				stateDesc.setCullMode(cullMode);
			else
				throw failure(__TEXT("Failed to set CullMode: ") + reader.getCurrentNodePath());
		}
		else if (name == "polygonMode")
		{
			GraphicsPolygonMode polygonMode;
			if (GetFillMode(reader.getValue<std::string>("value"), polygonMode))
				stateDesc.setPolygonMode(polygonMode);
			else
				throw failure(__TEXT("Failed to set PolygonMode: ") + reader.getCurrentNodePath());
		}
		else if (name == "scissortest")
			stateDesc.setScissorTestEnable(reader.getValue<bool>("value"));
		else if (name == "primitive")
		{
			GraphicsVertexType type;
			if (GetPrimitive(reader.getValue<std::string>("value"), type))
				stateDesc.setPrimitiveType(type);
			else
				throw failure(__TEXT("Failed to set PrimitiveType: ") + reader.getCurrentNodePath());
		}
		else if (name == "multisampleEnable")
			stateDesc.setMultisampleEnable(reader.getValue<bool>("value"));
		else if (name == "linear2srgb")
			stateDesc.setLinear2sRGBEnable(reader.getValue<bool>("value"));
		else if (name == "linewidth")
			stateDesc.setLineWidth(reader.getValue<float>("value"));
		else if (name.compare(0, 7, "blendop") == 0)
		{
			auto index = std::atoi(name.substr(7).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendOp blendOp;
			if (GetBlendOperation(reader.getValue<std::string>("value"), blendOp))
				blends[index].setBlendOp(blendOp);
			else
				throw failure(__TEXT("Failed to set BlendOp: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 8, "blendsrc") == 0)
		{
			auto index = std::atoi(name.substr(8).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendFactor blendFactor;
			if (GetBlendFactor(reader.getValue<std::string>("value"), blendFactor))
				blends[index].setBlendSrc(blendFactor);
			else
				throw failure(__TEXT("Failed to set Src Blend: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 8, "blenddst") == 0)
		{
			auto index = std::atoi(name.substr(8).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendFactor blendFactor;
			if (GetBlendFactor(reader.getValue<std::string>("value"), blendFactor))
				blends[index].setBlendDest(blendFactor);
			else
				throw failure(__TEXT("Failed to set Dest Blend: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 12, "blendalphaop") == 0)
		{
			auto index = std::atoi(name.substr(12).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendOp blendOp;
			if (GetBlendOperation(reader.getValue<std::string>("value"), blendOp))
				blends[index].setBlendAlphaOp(blendOp);
			else
				throw failure(__TEXT("Failed to set BlendAlphaOp: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 13, "blendalphasrc") == 0)
		{
			auto index = std::atoi(name.substr(13).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendFactor blendFactor;
			if (GetBlendFactor(reader.getValue<std::string>("value"), blendFactor))
				blends[index].setBlendAlphaSrc(blendFactor);
			else
				throw failure(__TEXT("Failed to set Dest Blend: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 13, "blendalphadst") == 0)
		{
			auto index = std::atoi(name.substr(13).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsBlendFactor blendFactor;
			if (GetBlendFactor(reader.getValue<std::string>("value"), blendFactor))
				blends[index].setBlendAlphaDest(blendFactor);
			else
				throw failure(__TEXT("Failed to set Dest Blend: ") + reader.getCurrentNodePath());
		}
		else if (name.compare(0, 5, "blend") == 0)
		{
			auto index = std::atoi(name.substr(5).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendEnable(reader.getValue<bool>("value"));
		}
		else if (name.compare(0, 9, "colormask") == 0)
		{
			auto index = std::atoi(name.substr(9).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			GraphicsColorMaskFlags writeMask;
			if (GetColorMask(reader.getValue<std::string>("value"), writeMask))
				blends[index].setColorWriteMask(writeMask);
			else
				throw failure(__TEXT("Failed to set ColorWriteMask: ") + reader.getCurrentNodePath());
		}
		else if (name == "depthtest")
			stateDesc.setDepthEnable(reader.getValue<bool>("value"));
		else if (name == "depthwrite")
			stateDesc.setDepthWriteEnable(reader.getValue<bool>("value"));
		else if (name == "depthfunc")
		{
			GraphicsCompareFunc func;
			if (GetCompareFunc(reader.getValue<std::string>("value"), func))
				stateDesc.setDepthFunc(func);
			else
				throw failure(__TEXT("Failed to set DepthFunc: ") + reader.getCurrentNodePath());
		}
		else if (name == "depthBiasEnable")
			stateDesc.setDepthBiasEnable(reader.getValue<bool>("value"));
		else if (name == "depthSlopeScaleBias")
			stateDesc.setDepthSlopeScaleBias(reader.getValue<float>("value"));
		else if (name == "depthBias")
			stateDesc.setDepthBias(reader.getValue<float>("value"));
		else if (name == "stencilTest")
			stateDesc.setStencilEnable(reader.getValue<bool>("value"));
		else if (name == "stencilRef")
			stateDesc.setStencilFrontRef(reader.getValue<int>("value"));
		else if (name == "stencilFunc")
		{
			GraphicsCompareFunc func;
			if (GetCompareFunc(reader.getValue<std::string>("value"), func))
				stateDesc.setStencilFrontFunc(func);
			else
				throw failure(__TEXT("Failed to set StencilFrontFunc: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilReadMask")
			stateDesc.setStencilFrontReadMask(reader.getValue<int>("value"));
		else if (name == "stencilWriteMask")
			stateDesc.setStencilFrontWriteMask(reader.getValue<int>("value"));
		else if (name == "stencilFail")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilFrontFail(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilFrontFail: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilZFail")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilFrontZFail(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilFrontZFail: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilPass")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilFrontPass(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilFrontPass: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilTwoRef")
			stateDesc.setStencilBackRef(reader.getValue<int>("value"));
		else if (name == "stencilTwoFunc")
		{
			GraphicsCompareFunc func;
			if (GetCompareFunc(reader.getValue<std::string>("value"), func))
				stateDesc.setStencilBackFunc(func);
			else
				throw failure(__TEXT("Failed to set StencilBackFunc: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilTwoReadMask")
			stateDesc.setStencilBackReadMask(reader.getValue<std::uint32_t>("value"));
		else if (name == "stencilTwoWriteMask")
			stateDesc.setStencilBackWriteMask(reader.getValue<std::uint32_t>("value"));
		else if (name == "stencilTwoFail")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilBackFail(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilBackFail: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilTwoZFail")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilBackZFail(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilBackZFail: ") + reader.getCurrentNodePath());
		}
		else if (name == "stencilTwoPass")
		{
			GraphicsStencilOp stencilOp;
			if (GetStencilOp(reader.getValue<std::string>("value"), stencilOp))
				stateDesc.setStencilBackPass(stencilOp);
			else
				throw failure(__TEXT("Failed to set StencilBackPass: ") + reader.getCurrentNodePath());
		}
		else
		{
			throw failure(__TEXT("Unkonwn node name : ") + nodeName + reader.getCurrentNodePath());
		}
	} while (reader.setToNextChild());

	stateDesc.setColorBlends(blends);

	if (manager.getDeviceType() == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		stateDesc.setFrontFace(GraphicsFrontFace::GraphicsFrontFaceCCW);
	else
		stateDesc.setFrontFace(GraphicsFrontFace::GraphicsFrontFaceCW);

	auto state = manager.createRenderState(stateDesc);
	if (!state)
		throw failure(__TEXT("Can't create render state : ") + reader.getCurrentNodePath());

	auto program = manager.createProgram(programDesc);
	if (!state)
		throw failure(__TEXT("Can't create program : ") + reader.getCurrentNodePath());

	auto pass = std::make_shared<MaterialPass>();
	pass->setName(std::move(passName));
	pass->setGraphicsState(std::move(state));
	pass->setGraphicsProgram(std::move(program));
	pass->setGraphicsInputLayout(std::move(inputLayout));

	tech->addPass(std::move(pass));
}

void
MaterialMaker::instanceTech(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string techName = reader.getValue<std::string>("name");
	if (techName.empty())
		throw failure(__TEXT("The technique name can not be empty"));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	auto tech = std::make_shared<MaterialTech>();
	tech->setName(std::move(techName));

	do
	{
		auto name = reader.getCurrentNodeName();
		if (name == "pass")
		{
			this->instancePass(manager, material, tech, reader);
		}
	} while (reader.setToNextChild());

	material.addTech(std::move(tech));
}

void
MaterialMaker::instanceParameter(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	GraphicsUniformType uniformType;
	if (!GetUniformType(type, uniformType))
		throw failure(__TEXT("Unknown parameter type : ") + type);

	if (_isHlsl)
	{
		type = type.substr(0, type.find_first_of('['));
		_hlslCodes += "uniform " + type + " " + name + ";\n";
	}

	auto pos = name.find_first_of('[');
	if (pos != std::string::npos)
		name = name.substr(0, pos);

	auto param = std::make_shared<MaterialParam>();
	param->setName(std::move(name));
	param->setType(uniformType);

	if (!semantic.empty())
	{
		GlobalSemanticType materialType;
		if (!GetSemanticType(semantic, materialType))
			throw failure(__TEXT("Unknown semantic : ") + semantic);

		param->setSemanticType(materialType);
	}

	if (!value.empty())
	{
		switch (uniformType)
		{
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			param->uniform1f(parseFloat<float>(value));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			param->uniform2f(parseFloat2<float>(value));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			param->uniform3f(parseFloat3<float>(value));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			param->uniform4f(parseFloat4<float>(value));
			break;
		}
	}

	material.addParameter(std::move(param));
}

void
MaterialMaker::instanceMacro(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	if (_isHlsl)
	{
		_hlslCodes += "#define " + name + " " + value + "\n";
	}

	if (!type.empty())
	{
		auto macro = std::make_shared<MaterialMacro>();
		macro->setName(std::move(name));

		if (type == "bool")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeBool);
			macro->uniform1b(reader.getValue<bool>("value"));
		}
		else if (type == "int")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeInt);
			macro->uniform1i(reader.getValue<int1>("value"));
		}
		else if (type == "int2")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeInt2);
			macro->uniform2i(reader.getValue<int2>("value"));
		}
		else if (type == "int3")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeInt3);
			macro->uniform3i(reader.getValue<int3>("value"));
		}
		else if (type == "int4")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeInt4);
			macro->uniform4i(reader.getValue<int4>("value"));
		}
		else if (type == "float")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeFloat);
			macro->uniform1f(reader.getValue<float1>("value"));
		}
		else if (type == "float2")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeFloat2);
			macro->uniform2f(reader.getValue<float2>("value"));
		}
		else if (type == "float3")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeFloat3);
			macro->uniform3f(reader.getValue<float3>("value"));
		}
		else if (type == "float4")
		{
			macro->setType(GraphicsUniformType::GraphicsUniformTypeFloat4);
			macro->uniform4f(reader.getValue<float4>("value"));
		}
		else
		{
			assert(false);
			throw failure(__TEXT("Unknown macro type : ") + macro->getName());
		}

		material.addMacro(std::move(macro));
	}
}

void
MaterialMaker::instanceSampler(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string samplerName = reader.getValue<std::string>("name");
	if (samplerName.empty())
		throw failure(__TEXT("Empty sampler empty"));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	GraphicsSamplerDesc samplerDesc;

	std::string stateName;
	std::string stateValue;

	if (_isHlsl)
		_hlslCodes += "SamplerState " + samplerName + "{";

	do
	{
		if (!reader.getValue("name", stateName))
			continue;

		if (!reader.getValue("value", stateValue))
			continue;

		if (stateName == "filter")
		{
			if (stateValue == "point")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
			else if (stateValue == "min_mag_mip_point")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
			else if (stateValue == "point_mip_lienar")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapLinear);
			else if (stateValue == "linear")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
			else if (stateValue == "min_mag_linear_mip_point")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest);
			else if (stateValue == "linear_mip_linear")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear);
			else
				throw failure(__TEXT("Unknown sampler filter type") + reader.getCurrentNodePath());

			if (_isHlsl)
				_hlslCodes += "filter = " + stateValue + ";";
		}
		else if (stateName == "wrap")
		{
			if (stateValue == "clamp")
				samplerDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
			else if (stateValue == "mirror")
				samplerDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapMirror);
			else if (stateValue == "repeat")
				samplerDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapRepeat);
			else
				throw failure(__TEXT("Unknown sampler wrap type ") + reader.getCurrentNodePath());

			if (_isHlsl)
			{
				if (stateValue == "clamp")
					_hlslCodes += "AddressU = Clamp; AddressV = Clamp;";
				else if (stateValue == "mirror")
					_hlslCodes += "AddressU = Mirror; AddressV = Mirror;";
				else if (stateValue == "repeat")
					_hlslCodes += "AddressU = Wrap; AddressV = Wrap;";
			}
		}
		else if (stateName == "anis")
		{
			if (stateValue == "1")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis1);
			else if (stateValue == "2")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis2);
			else if (stateValue == "4")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis4);
			else if (stateValue == "8")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis8);
			else if (stateValue == "16")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis16);
			else if (stateValue == "32")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis32);
			else if (stateValue == "64")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis::GraphicsSamplerAnis64);
			else
				throw failure(__TEXT("Unknown sampler anis level ") + reader.getCurrentNodePath());
		}
	} while (reader.setToNextChild());

	if (_isHlsl)
		_hlslCodes += "};\n";

	auto sampler = manager.getSampler(samplerName);
	if (sampler)
		return;

	sampler = manager.createSampler(samplerName, samplerDesc);
	if (!sampler)
		throw failure(__TEXT("Can't create sampler ") + reader.getCurrentNodePath());
}

void
MaterialMaker::instanceBuffer(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string name;
	if (!reader.getValue("name", name))
		throw failure(__TEXT("Empty cbuffer name") + reader.getCurrentNodePath());

	auto buffer = std::make_shared<MaterialParam>();
	buffer->setType(GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
	buffer->setName(std::move(name));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	if (_isHlsl)
	{
		_hlslCodes += "cbuffer " + buffer->getName() + " {";
	}

	do
	{
		auto nodename = reader.getCurrentNodeName();
		if (nodename == "parameter")
		{
			auto parmName = reader.getValue<std::string>("name");
			auto parmType = reader.getValue<std::string>("type");

			if (parmName.empty())
				continue;

			if (parmType.empty())
				continue;

			if (_isHlsl)
			{
				parmType = parmType.substr(0, parmType.find_first_of('['));
				_hlslCodes += "uniform " + parmType + " " + parmName + ";\n";
			}
		}
	} while (reader.setToNextChild());

	if (_isHlsl)
	{
		_hlslCodes += "}";
	}

	material.addParameter(std::move(buffer));
}

bool
MaterialMaker::instanceInclude(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	auto filename = reader.getValue<std::string>("name");
	if (filename.empty())
		throw failure(__TEXT("Empty file"));

	if (_onceInclude[filename])
		return true;

	StreamReaderPtr stream;
	if (!IoServer::instance()->openFile(stream, filename, ios_base::in))
		throw failure(__TEXT("Opening file fail:") + filename);

	XMLReader xml;
	if (!xml.open(*stream))
		return false;

	xml.setToFirstChild();
	if (!this->loadEffect(manager, material, xml))
		return false;

	_onceInclude[filename] = true;
	return true;
}

bool
MaterialMaker::load(MaterialManager& manager, Material& material, const std::string& filename) noexcept
{
	try
	{
		StreamReaderPtr stream;
		if (!IoServer::instance()->openFile(stream, filename, ios_base::in))
			throw failure(__TEXT("Opening file fail:") + filename);

		XMLReader reader;
		if (reader.open(*stream))
		{
			reader.setToFirstChild();
			if (!this->load(manager, material, reader))
				return false;
			return true;
		}

		return false;
	}
	catch (const failure& e)
	{
		std::cout << __TEXT("in ") + filename + __TEXT(" ") + e.message() + e.stack();
		return false;
	}
}

bool
MaterialMaker::load(MaterialManager& manager, Material& material, StreamReader& stream) noexcept
{
	try
	{
		XMLReader reader;
		if (reader.open(stream))
		{
			reader.setToFirstChild();
			return this->load(manager, material, reader);
		}

		return false;
	}
	catch (const failure& e)
	{
		std::cout << e.message() << e.stack();
		return false;
	}
}

bool
MaterialMaker::loadEffect(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string nodeName = reader.getCurrentNodeName();
	if (nodeName != "effect")
		throw failure(__TEXT("Unknown node name : " + nodeName));

	std::string language = reader.getValue<std::string>("language");
	if (language == "hlsl")
		_isHlsl = true;
	else if (language == "bytecodes")
		_isHlsl = false;
	else
		throw failure("Unsupported language : " + language);

	if (!reader.setToFirstChild())
		throw failure("The file has been damaged and can't be recovered.");

	_shaderCodes.clear();

	do
	{
		std::string name = reader.getCurrentNodeName();
		if (name == "include")
			instanceInclude(manager, material, reader);
		else if (name == "parameter")
			instanceParameter(manager, material, reader);
		else if (name == "buffer")
			instanceBuffer(manager, material, reader);
		else if (name == "macro")
			instanceMacro(manager, material, reader);
		else if (name == "sampler")
			instanceSampler(manager, material, reader);
		else if (name == "shader")
			instanceCodes(manager, reader);
		else if (name == "technique")
			instanceTech(manager, material, reader);
		else if (name == "inputlayout")
			instanceInputLayout(manager, material, reader);
	} while (reader.setToNextChild());

	return true;
}

bool
MaterialMaker::loadMaterial(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string nodeName = reader.getCurrentNodeName();
	if (nodeName != "material")
		throw failure(__TEXT("Unknown node name : " + nodeName));

	std::string name;
	std::map<std::string, std::string> args;

	reader.clearAttrs();
	reader.addAttrsInChildren("attribute");
	auto& attributes = reader.getAttrList();
	for (auto& it : attributes)
	{
		if (it == "shader")
			name = reader.getValue<std::string>(it);
		else
			args[it] = reader.getValue<std::string>(it);
	}

	if (name.empty())
		throw failure(__TEXT("Shader name cannot be empty"));

	MaterialMaker maker;
	if (!maker.load(manager, material, name))
		return false;

	for (auto& arg : args)
	{
		auto param = material.getParameter(arg.first);
		if (!param)
			continue;

		switch (param->getType())
		{
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			param->uniform1f(parseFloat<float>(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			param->uniform2f(parseFloat2<float>(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			param->uniform3f(parseFloat3<float>(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			param->uniform4f(parseFloat4<float>(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		default:
			assert(false);
		}
	}

	return true;
}

bool
MaterialMaker::load(MaterialManager& manager, Material& material, ixmlarchive& reader) except
{
	std::string nodeName = reader.getCurrentNodeName();
	if (nodeName == "material")
	{
		if (loadMaterial(manager, material, reader))
			return true;
	}
	else if (nodeName == "effect")
	{
		if (loadEffect(manager, material, reader))
			return material.setup();
	}
	else
	{
		throw failure(__TEXT("Unknown node name " + nodeName));
	}

	return false;
}

bool
MaterialMaker::GetShaderStage(const std::string& string, GraphicsShaderStageFlagBits& flags) noexcept
{
	if (string.empty())
		return false;

	if (string == "vertex") { flags = GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit; return true; }
	if (string == "fragment") { flags = GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit; return true; }
	if (string == "geometry") { flags = GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit; return true; }
	if (string == "compute") { flags = GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetUniformType(const std::string& string, GraphicsUniformType& type) noexcept
{
	if (string.empty())
		return false;

	if (string == "bool") { type = GraphicsUniformType::GraphicsUniformTypeBool; return true; }
	if (string == "int") { type = GraphicsUniformType::GraphicsUniformTypeInt; return true; }
	if (string == "int2") { type = GraphicsUniformType::GraphicsUniformTypeInt2; return true; }
	if (string == "int3") { type = GraphicsUniformType::GraphicsUniformTypeInt3; return true; }
	if (string == "int4") { type = GraphicsUniformType::GraphicsUniformTypeInt4; return true; }
	if (string == "uint") { type = GraphicsUniformType::GraphicsUniformTypeUInt; return true; }
	if (string == "uint2") { type = GraphicsUniformType::GraphicsUniformTypeUInt2; return true; }
	if (string == "uint3") { type = GraphicsUniformType::GraphicsUniformTypeUInt3; return true; }
	if (string == "uint4") { type = GraphicsUniformType::GraphicsUniformTypeUInt4; return true; }
	if (string == "float") { type = GraphicsUniformType::GraphicsUniformTypeFloat; return true; }
	if (string == "float2") { type = GraphicsUniformType::GraphicsUniformTypeFloat2; return true; }
	if (string == "float3") { type = GraphicsUniformType::GraphicsUniformTypeFloat3; return true; }
	if (string == "float4") { type = GraphicsUniformType::GraphicsUniformTypeFloat4; return true; }
	if (string == "float2x2") { type = GraphicsUniformType::GraphicsUniformTypeFloat2x2; return true; }
	if (string == "float3x3") { type = GraphicsUniformType::GraphicsUniformTypeFloat3x3; return true; }
	if (string == "float4x4") { type = GraphicsUniformType::GraphicsUniformTypeFloat4x4; return true; }
	if (string == "int[]") { type = GraphicsUniformType::GraphicsUniformTypeIntArray; return true; }
	if (string == "int2[]") { type = GraphicsUniformType::GraphicsUniformTypeInt2Array; return true; }
	if (string == "int3[]") { type = GraphicsUniformType::GraphicsUniformTypeInt3Array; return true; }
	if (string == "int4[]") { type = GraphicsUniformType::GraphicsUniformTypeInt4Array; return true; }
	if (string == "uint[]") { type = GraphicsUniformType::GraphicsUniformTypeUIntArray; return true; }
	if (string == "uint2[]") { type = GraphicsUniformType::GraphicsUniformTypeUInt2Array; return true; }
	if (string == "uint3[]") { type = GraphicsUniformType::GraphicsUniformTypeUInt3Array; return true; }
	if (string == "uint4[]") { type = GraphicsUniformType::GraphicsUniformTypeUInt4Array; return true; }
	if (string == "float[]") { type = GraphicsUniformType::GraphicsUniformTypeFloatArray; return true; }
	if (string == "float2[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat2Array; return true; }
	if (string == "float3[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat3Array; return true; }
	if (string == "float4[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat4Array; return true; }
	if (string == "float2x2[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat2x2Array; return true; }
	if (string == "float3x3[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat3x3Array; return true; }
	if (string == "float4x4[]") { type = GraphicsUniformType::GraphicsUniformTypeFloat4x4Array; return true; }
	if (string == "texture2D") { type = GraphicsUniformType::GraphicsUniformTypeSamplerImage; return true; }
	if (string == "texture3D") { type = GraphicsUniformType::GraphicsUniformTypeSamplerImage; return true; }
	if (string == "textureCUBE") { type = GraphicsUniformType::GraphicsUniformTypeSamplerImage; return true; }
	if (string == "buffer") { type = GraphicsUniformType::GraphicsUniformTypeUniformBuffer; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetPrimitive(const std::string& string, GraphicsVertexType& primitive) noexcept
{
	if (string.empty())
		return false;

	if (string == "point") { primitive = GraphicsVertexType::GraphicsVertexTypePointList; return true; }
	if (string == "line") { primitive = GraphicsVertexType::GraphicsVertexTypeLineList; return true; }
	if (string == "line_strip") { primitive = GraphicsVertexType::GraphicsVertexTypeLineStrip; return true; }
	if (string == "triangle") { primitive = GraphicsVertexType::GraphicsVertexTypeTriangleList; return true; }
	if (string == "triangle_strip") { primitive = GraphicsVertexType::GraphicsVertexTypeTriangleStrip; return true; }
	if (string == "triangle_fan") { primitive = GraphicsVertexType::GraphicsVertexTypeTriangleFan; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetCullMode(const std::string& string, GraphicsCullMode& cullmode) noexcept
{
	if (string.empty())
		return false;

	if (string == "back") { cullmode = GraphicsCullMode::GraphicsCullModeBack; return true; }
	if (string == "front") { cullmode = GraphicsCullMode::GraphicsCullModeFront; return true; }
	if (string == "frontback") { cullmode = GraphicsCullMode::GraphicsCullModeFrontBack; return true; }
	if (string == "none") { cullmode = GraphicsCullMode::GraphicsCullModeNone; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetFillMode(const std::string& string, GraphicsPolygonMode& fillmode) noexcept
{
	if (string.empty())
		return false;

	if (string == "point") { fillmode = GraphicsPolygonMode::GraphicsPolygonModePoint; return true; }
	if (string == "line") { fillmode = GraphicsPolygonMode::GraphicsPolygonModeWireframe; return true; }
	if (string == "solid") { fillmode = GraphicsPolygonMode::GraphicsPolygonModeSolid; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetBlendOperation(const std::string& string, GraphicsBlendOp& blendop) noexcept
{
	if (string.empty())
		return false;

	if (string == "sub") { blendop = GraphicsBlendOp::GraphicsBlendOpSubtract; return true; }
	if (string == "revsub") { blendop = GraphicsBlendOp::GraphicsBlendOpRevSubtract; return true; }
	if (string == "add") { blendop = GraphicsBlendOp::GraphicsBlendOpAdd; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetBlendFactor(const std::string& string, GraphicsBlendFactor& factor) noexcept
{
	if (string.empty())
		return false;

	if (string == "zero") { factor = GraphicsBlendFactor::GraphicsBlendFactorZero; return true; }
	if (string == "one") { factor = GraphicsBlendFactor::GraphicsBlendFactorOne; return true; }
	if (string == "dstcol") { factor = GraphicsBlendFactor::GraphicsBlendFactorDstCol; return true; }
	if (string == "srccol") { factor = GraphicsBlendFactor::GraphicsBlendFactorSrcColor; return true; }
	if (string == "srcalpha") { factor = GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha; return true; }
	if (string == "dstalpha") { factor = GraphicsBlendFactor::GraphicsBlendFactorDstAlpha; return true; }
	if (string == "invsrccol") { factor = GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcCol; return true; }
	if (string == "invdstcol") { factor = GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstCol; return true; }
	if (string == "invsrcalpha") { factor = GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha; return true; }
	if (string == "invdstalpha") { factor = GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstAlpha; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetColorMask(const std::string& string, GraphicsColorMaskFlags& mask) noexcept
{
	if (string.empty())
		return false;

	if (string == "none") { mask = GraphicsColorMaskFlags(0); return true; }
	if (string == "red") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRedBit; return true; }
	if (string == "green") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagGreendBit; return true; }
	if (string == "blue") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagBlurBit; return true; }
	if (string == "alpha") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagAlphaBit; return true; }
	if (string == "rgb") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBBit; return true; }
	if (string == "rgba") { mask = GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetCompareFunc(const std::string& string, GraphicsCompareFunc& func) noexcept
{
	if (string.empty())
		return false;

	if (string == "lequal") { func = GraphicsCompareFunc::GraphicsCompareFuncLequal; return true; }
	if (string == "equal") { func = GraphicsCompareFunc::GraphicsCompareFuncEqual; return true; }
	if (string == "greater") { func = GraphicsCompareFunc::GraphicsCompareFuncGreater; return true; }
	if (string == "less") { func = GraphicsCompareFunc::GraphicsCompareFuncLess; return true; }
	if (string == "gequal") { func = GraphicsCompareFunc::GraphicsCompareFuncGequal; return true; }
	if (string == "notequal") { func = GraphicsCompareFunc::GraphicsCompareFuncNotEqual; return true; }
	if (string == "always") { func = GraphicsCompareFunc::GraphicsCompareFuncAlways; return true; }
	if (string == "never") { func = GraphicsCompareFunc::GraphicsCompareFuncNever; return true; }
	if (string == "none") { func = GraphicsCompareFunc::GraphicsCompareFuncNone; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetStencilOp(const std::string& string, GraphicsStencilOp& stencilop) noexcept
{
	if (string.empty())
		return false;

	if (string == "keep") { stencilop = GraphicsStencilOp::GraphicsStencilOpKeep; return true; }
	if (string == "replace") { stencilop = GraphicsStencilOp::GraphicsStencilOpReplace; return true; }
	if (string == "inc") { stencilop = GraphicsStencilOp::GraphicsStencilOpIncr; return true; }
	if (string == "dec") { stencilop = GraphicsStencilOp::GraphicsStencilOpDecr; return true; }
	if (string == "incwrap") { stencilop = GraphicsStencilOp::GraphicsStencilOpIncrWrap; return true; }
	if (string == "decwrap") { stencilop = GraphicsStencilOp::GraphicsStencilOpDecrWrap; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetFormat(const std::string& string, GraphicsFormat& format) noexcept
{
	if (string.empty())
		return false;

	if (string == "R4G4UNormPack8") { format = GraphicsFormat::GraphicsFormatR4G4UNormPack8; return true; }
	if (string == "R8UNorm") { format = GraphicsFormat::GraphicsFormatR8UNorm; return true; }
	if (string == "R8SNorm") { format = GraphicsFormat::GraphicsFormatR8SNorm; return true; }
	if (string == "R8UScaled") { format = GraphicsFormat::GraphicsFormatR8UScaled; return true; }
	if (string == "R8SScaled") { format = GraphicsFormat::GraphicsFormatR8SScaled; return true; }
	if (string == "R8UInt") { format = GraphicsFormat::GraphicsFormatR8UInt; return true; }
	if (string == "R8SInt") { format = GraphicsFormat::GraphicsFormatR8SInt; return true; }
	if (string == "R8SRGB") { format = GraphicsFormat::GraphicsFormatR8SRGB; return true; }
	if (string == "S8UInt") { format = GraphicsFormat::GraphicsFormatS8UInt; return true; }
	if (string == "R4G4B4A4UNormPack16") { format = GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16; return true; }
	if (string == "B4G4R4A4UNormPack16") { format = GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16; return true; }
	if (string == "R5G6B5UNormPack16") { format = GraphicsFormat::GraphicsFormatR5G6B5UNormPack16; return true; }
	if (string == "B5G6R5UNormPack16") { format = GraphicsFormat::GraphicsFormatB5G6R5UNormPack16; return true; }
	if (string == "R5G5B5A1UNormPack16") { format = GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16; return true; }
	if (string == "B5G5R5A1UNormPack16") { format = GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16; return true; }
	if (string == "A1R5G5B5UNormPack16") { format = GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16; return true; }
	if (string == "R8G8UNorm") { format = GraphicsFormat::GraphicsFormatR8G8UNorm; return true; }
	if (string == "R8G8SNorm") { format = GraphicsFormat::GraphicsFormatR8G8SNorm; return true; }
	if (string == "R8G8UScaled") { format = GraphicsFormat::GraphicsFormatR8G8UScaled; return true; }
	if (string == "R8G8SScaled") { format = GraphicsFormat::GraphicsFormatR8G8SScaled; return true; }
	if (string == "R8G8UInt") { format = GraphicsFormat::GraphicsFormatR8G8UInt; return true; }
	if (string == "R8G8SInt") { format = GraphicsFormat::GraphicsFormatR8G8SInt; return true; }
	if (string == "R8G8SRGB") { format = GraphicsFormat::GraphicsFormatR8G8SRGB; return true; }
	if (string == "R16UNorm") { format = GraphicsFormat::GraphicsFormatR16UNorm; return true; }
	if (string == "R16SNorm") { format = GraphicsFormat::GraphicsFormatR16SNorm; return true; }
	if (string == "R16UScaled") { format = GraphicsFormat::GraphicsFormatR16UScaled; return true; }
	if (string == "R16SScaled") { format = GraphicsFormat::GraphicsFormatR16SScaled; return true; }
	if (string == "R16UInt") { format = GraphicsFormat::GraphicsFormatR16UInt; return true; }
	if (string == "R16SInt") { format = GraphicsFormat::GraphicsFormatR16SInt; return true; }
	if (string == "R16SFloat") { format = GraphicsFormat::GraphicsFormatR16SFloat; return true; }
	if (string == "D16UNorm") { format = GraphicsFormat::GraphicsFormatD16UNorm; return true; }
	if (string == "R8G8B8UNorm") { format = GraphicsFormat::GraphicsFormatR8G8B8UNorm; return true; }
	if (string == "R8G8B8SNorm") { format = GraphicsFormat::GraphicsFormatR8G8B8SNorm; return true; }
	if (string == "R8G8B8UScaled") { format = GraphicsFormat::GraphicsFormatR8G8B8UScaled; return true; }
	if (string == "R8G8B8SScaled") { format = GraphicsFormat::GraphicsFormatR8G8B8SScaled; return true; }
	if (string == "R8G8B8UInt") { format = GraphicsFormat::GraphicsFormatR8G8B8UInt; return true; }
	if (string == "R8G8B8SInt") { format = GraphicsFormat::GraphicsFormatR8G8B8SInt; return true; }
	if (string == "R8G8B8SRGB") { format = GraphicsFormat::GraphicsFormatR8G8B8SRGB; return true; }
	if (string == "B8G8R8UNorm") { format = GraphicsFormat::GraphicsFormatB8G8R8UNorm; return true; }
	if (string == "B8G8R8SNorm") { format = GraphicsFormat::GraphicsFormatB8G8R8SNorm; return true; }
	if (string == "B8G8R8UScaled") { format = GraphicsFormat::GraphicsFormatB8G8R8UScaled; return true; }
	if (string == "B8G8R8SScaled") { format = GraphicsFormat::GraphicsFormatB8G8R8SScaled; return true; }
	if (string == "B8G8R8UInt") { format = GraphicsFormat::GraphicsFormatB8G8R8UInt; return true; }
	if (string == "B8G8R8SInt") { format = GraphicsFormat::GraphicsFormatB8G8R8SInt; return true; }
	if (string == "B8G8R8SRGB") { format = GraphicsFormat::GraphicsFormatB8G8R8SRGB; return true; }
	if (string == "R8G8B8A8UNorm") { format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm; return true; }
	if (string == "R8G8B8A8SNorm") { format = GraphicsFormat::GraphicsFormatR8G8B8A8SNorm; return true; }
	if (string == "R8G8B8A8UScaled") { format = GraphicsFormat::GraphicsFormatR8G8B8A8UScaled; return true; }
	if (string == "R8G8B8A8SScaled") { format = GraphicsFormat::GraphicsFormatR8G8B8A8SScaled; return true; }
	if (string == "R8G8B8A8UInt") { format = GraphicsFormat::GraphicsFormatR8G8B8A8UInt; return true; }
	if (string == "R8G8B8A8SInt") { format = GraphicsFormat::GraphicsFormatR8G8B8A8SInt; return true; }
	if (string == "R8G8B8A8SRGB") { format = GraphicsFormat::GraphicsFormatR8G8B8A8SRGB; return true; }
	if (string == "B8G8R8A8UNorm") { format = GraphicsFormat::GraphicsFormatB8G8R8A8UNorm; return true; }
	if (string == "B8G8R8A8SNorm") { format = GraphicsFormat::GraphicsFormatB8G8R8A8SNorm; return true; }
	if (string == "B8G8R8A8UScaled") { format = GraphicsFormat::GraphicsFormatB8G8R8A8UScaled; return true; }
	if (string == "B8G8R8A8SScaled") { format = GraphicsFormat::GraphicsFormatB8G8R8A8SScaled; return true; }
	if (string == "B8G8R8A8UInt") { format = GraphicsFormat::GraphicsFormatB8G8R8A8UInt; return true; }
	if (string == "B8G8R8A8SInt") { format = GraphicsFormat::GraphicsFormatB8G8R8A8SInt; return true; }
	if (string == "B8G8R8A8SRGB") { format = GraphicsFormat::GraphicsFormatB8G8R8A8SRGB; return true; }
	if (string == "A8B8G8R8UNormPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32; return true; }
	if (string == "A8B8G8R8SNormPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32; return true; }
	if (string == "A8B8G8R8UScaledPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32; return true; }
	if (string == "A8B8G8R8SScaledPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32; return true; }
	if (string == "A8B8G8R8UIntPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32; return true; }
	if (string == "A8B8G8R8SIntPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32; return true; }
	if (string == "A8B8G8R8SRGBPack32") { format = GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32; return true; }
	if (string == "R16G16SInt") { format = GraphicsFormat::GraphicsFormatR16G16SInt; return true; }
	if (string == "R16G16UInt") { format = GraphicsFormat::GraphicsFormatR16G16UInt; return true; }
	if (string == "R16G16SNorm") { format = GraphicsFormat::GraphicsFormatR16G16SNorm; return true; }
	if (string == "R16G16UNorm") { format = GraphicsFormat::GraphicsFormatR16G16UNorm; return true; }
	if (string == "R16G16UScaled") { format = GraphicsFormat::GraphicsFormatR16G16UScaled; return true; }
	if (string == "R16G16SScaled") { format = GraphicsFormat::GraphicsFormatR16G16SScaled; return true; }
	if (string == "R16G16SFloat") { format = GraphicsFormat::GraphicsFormatR16G16SFloat; return true; }
	if (string == "R32UInt") { format = GraphicsFormat::GraphicsFormatR32UInt; return true; }
	if (string == "R32SInt") { format = GraphicsFormat::GraphicsFormatR32SInt; return true; }
	if (string == "R32SFloat") { format = GraphicsFormat::GraphicsFormatR32SFloat; return true; }
	if (string == "A2R10G10B10UNormPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32; return true; }
	if (string == "A2R10G10B10SNormPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10SNormPack32; return true; }
	if (string == "A2R10G10B10UScaledPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10UScaledPack32; return true; }
	if (string == "A2R10G10B10SScaledPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10SScaledPack32; return true; }
	if (string == "A2R10G10B10UIntPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32; return true; }
	if (string == "A2R10G10B10SIntPack32") { format = GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32; return true; }
	if (string == "A2B10G10R10UNormPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32; return true; }
	if (string == "A2B10G10R10SNormPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32; return true; }
	if (string == "A2B10G10R10UScaledPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32; return true; }
	if (string == "A2B10G10R10SScaledPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32; return true; }
	if (string == "A2B10G10R10UIntPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32; return true; }
	if (string == "A2B10G10R10SIntPack32") { format = GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32; return true; }
	if (string == "X8_D24UNormPack32") { format = GraphicsFormat::GraphicsFormatX8_D24UNormPack32; return true; }
	if (string == "B10G11R11UFloatPack32") { format = GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32; return true; }
	if (string == "E5B9G9R9UFloatPack32") { format = GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32; return true; }
	if (string == "D32_SFLOAT") { format = GraphicsFormat::GraphicsFormatD32_SFLOAT; return true; }
	if (string == "D16UNorm_S8UInt") { format = GraphicsFormat::GraphicsFormatD16UNorm_S8UInt; return true; }
	if (string == "D24UNorm_S8UInt") { format = GraphicsFormat::GraphicsFormatD24UNorm_S8UInt; return true; }
	if (string == "D32_SFLOAT_S8UInt") { format = GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt; return true; }
	if (string == "R16G16B16SInt") { format = GraphicsFormat::GraphicsFormatR16G16B16SInt; return true; }
	if (string == "R16G16B16UInt") { format = GraphicsFormat::GraphicsFormatR16G16B16UInt; return true; }
	if (string == "R16G16B16SNorm") { format = GraphicsFormat::GraphicsFormatR16G16B16SNorm; return true; }
	if (string == "R16G16B16UNorm") { format = GraphicsFormat::GraphicsFormatR16G16B16UNorm; return true; }
	if (string == "R16G16B16UScaled") { format = GraphicsFormat::GraphicsFormatR16G16B16UScaled; return true; }
	if (string == "R16G16B16SScaled") { format = GraphicsFormat::GraphicsFormatR16G16B16SScaled; return true; }
	if (string == "R16G16B16SFloat") { format = GraphicsFormat::GraphicsFormatR16G16B16SFloat; return true; }
	if (string == "R32G32SFloat") { format = GraphicsFormat::GraphicsFormatR32G32SFloat; return true; }
	if (string == "R32G32SInt") { format = GraphicsFormat::GraphicsFormatR32G32SInt; return true; }
	if (string == "R32G32UInt") { format = GraphicsFormat::GraphicsFormatR32G32UInt; return true; }
	if (string == "R16G16B16A16SNorm") { format = GraphicsFormat::GraphicsFormatR16G16B16A16SNorm; return true; }
	if (string == "R16G16B16A16UNorm") { format = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm; return true; }
	if (string == "R16G16B16A16SScaled") { format = GraphicsFormat::GraphicsFormatR16G16B16A16SScaled; return true; }
	if (string == "R16G16B16A16UScaled") { format = GraphicsFormat::GraphicsFormatR16G16B16A16UScaled; return true; }
	if (string == "R16G16B16A16SFloat") { format = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat; return true; }
	if (string == "R16G16B16A16SInt") { format = GraphicsFormat::GraphicsFormatR16G16B16A16SInt; return true; }
	if (string == "R16G16B16A16UInt") { format = GraphicsFormat::GraphicsFormatR16G16B16A16UInt; return true; }
	if (string == "R64UInt") { format = GraphicsFormat::GraphicsFormatR64UInt; return true; }
	if (string == "R64SInt") { format = GraphicsFormat::GraphicsFormatR64SInt; return true; }
	if (string == "R64SFloat") { format = GraphicsFormat::GraphicsFormatR64SFloat; return true; }
	if (string == "R32G32B32SFloat") { format = GraphicsFormat::GraphicsFormatR32G32B32SFloat; return true; }
	if (string == "R32G32B32SInt") { format = GraphicsFormat::GraphicsFormatR32G32B32SInt; return true; }
	if (string == "R32G32B32UInt") { format = GraphicsFormat::GraphicsFormatR32G32B32UInt; return true; }
	if (string == "R32G32B32A32SFloat") { format = GraphicsFormat::GraphicsFormatR32G32B32A32SFloat; return true; }
	if (string == "R32G32B32A32SInt") { format = GraphicsFormat::GraphicsFormatR32G32B32A32SInt; return true; }
	if (string == "R32G32B32A32UInt") { format = GraphicsFormat::GraphicsFormatR32G32B32A32UInt; return true; }
	if (string == "R64G64UInt") { format = GraphicsFormat::GraphicsFormatR64G64UInt; return true; }
	if (string == "R64G64SInt") { format = GraphicsFormat::GraphicsFormatR64G64SInt; return true; }
	if (string == "R64G64SFloat") { format = GraphicsFormat::GraphicsFormatR64G64SFloat; return true; }
	if (string == "R64G64B64UInt") { format = GraphicsFormat::GraphicsFormatR64G64B64UInt; return true; }
	if (string == "R64G64B64SInt") { format = GraphicsFormat::GraphicsFormatR64G64B64SInt; return true; }
	if (string == "R64G64B64SFloat") { format = GraphicsFormat::GraphicsFormatR64G64B64SFloat; return true; }
	if (string == "R64G64B64A64UInt") { format = GraphicsFormat::GraphicsFormatR64G64B64A64UInt; return true; }
	if (string == "R64G64B64A64SInt") { format = GraphicsFormat::GraphicsFormatR64G64B64A64SInt; return true; }
	if (string == "R64G64B64A64SFloat") { format = GraphicsFormat::GraphicsFormatR64G64B64A64SFloat; return true; }

	assert(false);
	return false;
}

bool
MaterialMaker::GetSemanticType(const std::string& string, GlobalSemanticType& type) noexcept
{
	if (string.empty())
		return false;

	if (string == "matModel") { type = GlobalSemanticType::GlobalSemanticTypeModel; return true; }
	if (string == "matModelInverse") { type = GlobalSemanticType::GlobalSemanticTypeModelInverse; return true; }
	if (string == "matView") { type = GlobalSemanticType::GlobalSemanticTypeView; return true; }
	if (string == "matViewInverse") { type = GlobalSemanticType::GlobalSemanticTypeViewInverse; return true; }
	if (string == "matProject") { type = GlobalSemanticType::GlobalSemanticTypeProject; return true; }
	if (string == "matProjectInverse") { type = GlobalSemanticType::GlobalSemanticTypeProjectInverse; return true; }
	if (string == "matViewProject") { type = GlobalSemanticType::GlobalSemanticTypeViewProject; return true; }
	if (string == "matViewProjectInverse") { type = GlobalSemanticType::GlobalSemanticTypeViewProjectInverse; return true; }
	if (string == "matModelView") { type = GlobalSemanticType::GlobalSemanticTypeModelView; return true; }
	if (string == "matModelViewProject") { type = GlobalSemanticType::GlobalSemanticTypeModelViewProject; return true; }
	if (string == "matModelViewInverse") { type = GlobalSemanticType::GlobalSemanticTypeModelViewInverse; return true; }
	if (string == "CameraAperture") { type = GlobalSemanticType::GlobalSemanticTypeCameraAperture; return true; }
	if (string == "CameraNear") { type = GlobalSemanticType::GlobalSemanticTypeCameraNear; return true; }
	if (string == "CameraFar") { type = GlobalSemanticType::GlobalSemanticTypeCameraFar; return true; }
	if (string == "CameraPosition") { type = GlobalSemanticType::GlobalSemanticTypeCameraPosition; return true; }
	if (string == "CameraDirection") { type = GlobalSemanticType::GlobalSemanticTypeCameraDirection; return true; }
	if (string == "DepthMap") { type = GlobalSemanticType::GlobalSemanticTypeDepthMap; return true; }
	if (string == "DepthLinearMap") { type = GlobalSemanticType::GlobalSemanticTypeDepthLinearMap; return true; }
	if (string == "DiffuseMap") { type = GlobalSemanticType::GlobalSemanticTypeDiffuseMap; return true; }
	if (string == "NormalMap") { type = GlobalSemanticType::GlobalSemanticTypeNormalMap; return true; }
	if (string == "Gbuffer3Map") { type = GlobalSemanticType::GlobalSemanticTypeGbuffer3Map; return true; }
	if (string == "LightingMap") { type = GlobalSemanticType::GlobalSemanticTypeLightingMap; return true; }

	assert(false);
	return false;
}

_NAME_END