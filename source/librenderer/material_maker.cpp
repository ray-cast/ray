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

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#include <glsl/glsl_optimizer.h>

_NAME_BEGIN

MaterialMaker::MaterialMaker() noexcept
{
}

MaterialMaker::~MaterialMaker() noexcept
{
}

void
MaterialMaker::instanceInputLayout(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
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
			std::string layoutFormat = reader.getValue<std::string>("format");

			if (layoutName.empty())
				throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

			GraphicsFormat format = stringToFormat(layoutFormat);
			if (format == GraphicsFormat::GraphicsFormatMaxEnum)
				throw failure(__TEXT("Undefined format: ") + reader.getCurrentNodePath());

			inputLayoutDesc.addComponent(GraphicsVertexLayout(layoutName, 0, format));
		}

	} while (reader.setToNextChild());

	inputLayout = manager.createInputLayout(inputLayoutName, inputLayoutDesc);
	if (!inputLayout)
		throw failure(__TEXT("Can't create input layout") + reader.getCurrentNodeName());
}

void
MaterialMaker::instanceCodes(MaterialManager& manager, iarchive& reader) except
{
	std::string name = reader.getValue<std::string>("name");
	if (name.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	std::string str = reader.getText();
	util::str2hex(_shaderCodes[name], str.c_str(), str.size());
}

void
MaterialMaker::instanceShader(MaterialManager& manager, MaterialDesc& material, GraphicsProgramDesc& programDesc, iarchive& reader) except
{
	std::string type = reader.getValue<std::string>("name");
	std::string value = reader.getValue<std::string>("value");

	if (type.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	if (value.empty())
		throw failure(__TEXT("Empty shader entrypoint : ") + reader.getCurrentNodePath());

	GraphicsShaderStage shaderStage = stringToShaderStage(type);
	if (shaderStage == GraphicsShaderStage::GraphicsShaderStageMaxEnum)
		throw failure(__TEXT("Unknown shader type : ") + type + reader.getCurrentNodePath());

	std::vector<char>& bytecodes = _shaderCodes[value];
	if (bytecodes.empty())
		throw failure(__TEXT("Empty shader code : ") + value + reader.getCurrentNodePath());

	GraphicsShaderDesc shaderDesc;
	shaderDesc.setStage(shaderStage);

	GraphicsDeviceType deviceType = manager.getDeviceType();
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
	{
		std::uint32_t flags = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES;
		if (shaderStage == GraphicsShaderStage::GraphicsShaderStageGeometry)
			flags = HLSLCC_FLAG_GS_ENABLED;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageTessControl)
			flags = HLSLCC_FLAG_TESS_ENABLED;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageTessEvaluation)
			flags = HLSLCC_FLAG_TESS_ENABLED;

		GlExtensions extensions;
		std::memset(&extensions, 0, sizeof(extensions));
		if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		{
			extensions.ARB_shading_language_420pack = true;
			extensions.ARB_explicit_attrib_location = false;
			extensions.ARB_explicit_uniform_location = true;
			flags |= HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;
		}
		else
		{
			flags |= HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
		}

		GLLang hlsl2glslLangType;
		if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2 ||
			deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
			deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31)
		{
			hlsl2glslLangType = GLLang::LANG_ES_300;
		}
		else
		{
			hlsl2glslLangType = GLLang::LANG_DEFAULT;
		}

		GLSLShader shader;
		GLSLCrossDependencyData dependency;
		if (!TranslateHLSLFromMem(bytecodes.data(), flags, hlsl2glslLangType, &extensions, &dependency, &shader))
		{
			FreeGLSLShader(&shader);
			throw failure(__TEXT("Can't conv hlsl bytecodes to glsl.") + value);
		}

		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangGLSL);
		shaderDesc.setByteCodes(shader.sourceCode);

		FreeGLSLShader(&shader);

		/*if (hlsl2glslLangType == GLLang::LANG_ES_100 ||
			hlsl2glslLangType == GLLang::LANG_ES_300 ||
			hlsl2glslLangType == GLLang::LANG_ES_310)
		{
			if (shaderStage == GraphicsShaderStage::GraphicsShaderStageVertex || shaderStage == GraphicsShaderStage::GraphicsShaderStageFragment)
			{
				glslopt_shader_type glslopt_type = glslopt_shader_type::kGlslOptShaderVertex;
				if (shaderStage == GraphicsShaderStage::GraphicsShaderStageFragment)
					glslopt_type = glslopt_shader_type::kGlslOptShaderFragment;
				
				glslopt_target glslopt_target_type = glslopt_target::kGlslTargetOpenGLES20;
				if (hlsl2glslLangType == GLLang::LANG_ES_300 || hlsl2glslLangType == GLLang::LANG_ES_310)
					glslopt_target_type = glslopt_target::kGlslTargetOpenGLES30;

				auto ctx = glslopt_initialize(glslopt_target_type);
				if (ctx)
				{
					glslopt_shader* glslopt_shader = glslopt_optimize(ctx, glslopt_type, shaderDesc.getByteCodes().c_str(), 0);
					bool optimizeOk = glslopt_get_status(glslopt_shader);
					if (optimizeOk)
					{
						std::string textOpt = glslopt_get_output(glslopt_shader);
						shaderDesc.setByteCodes(textOpt);
					}

					glslopt_cleanup(ctx);
				}
			}
		}*/
	}
	else
	{
		shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangHLSL);
		shaderDesc.setByteCodes(std::string(bytecodes.data(), bytecodes.size()));
	}

	auto shaderModule = manager.createShader(shaderDesc);
	if (!shaderModule)
		throw failure(__TEXT("Can't create shader : ") + reader.getCurrentNodePath());

	programDesc.addShader(shaderModule);
}

void
MaterialMaker::instancePass(MaterialManager& manager, MaterialDesc& material, MaterialTechPtr& tech, iarchive& reader) except
{
	std::string passName;
	reader.getValue("name", passName);
	if (passName.empty())
		throw failure(__TEXT("Empty pass name : ") + reader.getCurrentNodePath());

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());
	
	GraphicsStateDesc stateDesc;
	GraphicsProgramDesc programDesc;
	GraphicsInputLayoutPtr inputLayout;

	std::string name;

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
		else if (name == "blend")
			stateDesc.setBlendEnable(reader.getValue<bool>("value"));
		else if (name == "blendSeparate")
			stateDesc.setBlendSeparateEnable(reader.getValue<bool>("value"));
		else if (name == "blendOp")
			stateDesc.setBlendOp(stringToBlendOperation(reader.getValue<std::string>("value")));
		else if (name == "blendsrc")
			stateDesc.setBlendSrc(stringToBlendFactor(reader.getValue<std::string>("value")));
		else if (name == "blenddst")
			stateDesc.setBlendDest(stringToBlendFactor(reader.getValue<std::string>("value")));
		else if (name == "blendalphaop")
			stateDesc.setBlendAlphaOp(stringToBlendOperation(reader.getValue<std::string>("value")));
		else if (name == "blendalphasrc")
			stateDesc.setBlendAlphaSrc(stringToBlendFactor(reader.getValue<std::string>("value")));
		else if (name == "blendalphadst")
			stateDesc.setBlendAlphaDest(stringToBlendFactor(reader.getValue<std::string>("value")));
		else if (name == "colormask")
			stateDesc.setColorWriteMask(stringToColorMask(reader.getValue<std::string>("value")));
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

	auto state = manager.createRenderState(stateDesc);
	if (!state)
		throw failure(__TEXT("Can't create render state : ") + reader.getCurrentNodePath());

	auto program = manager.createProgram(programDesc);
	if (!state)
		throw failure(__TEXT("Can't create program : ") + reader.getCurrentNodePath());

	auto pass = std::make_shared<MaterialPass>();
	pass->setName(passName);
	pass->setGraphicsState(state);
	pass->setGraphicsProgram(program);
	pass->setGraphicsInputLayout(inputLayout);

	tech->addPass(pass);
}

void
MaterialMaker::instanceTech(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	std::string techName = reader.getValue<std::string>("name");
	if (techName.empty())
		throw failure(__TEXT("The technique name can not be empty"));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	auto tech = std::make_shared<MaterialTech>();
	tech->setName(techName);

	do
	{
		auto name = reader.getCurrentNodeName();
		if (name == "pass")
		{
			this->instancePass(manager, material, tech, reader);
		}
	} while (reader.setToNextChild());

	material.addTech(tech);
}

void
MaterialMaker::instanceParameter(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
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

	auto pos = name.find_first_of('[');
	if (pos != std::string::npos)
		name = name.substr(0, pos);

	auto param = std::make_shared<MaterialParam>();
	param->setName(name);
	param->setType(uniformType);

	if (!semantic.empty())
	{
		auto materialSemantic = manager.getSemantic(semantic);
		if (!materialSemantic)
			throw failure(__TEXT("Unknown semantic : ") + semantic);

		param->setSemantic(materialSemantic);
	}

	material.addParameter(param);
}

void
MaterialMaker::instanceMacro(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	if (!type.empty())
	{
		auto macro = std::make_shared<MaterialParam>();
		macro->setName(name);
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
			throw failure(__TEXT("Unknown macro type : ") + name);
		}

		material.addMacro(macro);
	}
}

void
MaterialMaker::instanceSampler(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	std::string samplerName = reader.getValue<std::string>("name");
	if (samplerName.empty())
		throw failure(__TEXT("Empty sampler empty"));

	auto sampler = manager.getSampler(samplerName);
	if (sampler)
		return;

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	GraphicsSamplerDesc samplerDesc;

	std::string stateName;
	std::string stateValue;
	
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
			else if (stateValue == "point_mip_nearest")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
			else if (stateValue == "point_mip_lienar")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapLinear);
			else if (stateValue == "linear")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
			else if (stateValue == "linear_mip_nearest")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest);
			else if (stateValue == "linear_mip_linear")
				samplerDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear);
			else
				throw failure(__TEXT("Unknown sampler filter type") + reader.getCurrentNodePath());

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
	
	sampler = manager.createSampler(samplerName, samplerDesc);
	if (!sampler)
		throw failure(__TEXT("Can't create sampler ") + reader.getCurrentNodePath());
}

void
MaterialMaker::instanceBuffer(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	auto buffer = std::make_shared<MaterialParam>();
	buffer->setType(GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
	buffer->setName(reader.getValue<std::string>("name"));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("Empty child : ") + reader.getCurrentNodePath());

	do
	{
	} while (reader.setToNextChild());
}

void
MaterialMaker::instanceInclude(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	auto path = reader.getValue<std::string>("name");
	if (!path.empty())
	{
		this->load(manager, path);
	}
}

MaterialPtr
MaterialMaker::load(MaterialManager& manager, const std::string& filename) noexcept
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
			return this->load(manager, reader);
		}

		return nullptr;
	}
	catch (const failure& e)
	{
		std::cout << __TEXT("in ") + filename + __TEXT(" ") + e.message() + e.stack();
		return nullptr;
	}
}

bool
MaterialMaker::load(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except
{
	std::string nodeName;
	nodeName = reader.getCurrentNodeName();
	if (nodeName != "material" && nodeName != "effect")
		throw failure("Unknown node name " + nodeName + ", so I can't open it");

	std::string language = reader.getValue<std::string>("language");
	if (language != "bytecodes")
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

MaterialPtr
MaterialMaker::load(MaterialManager& manager, iarchive& reader) except
{
	std::string nodeName;
	nodeName = reader.getCurrentNodeName();
	if (nodeName != "material" && nodeName != "effect")
		throw failure(__TEXT("Unknown node name " + nodeName + ", so I can't open it"));

	if (nodeName == "material")
	{
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

		if (!name.empty())
		{
			MaterialMaker maker;
			auto material = maker.load(manager, name);
			if (material)
			{
				for (auto& arg : args)
				{
					auto param = material->getParameter(arg.first);
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
					case GraphicsUniformType::GraphicsUniformTypeStorageImage:
						param->uniformTexture(RenderSystem::instance()->createTexture(arg.second, GraphicsTextureDim::GraphicsTextureDim2D));
						break;
					default:
						assert(false);
					}
				}

				return material;
			}
		}
	}
	else if (nodeName == "effect")
	{
		MaterialDesc materialDesc;
		if (this->load(manager, materialDesc, reader))
		{
			auto material = std::make_shared<Material>();
			material->setup(materialDesc);
			return material;
		}
	}

	return nullptr;
}

GraphicsShaderStage
MaterialMaker::stringToShaderStage(const std::string& stage) noexcept
{
	if (stage == "vertex")   return GraphicsShaderStage::GraphicsShaderStageVertex;
	if (stage == "fragment") return GraphicsShaderStage::GraphicsShaderStageFragment;
	if (stage == "geometry") return GraphicsShaderStage::GraphicsShaderStageGeometry;
	if (stage == "compute")  return GraphicsShaderStage::GraphicsShaderStageCompute;

	assert(false);
	return GraphicsShaderStage::GraphicsShaderStageMaxEnum;
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
	if (type == "texture2D")   return GraphicsUniformType::GraphicsUniformTypeStorageImage;
	if (type == "texture3D")   return GraphicsUniformType::GraphicsUniformTypeStorageImage;
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

GraphicsColorMask
MaterialMaker::stringToColorMask(const std::string& mask) noexcept
{
	if (mask == "red")   return GraphicsColorMask::GraphicsColorMaskR;
	if (mask == "green") return GraphicsColorMask::GraphicsColorMaskG;
	if (mask == "blue")  return GraphicsColorMask::GraphicsColorMaskB;
	if (mask == "alpha") return GraphicsColorMask::GraphicsColorMaskA;
	if (mask == "rgb")   return GraphicsColorMask::GraphicsColorMaskRGB;
	if (mask == "rgba")  return GraphicsColorMask::GraphicsColorMaskRGBA;

	assert(false);
	return GraphicsColorMask::GraphicsColorMaskRGBA;
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

_NAME_END