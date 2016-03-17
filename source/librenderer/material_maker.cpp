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
#include <ray/graphics_state.h>
#include <ray/graphics_device.h>
#include <ray/graphics_shader.h>
#include <ray/render_system.h>
#include <ray/ioserver.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>

_NAME_BEGIN

MaterialMaker::MaterialMaker() noexcept
{
}

MaterialMaker::~MaterialMaker() noexcept
{
}

void
MaterialMaker::instanceCodes(MaterialManager& manager, iarchive& reader) except
{
	std::string name = reader.getValue<std::string>("name");
	if (name.empty())
		throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());

	std::string str = reader.getText();
	ray::util::str2hex(_shaderCodes[name], str.c_str(), str.size());
}

void
MaterialMaker::instanceShader(MaterialManager& manager, ShaderObjectDesc& program, iarchive& reader) except
{
	std::string type;
	reader.getValue("name", type);

	std::string value;
	reader.getValue("value", value);

	if (type.empty())
		throw failure(__TEXT("The shader name can not be empty"));

	if (value.empty())
		throw failure(__TEXT("The shader value can not be empty"));

	GraphicsShaderStage shaderType;

	if (type == "vertex")
		shaderType = GraphicsShaderStage::GraphicsShaderStageVertex;
	else if (type == "fragment")
		shaderType = GraphicsShaderStage::GraphicsShaderStageFragment;
	else if (type == "geometry")
		shaderType = GraphicsShaderStage::GraphicsShaderStageGeometry;
	else if (type == "compute")
		shaderType = GraphicsShaderStage::GraphicsShaderStageCompute;
	else
		throw failure(__TEXT("Unknown shader type : ") + type);

	std::vector<char>& bytecodes = _shaderCodes[value];
	if (bytecodes.empty())
		throw failure(__TEXT("Empty shader code : ") + value);

	ShaderDesc shader;
	shader.setType(shaderType);
	shader.setByteCodes(bytecodes);

	program.addShader(shader);
}

void
MaterialMaker::instancePass(MaterialManager& manager, MaterialTechPtr& tech, iarchive& reader) except
{
	RenderPass passType = RenderPass::RP_CUSTOM;

	std::string passName;
	reader.getValue("name", passName);
	if (passName.empty())
		throw failure(__TEXT("The pass name can not be empty"));

	if (passName == "custom")
		passType = RenderPass::RP_CUSTOM;
	else if (passName == "shadow")
		passType = RenderPass::RP_SHADOW;
	else if (passName == "opaque")
		passType = RenderPass::RP_OPAQUES;
	else if (passName == "transparent")
		passType = RenderPass::RP_TRANSPARENT;
	else if (passName == "light")
		passType = RenderPass::RP_LIGHTS;
	else if (passName == "specific")
		passType = RenderPass::RP_SPECIFIC;
	else if (passName == "postprocess")
		passType = RenderPass::RP_POSTPROCESS;

	auto pass = std::make_shared<MaterialPass>(passType);
	pass->setName(passName);

	if (reader.setToFirstChild())
	{
		RenderDepthState depthState;
		RenderRasterState rasterState;
		RenderBlendState blendState;
		RenderStencilState stencilState;

		ShaderObjectDesc shaderObjectDesc;

		do
		{
			auto nodeName = reader.getCurrentNodeName();
			if (nodeName != "state")
				throw failure(__TEXT("Unkonwn node name ") + nodeName + reader.getCurrentNodePath());

			std::string name = reader.getValue<std::string>("name");
			std::string value = reader.getValue<std::string>("value");

			if (name == "vertex")
				this->instanceShader(manager, shaderObjectDesc, reader);
			else if (name == "fragment")
				this->instanceShader(manager, shaderObjectDesc, reader);
			else if (name == "cullmode")
				rasterState.cullMode = stringToCullMode(reader.getValue<std::string>("value"));
			else if (name == "fillmode")
				rasterState.fillMode = stringToFillMode(reader.getValue<std::string>("value"));
			else if (name == "scissorTestEnable")
				rasterState.scissorTestEnable = reader.getValue<bool>("value");
			else if (name == "primitive")
				rasterState.primitiveType = stringToPrimitive(reader.getValue<std::string>("value"));
			else if (name == "multisampleEnable")
				rasterState.multisampleEnable = reader.getValue<bool>("value");
			else if (name == "srgbEnable")
				rasterState.srgbEnable = reader.getValue<bool>("value");
			else if (name == "blend")
				blendState.blendEnable = reader.getValue<bool>("value");
			else if (name == "blendSeparate")
				blendState.blendSeparateEnable = reader.getValue<bool>("value");
			else if (name == "blendOp")
				blendState.blendOp = stringToBlendOperation(reader.getValue<std::string>("value"));
			else if (name == "blendsrc")
				blendState.blendSrc = stringToBlendFactor(reader.getValue<std::string>("value"));
			else if (name == "blenddst")
				blendState.blendDest = stringToBlendFactor(reader.getValue<std::string>("value"));
			else if (name == "blendalphaop")
				blendState.blendAlphaOp = stringToBlendOperation(reader.getValue<std::string>("value"));
			else if (name == "blendalphasrc")
				blendState.blendAlphaSrc = stringToBlendFactor(reader.getValue<std::string>("value"));
			else if (name == "blendalphadst")
				blendState.blendAlphaDest = stringToBlendFactor(reader.getValue<std::string>("value"));
			else if (name == "colormask")
				blendState.colorWriteMask = stringToColorMask(reader.getValue<std::string>("value"));
			else if (name == "depthtest")
				depthState.depthEnable = reader.getValue<bool>("value");
			else if (name == "depthwrite")
				depthState.depthWriteMask = reader.getValue<bool>("value");
			else if (name == "depthfunc")
				depthState.depthFunc = stringToCompareFunc(reader.getValue<std::string>("value"));
			else if (name == "depthBiasEnable")
				depthState.depthBiasEnable = reader.getValue<bool>("value");
			else if (name == "depthSlopScaleBias")
				depthState.depthSlopScaleBias = reader.getValue<float>("value");
			else if (name == "depthBias")
				depthState.depthBias = reader.getValue<float>("value");
			else if (name == "stencilTest")
				stencilState.stencilEnable = reader.getValue<bool>("value");
			else if (name == "stencilRef")
				stencilState.stencilRef = reader.getValue<int>("value");
			else if (name == "stencilFunc")
				stencilState.stencilFunc = stringToCompareFunc(reader.getValue<std::string>("value"));
			else if (name == "stencilReadMask")
				stencilState.stencilReadMask = reader.getValue<int>("value");
			else if (name == "stencilWriteMask")
				stencilState.stencilWriteMask = reader.getValue<int>("value");
			else if (name == "stencilFail")
				stencilState.stencilFail = stringToStencilOp(reader.getValue<std::string>("value"));
			else if (name == "stencilZFail")
				stencilState.stencilZFail = stringToStencilOp(reader.getValue<std::string>("value"));
			else if (name == "stencilPass")
				stencilState.stencilPass = stringToStencilOp(reader.getValue<std::string>("value"));
			else if (name == "stencilTwoTest")
				stencilState.stencilTwoEnable = reader.getValue<bool>("value");
			else if (name == "stencilTwoFunc")
				stencilState.stencilTwoFunc = stringToCompareFunc(reader.getValue<std::string>("value"));
			else if (name == "stencilTwoReadMask")
				stencilState.stencilTwoReadMask = reader.getValue<int>("value");
			else if (name == "stencilTwoWriteMask")
				stencilState.stencilTwoWriteMask = reader.getValue<int>("value");
			else if (name == "stencilTwoFail")
				stencilState.stencilTwoFail = stringToStencilOp(reader.getValue<std::string>("value"));
			else if (name == "stencilTwoZFail")
				stencilState.stencilTwoZFail = stringToStencilOp(reader.getValue<std::string>("value"));
			else if (name == "stencilTwoPass")
				stencilState.stencilTwoPass = stringToStencilOp(reader.getValue<std::string>("value"));

		} while (reader.setToNextChild());

		GraphicsStateDesc stateDesc;
		stateDesc.setDepthState(depthState);
		stateDesc.setBlendState(blendState);
		stateDesc.setRasterState(rasterState);
		stateDesc.setStencilState(stencilState);

		auto state = manager.getGraphicsDevice()->createGraphicsState(stateDesc);
		auto program = manager.getGraphicsDevice()->createShaderProgram(shaderObjectDesc);

		pass->setGraphicsState(state);
		pass->setGraphicsProgram(program);
		tech->addPass(pass);
	}
}

void
MaterialMaker::instanceTech(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	RenderQueue queue = RenderQueue::RQ_OPAQUE;

	std::string techName = reader.getValue<std::string>("name");
	if (techName.empty())
		throw failure(__TEXT("The technique name can not be empty"));

	if (techName == "custom")
		queue = RenderQueue::RQ_CUSTOM;
	else if (techName == "opaque")
		queue = RenderQueue::RQ_OPAQUE;
	else if (techName == "transparent")
		queue = RenderQueue::RQ_TRANSPARENT;
	else if (techName == "lighting")
		queue = RenderQueue::RQ_LIGHTING;
	else if (techName == "postprocess")
		queue = RenderQueue::RQ_POSTPROCESS;
	else
	{
		throw failure(__TEXT("Unknown technique name : ") + techName);
	}

	auto tech = std::make_shared<MaterialTech>(queue);

	if (reader.setToFirstChild())
	{
		do
		{
			auto name = reader.getCurrentNodeName();
			if (name == "pass")
			{
				this->instancePass(manager, tech, reader);
			}
		} while (reader.setToNextChild());
	}

	material->addTech(tech);
}

void
MaterialMaker::instanceParameter(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	if (!type.empty())
	{
		auto param = std::make_shared<MaterialParam>();
		param->setName(name);
		if (type == "bool")
			param->setType(GraphicsVariantType::GraphicsVariantTypeBool);
		else if (type == "int")
			param->setType(GraphicsVariantType::GraphicsVariantTypeInt);
		else if (type == "int2")
			param->setType(GraphicsVariantType::GraphicsVariantTypeInt2);
		else if (type == "int3")
			param->setType(GraphicsVariantType::GraphicsVariantTypeInt3);
		else if (type == "int4")
			param->setType(GraphicsVariantType::GraphicsVariantTypeInt4);
		else if (type == "float")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat);
		else if (type == "float2")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat2);
		else if (type == "float3")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat3);
		else if (type == "float4")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat4);
		else if (type == "mat3")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat3x3);
		else if (type == "mat4")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat4x4);
		else if (type == "float3x3")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat3x3);
		else if (type == "float4x4")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat4x4);
		else if (type == "float[]")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloatArray);
		else if (type == "float2[]")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat2Array);
		else if (type == "float3[]")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat3Array);
		else if (type == "float4[]")
			param->setType(GraphicsVariantType::GraphicsVariantTypeFloat4Array);
		else if (type == "texture")
			param->setType(GraphicsVariantType::GraphicsVariantTypeTexture);
		else if (type == "buffer")
			param->setType(GraphicsVariantType::GraphicsVariantTypeBuffer);
		else
		{
			assert(false);
			throw failure(__TEXT("Unknown parameter type : ") + name);
		}

		material->addParameter(param);
	}
	else if (!semantic.empty())
	{
		auto materialSemantic = manager.getSemantic(semantic);
		if (!materialSemantic)
			throw failure(__TEXT("Unknown semantic : ") + semantic);

		auto param = std::make_shared<MaterialParam>();
		param->setName(name);
		param->setSemantic(materialSemantic);
		material->addParameter(param);
	}
}

void
MaterialMaker::instanceMacro(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	auto name = reader.getValue<std::string>("name");
	auto type = reader.getValue<std::string>("type");
	auto semantic = reader.getValue<std::string>("semantic");
	auto value = reader.getValue<std::string>("value");

	if (name.empty())
		throw failure(__TEXT("The parameter name can not be empty"));

	if (!type.empty())
	{
		auto macro = std::make_shared<MaterialVariant>();
		macro->setName(name);
		if (type == "bool")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeBool);
			macro->assign(reader.getValue<bool>("value"));
		}
		else if (type == "int")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeInt);
			macro->assign(reader.getValue<int1>("value"));
		}
		else if (type == "int2")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeInt2);
			macro->assign(reader.getValue<int2>("value"));
		}
		else if (type == "int3")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeInt3);
			macro->assign(reader.getValue<int3>("value"));
		}
		else if (type == "int4")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeInt4);
			macro->assign(reader.getValue<int4>("value"));
		}
		else if (type == "float")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeFloat);
			macro->assign(reader.getValue<float1>("value"));
		}
		else if (type == "float2")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeFloat2);
			macro->assign(reader.getValue<float2>("value"));
		}
		else if (type == "float3")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeFloat3);
			macro->assign(reader.getValue<float3>("value"));
		}
		else if (type == "float4")
		{
			macro->setType(GraphicsVariantType::GraphicsVariantTypeFloat4);
			macro->assign(reader.getValue<float4>("value"));
		}
		else
		{
			assert(false);
			throw failure(__TEXT("Unknown macro type : ") + name);
		}

		material->addMacro(macro);
	}
}

void
MaterialMaker::instanceSampler(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	std::string samplerName;
	reader.getValue("name", samplerName);

	if (samplerName.empty())
		throw failure(__TEXT("Empty sampler empty"));

	if (reader.setToFirstChild())
	{
		GraphicsSamplerDesc desc;
		desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapRepeat);

		do
		{
			std::string stateName = reader.getValue<std::string>("name");
			std::string stateValue = reader.getValue<std::string>("value");
			if (stateName == "filter")
			{
				if (stateValue == "point")
					desc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
				else if (stateValue == "linear")
					desc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
			}
			else if (stateName == "wrap")
			{
				if (stateValue == "clamp")
					desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
				else if (stateValue == "mirror")
					desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapMirror);
				else if (stateValue == "repeat")
					desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapRepeat);
			}

		} while (reader.setToNextChild());

		auto param = std::make_shared<MaterialParam>();
		param->setName(samplerName);
		param->assign(manager.getGraphicsDevice()->createGraphicsSampler(desc));	

		material->addParameter(param);
	}
}

void
MaterialMaker::instanceBuffer(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	auto buffer = std::make_shared<MaterialParam>();
	buffer->setType(GraphicsVariantType::GraphicsVariantTypeBuffer);
	buffer->setName(reader.getValue<std::string>("name"));

	if (reader.setToFirstChild())
	{
		do
		{
		} while (reader.setToNextChild());
	}
}

void 
MaterialMaker::instanceInclude(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	auto path = reader.getValue<std::string>("name");
	if (!path.empty())
	{
		this->load(manager, path);
	}
}

MaterialPtr
MaterialMaker::load(MaterialManager& manager, const std::string& filename) except
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
		throw failure(__TEXT("in ") + filename + __TEXT(" ") + e.message(), e.stack());
	}
}

MaterialPtr 
MaterialMaker::load(MaterialManager& manager, MaterialPtr& material, iarchive& reader) except
{
	std::string nodeName;
	nodeName = reader.getCurrentNodeName();
	if (nodeName != "material" && nodeName != "effect")
		throw failure("Unknown node name " + nodeName + ", so I can't open it");

	std::string language = reader.getValue<std::string>("language");
	if (language != "bytecodes")
		throw failure("Can't support language : " + language);

	if (!reader.setToFirstChild())
		throw failure("The file has been damaged and can't be recovered, so I can't open it");

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
	} while (reader.setToNextChild());

	return material;
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
					if (param)
					{
						auto type = param->getType();
						switch (type)
						{
						case GraphicsVariantType::GraphicsVariantTypeFloat:
							param->assign(parseFloat<Float>(arg.second));
							break;
						case GraphicsVariantType::GraphicsVariantTypeFloat2:
							param->assign(parseFloat2(arg.second));
							break;
						case GraphicsVariantType::GraphicsVariantTypeFloat3:
							param->assign(parseFloat3(arg.second));
							break;
						case GraphicsVariantType::GraphicsVariantTypeFloat4:
							param->assign(parseFloat4(arg.second));
							break;
						case GraphicsVariantType::GraphicsVariantTypeTexture:
							param->assign(RenderSystem::instance()->createTexture(arg.second));
							break;
						}
					}
				}

				return material;
			}
		}
	}
	else if (nodeName == "effect")
	{
		auto material = std::make_shared<Material>();
		if (this->load(manager, material, reader))
		{
			material->setup();
			return material;
		}	
	}

	return nullptr;
}

GraphicsVertexType
MaterialMaker::stringToPrimitive(const std::string& primitive) noexcept
{
	if (primitive == "point")
		return GraphicsVertexType::GraphicsVertexTypePointList;
	else if (primitive == "line")
		return GraphicsVertexType::GraphicsVertexTypeLineList;
	else if (primitive == "line_strip")
		return GraphicsVertexType::GraphicsVertexTypeLineStrip;
	else if (primitive == "triangle")
		return GraphicsVertexType::GraphicsVertexTypeTriangleList;
	else if (primitive == "triangle_strip")
		return GraphicsVertexType::GraphicsVertexTypeTriangleStrip;
	else if (primitive == "triangle_fan")
		return GraphicsVertexType::GraphicsVertexTypeTriangleFan;
	else
	{
		assert(false);
		return GraphicsVertexType::GraphicsVertexTypeTriangleList;
	}
}

GraphicsCullMode
MaterialMaker::stringToCullMode(const std::string& cullmode) noexcept
{
	if (cullmode == "back")
		return GraphicsCullMode::GraphicsCullModeBack;
	else if (cullmode == "front")
		return GraphicsCullMode::GraphicsCullModeFront;
	else if (cullmode == "frontback")
		return GraphicsCullMode::GraphicsCullModeFrontBack;
	else if (cullmode == "none")
		return GraphicsCullMode::GraphicsCullModeNone;
	else
	{
		assert(false);
		return GraphicsCullMode::GraphicsCullModeNone;
	}
}

GraphicsPolygonMode
MaterialMaker::stringToFillMode(const std::string& fillmode) noexcept
{
	if (fillmode == "point")
		return GraphicsPolygonMode::GraphicsPolygonModePoint;
	else if (fillmode == "line")
		return GraphicsPolygonMode::GraphicsPolygonModeWireframe;
	else if (fillmode == "solid")
		return GraphicsPolygonMode::GraphicsPolygonModeSolid;
	else
	{
		assert(false);
		return GraphicsPolygonMode::GraphicsPolygonModeSolid;
	}
}

GraphicsBlendOp
MaterialMaker::stringToBlendOperation(const std::string& blendop) noexcept
{
	if (blendop == "sub")
		return GraphicsBlendOp::GraphicsBlendOpSubtract;
	else if (blendop == "revsub")
		return GraphicsBlendOp::GraphicsBlendOpRevSubtract;
	else if (blendop == "add")
		return GraphicsBlendOp::GraphicsBlendOpAdd;
	else
	{
		assert(false);
		return GraphicsBlendOp::GraphicsBlendOpAdd;
	}
}

GraphicsBlendFactor
MaterialMaker::stringToBlendFactor(const std::string& factor) noexcept
{
	if (factor == "zero")
		return GraphicsBlendFactor::GraphicsBlendFactorZero;
	else if (factor == "one")
		return GraphicsBlendFactor::GraphicsBlendFactorOne;
	else if (factor == "dstcol")
		return GraphicsBlendFactor::GraphicsBlendFactorDstCol;
	else if (factor == "srccol")
		return GraphicsBlendFactor::GraphicsBlendFactorSrcColor;
	else if (factor == "srcalpha")
		return GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha;
	else if (factor == "dstalpha")
		return GraphicsBlendFactor::GraphicsBlendFactorDstAlpha;
	else if (factor == "invsrccol")
		return GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcCol;
	else if (factor == "invdstcol")
		return GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstCol;
	else if (factor == "invsrcalpha")
		return GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha;
	else if (factor == "invdstalpha")
		return GraphicsBlendFactor::GraphicsBlendFactorOneMinusDstAlpha;
	else
	{
		assert(false);
		return GraphicsBlendFactor::GraphicsBlendFactorZero;
	}
}

GraphicsColorMask
MaterialMaker::stringToColorMask(const std::string& mask) noexcept
{
	if (mask == "red")
		return GraphicsColorMask::GraphicsColorMaskR;
	else if (mask == "green")
		return GraphicsColorMask::GraphicsColorMaskG;
	else if (mask == "blue")
		return GraphicsColorMask::GraphicsColorMaskB;
	else if (mask == "alpha")
		return GraphicsColorMask::GraphicsColorMaskA;
	else if (mask == "rgb")
		return GraphicsColorMask::GraphicsColorMaskRGB;
	else if (mask == "rgba")
		return GraphicsColorMask::GraphicsColorMaskRGBA;
	else
	{
		assert(false);
		return GraphicsColorMask::GraphicsColorMaskRGBA;
	}
}

GraphicsCompareFunc
MaterialMaker::stringToCompareFunc(const std::string& func) noexcept
{
	if (func == "lequal")
		return GraphicsCompareFunc::GraphicsCompareFuncLequal;
	else if (func == "equal")
		return GraphicsCompareFunc::GraphicsCompareFuncEqual;
	else if (func == "greater")
		return GraphicsCompareFunc::GraphicsCompareFuncGreater;
	else if (func == "less")
		return GraphicsCompareFunc::GraphicsCompareFuncLess;
	else if (func == "gequal")
		return GraphicsCompareFunc::GraphicsCompareFuncGequal;
	else if (func == "notequal")
		return GraphicsCompareFunc::GraphicsCompareFuncNotEqual;
	else if (func == "always")
		return GraphicsCompareFunc::GraphicsCompareFuncAlways;
	else if (func == "never")
		return GraphicsCompareFunc::GraphicsCompareFuncNever;
	else if (func == "none")
		return GraphicsCompareFunc::GraphicsCompareFuncNone;
	else
	{
		assert(false);
		return GraphicsCompareFunc::GraphicsCompareFuncNone;
	}
}

GraphicsStencilOp
MaterialMaker::stringToStencilOp(const std::string& stencilop) noexcept
{
	if (stencilop == "keep")
		return GraphicsStencilOp::GraphicsStencilOpKeep;
	else if (stencilop == "replace")
		return GraphicsStencilOp::GraphicsStencilOpReplace;
	else if (stencilop == "inc")
		return GraphicsStencilOp::GraphicsStencilOpIncr;
	else if (stencilop == "dec")
		return GraphicsStencilOp::GraphicsStencilOpDecr;
	else if (stencilop == "incwrap")
		return GraphicsStencilOp::GraphicsStencilOpIncrWrap;
	else if (stencilop == "decwrap")
		return GraphicsStencilOp::GraphicsStencilOpDecrWrap;
	else
	{
		assert(false);
		return GraphicsStencilOp::GraphicsStencilOpKeep;
	}
}

_NAME_END