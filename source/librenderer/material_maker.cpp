// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/shader.h>
#include <ray/render_system.h>
#include <ray/ioserver.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

_NAME_BEGIN

MaterialMaker::MaterialMaker() noexcept
{
}

MaterialMaker::~MaterialMaker() noexcept
{
}

GraphicsStatePtr
MaterialMaker::instanceState(MaterialManager& manager, iarchive& reader) except
{
	auto state = manager.getGraphicsDevice()->createGraphicsState();
	return state;
}

ShaderPtr
MaterialMaker::instanceShader(MaterialManager& manager, iarchive& reader) except
{
	auto shader = manager.getGraphicsDevice()->createShader();

	std::string type;
	reader.getValue("name", type);

	std::string value;
	reader.getValue("value", value);

	if (type.empty())
		throw failure(__TEXT("The shader name can not be empty"));

	if (value.empty())
		throw failure(__TEXT("The shader value can not be empty"));

	std::string method = _shaderCodes[type];
	if (method.empty())
		throw failure(__TEXT("Unknown shader type : ") + type);

	std::size_t off = method.find(value.data());
	if (off == std::string::npos)
		throw failure(__TEXT("Unknown shader entry point : ") + value);

	method.replace(off, value.size(), "main");

	shader->setType(type);
	shader->setSource(method);

	return shader;
}

MaterialPassPtr
MaterialMaker::instancePass(MaterialManager& manager, iarchive& reader) except
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
		ShaderObjectPtr shaderObject = manager.getGraphicsDevice()->createShaderObject();
		GraphicsStatePtr state = manager.getGraphicsDevice()->createGraphicsState();

		RenderDepthState depthState;
		RenderRasterState rasterState;
		RenderBlendState blendState;
		RenderStencilState stencilState;

		do
		{
			auto nodeName = reader.getCurrentNodeName();
			if (nodeName != "state")
				throw failure(__TEXT("Unkonwn node name ") + nodeName + reader.getCurrentNodePath());

			std::string name = reader.getValue<std::string>("name");
			std::string value = reader.getValue<std::string>("value");

			if (name == "vertex")
				shaderObject->addShader(instanceShader(manager, reader));
			else if (name == "fragment")
				shaderObject->addShader(instanceShader(manager, reader));
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

		state->setDepthState(depthState);
		state->setBlendState(blendState);
		state->setRasterState(rasterState);
		state->setStencilState(stencilState);

		pass->setGraphicsState(state);
		pass->setShaderObject(shaderObject);
	}

	return pass;
}

MaterialTechPtr
MaterialMaker::instanceTech(MaterialManager& manager, iarchive& reader) except
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
				tech->addPass(instancePass(manager, reader));
			}
		} while (reader.setToNextChild());
	}

	return tech;
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
			param->setType(ShaderVariantType::Bool);
		else if (type == "int")
			param->setType(ShaderVariantType::Int);
		else if (type == "int2")
			param->setType(ShaderVariantType::Int2);
		else if (type == "float")
			param->setType(ShaderVariantType::Float);
		else if (type == "float2")
			param->setType(ShaderVariantType::Float2);
		else if (type == "float3")
			param->setType(ShaderVariantType::Float3);
		else if (type == "float4")
			param->setType(ShaderVariantType::Float4);
		else if (type == "mat3")
			param->setType(ShaderVariantType::Float3x3);
		else if (type == "mat4")
			param->setType(ShaderVariantType::Float4x4);
		else if (type == "float[]")
			param->setType(ShaderVariantType::FloatArray);
		else if (type == "float2[]")
			param->setType(ShaderVariantType::Float2Array);
		else if (type == "float3[]")
			param->setType(ShaderVariantType::Float3Array);
		else if (type == "float4[]")
			param->setType(ShaderVariantType::Float4Array);
		else if (type == "sampler2D")
			param->setType(ShaderVariantType::Texture);
		else if (type == "buffer")
			param->setType(ShaderVariantType::Buffer);
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

MaterialParamPtr
MaterialMaker::instanceBuffer(MaterialPtr& material, iarchive& reader) except
{
	auto buffer = std::make_shared<MaterialParam>();
	buffer->setType(ShaderVariantType::Buffer);
	buffer->setName(reader.getValue<std::string>("name"));

	if (reader.setToFirstChild())
	{
		do
		{
		} while (reader.setToNextChild());

		return buffer;
	}

	return nullptr;
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
			return this->load(manager, reader);

		return nullptr;
	}
	catch (const failure& e)
	{
		throw failure(__TEXT("in ") + filename + __TEXT(" ") + e.message(), e.stack());
	}
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
						case ShaderVariantType::Float:
							param->assign(parseFloat<Float>(arg.second));
							break;
						case ShaderVariantType::Float2:
							param->assign(parseFloat2(arg.second));
							break;
						case ShaderVariantType::Float3:
							param->assign(parseFloat3(arg.second));
							break;
						case ShaderVariantType::Float4:
							param->assign(parseFloat4(arg.second));
							break;
						case ShaderVariantType::Texture:
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

		if (!reader.setToFirstChild())
			throw failure(__TEXT("The file has been damaged and can't be recovered, so I can't open it"));

		do
		{
			std::string name = reader.getCurrentNodeName();
			if (name == "parameter")
			{
				instanceParameter(manager, material, reader);
			}
			else if (name == "buffer")
			{
				auto buffer = instanceBuffer(material, reader);
				if (buffer)
					material->addParameter(buffer);
			}
			else if (name == "include")
			{
				auto path = reader.getValue<std::string>("name");
				if (!path.empty())
				{
					this->load(manager, path);
				}
			}
			else if (name == "shader")
			{
				auto type = reader.getValue<std::string>("name");
				if (!type.empty())
				{
					auto str = reader.getText();
					for (std::size_t i = 0; i < str.size(); i++)
					{
						if (util::isSpaceOrNewLine(str[i]))
							str.erase(str.begin());
						else
							break;
					}

					_shaderCodes[type] += str;
				}
				else
				{
					throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());
				}
			}
			else if (name == "technique")
			{
				auto tech = instanceTech(manager, reader);
				if (tech)
					material->addTech(tech);
			}
		} while (reader.setToNextChild());
		
		material->setup();
		return material;
	}

	return nullptr;
}

VertexType
MaterialMaker::stringToPrimitive(const std::string& primitive) noexcept
{
	if (primitive == "point")
		return VertexType::Point;
	else if (primitive == "point_or_line")
		return VertexType::PointOrLine;
	else if (primitive == "line")
		return VertexType::Line;
	else if (primitive == "triangle")
		return VertexType::Triangle;
	else if (primitive == "triangle_or_line")
		return VertexType::TriangleOrLine;
	else if (primitive == "fan")
		return VertexType::Fan;
	else if (primitive == "fan_or_line")
		return VertexType::FanOrLine;
	else
	{
		assert(false);
		return VertexType::Triangle;
	}
}

CullMode
MaterialMaker::stringToCullMode(const std::string& cullmode) noexcept
{
	if (cullmode == "back")
		return CullMode::GPU_CULL_BACK;
	else if (cullmode == "front")
		return CullMode::GPU_CULL_FRONT;
	else if (cullmode == "frontback")
		return CullMode::GPU_CULL_FRONT_BACK;
	else if (cullmode == "none")
		return CullMode::GPU_CULL_NONE;
	else
	{
		assert(false);
		return CullMode::GPU_CULL_NONE;
	}
}

FillMode
MaterialMaker::stringToFillMode(const std::string& fillmode) noexcept
{
	if (fillmode == "point")
		return FillMode::PointMode;
	else if (fillmode == "line")
		return FillMode::WireframeMode;
	else if (fillmode == "solid")
		return FillMode::SolidMode;
	else
	{
		assert(false);
		return FillMode::SolidMode;
	}
}

BlendOperation
MaterialMaker::stringToBlendOperation(const std::string& blendop) noexcept
{
	if (blendop == "sub")
		return GPU_SUBSTRACT;
	else if (blendop == "revsub")
		return GPU_REVSUBTRACT;
	else if (blendop == "add")
		return GPU_ADD;
	else
	{
		assert(false);
		return GPU_ADD;
	}
}

BlendFactor
MaterialMaker::stringToBlendFactor(const std::string& factor) noexcept
{
	if (factor == "zero")
		return GPU_ZERO;
	else if (factor == "one")
		return GPU_ONE;
	else if (factor == "dstcol")
		return GPU_DSTCOL;
	else if (factor == "srccol")
		return GPU_SRCCOLOR;
	else if (factor == "srcalpha")
		return GPU_SRCALPHA;
	else if (factor == "dstalpha")
		return GPU_DSTALPHA;
	else if (factor == "invsrccol")
		return GPU_ONEMINUSSRCCOL;
	else if (factor == "invdstcol")
		return GPU_ONEMINUSDSTCOL;
	else if (factor == "invsrcalpha")
		return GPU_ONEMINUSSRCALPHA;
	else if (factor == "invdstalpha")
		return GPU_ONEMINUSDSTALPHA;
	else
	{
		assert(false);
		return GPU_ZERO;
	}
}

ColorMask
MaterialMaker::stringToColorMask(const std::string& mask) noexcept
{
	if (mask == "red")
		return ColorMask::GPU_COLORMASK_RED;
	else if (mask == "green")
		return ColorMask::GPU_COLORMASK_GREEN;
	else if (mask == "blue")
		return ColorMask::GPU_COLORMASK_BLUE;
	else if (mask == "alpha")
		return ColorMask::GPU_COLORMASK_ALPHA;
	else if (mask == "rgb")
		return ColorMask::GPU_COLORMASK_RGB;
	else if (mask == "rgba")
		return ColorMask::GPU_COLORMASK_RGBA;
	else
	{
		assert(false);
		return ColorMask::GPU_COLORMASK_RGBA;
	}
}

CompareFunction
MaterialMaker::stringToCompareFunc(const std::string& func) noexcept
{
	if (func == "lequal")
		return GPU_LEQUAL;
	else if (func == "equal")
		return GPU_EQUAL;
	else if (func == "greater")
		return GPU_GREATER;
	else if (func == "less")
		return GPU_LESS;
	else if (func == "gequal")
		return GPU_GEQUAL;
	else if (func == "notequal")
		return GPU_NOTEQUAL;
	else if (func == "always")
		return GPU_ALWAYS;
	else if (func == "never")
		return GPU_NEVER;
	else if (func == "none")
		return GPU_NONE;
	else
	{
		assert(false);
		return GPU_NONE;
	}
}

StencilOperation
MaterialMaker::stringToStencilOp(const std::string& stencilop) noexcept
{
	if (stencilop == "keep")
		return STENCILOP_KEEP;
	else if (stencilop == "replace")
		return STENCILOP_REPLACE;
	else if (stencilop == "inc")
		return STENCILOP_INCR;
	else if (stencilop == "dec")
		return STENCILOP_DECR;
	else if (stencilop == "incwrap")
		return STENCILOP_INCR_WRAP;
	else if (stencilop == "decwrap")
		return STENCILOP_DECR_WRAP;
	else
	{
		assert(false);
		return STENCILOP_KEEP;
	}
}

_NAME_END