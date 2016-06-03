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

#if !defined(_BUILD_OPENGL_ES) || defined(__WINDOWS__)
#	include <d3dcompiler.h>
#endif

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

void
MaterialMaker::instanceInputLayout(MaterialManager& manager, Material& material, iarchive& reader) except
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
			GraphicsFormat format = stringToFormat(layoutFormat);
			if (format == GraphicsFormat::GraphicsFormatMaxEnum)
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
MaterialMaker::instanceCodes(MaterialManager& manager, iarchive& reader) except
{
	std::string name = reader.getValue<std::string>("name");

	if (!_isHlsl && name.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	std::string codes = reader.getText();
	if (codes.empty())
		return;

	if (!_isHlsl)
		util::str2hex(_shaderCodes[name], codes.c_str(), codes.size());
	else
		_hlslCodes.append(codes.begin(), codes.end());
}

void
MaterialMaker::instanceShader(MaterialManager& manager, Material& material, GraphicsProgramDesc& programDesc, iarchive& reader) except
{
	std::string type = reader.getValue<std::string>("name");
	std::string value = reader.getValue<std::string>("value");

	if (type.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	if (value.empty())
		throw failure(__TEXT("Empty shader entrypoint : ") + reader.getCurrentNodePath());

	GraphicsShaderStageFlagBits shaderStage = stringToShaderStage(type);
	if (shaderStage == GraphicsShaderStageFlagBits::GraphicsShaderStageFlagBitsMaxEnum)
		throw failure(__TEXT("Unknown shader type : ") + type + reader.getCurrentNodePath());

	GraphicsShaderDesc shaderDesc;
	if (!_isHlsl)
	{
		std::vector<char> codes = _shaderCodes[value];
		if (codes.empty())
			throw failure(__TEXT("Empty shader code : ") + value + reader.getCurrentNodePath());

		shaderDesc.setStage(shaderStage);
		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangHLSLbytecodes);
		shaderDesc.setByteCodes(std::string(codes.data(), codes.size()));
	}
	else
	{
		std::string profile;
		if (type == "vertex")
			profile = "vs_4_0";
		else if (type == "fragment")
			profile = "ps_4_0";

		ID3DBlob* binary = nullptr;
		ID3DBlob* error = nullptr;

		D3DCreateBlob(4096, &binary);
		D3DCreateBlob(4096, &error);

		HRESULT hr = D3DCompile(
			_hlslCodes.data(),
			_hlslCodes.size(),
			nullptr,
			nullptr,
			nullptr,
			value.c_str(),
			profile.c_str(),
			D3DCOMPILE_OPTIMIZATION_LEVEL3,
			0,
			&binary,
			&error
			);

		if (hr != S_OK)
		{
			std::string line;
			std::size_t index = 1;
			std::ostringstream ostream;
			std::istringstream istream(_hlslCodes);

			ostream << (const char*)error->GetBufferPointer() << std::endl;
			while (std::getline(istream, line))
			{
				ostream << index << '\t' << line << std::endl;
				index++;
			}

			throw ray::failure(ostream.str().c_str());
		}

		shaderDesc.setStage(shaderStage);
		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangHLSLbytecodes);
		shaderDesc.setByteCodes(std::string((char*)binary->GetBufferPointer(), binary->GetBufferSize()));
	}

	auto shaderModule = manager.createShader(shaderDesc);
	if (!shaderModule)
		throw failure(__TEXT("Can't create shader : ") + reader.getCurrentNodePath());

	programDesc.addShader(std::move(shaderModule));
}

void
MaterialMaker::instancePass(MaterialManager& manager, Material& material, MaterialTechPtr& tech, iarchive& reader) except
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
			stateDesc.setCullMode(stringToCullMode(reader.getValue<std::string>("value")));
		else if (name == "polygonMode")
			stateDesc.setPolygonMode(stringToFillMode(reader.getValue<std::string>("value")));
		else if (name == "scissorTestEnable")
			stateDesc.setScissorTestEnable(reader.getValue<bool>("value"));
		else if (name == "primitive")
			stateDesc.setPrimitiveType(stringToPrimitive(reader.getValue<std::string>("value")));
		else if (name == "multisampleEnable")
			stateDesc.setMultisampleEnable(reader.getValue<bool>("value"));
		else if (name == "linear2srgb")
			stateDesc.setLinear2sRGBEnable(reader.getValue<bool>("value"));
		else if (name == "linewidth")
			stateDesc.setLineWidth(reader.getValue<float>("value"));	
		else if (name.compare(0, 7, "blendop") == 0)
		{
			std::size_t index = std::atoi(name.substr(7).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendOp(stringToBlendOperation(reader.getValue<std::string>("value")));
		}			
		else if (name.compare(0, 8, "blendsrc") == 0)
		{
			std::size_t index = std::atoi(name.substr(8).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendSrc(stringToBlendFactor(reader.getValue<std::string>("value")));
		}
		else if (name.compare(0, 8, "blenddst") == 0)
		{
			std::size_t index = std::atoi(name.substr(8).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendDest(stringToBlendFactor(reader.getValue<std::string>("value")));
		}
		else if (name.compare(0, 12, "blendalphaop") == 0)
		{
			std::size_t index = std::atoi(name.substr(12).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendAlphaOp(stringToBlendOperation(reader.getValue<std::string>("value")));
		}
		else if (name.compare(0, 13, "blendalphasrc") == 0)
		{
			std::size_t index = std::atoi(name.substr(13).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendAlphaSrc(stringToBlendFactor(reader.getValue<std::string>("value")));
		}			
		else if (name.compare(0, 13, "blendalphadst") == 0)
		{
			std::size_t index = std::atoi(name.substr(13).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendAlphaDest(stringToBlendFactor(reader.getValue<std::string>("value")));
		}
		else if (name.compare(0, 5, "blend") == 0)
		{
			std::size_t index = std::atoi(name.substr(5).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setBlendEnable(reader.getValue<bool>("value"));
		}
		else if (name.compare(0, 9, "colormask") == 0)
		{
			std::size_t index = std::atoi(name.substr(9).c_str());
			if (blends.size() <= index)
				blends.resize(index + 1);

			blends[index].setColorWriteMask(stringToColorMask(reader.getValue<std::string>("value")));
		}
		else if (name == "depthtest")
			stateDesc.setDepthEnable(reader.getValue<bool>("value"));
		else if (name == "depthwrite")
			stateDesc.setDepthWriteEnable(reader.getValue<bool>("value"));
		else if (name == "depthfunc")
			stateDesc.setDepthFunc(stringToCompareFunc(reader.getValue<std::string>("value")));
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
			stateDesc.setStencilFrontFunc(stringToCompareFunc(reader.getValue<std::string>("value")));
		else if (name == "stencilReadMask")
			stateDesc.setStencilFrontReadMask(reader.getValue<int>("value"));
		else if (name == "stencilWriteMask")
			stateDesc.setStencilFrontWriteMask(reader.getValue<int>("value"));
		else if (name == "stencilFail")
			stateDesc.setStencilFrontFail(stringToStencilOp(reader.getValue<std::string>("value")));
		else if (name == "stencilZFail")
			stateDesc.setStencilFrontZFail(stringToStencilOp(reader.getValue<std::string>("value")));
		else if (name == "stencilPass")
			stateDesc.setStencilFrontPass(stringToStencilOp(reader.getValue<std::string>("value")));
		else if (name == "stencilTwoRef")
			stateDesc.setStencilBackRef(reader.getValue<int>("value"));
		else if (name == "stencilTwoFunc")
			stateDesc.setStencilBackFunc(stringToCompareFunc(reader.getValue<std::string>("value")));
		else if (name == "stencilTwoReadMask")
			stateDesc.setStencilBackReadMask(reader.getValue<std::uint32_t>("value"));
		else if (name == "stencilTwoWriteMask")
			stateDesc.setStencilBackWriteMask(reader.getValue<std::uint32_t>("value"));
		else if (name == "stencilTwoFail")
			stateDesc.setStencilBackFail(stringToStencilOp(reader.getValue<std::string>("value")));
		else if (name == "stencilTwoZFail")
			stateDesc.setStencilBackZFail(stringToStencilOp(reader.getValue<std::string>("value")));
		else if (name == "stencilTwoPass")
			stateDesc.setStencilBackPass(stringToStencilOp(reader.getValue<std::string>("value")));
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
MaterialMaker::instanceTech(MaterialManager& manager, Material& material, iarchive& reader) except
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
MaterialMaker::instanceParameter(MaterialManager& manager, Material& material, iarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	auto uniformType = stringToUniformType(type);
	if (uniformType == GraphicsUniformType::GraphicsUniformTypeMaxEnum)
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
		auto materialType = stringToSemanticType(semantic);
		if (!materialType)
			throw failure(__TEXT("Unknown semantic : ") + semantic);

		param->setSemanticType(materialType);
	}

	material.addParameter(std::move(param));
}

void
MaterialMaker::instanceMacro(MaterialManager& manager, Material& material, iarchive& reader) except
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
MaterialMaker::instanceSampler(MaterialManager& manager, Material& material, iarchive& reader) except
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
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis1);
			else if (stateValue == "2")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis2);
			else if (stateValue == "4")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis4);
			else if (stateValue == "8")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis8);
			else if (stateValue == "16")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis16);
			else if (stateValue == "32")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis32);
			else if (stateValue == "64")
				samplerDesc.setSamplerAnis(GraphicsSamplerAnis64);
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
MaterialMaker::instanceBuffer(MaterialManager& manager, Material& material, iarchive& reader) except
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
MaterialMaker::instanceInclude(MaterialManager& manager, Material& material, iarchive& reader) except
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
MaterialMaker::loadEffect(MaterialManager& manager, Material& material, iarchive& reader) except
{
	std::string nodeName = reader.getCurrentNodeName();
	if (nodeName != "effect")
		throw failure(__TEXT("Unknown node name " + nodeName + ", so I can't open it"));

	std::string language = reader.getValue<std::string>("language");
	if (language == "hlsl")
		_isHlsl = true;
	else if (language == "bytecodes")
		_isHlsl = false;
	else
		throw failure("Can't support language : " + language + ", so I can't open it");

	if (!reader.setToFirstChild())
		throw failure("The file has been damaged and can't be recovered, so I can't open it");

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
MaterialMaker::loadMaterial(MaterialManager& manager, Material& material, iarchive& reader) except
{
	std::string nodeName = reader.getCurrentNodeName();
	if (nodeName != "material")
		throw failure(__TEXT("Unknown node name " + nodeName + ", so I can't open it"));

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
		throw failure(__TEXT("The shader is not set"));

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
			param->uniform1f(parseFloat<Float>(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			param->uniform2f(parseFloat2(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			param->uniform3f(parseFloat3(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			param->uniform4f(parseFloat4(arg.second));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			param->uniformTexture(RenderSystem::instance()->createTexture(arg.second, GraphicsTextureDim::GraphicsTextureDim2D));
			break;
		default:
			assert(false);
		}
	}

	return true;
}

bool
MaterialMaker::load(MaterialManager& manager, Material& material, iarchive& reader) except
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
		throw failure(__TEXT("Unknown node name " + nodeName + ", so I can't open it"));
	}

	return false;
}

GraphicsShaderStageFlagBits
MaterialMaker::stringToShaderStage(const std::string& stage) noexcept
{
	if (stage == "vertex")   return GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit;
	if (stage == "fragment") return GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit;
	if (stage == "geometry") return GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit;
	if (stage == "compute")  return GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit;

	assert(false);
	return GraphicsShaderStageFlagBits::GraphicsShaderStageFlagBitsMaxEnum;
}

GraphicsUniformType
MaterialMaker::stringToUniformType(const std::string& type) noexcept
{
	if (type == "bool") 	   return GraphicsUniformType::GraphicsUniformTypeBool;
	if (type == "int") 	       return GraphicsUniformType::GraphicsUniformTypeInt;
	if (type == "int2") 	   return GraphicsUniformType::GraphicsUniformTypeInt2;
	if (type == "int3") 	   return GraphicsUniformType::GraphicsUniformTypeInt3;
	if (type == "int4") 	   return GraphicsUniformType::GraphicsUniformTypeInt4;
	if (type == "uint") 	   return GraphicsUniformType::GraphicsUniformTypeUInt;
	if (type == "uint2") 	   return GraphicsUniformType::GraphicsUniformTypeUInt2;
	if (type == "uint3") 	   return GraphicsUniformType::GraphicsUniformTypeUInt3;
	if (type == "uint4") 	   return GraphicsUniformType::GraphicsUniformTypeUInt4;
	if (type == "float") 	   return GraphicsUniformType::GraphicsUniformTypeFloat;
	if (type == "float2") 	   return GraphicsUniformType::GraphicsUniformTypeFloat2;
	if (type == "float3") 	   return GraphicsUniformType::GraphicsUniformTypeFloat3;
	if (type == "float4") 	   return GraphicsUniformType::GraphicsUniformTypeFloat4;
	if (type == "float2x2")    return GraphicsUniformType::GraphicsUniformTypeFloat2x2;
	if (type == "float3x3")    return GraphicsUniformType::GraphicsUniformTypeFloat3x3;
	if (type == "float4x4")	   return GraphicsUniformType::GraphicsUniformTypeFloat4x4;
	if (type == "int[]") 	   return GraphicsUniformType::GraphicsUniformTypeIntArray;
	if (type == "int2[]") 	   return GraphicsUniformType::GraphicsUniformTypeInt2Array;
	if (type == "int3[]") 	   return GraphicsUniformType::GraphicsUniformTypeInt3Array;
	if (type == "int4[]") 	   return GraphicsUniformType::GraphicsUniformTypeInt4Array;
	if (type == "uint[]") 	   return GraphicsUniformType::GraphicsUniformTypeUIntArray;
	if (type == "uint2[]") 	   return GraphicsUniformType::GraphicsUniformTypeUInt2Array;
	if (type == "uint3[]") 	   return GraphicsUniformType::GraphicsUniformTypeUInt3Array;
	if (type == "uint4[]") 	   return GraphicsUniformType::GraphicsUniformTypeUInt4Array;
	if (type == "float[]") 	   return GraphicsUniformType::GraphicsUniformTypeFloatArray;
	if (type == "float2[]")	   return GraphicsUniformType::GraphicsUniformTypeFloat2Array;
	if (type == "float3[]")    return GraphicsUniformType::GraphicsUniformTypeFloat3Array;
	if (type == "float4[]")    return GraphicsUniformType::GraphicsUniformTypeFloat4Array;
	if (type == "float2x2[]")  return GraphicsUniformType::GraphicsUniformTypeFloat2x2Array;
	if (type == "float3x3[]")  return GraphicsUniformType::GraphicsUniformTypeFloat3x3Array;
	if (type == "float4x4[]")  return GraphicsUniformType::GraphicsUniformTypeFloat4x4Array;
	if (type == "texture2D")   return GraphicsUniformType::GraphicsUniformTypeSamplerImage;
	if (type == "texture3D")   return GraphicsUniformType::GraphicsUniformTypeSamplerImage;
	if (type == "textureCUBE")   return GraphicsUniformType::GraphicsUniformTypeSamplerImage;
	if (type == "buffer") 	   return GraphicsUniformType::GraphicsUniformTypeUniformBuffer;

	assert(false);
	return GraphicsUniformType::GraphicsUniformTypeMaxEnum;
}

GraphicsVertexType
MaterialMaker::stringToPrimitive(const std::string& primitive) noexcept
{
	if (primitive == "point")          return GraphicsVertexType::GraphicsVertexTypePointList;
	if (primitive == "line")           return GraphicsVertexType::GraphicsVertexTypeLineList;
	if (primitive == "line_strip")     return GraphicsVertexType::GraphicsVertexTypeLineStrip;
	if (primitive == "triangle")       return GraphicsVertexType::GraphicsVertexTypeTriangleList;
	if (primitive == "triangle_strip") return GraphicsVertexType::GraphicsVertexTypeTriangleStrip;
	if (primitive == "triangle_fan")   return GraphicsVertexType::GraphicsVertexTypeTriangleFan;

	assert(false);
	return GraphicsVertexType::GraphicsVertexTypeMaxEnum;
}

GraphicsCullMode
MaterialMaker::stringToCullMode(const std::string& cullmode) noexcept
{
	if (cullmode == "back")      return GraphicsCullMode::GraphicsCullModeBack;
	if (cullmode == "front")     return GraphicsCullMode::GraphicsCullModeFront;
	if (cullmode == "frontback") return GraphicsCullMode::GraphicsCullModeFrontBack;
	if (cullmode == "none")      return GraphicsCullMode::GraphicsCullModeNone;

	assert(false);
	return GraphicsCullMode::GraphicsCullModeMaxEnum;
}

GraphicsPolygonMode
MaterialMaker::stringToFillMode(const std::string& fillmode) noexcept
{
	if (fillmode == "point") return GraphicsPolygonMode::GraphicsPolygonModePoint;
	if (fillmode == "line")  return GraphicsPolygonMode::GraphicsPolygonModeWireframe;
	if (fillmode == "solid") return GraphicsPolygonMode::GraphicsPolygonModeSolid;

	assert(false);
	return GraphicsPolygonMode::GraphicsPolygonModeMaxEnum;
}

GraphicsBlendOp
MaterialMaker::stringToBlendOperation(const std::string& blendop) noexcept
{
	if (blendop == "sub")    return GraphicsBlendOp::GraphicsBlendOpSubtract;
	if (blendop == "revsub") return GraphicsBlendOp::GraphicsBlendOpRevSubtract;
	if (blendop == "add")    return GraphicsBlendOp::GraphicsBlendOpAdd;

	assert(false);
	return GraphicsBlendOp::GraphicsBlendOpMaxEnum;
}

GraphicsBlendFactor
MaterialMaker::stringToBlendFactor(const std::string& factor) noexcept
{
	if (factor == "zero")        return GraphicsBlendFactor::GraphicsBlendFactorZero;
	if (factor == "one")         return GraphicsBlendFactor::GraphicsBlendFactorOne;
	if (factor == "dstcol")      return GraphicsBlendFactor::GraphicsBlendFactorDstCol;
	if (factor == "srccol")      return GraphicsBlendFactor::GraphicsBlendFactorSrcColor;
	if (factor == "srcalpha")    return GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha;
	if (factor == "dstalpha")    return GraphicsBlendFactor::GraphicsBlendFactorDstAlpha;
	if (factor == "invsrccol")   return GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcCol;
	if (factor == "invdstcol")   return GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstCol;
	if (factor == "invsrcalpha") return GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha;
	if (factor == "invdstalpha") return GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstAlpha;

	assert(false);
	return GraphicsBlendFactor::GraphicsBlendFactorMaxEnum;
}

GraphicsColorMaskFlags
MaterialMaker::stringToColorMask(const std::string& mask) noexcept
{
	if (mask == "none")  return GraphicsColorMaskFlags(0);
	if (mask == "red")   return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRedBit;
	if (mask == "green") return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagGreendBit;
	if (mask == "blue")  return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagBlurBit;
	if (mask == "alpha") return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagAlphaBit;
	if (mask == "rgb")   return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBBit;
	if (mask == "rgba")  return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit;

	assert(false);
	return GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit;
}

GraphicsCompareFunc
MaterialMaker::stringToCompareFunc(const std::string& func) noexcept
{
	if (func == "lequal")   return GraphicsCompareFunc::GraphicsCompareFuncLequal;
	if (func == "equal")    return GraphicsCompareFunc::GraphicsCompareFuncEqual;
	if (func == "greater")  return GraphicsCompareFunc::GraphicsCompareFuncGreater;
	if (func == "less")     return GraphicsCompareFunc::GraphicsCompareFuncLess;
	if (func == "gequal")   return GraphicsCompareFunc::GraphicsCompareFuncGequal;
	if (func == "notequal") return GraphicsCompareFunc::GraphicsCompareFuncNotEqual;
	if (func == "always")   return GraphicsCompareFunc::GraphicsCompareFuncAlways;
	if (func == "never")    return GraphicsCompareFunc::GraphicsCompareFuncNever;
	if (func == "none")     return GraphicsCompareFunc::GraphicsCompareFuncNone;
	
	assert(false);
	return GraphicsCompareFunc::GraphicsCompareFuncMaxEnum;
}

GraphicsStencilOp
MaterialMaker::stringToStencilOp(const std::string& stencilop) noexcept
{
	if (stencilop == "keep")    return GraphicsStencilOp::GraphicsStencilOpKeep;
	if (stencilop == "replace") return GraphicsStencilOp::GraphicsStencilOpReplace;
	if (stencilop == "inc")     return GraphicsStencilOp::GraphicsStencilOpIncr;
	if (stencilop == "dec")     return GraphicsStencilOp::GraphicsStencilOpDecr;
	if (stencilop == "incwrap") return GraphicsStencilOp::GraphicsStencilOpIncrWrap;
	if (stencilop == "decwrap") return GraphicsStencilOp::GraphicsStencilOpDecrWrap;

	assert(false);
	return GraphicsStencilOp::GraphicsStencilOpMaxEnum;
}

GraphicsFormat
MaterialMaker::stringToFormat(const std::string& format) noexcept
{
	if (format == "R4G4UNormPack8")           return GraphicsFormat::GraphicsFormatR4G4UNormPack8;
	if (format == "R8UNorm")                  return GraphicsFormat::GraphicsFormatR8UNorm;
	if (format == "R8SNorm")                  return GraphicsFormat::GraphicsFormatR8SNorm;
	if (format == "R8UScaled")                return GraphicsFormat::GraphicsFormatR8UScaled;
	if (format == "R8SScaled")                return GraphicsFormat::GraphicsFormatR8SScaled;
	if (format == "R8UInt")                   return GraphicsFormat::GraphicsFormatR8UInt;
	if (format == "R8SInt")                   return GraphicsFormat::GraphicsFormatR8SInt;
	if (format == "R8SRGB")                   return GraphicsFormat::GraphicsFormatR8SRGB;
	if (format == "S8UInt")                   return GraphicsFormat::GraphicsFormatS8UInt;
	if (format == "R4G4B4A4UNormPack16")      return GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16;
	if (format == "B4G4R4A4UNormPack16")      return GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16;
	if (format == "R5G6B5UNormPack16")        return GraphicsFormat::GraphicsFormatR5G6B5UNormPack16;
	if (format == "B5G6R5UNormPack16")        return GraphicsFormat::GraphicsFormatB5G6R5UNormPack16;
	if (format == "R5G5B5A1UNormPack16")      return GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16;
	if (format == "B5G5R5A1UNormPack16")      return GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16;
	if (format == "A1R5G5B5UNormPack16")      return GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16;
	if (format == "R8G8UNorm")                return GraphicsFormat::GraphicsFormatR8G8UNorm;
	if (format == "R8G8SNorm")                return GraphicsFormat::GraphicsFormatR8G8SNorm;
	if (format == "R8G8UScaled")              return GraphicsFormat::GraphicsFormatR8G8UScaled;
	if (format == "R8G8SScaled")              return GraphicsFormat::GraphicsFormatR8G8SScaled;
	if (format == "R8G8UInt")                 return GraphicsFormat::GraphicsFormatR8G8UInt;
	if (format == "R8G8SInt")                 return GraphicsFormat::GraphicsFormatR8G8SInt;
	if (format == "R8G8SRGB")                 return GraphicsFormat::GraphicsFormatR8G8SRGB;
	if (format == "R16UNorm")                 return GraphicsFormat::GraphicsFormatR16UNorm;
	if (format == "R16SNorm")                 return GraphicsFormat::GraphicsFormatR16SNorm;
	if (format == "R16UScaled")               return GraphicsFormat::GraphicsFormatR16UScaled;
	if (format == "R16SScaled")               return GraphicsFormat::GraphicsFormatR16SScaled;
	if (format == "R16UInt")                  return GraphicsFormat::GraphicsFormatR16UInt;
	if (format == "R16SInt")                  return GraphicsFormat::GraphicsFormatR16SInt;
	if (format == "R16SFloat")                return GraphicsFormat::GraphicsFormatR16SFloat;
	if (format == "D16UNorm")                 return GraphicsFormat::GraphicsFormatD16UNorm;
	if (format == "R8G8B8UNorm")              return GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	if (format == "R8G8B8SNorm")              return GraphicsFormat::GraphicsFormatR8G8B8SNorm;
	if (format == "R8G8B8UScaled")            return GraphicsFormat::GraphicsFormatR8G8B8UScaled;
	if (format == "R8G8B8SScaled")            return GraphicsFormat::GraphicsFormatR8G8B8SScaled;
	if (format == "R8G8B8UInt")               return GraphicsFormat::GraphicsFormatR8G8B8UInt;
	if (format == "R8G8B8SInt")               return GraphicsFormat::GraphicsFormatR8G8B8SInt;
	if (format == "R8G8B8SRGB")               return GraphicsFormat::GraphicsFormatR8G8B8SRGB;
	if (format == "B8G8R8UNorm")              return GraphicsFormat::GraphicsFormatB8G8R8UNorm;
	if (format == "B8G8R8SNorm")              return GraphicsFormat::GraphicsFormatB8G8R8SNorm;
	if (format == "B8G8R8UScaled")            return GraphicsFormat::GraphicsFormatB8G8R8UScaled;
	if (format == "B8G8R8SScaled")            return GraphicsFormat::GraphicsFormatB8G8R8SScaled;
	if (format == "B8G8R8UInt")               return GraphicsFormat::GraphicsFormatB8G8R8UInt;
	if (format == "B8G8R8SInt")               return GraphicsFormat::GraphicsFormatB8G8R8SInt;
	if (format == "B8G8R8SRGB")               return GraphicsFormat::GraphicsFormatB8G8R8SRGB;
	if (format == "R8G8B8A8UNorm")            return GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	if (format == "R8G8B8A8SNorm")            return GraphicsFormat::GraphicsFormatR8G8B8A8SNorm;
	if (format == "R8G8B8A8UScaled")          return GraphicsFormat::GraphicsFormatR8G8B8A8UScaled;
	if (format == "R8G8B8A8SScaled")          return GraphicsFormat::GraphicsFormatR8G8B8A8SScaled;
	if (format == "R8G8B8A8UInt")             return GraphicsFormat::GraphicsFormatR8G8B8A8UInt;
	if (format == "R8G8B8A8SInt")             return GraphicsFormat::GraphicsFormatR8G8B8A8SInt;
	if (format == "R8G8B8A8SRGB")             return GraphicsFormat::GraphicsFormatR8G8B8A8SRGB;
	if (format == "B8G8R8A8UNorm")            return GraphicsFormat::GraphicsFormatB8G8R8A8UNorm;
	if (format == "B8G8R8A8SNorm")            return GraphicsFormat::GraphicsFormatB8G8R8A8SNorm;
	if (format == "B8G8R8A8UScaled")          return GraphicsFormat::GraphicsFormatB8G8R8A8UScaled;
	if (format == "B8G8R8A8SScaled")          return GraphicsFormat::GraphicsFormatB8G8R8A8SScaled;
	if (format == "B8G8R8A8UInt")             return GraphicsFormat::GraphicsFormatB8G8R8A8UInt;
	if (format == "B8G8R8A8SInt")             return GraphicsFormat::GraphicsFormatB8G8R8A8SInt;
	if (format == "B8G8R8A8SRGB")             return GraphicsFormat::GraphicsFormatB8G8R8A8SRGB;
	if (format == "A8B8G8R8UNormPack32")      return GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32;
	if (format == "A8B8G8R8SNormPack32")      return GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32;
	if (format == "A8B8G8R8UScaledPack32")    return GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32;
	if (format == "A8B8G8R8SScaledPack32")    return GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32;
	if (format == "A8B8G8R8UIntPack32")       return GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32;
	if (format == "A8B8G8R8SIntPack32")       return GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32;
	if (format == "A8B8G8R8SRGBPack32")       return GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32;
	if (format == "R16G16SInt")               return GraphicsFormat::GraphicsFormatR16G16SInt;
	if (format == "R16G16UInt")               return GraphicsFormat::GraphicsFormatR16G16UInt;
	if (format == "R16G16SNorm")              return GraphicsFormat::GraphicsFormatR16G16SNorm;
	if (format == "R16G16UNorm")              return GraphicsFormat::GraphicsFormatR16G16UNorm;
	if (format == "R16G16UScaled")            return GraphicsFormat::GraphicsFormatR16G16UScaled;
	if (format == "R16G16SScaled")            return GraphicsFormat::GraphicsFormatR16G16SScaled;
	if (format == "R16G16SFloat")             return GraphicsFormat::GraphicsFormatR16G16SFloat;
	if (format == "R32UInt")                  return GraphicsFormat::GraphicsFormatR32UInt;
	if (format == "R32SInt")                  return GraphicsFormat::GraphicsFormatR32SInt;
	if (format == "R32SFloat")                return GraphicsFormat::GraphicsFormatR32SFloat;
	if (format == "A2R10G10B10UNormPack32")   return GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32;
	if (format == "A2R10G10B10SNormPack32")   return GraphicsFormat::GraphicsFormatA2R10G10B10SNormPack32;
	if (format == "A2R10G10B10UScaledPack32") return GraphicsFormat::GraphicsFormatA2R10G10B10UScaledPack32;
	if (format == "A2R10G10B10SScaledPack32") return GraphicsFormat::GraphicsFormatA2R10G10B10SScaledPack32;
	if (format == "A2R10G10B10UIntPack32")    return GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32;
	if (format == "A2R10G10B10SIntPack32")    return GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32;
	if (format == "A2B10G10R10UNormPack32")   return GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32;
	if (format == "A2B10G10R10SNormPack32")   return GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32;
	if (format == "A2B10G10R10UScaledPack32") return GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32;
	if (format == "A2B10G10R10SScaledPack32") return GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32;
	if (format == "A2B10G10R10UIntPack32")    return GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32;
	if (format == "A2B10G10R10SIntPack32")    return GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32;
	if (format == "X8_D24UNormPack32")        return GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	if (format == "B10G11R11UFloatPack32")    return GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32;
	if (format == "E5B9G9R9UFloatPack32")     return GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32;
	if (format == "D32_SFLOAT")               return GraphicsFormat::GraphicsFormatD32_SFLOAT;
	if (format == "D16UNorm_S8UInt")          return GraphicsFormat::GraphicsFormatD16UNorm_S8UInt;
	if (format == "D24UNorm_S8UInt")          return GraphicsFormat::GraphicsFormatD24UNorm_S8UInt;
	if (format == "D32_SFLOAT_S8UInt")        return GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt;
	if (format == "R16G16B16SInt")            return GraphicsFormat::GraphicsFormatR16G16B16SInt;
	if (format == "R16G16B16UInt")            return GraphicsFormat::GraphicsFormatR16G16B16UInt;
	if (format == "R16G16B16SNorm")           return GraphicsFormat::GraphicsFormatR16G16B16SNorm;
	if (format == "R16G16B16UNorm")           return GraphicsFormat::GraphicsFormatR16G16B16UNorm;
	if (format == "R16G16B16UScaled")         return GraphicsFormat::GraphicsFormatR16G16B16UScaled;
	if (format == "R16G16B16SScaled")         return GraphicsFormat::GraphicsFormatR16G16B16SScaled;
	if (format == "R16G16B16SFloat")          return GraphicsFormat::GraphicsFormatR16G16B16SFloat;
	if (format == "R32G32SFloat")             return GraphicsFormat::GraphicsFormatR32G32SFloat;
	if (format == "R32G32SInt")               return GraphicsFormat::GraphicsFormatR32G32SInt;
	if (format == "R32G32UInt")               return GraphicsFormat::GraphicsFormatR32G32UInt;
	if (format == "R16G16B16A16SNorm")        return GraphicsFormat::GraphicsFormatR16G16B16A16SNorm;
	if (format == "R16G16B16A16UNorm")        return GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	if (format == "R16G16B16A16SScaled")      return GraphicsFormat::GraphicsFormatR16G16B16A16SScaled;
	if (format == "R16G16B16A16UScaled")      return GraphicsFormat::GraphicsFormatR16G16B16A16UScaled;
	if (format == "R16G16B16A16SFloat")       return GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	if (format == "R16G16B16A16SInt")         return GraphicsFormat::GraphicsFormatR16G16B16A16SInt;
	if (format == "R16G16B16A16UInt")         return GraphicsFormat::GraphicsFormatR16G16B16A16UInt;
	if (format == "R64UInt")                  return GraphicsFormat::GraphicsFormatR64UInt;
	if (format == "R64SInt")                  return GraphicsFormat::GraphicsFormatR64SInt;
	if (format == "R64SFloat")                return GraphicsFormat::GraphicsFormatR64SFloat;
	if (format == "R32G32B32SFloat")          return GraphicsFormat::GraphicsFormatR32G32B32SFloat;
	if (format == "R32G32B32SInt")            return GraphicsFormat::GraphicsFormatR32G32B32SInt;
	if (format == "R32G32B32UInt")            return GraphicsFormat::GraphicsFormatR32G32B32UInt;
	if (format == "R32G32B32A32SFloat")       return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
	if (format == "R32G32B32A32SInt")         return GraphicsFormat::GraphicsFormatR32G32B32A32SInt;
	if (format == "R32G32B32A32UInt")         return GraphicsFormat::GraphicsFormatR32G32B32A32UInt;
	if (format == "R64G64UInt")               return GraphicsFormat::GraphicsFormatR64G64UInt;
	if (format == "R64G64SInt")               return GraphicsFormat::GraphicsFormatR64G64SInt;
	if (format == "R64G64SFloat")             return GraphicsFormat::GraphicsFormatR64G64SFloat;
	if (format == "R64G64B64UInt")            return GraphicsFormat::GraphicsFormatR64G64B64UInt;
	if (format == "R64G64B64SInt")            return GraphicsFormat::GraphicsFormatR64G64B64SInt;
	if (format == "R64G64B64SFloat")          return GraphicsFormat::GraphicsFormatR64G64B64SFloat;
	if (format == "R64G64B64A64UInt")         return GraphicsFormat::GraphicsFormatR64G64B64A64UInt;
	if (format == "R64G64B64A64SInt")         return GraphicsFormat::GraphicsFormatR64G64B64A64SInt;
	if (format == "R64G64B64A64SFloat")       return GraphicsFormat::GraphicsFormatR64G64B64A64SFloat;

	assert(false);
	return GraphicsFormat::GraphicsFormatMaxEnum;
}

GlobalSemanticType 
MaterialMaker::stringToSemanticType(const std::string& type) noexcept
{
	if (type == "matModel") return GlobalSemanticType::GlobalSemanticTypeModel;
	if (type == "matModelInverse") return GlobalSemanticType::GlobalSemanticTypeModelInverse;
	if (type == "matView") return GlobalSemanticType::GlobalSemanticTypeView;
	if (type == "matViewInverse") return GlobalSemanticType::GlobalSemanticTypeViewInverse;
	if (type == "matProject") return GlobalSemanticType::GlobalSemanticTypeProject;
	if (type == "matProjectInverse") return GlobalSemanticType::GlobalSemanticTypeProjectInverse;
	if (type == "matViewProject") return GlobalSemanticType::GlobalSemanticTypeViewProject;
	if (type == "matViewProjectInverse") return GlobalSemanticType::GlobalSemanticTypeViewProjectInverse;
	if (type == "matModelView") return GlobalSemanticType::GlobalSemanticTypeModelView;
	if (type == "matModelViewProject") return GlobalSemanticType::GlobalSemanticTypeModelViewProject;
	if (type == "matModelViewInverse") return GlobalSemanticType::GlobalSemanticTypeModelViewInverse;
	if (type == "CameraAperture") return GlobalSemanticType::GlobalSemanticTypeCameraAperture;
	if (type == "CameraNear") return GlobalSemanticType::GlobalSemanticTypeCameraNear;
	if (type == "CameraFar") return GlobalSemanticType::GlobalSemanticTypeCameraFar;
	if (type == "CameraPosition") return GlobalSemanticType::GlobalSemanticTypeCameraPosition;
	if (type == "CameraDirection") return GlobalSemanticType::GlobalSemanticTypeCameraDirection;
	if (type == "DepthMap") return GlobalSemanticType::GlobalSemanticTypeDepthMap;
	if (type == "DepthLinearMap") return GlobalSemanticType::GlobalSemanticTypeDepthLinearMap;
	if (type == "DiffuseMap") return GlobalSemanticType::GlobalSemanticTypeDiffuseMap;
	if (type == "NormalMap") return GlobalSemanticType::GlobalSemanticTypeNormalMap;
	if (type == "LightingMap") return GlobalSemanticType::GlobalSemanticTypeLightingMap;

	assert(false);
	return GlobalSemanticType::GlobalSemanticTypeNone;
}

_NAME_END