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
#include <ray/atmospheric.h>
#include <ray/material_maker.h>
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/render_factory.h>

_NAME_BEGIN

Atmospheric::Setting::Setting() noexcept
{
    outerRadius = 32800.0f;
    innerRadius = 32000.0f;

    //outerRadius = 10.25f * 10.0f;
    //innerRadius = 10.00f * 10.0f;
    wavelength.x = 0.650f; // 650 nm for red
    wavelength.y = 0.570f; // 570 nm for green
    wavelength.z = 0.475f; // 475 nm for blue

    kr = 0.0025;
    km = 0.0025;
    sun = 16.0f;
};

Atmospheric::Atmospheric() noexcept
{
}

Atmospheric::~Atmospheric() noexcept
{
}

void
Atmospheric::onActivate(RenderPipeline& pipeline) noexcept
{
    MeshProperty mesh;
    mesh.makeSphere(1, 128, 96);

    _sphere = RenderFactory::createRenderBuffer(mesh);

    _sat = MaterialMaker("sys:fx/atmospheric.glsl");

    _sky = _sat->getTech(RenderQueue::PostProcess)->getPass("sky");
    _ground = _sat->getTech(RenderQueue::PostProcess)->getPass("ground");
    _lightDirection = _sat->getParameter("lightDirection");
    _invWavelength = _sat->getParameter("invWavelength");
    _outerRadius = _sat->getParameter("outerRadius");
    _outerRadius2 = _sat->getParameter("outerRadius2");
    _innerRadius = _sat->getParameter("innerRadius");
    _innerRadius2 = _sat->getParameter("innerRadius2");
    _krESun = _sat->getParameter("krESun");
    _kmESun = _sat->getParameter("kmESun");
    _kr4PI = _sat->getParameter("kr4PI");
    _km4PI = _sat->getParameter("km4PI");
    _scaleFactor = _sat->getParameter("scaleFactor");
    _scaleDepth = _sat->getParameter("scaleDepth");
    _scaleOverScaleDepth = _sat->getParameter("scaleOverScaleDepth");

    float3 invWavelength4;
    invWavelength4.x = 1.0 / powf(_setting.wavelength.x, 4.0f);
    invWavelength4.y = 1.0 / powf(_setting.wavelength.y, 4.0f);
    invWavelength4.z = 1.0 / powf(_setting.wavelength.z, 4.0f);

    _invWavelength->assign(invWavelength4);

    _innerRadius->assign(_setting.innerRadius);
    _innerRadius2->assign(_setting.innerRadius * _setting.innerRadius);
    _outerRadius->assign(_setting.outerRadius);
    _outerRadius2->assign(_setting.outerRadius * _setting.outerRadius);

    _krESun->assign(_setting.kr * _setting.sun);
    _kmESun->assign(_setting.km * _setting.sun);

    _kr4PI->assign(_setting.kr * 4.0f * M_PI);
    _km4PI->assign(_setting.km * 4.0f * M_PI);

    _scaleFactor->assign(1.0f / (_setting.outerRadius - _setting.innerRadius));
    _scaleDepth->assign(0.25f);
    _scaleOverScaleDepth->assign((1.0f / (_setting.outerRadius - _setting.innerRadius)) / 0.25f);
}

void
Atmospheric::onDectivate(RenderPipeline& pipeline) noexcept
{
}

void
Atmospheric::onRender(RenderPipeline& pipeline, RenderTargetPtr source) noexcept
{
    Renderable renderable;
    renderable.startVertice = 0;
    renderable.startIndice = 0;
    renderable.numVertices = _sphere->getNumVertices();
    renderable.numIndices = _sphere->getNumIndices();

    auto lights = pipeline.getRenderData(RenderQueue::Lighting);
    for (auto& it : lights)
    {
        auto light = dynamic_cast<Light*>(it);

        if (light->getLightType() == LightType::LT_SUN)
        {
            _lightDirection->assign(light->getLightDirection());

            pipeline.setRenderTarget(source);
            pipeline.setTechnique(_ground);
            pipeline.drawMesh(_sphere, renderable);

            pipeline.setRenderTarget(source);
            pipeline.setTechnique(_sky);
            pipeline.drawMesh(_sphere, renderable);
        }
    }
}

_NAME_END