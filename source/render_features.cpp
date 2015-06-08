// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include <ray/render_features.h>
#include <ray/render_system.h>
#include <ray/window_features.h>
#include <ray/material_maker.h>
#include <ray/parse.h>
#include <ray/camera_component.h>
#include <ray/script_component.h>
#include <ray/light_component.h>
#include <ray/mesh_component.h>
#include <ray/skybox_component.h>
#include <ray/mesh_render_component.h>
#include <ray/image.h>
#include <ray/model.h>

#if _BUILD_OPENGL
#   include <ray/ogl_renderer.h>
#endif

_NAME_BEGIN

RenderFeatures::RenderFeatures() noexcept
{
    this->setName(typeid(RenderFeatures).name());
}

RenderFeatures::~RenderFeatures() noexcept
{
}

void
RenderFeatures::setRenderSystem(RenderSystemPtr renderSystem) noexcept
{
    _renderSystem = renderSystem;
}

RenderSystemPtr
RenderFeatures::getRenderSystem() const noexcept
{
    return _renderSystem;
}

RenderScenePtr
RenderFeatures::getRenderScene(GameScene* scene) noexcept
{
    return _renderScenes[scene->getInstanceID()];
}

RenderScenePtr
RenderFeatures::getRenderScene(GameScenePtr scene) noexcept
{
    return _renderScenes[scene->getInstanceID()];
}

GameComponentPtr
RenderFeatures::onSerialization(XMLReader* reader) except
{
    auto component = reader->getString("name");
    if (component == "camera")
        return instanceCamera(reader);
    else if (component == "mesh")
        return instanceMesh(reader);
    else if (component == "meshrender")
        return instanceMeshRender(reader);
    else if (component == "light")
        return instanceLight(reader);
    else if (component == "sky")
        return instanceSky(reader);

    return nullptr;
}

void
RenderFeatures::onActivate() except
{
    WindowPtr window = nullptr;

    auto features = this->getGameServer()->getFeature<WindowFeatures>();
    if (features)
    {
        window = features->getWindow();
        if (window)
        {
            _renderDevice = OGLRenderer::create();
            _renderWindow = RenderWindow::create();

            _renderSystem = RenderSystem::create();
            _renderSystem->setup(_renderDevice, _renderWindow, window->getWindowHandle(), window->getWindowWidth(), window->getWindowHeight());

            _renderSystem->setTimer(this->getGameServer()->getTimer());
        }
    }
}

void
RenderFeatures::onDeactivate() except
{
    _renderWindow.reset();
    _renderDevice.reset();
    _renderSystem.reset();
}

void
RenderFeatures::onOpenScene(GameScenePtr scene) except
{
    if (_renderSystem)
    {
        if (!_renderScenes[scene->getInstanceID()])
        {
            auto renderScene = std::make_shared<RenderScene>();
            _renderSystem->addRenderScene(renderScene);
            _renderScenes[scene->getInstanceID()] = renderScene;
        }
    }
}

void
RenderFeatures::onCloseScene(GameScenePtr scene) except
{
    if (_renderSystem)
    {
        auto renderScene = _renderScenes[scene->getInstanceID()];
        if (renderScene)
        {
            _renderSystem->removeRenderScene(renderScene);
            _renderScenes[scene->getInstanceID()] = nullptr;
        }
    }
}

void
RenderFeatures::onFrameBegin()
{
    if (_renderSystem)
        _renderSystem->renderBegin();
}

void
RenderFeatures::onFrame()
{
}

void
RenderFeatures::onFrameEnd()
{
    if (_renderSystem)
    {
        _renderSystem->render();
        _renderSystem->renderEnd();
    }
}

MaterialPtr
RenderFeatures::instanceMaterial(XMLReader* reader) except
{
    assert(reader);

    std::string url = reader->getString("material");
    if (!url.empty())
    {
        MemoryStream stream;

        IoServer::instance()->openFile(url, stream);
        if (!stream.is_open())
            return false;

        XMLReader xml;
        if (xml.open(stream))
        {
            std::string name;
            std::map<std::string, std::string> args;

            if (!xml.setToFirstChild())
            {
                throw failure("The file has been damaged and can't be recovered, so I can't open it" + url);
            }

            do
            {
                auto key = xml.getCurrentNodeName();
                if (key == "attribute")
                {
                    auto attributes = xml.getAttrs();
                    for (auto& it : attributes)
                    {
                        if (it == "shader")
                        {
                            name = xml.getString(it);
                        }
                        else
                        {
                            args[it] = xml.getString(it);
                        }
                    }
                }
            } while (xml.setToNextChild());

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
                            case ShaderParamType::SPT_FLOAT:
                            {
                                param->assign(parseFloat<Float>(arg.second));
                            }
                            break;
                            case ShaderParamType::SPT_FLOAT4:
                            {
                                param->assign(parseFloat4(arg.second));
                            }
                            break;
                            case ShaderParamType::SPT_TEXTURE:
                            {
                                param->setTexture(createTexture(arg.second));
                            }
                            break;
                            }
                        }
                    }

                    return material;
                }
            }
        }
    }

    return nullptr;
}

GameComponentPtr
RenderFeatures::instanceCamera(XMLReader* reader) except
{
    if (reader->setToFirstChild())
    {
        auto camera = std::make_shared<CameraComponent>();

        float aperture = 70.0;
        float ratio = 1.0;
        float znear = 0.1;
        float zfar = 1000.0;
        int x = 0, y = 0, w = 0, h = 0;

        do
        {
            auto key = reader->getCurrentNodeName();
            if (key == "attribute")
            {
                auto attributes = reader->getAttrs();
                for (auto& it : attributes)
                {
                    if (it == "aperture")
                    {
                        aperture = reader->getFloat(it);
                    }
                    else if (it == "ratio")
                    {
                        ratio = reader->getFloat(it);
                    }
                    else if (it == "znear")
                    {
                        znear = reader->getFloat(it);
                    }
                    else if (it == "zfar")
                    {
                        zfar = reader->getFloat(it);
                    }
                    else if (it == "x")
                    {
                        x = reader->getInteger(it);
                    }
                    else if (it == "y")
                    {
                        y = reader->getInteger(it);
                    }
                    else if (it == "w")
                    {
                        w = reader->getInteger(it);
                    }
                    else if (it == "h")
                    {
                        h = reader->getInteger(it);
                    }
                    else if (it == "visible")
                    {
                        camera->setVisible(reader->getBoolean(it));
                    }
                }
            }
        } while (reader->setToNextChild());

        camera->setViewport(Viewport(x, y, w, h));
        camera->makePerspective(aperture, ratio, znear, zfar);

        return camera;;
    }

    return nullptr;
}

GameComponentPtr
RenderFeatures::instanceMesh(XMLReader* reader) except
{
    if (reader->setToFirstChild())
    {
        auto mesh = std::make_shared<MeshComponent>();

        do
        {
            auto key = reader->getCurrentNodeName();
            if (key == "attribute")
            {
                auto attributes = reader->getAttrs();
                for (auto& it : attributes)
                {
                    if (it == "name")
                    {
                        mesh->setName(reader->getString(it));
                    }
                    else if (it == "model")
                    {
                        mesh->setName(reader->getString(it));
                    }
                }
            }
        } while (reader->setToNextChild());

        Model model;
        if (model.load(mesh->getName()))
        {
            if (model.hasMeshes())
            {
                mesh->setMesh(model.getMeshsList()[0]);
            }
        }

        return mesh;
    }

    return nullptr;
}

GameComponentPtr
RenderFeatures::instanceMeshRender(XMLReader* reader) except
{
    if (reader->setToFirstChild())
    {
        auto render = std::make_shared<MeshRenderComponent>();

        do
        {
            auto key = reader->getCurrentNodeName();
            if (key == "attribute")
            {
                auto attributes = reader->getAttrs();
                for (auto& it : attributes)
                {
                    if (it == "name")
                    {
                        render->setName(reader->getString(it));
                    }
                    else if (it == "material")
                    {
                        render->setMaterial(instanceMaterial(reader));
                    }
                    else if (it == "castshadow")
                    {
                        render->setCastShadow(reader->getBoolean(it));
                    }
                    else if (it == "receiveshadow")
                    {
                        render->setReceiveShadow(reader->getBoolean(it));
                    }
                    else if (it == "visible")
                    {
                        render->setVisible(reader->getBoolean(it));
                    }
                }
            }
        } while (reader->setToNextChild());

        return render;
    }

    return nullptr;
}

GameComponentPtr
RenderFeatures::instanceLight(XMLReader* reader) except
{
    if (reader->setToFirstChild())
    {
        auto light = std::make_shared<LightComponent>();

        do
        {
            auto key = reader->getCurrentNodeName();
            if (key == "attribute")
            {
                auto attributes = reader->getAttrs();
                for (auto& it : attributes)
                {
                    std::string value = reader->getString(it);
                    if (it == "type")
                    {
                        if (value == "sun")
                            light->setLightType(LightType::LT_SUN);
                        else if (value == "point")
                            light->setLightType(LightType::LT_POINT);
                        else if (value == "spot")
                            light->setLightType(LightType::LT_SPOT);
                        else if (value == "area")
                            light->setLightType(LightType::LT_AREA);
                        else if (value == "hemiSphere")
                            light->setLightType(LightType::LT_HEMI_SPHERE);
                    }
                    else if (it == "color")
                    {
                        light->setLightColor(reader->getFloat3(it));
                    }
                    else if (it == "intensity")
                    {
                        light->setIntensity(reader->getFloat(it));
                    }
                    else if (it == "range")
                    {
                        light->setRange(reader->getFloat(it));
                    }
                    else if (it == "exponent")
                    {
                        light->setExponent(reader->getFloat(it));
                    }
                    else if (it == "shadow")
                    {
                        light->setShadow(reader->getBoolean(it));
                    }
                }
            }
        } while (reader->setToNextChild());

        return light;
    }

    return nullptr;
}

GameComponentPtr
RenderFeatures::instanceSky(XMLReader* reader) except
{
    if (reader->setToFirstChild())
    {
        auto sky = std::make_shared<SkyBoxComponent>();

        do
        {
            auto key = reader->getCurrentNodeName();
            if (key == "attribute")
            {
                auto attributes = reader->getAttrs();
                for (auto& it : attributes)
                {
                    std::string value = reader->getString(it);
                    if (it == "material")
                    {
                        sky->setMaterial(instanceMaterial(reader));
                    }
                }
            }
        } while (reader->setToNextChild());

        return sky;
    }

    return nullptr;
}

_NAME_END