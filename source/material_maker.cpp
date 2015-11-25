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
#include <ray/render_system.h>
#include <ray/ioserver.h>
#include <ray/resource.h>
#include <ray/except.h>
#include <ray/mstream.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>

_NAME_BEGIN

MaterialMaker::MaterialMaker() noexcept
{
}

MaterialMaker::~MaterialMaker() noexcept
{
}

RenderStatePtr
MaterialMaker::instanceState(iarchive& reader) except
{
	auto state = RenderSystem::instance()->createRenderState();
	return state;
}

ShaderPtr
MaterialMaker::instanceShader(iarchive& reader) except
{
	auto shader = RenderSystem::instance()->createShader();

	std::string type = reader.getString("name");
	std::string value = reader.getString("value");

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
MaterialMaker::instancePass(iarchive& reader) except
{
	RenderPass passType = RenderPass::RP_CUSTOM;

	std::string passName = reader.getString("name");
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
		ShaderObjectPtr shaderObject = RenderSystem::instance()->createShaderObject();
		RenderStatePtr state = RenderSystem::instance()->createRenderState();

		RenderDepthState depthState;
		RenderRasterState rasterState;
		RenderBlendState blendState;
		RenderStencilState stencilState;

		do
		{
			auto nodeName = reader.getCurrentNodeName();
			if (nodeName == "state")
			{
				std::string name = reader.getString("name");
				std::string value = reader.getString("value");

				if (name == "vertex")
				{
					auto shader = instanceShader(reader);
					if (shader)
					{
						shaderObject->addShader(shader);
					}
				}
				else if (name == "fragment")
				{
					auto shader = instanceShader(reader);
					if (shader)
					{
						shaderObject->addShader(shader);
					}
				}
				else if (name == "cullmode")
					rasterState.cullMode = stringToCullMode(reader.getString("value"));
				else if (name == "fillmode")
					rasterState.fillMode = stringToFillMode(reader.getString("value"));
				else if (name == "scissorTestEnable")
					rasterState.scissorTestEnable = reader.getBoolean("value");
				else if (name == "primitive")
					rasterState.primitiveType = stringToPrimitive(reader.getString("value"));
				else if (name == "multisampleEnable")
					rasterState.multisampleEnable = reader.getBoolean("value");
				else if (name == "srgbEnable")
					rasterState.srgbEnable = reader.getBoolean("value");
				else if (name == "blend")
					blendState.blendEnable = reader.getBoolean("value");
				else if (name == "blendSeparate")
					blendState.blendSeparateEnable = reader.getBoolean("value");
				else if (name == "blendOp")
					blendState.blendOp = stringToBlendOperation(reader.getString("value"));
				else if (name == "blendsrc")
					blendState.blendSrc = stringToBlendFactor(reader.getString("value"));
				else if (name == "blenddst")
					blendState.blendDest = stringToBlendFactor(reader.getString("value"));
				else if (name == "blendalphaop")
					blendState.blendAlphaOp = stringToBlendOperation(reader.getString("value"));
				else if (name == "blendalphasrc")
					blendState.blendAlphaSrc = stringToBlendFactor(reader.getString("value"));
				else if (name == "blendalphadst")
					blendState.blendAlphaDest = stringToBlendFactor(reader.getString("value"));
				else if (name == "colormask")
					blendState.colorWriteMask = stringToColorMask(reader.getString("value"));
				else if (name == "depthtest")
					depthState.depthEnable = reader.getBoolean("value");
				else if (name == "depthwrite")
					depthState.depthWriteMask = reader.getBoolean("value");
				else if (name == "depthfunc")
					depthState.depthFunc = stringToCompareFunc(reader.getString("value"));
				else if (name == "depthBiasEnable")
					depthState.depthBiasEnable = reader.getBoolean("value");
				else if (name == "depthSlopScaleBias")
					depthState.depthSlopScaleBias = reader.getFloat("value");
				else if (name == "depthBias")
					depthState.depthBias = reader.getFloat("value");
				else if (name == "stencilTest")
					stencilState.stencilEnable = reader.getBoolean("value");
				else if (name == "stencilRef")
					stencilState.stencilRef = reader.getInteger("value");
				else if (name == "stencilFunc")
					stencilState.stencilFunc = stringToCompareFunc(reader.getString("value"));
				else if (name == "stencilReadMask")
					stencilState.stencilReadMask = reader.getInteger("value");
				else if (name == "stencilWriteMask")
					stencilState.stencilWriteMask = reader.getInteger("value");
				else if (name == "stencilFail")
					stencilState.stencilFail = stringToStencilOp(reader.getString("value"));
				else if (name == "stencilZFail")
					stencilState.stencilZFail = stringToStencilOp(reader.getString("value"));
				else if (name == "stencilPass")
					stencilState.stencilPass = stringToStencilOp(reader.getString("value"));
				else if (name == "stencilTwoTest")
					stencilState.stencilTwoEnable = reader.getBoolean("value");
				else if (name == "stencilTwoFunc")
					stencilState.stencilTwoFunc = stringToCompareFunc(reader.getString("value"));
				else if (name == "stencilTwoReadMask")
					stencilState.stencilTwoReadMask = reader.getInteger("value");
				else if (name == "stencilTwoWriteMask")
					stencilState.stencilTwoWriteMask = reader.getInteger("value");
				else if (name == "stencilTwoFail")
					stencilState.stencilTwoFail = stringToStencilOp(reader.getString("value"));
				else if (name == "stencilTwoZFail")
					stencilState.stencilTwoZFail = stringToStencilOp(reader.getString("value"));
				else if (name == "stencilTwoPass")
					stencilState.stencilTwoPass = stringToStencilOp(reader.getString("value"));
			}
			else
			{
				throw failure(__TEXT("Unkonwn node name ") + nodeName + reader.getCurrentNodePath());
			}
		} while (reader.setToNextChild());

		state->setDepthState(depthState);
		state->setBlendState(blendState);
		state->setRasterState(rasterState);
		state->setStencilState(stencilState);

		pass->setRenderState(state);
		pass->setShaderObject(shaderObject);
	}

	return pass;
}

MaterialTechPtr
MaterialMaker::instanceTech(iarchive& reader) except
{
	RenderQueue queue = RenderQueue::RQ_OPAQUE;

	std::string techName = reader.getString("name");
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
				tech->addPass(instancePass(reader));
			}
		} while (reader.setToNextChild());
	}

	return tech;
}

MaterialParamPtr
MaterialMaker::instanceParameter(iarchive& reader) except
{
	auto name = reader.getString("name");
	auto type = reader.getString("type");
	auto semantic = reader.getString("semantic");
	auto value = reader.getString("value");

	if (name.empty())
	{
		throw failure(__TEXT("The parameter name can not be empty"));
	}

	if (!type.empty())
	{
		auto param = std::make_shared<MaterialParam>();
		param->setName(name);
		if (type == "bool")
			param->setType(ShaderVariantType::SPT_BOOL);
		else if (type == "int")
			param->setType(ShaderVariantType::SPT_INT);
		else if (type == "int2")
			param->setType(ShaderVariantType::SPT_INT2);
		else if (type == "float")
			param->setType(ShaderVariantType::SPT_FLOAT);
		else if (type == "float2")
			param->setType(ShaderVariantType::SPT_FLOAT2);
		else if (type == "float3")
			param->setType(ShaderVariantType::SPT_FLOAT3);
		else if (type == "float4")
			param->setType(ShaderVariantType::SPT_FLOAT4);
		else if (type == "mat3")
			param->setType(ShaderVariantType::SPT_FLOAT3X3);
		else if (type == "mat4")
			param->setType(ShaderVariantType::SPT_FLOAT4X4);
		else if (type == "float[]")
			param->setType(ShaderVariantType::SPT_FLOAT_ARRAY);
		else if (type == "float2[]")
			param->setType(ShaderVariantType::SPT_FLOAT2_ARRAY);
		else if (type == "float3[]")
			param->setType(ShaderVariantType::SPT_FLOAT3_ARRAY);
		else if (type == "float4[]")
			param->setType(ShaderVariantType::SPT_FLOAT4_ARRAY);
		else if (type == "sampler2D")
			param->setType(ShaderVariantType::SPT_TEXTURE);
		else if (type == "buffer")
			param->setType(ShaderVariantType::SPT_BUFFER);
		else
		{
			assert(false);
			throw failure(__TEXT("Unknown parameter type : ") + name);
		}

		return  param;
	}
	else if (!semantic.empty())
	{
		auto param = std::make_shared<MaterialParam>();
		param->setName(name);
		param->setSemantic(semantic);
		return param;
	}

	return nullptr;
}

MaterialParamPtr
MaterialMaker::instanceBuffer(iarchive& reader) except
{
	auto buffer = std::make_shared<MaterialParam>();
	buffer->setType(ShaderVariantType::SPT_BUFFER);
	buffer->setName(reader.getString("name"));

	if (reader.setToFirstChild())
	{
		do
		{
			auto param = instanceParameter(reader);
			if (param)
				buffer->addParameter(param);
		} while (reader.setToNextChild());

		return buffer;
	}

	return nullptr;
}

MaterialPtr
MaterialMaker::load(const std::string& filename) except
{
	try
	{
		MemoryReader stream;

		IoServer::instance()->openFile(filename, stream, ios_base::in);
		if (!stream.is_open())
			throw failure(__TEXT("Opening file fail:") + filename);

		XMLReader reader;
		if (reader.open(stream))
			return this->load(reader);

		return nullptr;
	}
	catch (const failure& e)
	{
		throw failure(__TEXT("in ") + filename + __TEXT(" ") + e.message(), e.stack());
	}
}

MaterialPtr
MaterialMaker::load(iarchive& reader) except
{
	std::string nodeName;
	nodeName = reader.getCurrentNodeName();
	if (nodeName != "material" && nodeName != "effect")
		throw failure(__TEXT("Unknown node name " + nodeName + ", so I can't open it"));

	if (!reader.setToFirstChild())
		throw failure(__TEXT("The file has been damaged and can't be recovered, so I can't open it"));

	if (nodeName == "material")
	{
		std::string name;
		std::map<std::string, std::string> args;

		do
		{
			auto key = reader.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					if (it == "shader")
					{
						name = reader.getString(it);
					}
					else
					{
						args[it] = reader.getString(it);
					}
				}
			}
		} while (reader.setToNextChild());

		if (!name.empty())
		{
			MaterialMaker maker;
			auto material = maker.load(name);
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
						case ShaderVariantType::SPT_FLOAT:
							param->assign(parseFloat<Float>(arg.second));
							break;
						case ShaderVariantType::SPT_FLOAT2:
							param->assign(parseFloat2(arg.second));
							break;
						case ShaderVariantType::SPT_FLOAT3:
							param->assign(parseFloat3(arg.second));
							break;
						case ShaderVariantType::SPT_FLOAT4:
							param->assign(parseFloat4(arg.second));
							break;
						case ShaderVariantType::SPT_TEXTURE:
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

		do
		{
			std::string name = reader.getCurrentNodeName();
			if (name == "parameter")
			{
				auto param = instanceParameter(reader);
				if (param)
					material->addParameter(param);
			}
			else if (name == "buffer")
			{
				auto buffer = instanceBuffer(reader);
				if (buffer)
					material->addParameter(buffer);
			}
			else if (name == "include")
			{
				auto path = reader.getString("name");
				if (!path.empty())
				{
					this->load(path);
				}
			}
			else if (name == "shader")
			{
				auto type = reader.getString("name");
				if (!type.empty())
				{
					_shaderCodes[type] += reader.getText();
				}
				else
				{
					throw failure(__TEXT("Empty shader name : ") + reader.getCurrentNodePath());
				}
			}
			else if (name == "technique")
			{
				auto tech = instanceTech(reader);
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
		return VertexType::GPU_POINT;
	else if (primitive == "point_or_line")
		return VertexType::GPU_POINT_OR_LINE;
	else if (primitive == "line")
		return VertexType::GPU_LINE;
	else if (primitive == "triangle")
		return VertexType::GPU_TRIANGLE;
	else if (primitive == "triangle_or_line")
		return VertexType::GPU_TRIANGLE_OR_LINE;
	else if (primitive == "fan")
		return VertexType::GPU_FAN;
	else if (primitive == "fan_or_line")
		return VertexType::GPU_FAN_OR_LINE;
	else
	{
		assert(false);
		return GPU_TRIANGLE;
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
		return FillMode::GPU_POINT_MODE;
	else if (fillmode == "line")
		return FillMode::GPU_WIREFRAME_MODE;
	else if (fillmode == "solid")
		return FillMode::GPU_SOLID_MODE;
	else
	{
		assert(false);
		return FillMode::GPU_SOLID_MODE;
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