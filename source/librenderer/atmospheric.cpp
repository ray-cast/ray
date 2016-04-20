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
#include "atmospheric.h"
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

Atmospheric::Setting::Setting() noexcept
{
	outerRadius = 3230.0f;
	innerRadius = 3200.0f;

	wavelength.x = 0.650f; // 650 nm for red
	wavelength.y = 0.570f; // 570 nm for green
	wavelength.z = 0.475f; // 475 nm for blue

	kr = 0.0025;
	km = 0.0025;
	sun = 16.0f;
}

Atmospheric::Atmospheric() noexcept
{
	this->setRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
}

Atmospheric::~Atmospheric() noexcept
{
}

void
Atmospheric::onActivate(RenderPipeline& pipeline) noexcept
{
	MeshProperty mesh;
	mesh.makeSphere(1, 128, 96);

	_renderable.startVertice = 0;
	_renderable.startIndice = 0;
	_renderable.numVertices = mesh.getNumVertices();
	_renderable.numIndices = mesh.getNumIndices();

	_sphere = pipeline.createRenderMesh(mesh);

	_sat = pipeline.createMaterial("sys:fx/atmospheric.fxml.o");

	_sky = _sat->getTech("sky");
	_ground = _sat->getTech("ground");
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

	_invWavelength->uniform3f(invWavelength4);

	_innerRadius->uniform1f(_setting.innerRadius);
	_innerRadius2->uniform1f(_setting.innerRadius * _setting.innerRadius);
	_outerRadius->uniform1f(_setting.outerRadius);
	_outerRadius2->uniform1f(_setting.outerRadius * _setting.outerRadius);

	_krESun->uniform1f(_setting.kr * _setting.sun);
	_kmESun->uniform1f(_setting.km * _setting.sun);

	_kr4PI->uniform1f(float(_setting.kr * 4.0f * M_PI));
	_km4PI->uniform1f(float(_setting.km * 4.0f * M_PI));

	_scaleFactor->uniform1f(1.0f / (_setting.outerRadius - _setting.innerRadius));
	_scaleDepth->uniform1f(0.25f);
	_scaleOverScaleDepth->uniform1f((1.0f / (_setting.outerRadius - _setting.innerRadius)) / 0.25f);
}

void
Atmospheric::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
Atmospheric::onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto& lighting = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lighting)
	{
		if (it->downcast<Light>()->getLightType() != LightType::LightTypeSun)
			continue;

		_lightDirection->uniform3f(-it->downcast<Light>()->getForward());

		pipeline.setFramebuffer(source);
		pipeline.drawMesh(_ground, _sphere, _renderable);
		pipeline.drawMesh(_sky, _sphere, _renderable);
	}

	return false;
}

_NAME_END