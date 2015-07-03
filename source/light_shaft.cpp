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
#include <ray/light_shaft.h>
#include <ray/material_maker.h>
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/render_factory.h>

_NAME_BEGIN

LightShaft::LightShaft() noexcept
{
}

LightShaft::~LightShaft() noexcept
{
}

void
LightShaft::onActivate(RenderPipeline& pipeline) except
{
    std::size_t width = pipeline.getWindowWidth();
    std::size_t height = pipeline.getWindowHeight();

    MaterialMaker maker;
    _material = maker.load("sys:fx/light_shaft.glsl");

    _texSample = RenderFactory::createRenderTarget();
    _texSample->setup(width * 0.5, height * 0.5, TextureDim::DIM_2D, PixelFormat::R8G8B8A8F);

    _lightShaft = _material->getTech(RenderQueue::PostProcess)->getPass("scatter");
    _lightShaftCopy = _material->getTech(RenderQueue::PostProcess)->getPass("copy");

    _illuminationSample = _material->getParameter("illuminationSample");
    _illuminationPosition = _material->getParameter("illuminationPosition");

    float sampleNumber = 50;
    float sampleInv = 1 / sampleNumber;
    float sampleWeight = 1 / sampleNumber;
    float sampleDecay = 0.0156;

    float4 sample;
    sample.x = sampleNumber;
    sample.y = sampleInv;
    sample.z = sampleWeight;
    sample.w = sampleDecay;

    _illuminationSample->assign(sample);

    _cameraRadio = _material->getParameter("cameraRadio");
    _texSource = _material->getParameter("texSource");
}

void
LightShaft::onDeactivate(RenderPipeline& pipeline) except
{
}

void
LightShaft::onRender(RenderPipeline& pipeline, RenderTargetPtr source) except
{
    _cameraRadio->assign(pipeline.getCamera()->getRatio());

    auto lights = pipeline.getRenderData(RenderQueue::Lighting);
    for (auto& it : lights)
    {
        auto light = dynamic_cast<Light*>(it);

        if (light->getLightType() == LightType::LT_SUN)
        {
            auto sun = pipeline.getCamera()->getTranslate() + light->getTransform().getTranslate();
            auto view = pipeline.getCamera()->worldToProject(sun);
            view.x = view.x * 0.5 + 0.5;
            view.y = view.y * 0.5 + 0.5;

            if (view.x >= -0.5f && view.x <= 2.0f &&
                view.y >= -0.5f && view.y <= 2.0f && view.z < 1.0f)
            {
                _illuminationPosition->assign(Vector2(view.x, view.y));

                _texSource->assign(source->getResolveTexture());
                pipeline.setRenderTarget(_texSample);
                pipeline.setTechnique(_lightShaft);
                pipeline.drawSceneQuad();

                _texSource->assign(_texSample->getResolveTexture());

                pipeline.setRenderTarget(source);
                pipeline.setTechnique(_lightShaftCopy);
                pipeline.drawSceneQuad();
            }
        }
    }
}

_NAME_END