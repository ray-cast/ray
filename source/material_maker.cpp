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
#include <ray/render_impl.h>
#include <ray/render_factory.h>
#include <ray/image.h>
#include <ray/except.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>

_NAME_BEGIN

TexturePtr createTexture(const std::string& name) except
{
    Image image;
    if (image.load(name))
    {
        PixelFormat format = PixelFormat::R8G8B8A8;

        if (image.bpp() == 24)
            format = PixelFormat::R8G8B8;
        else if (image.bpp() == 32)
            format = PixelFormat::R8G8B8A8;

        auto texture = RenderFactory::createTexture();
        texture->setSize(image.width(), image.height());
        texture->setTexDim(TextureDim::DIM_2D);
        texture->setTexFormat(format);
        texture->setStream(image.data());
        texture->setup();

        return texture;
    }
    else
    {
        throw failure("fail to open : " + name);
    }

    return nullptr;
}

MaterialMaker::MaterialMaker() noexcept
{
}

MaterialMaker::MaterialMaker(const std::string& filename) except
{
    this->load(filename);
}

MaterialMaker::~MaterialMaker() noexcept
{
}

RenderStatePtr
MaterialMaker::instanceState(XMLReader& reader) except
{
    auto state = std::make_shared<RenderState>();
    return state;
}

ShaderPtr
MaterialMaker::instanceShader(XMLReader& reader) except
{
    auto shader = RenderFactory::createShader();

    std::string type = reader.getString("name");
    std::string value = reader.getString("value");

    if (type.empty())
        throw failure("The shader name can not be empty");

    if (value.empty())
        throw failure("The shader value can not be empty");

    std::string method = _shaderCodes[type];
    if (method.empty())
        throw failure("Unknown shader type : " + type);

    std::size_t off = method.find(value.data());
    if (off == std::string::npos)
        throw failure("Unknown shader entry point : " + value);

    method.replace(off, value.size(), "main");

    shader->setType(type);
    shader->setSource(method);

    return shader;
}

MaterialPassPtr
MaterialMaker::instancePass(XMLReader& reader) except
{
    RenderPass passType = RenderPass::RP_CUSTOM;

    std::string passName = reader.getString("name");
    if (passName.empty())
        throw failure("The pass name can not be empty");

    if (passName == "forward")
        passType = RenderPass::RP_FORWARD;
    else if (passName == "depth")
        passType = RenderPass::RP_DEPTH;
    else if (passName == "shadow")
        passType = RenderPass::RP_SHADOW;
    else if (passName == "gbuffer")
        passType = RenderPass::RP_GBUFFER;
    else if (passName == "light")
        passType = RenderPass::RP_LIGHT;
    else if (passName == "custom")
        passType = RenderPass::RP_CUSTOM;

    auto pass = std::make_shared<MaterialPass>(passType);
    pass->setName(passName);

    if (reader.setToFirstChild())
    {
        ShaderObjectPtr shaderObject = RenderFactory::createShaderObject();
        RenderStatePtr state = std::make_shared<RenderState>();

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
                    rasterState.cullMode = RenderState::stringToCullMode(reader.getString("value"));
                else if (name == "fillmode")
                    rasterState.fillMode = RenderState::stringToFillMode(reader.getString("value"));
                else if (name == "scissorTestEnable")
                    rasterState.scissorTestEnable = reader.getBoolean("value");
                else if (name == "multisampleEnable")
                    rasterState.multisampleEnable = reader.getBoolean("value");
                else if (name == "blend")
                    blendState.blendEnable = reader.getBoolean("value");
                else if (name == "blendSeparate")
                    blendState.blendSeparateEnable = reader.getBoolean("value");
                else if (name == "blendOp")
                    blendState.blendOp = RenderState::stringToBlendOperation(reader.getString("value"));
                else if (name == "blendsrc")
                    blendState.blendSrc = RenderState::stringToBlendFactor(reader.getString("value"));
                else if (name == "blenddst")
                    blendState.blendDest = RenderState::stringToBlendFactor(reader.getString("value"));
                else if (name == "blendalphaop")
                    blendState.blendAlphaOp = RenderState::stringToBlendOperation(reader.getString("value"));
                else if (name == "blendalphasrc")
                    blendState.blendAlphaSrc = RenderState::stringToBlendFactor(reader.getString("value"));
                else if (name == "blendalphadst")
                    blendState.blendAlphaDest = RenderState::stringToBlendFactor(reader.getString("value"));
                else if (name == "colormask")
                    blendState.colorWriteMask = RenderState::stringToColorMask(reader.getString("value"));
                else if (name == "depthtest")
                    depthState.depthEnable = reader.getBoolean("value");
                else if (name == "depthwrite")
                    depthState.depthWriteMask = reader.getBoolean("value");
                else if (name == "depthfunc")
                    depthState.depthFunc = RenderState::stringToCompareFunc(reader.getString("value"));
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
                    stencilState.stencilFunc = RenderState::stringToCompareFunc(reader.getString("value"));
                else if (name == "stencilReadMask")
                    stencilState.stencilReadMask = reader.getInteger("value");
                else if (name == "stencilWriteMask")
                    stencilState.stencilWriteMask = reader.getInteger("value");
                else if (name == "stencilFail")
                    stencilState.stencilFail = RenderState::stringToStencilOp(reader.getString("value"));
                else if (name == "stencilZFail")
                    stencilState.stencilZFail = RenderState::stringToStencilOp(reader.getString("value"));
                else if (name == "stencilPass")
                    stencilState.stencilPass = RenderState::stringToStencilOp(reader.getString("value"));
                else if (name == "stencilTwoTest")
                    stencilState.stencilTwoEnable = reader.getBoolean("value");
                else if (name == "stencilTwoFunc")
                    stencilState.stencilTwoFunc = RenderState::stringToCompareFunc(reader.getString("value"));
                else if (name == "stencilTwoReadMask")
                    stencilState.stencilTwoReadMask = reader.getInteger("value");
                else if (name == "stencilTwoWriteMask")
                    stencilState.stencilTwoWriteMask = reader.getInteger("value");
                else if (name == "stencilTwoFail")
                    stencilState.stencilTwoFail = RenderState::stringToStencilOp(reader.getString("value"));
                else if (name == "stencilTwoZFail")
                    stencilState.stencilTwoZFail = RenderState::stringToStencilOp(reader.getString("value"));
                else if (name == "stencilTwoPass")
                    stencilState.stencilTwoPass = RenderState::stringToStencilOp(reader.getString("value"));

                state->setDepthState(depthState);
                state->setBlendState(blendState);
                state->setRasterState(rasterState);
                state->setStencilState(stencilState);
            }
            else
            {
                throw failure("Unkonwn node name " + nodeName + reader.getCurrentNodePath());
            }
        } while (reader.setToNextChild());

        pass->setRenderState(state);
        pass->setShaderObject(shaderObject);
    }

    return pass;
}

MaterialTechPtr
MaterialMaker::instanceTech(XMLReader& reader) except
{
    RenderQueue queue = Background;

    std::string techName = reader.getString("name");
    if (techName.empty())
        throw failure("The technique name can not be empty");

    if (techName == "background")
        queue = RenderQueue::Background;
    else if (techName == "shadow")
        queue = RenderQueue::Shadow;
    else if (techName == "opaque")
        queue = RenderQueue::Opaque;
    else if (techName == "transparent")
        queue = RenderQueue::Transparent;
    else if (techName == "deferredlight")
        queue = RenderQueue::DeferredLight;
    else if (techName == "postprocess")
        queue = RenderQueue::PostProcess;
    else
    {
        throw failure("Unknown technique name : " + techName);
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
MaterialMaker::instanceParameter(XMLReader& reader) except
{
    auto name = reader.getString("name");
    auto type = reader.getString("type");
    auto semantic = reader.getString("semantic");
    auto value = reader.getString("value");

    if (name.empty())
    {
        throw failure("The parameter name can not be empty");
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
        {
            param->setType(ShaderVariantType::SPT_FLOAT);
            if (!value.empty())
            {
                param->assign(fast_atof(value.c_str()));
            }
        }
        else if (type == "float2")
            param->setType(ShaderVariantType::SPT_FLOAT2);
        else if (type == "float3")
            param->setType(ShaderVariantType::SPT_FLOAT3);
        else if (type == "float4")
            param->setType(ShaderVariantType::SPT_FLOAT4);
        else if (type == "mat4")
            param->setType(ShaderVariantType::SPT_FLOAT4X4);
        else if (type == "float[]")
            param->setType(ShaderVariantType::SPT_FLOAT_ARRAY);
        else if (type == "float2")
            param->setType(ShaderVariantType::SPT_FLOAT2);
        else if (type == "sampler2D")
        {
            param->setType(ShaderVariantType::SPT_TEXTURE);
            if (!value.empty())
            {
                auto texture = createTexture(value.c_str());
                if (texture)
                {
                    param->assign(texture);
                }
            }
        }
        else if (type == "buffer")
            param->setType(ShaderVariantType::SPT_BUFFER);
        else
        {
            assert(false);
            throw failure("Unknown parameter type : " + name);
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
MaterialMaker::instanceBuffer(XMLReader& reader) except
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
        MemoryStream stream;

        IoServer::instance()->openFile(filename, stream);
        if (!stream.is_open())
            return false;

        XMLReader reader;
        if (reader.open(stream))
            return this->load(reader);

        return nullptr;
    }
    catch (const failure& e)
    {
        throw failure("in " + filename + " " + e.message(), e.stack());
    }
}

MaterialPtr
MaterialMaker::load(XMLReader& reader) except
{
    std::string nodeName;
    nodeName = reader.getCurrentNodeName();
    if (nodeName == "effect")
    {
        if (reader.setToFirstChild())
        {
            _material = std::make_shared<Material>();

            do
            {
                std::string name = reader.getCurrentNodeName();
                if (name == "parameter")
                {
                    auto param = instanceParameter(reader);
                    if (param)
                        _material->addParameter(param);
                }
                else if (name == "buffer")
                {
                    auto buffer = instanceBuffer(reader);
                    if (buffer)
                        _material->addParameter(buffer);
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
                        throw failure("Empty shader name : " + reader.getCurrentNodePath());
                    }
                }
                else if (name == "technique")
                {
                    auto tech = instanceTech(reader);
                    if (tech)
                        _material->addTech(tech);
                }
            } while (reader.setToNextChild());

            _material->setup();
            return _material;
        }
    }

    return nullptr;
}

MaterialMaker::operator MaterialPtr() noexcept
{
    return _material;
}

_NAME_END