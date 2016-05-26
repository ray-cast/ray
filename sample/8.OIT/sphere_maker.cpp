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
#include "sphere_maker.h"
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/res_manager.h>
#include <ray/material.h>

__ImplementSubClass(SphereMakerComponent, GameComponent, "SphereMaker")

ray::float4 diff_spec_parametes[] =
{
	ray::float4(0.0147f,  0.0332f,  0.064f,   0.5f),
	ray::float4(0.0183f,  0.0657f,  0.0248f,  0.5f),
	ray::float4(0.0712f,  0.0263f,  0.03f,    0.5f),
	ray::float4(0.314f,   0.259f,   0.156f,   0.5f),
	ray::float4(0.226f,   0.233f,   0.0489f,  0.5f),
	ray::float4(0.0864f,  0.0597f,  0.0302f,  0.5f),
	ray::float4(0.31f,    0.248f,   0.151f,   0.5f),
	ray::float4(0.0335f,  0.028f,   0.00244f, 0.5f),
	ray::float4(0.0633f,  0.0603f,  0.0372f,  0.5f),
	ray::float4(0.00289f, 0.00263f, 0.00227f, 0.5f),
	ray::float4(0.000547f,0.00133f, 0.0013f,  0.5f),
	ray::float4(0.0192f,  0.0156f,  0.0104f,  0.5f),
	ray::float4(0.00272f, 0.0022f,  0.00169f, 0.5f),
	ray::float4(0.0173f,  0.0142f,  0.00972f, 0.5f),
	ray::float4(0.00793f, 0.0134f,  0.0198f,  0.5f),
	ray::float4(0.0425f,  0.0698f,  0.095f,   0.5f),
	ray::float4(0.0382f,  0.0272f,  0.0119f,  0.5f),
	ray::float4(0.0173f,  0.0221f,  0.0302f,  0.5f),
	ray::float4(0.1f,     0.0613f,  0.0223f,  0.5f),
	ray::float4(0.0633f,  0.0452f,  0.0226f,  0.5f),
	ray::float4(0.0119f,  0.025f,   0.00997f, 0.5f),
	ray::float4(0.0837f,  0.0128f,  0.00775f, 0.5f),
	ray::float4(0.0432f,  0.0167f,  0.00699f, 0.5f),
	ray::float4(0.00817f, 0.0063f,  0.00474f, 0.5f),
	ray::float4(0.0299f,  0.0273f,  0.0196f,  0.5f),
	ray::float4(0.0916f,  0.027f,   0.00942f, 0.5f),
	ray::float4(0.0749f,  0.0414f,  0.027f,   0.5f),
	ray::float4(0.012f,   0.0143f,  0.0267f,  0.5f),
	ray::float4(0.256f,   0.0341f,  0.0102f,  0.5f),
	ray::float4(0.299f,   0.249f,   0.15f,    0.5f),
	ray::float4(0.314f,   0.183f,   0.0874f,  0.5f),
	ray::float4(0.25f,    0.148f,   0.088f,   0.5f),
	ray::float4(0.0122f,  0.0058f,  0.00354f, 0.5f),
	ray::float4(0.0232f,  0.0216f,  0.0349f,  0.5f),
	ray::float4(0.0474f,  0.0375f,  0.0302f,  0.5f),
	ray::float4(0.0731f,  0.0894f,  0.0271f,  0.5f),
	ray::float4(0.247f,   0.0676f,  0.0414f,  0.5f),
	ray::float4(0.296f,   0.182f,   0.12f,    0.5f),
	ray::float4(0.191f,   0.0204f,  0.00426f, 0.5f),
	ray::float4(0.0515f,  0.0327f,  0.0141f,  0.5f),
	ray::float4(0.164f,   0.0796f,  0.0205f,  0.5f),
	ray::float4(0.102f,   0.0887f,  0.0573f,  0.5f),
	ray::float4(0.00727f, 0.0219f,  0.0132f,  0.5f),
	ray::float4(0.00479f, 0.0318f,  0.0267f,  0.5f),
	ray::float4(0.016f,   0.0701f,  0.0538f,  0.5f),
	ray::float4(0.0307f,  0.0267f,  0.0186f,  0.5f),
	ray::float4(0.0591f,  0.0204f,  0.0062f,  0.5f),
	ray::float4(0.152f,   0.023f,   0.00514f, 0.5f),
	ray::float4(0.191f,   0.0302f,  0.0187f,  0.5f),
	ray::float4(0.0152f,  0.00973f, 0.0177f,  0.5f),
	ray::float4(0.069f,   0.0323f,  0.00638f, 0.5f),
	ray::float4(0.0695f,  0.0628f,  0.0446f,  0.5f),
	ray::float4(0.102f,   0.036f,   0.00995f, 0.5f),
	ray::float4(0.252f,   0.186f,   0.106f,   0.5f),
	ray::float4(0.0301f,  0.0257f,  0.0173f,  0.5f),
	ray::float4(0.236f,   0.204f,   0.127f,   0.5f),
	ray::float4(0.325f,   0.0469f,  0.00486f, 0.5f),
	ray::float4(0.096f,   0.0534f,  0.0184f,  0.5f),
	ray::float4(0.41f,    0.124f,   0.00683f, 0.5f),
	ray::float4(0.00198f, 0.0022f,  0.00203f, 0.5f),
	ray::float4(0.418f,   0.0415f,  0.00847f, 0.5f),
	ray::float4(0.181f,   0.129f,   0.0776f,  0.5f),
	ray::float4(0.29f,    0.161f,   0.0139f,  0.5f),
	ray::float4(0.189f,   0.146f,   0.0861f,  0.5f),
	ray::float4(0.288f,   0.18f,    0.0597f,  0.5f),
	ray::float4(0.146f,   0.0968f,  0.0559f,  0.5f),
	ray::float4(0.201f,   0.109f,   0.0599f,  0.5f),
	ray::float4(0.388f,   0.0835f,  0.043f,   0.5f),
	ray::float4(0.267f,   0.236f,   0.161f,   0.5f),
	ray::float4(0.0555f,  0.0578f,  0.0432f,  0.5f),
	ray::float4(0.0194f,  0.0152f,  0.0105f,  0.5f),
	ray::float4(0.0876f,  0.0322f,  0.0165f,  0.5f),
	ray::float4(0.00498f, 0.00255f, 0.00151f, 0.5f),
	ray::float4(0.289f,   0.22f,    0.13f,    0.5f),
	ray::float4(0.0275f,  0.00723f, 0.00123f, 0.5f),
	ray::float4(0.273f,   0.0276f,  0.0186f,  0.5f),
	ray::float4(0.0316f,  0.0308f,  0.0238f,  0.5f),
	ray::float4(0.302f,   0.0316f,  0.00636f, 0.5f),
	ray::float4(0.132f,   0.0182f,  0.00668f, 0.5f),
	ray::float4(0.00568f, 0.00249f, 0.00118f, 0.5f),
	ray::float4(0.167f,   0.0245f,  0.00789f, 0.5f),
	ray::float4(0.276f,   0.0456f,  0.0109f,  0.5f),
	ray::float4(0.242f,   0.0316f,  0.00946f, 0.5f),
	ray::float4(0.161f,   0.0841f,  0.0537f,  0.5f),
	ray::float4(0.0146f,  0.011f,   0.00606f, 0.5f),
	ray::float4(0.021f,   0.0162f,  0.0106f,  0.5f),
	ray::float4(0.0303f,  0.0187f,  0.0122f,  0.5f),
	ray::float4(0.0156f,  0.0162f,  0.0112f,  0.5f),
	ray::float4(0.345f,   0.291f,   0.196f,   0.5f),
	ray::float4(0.303f,   0.261f,   0.178f,   0.5f),
	ray::float4(0.026f,   0.0172f,  0.00442f, 0.5f),
	ray::float4(0.0708f,  0.0167f,  0.013f,   0.5f),
	ray::float4(0.245f,   0.053f,   0.0749f,  0.5f),
	ray::float4(0.00321f, 0.00218f, 0.00141f, 0.5f),
	ray::float4(0.284f,   0.196f,   0.075f,   0.5f),
	ray::float4(0.317f,   0.234f,   0.107f,   0.5f),
	ray::float4(0.312f,   0.265f,   0.178f,   0.5f),
	ray::float4(0.307f,   0.118f,   0.0101f,  0.5f),
	ray::float4(0.293f,   0.104f,   0.0162f,  0.5f),
	ray::float4(0.253f,   0.187f,   0.0263f,  0.5f)
};

ray::float2 spec_gloss_parametes[] =
{
	ray::float2(0.00121383f, 1),
	ray::float2(0.001264066f, 1),
	ray::float2(0.000666173f, 1),
	ray::float2(0.000782239f, 1),
	ray::float2(0.00103061f, 1),
	ray::float2(0.009314418f, 1),
	ray::float2(0.001512634f, 1),
	ray::float2(0.019185166f, 1),
	ray::float2(0.004406822f, 1),
	ray::float2(0.001797238f, 1),
	ray::float2(0.001568741f, 0.945208645f),
	ray::float2(0.00573269f, 0.458015101f),
	ray::float2(0.005357428f, 0.910586764f),
	ray::float2(0.00116849f, 0.583458654f),
	ray::float2(0.000659981f, 1),
	ray::float2(0.004742176f, 0.418942787f),
	ray::float2(0.018918134f, 1),
	ray::float2(0.011137844f, 1),
	ray::float2(0.022533376f, 0.943868871f),
	ray::float2(0.03561164f, 0.48518681f),
	ray::float2(0.003614732f, 1),
	ray::float2(0.003173843f, 1),
	ray::float2(0.00796655f, 0.489177717f),
	ray::float2(0.015880952f, 1),
	ray::float2(0.017748664f, 1),
	ray::float2(0.00776415f, 0.459261041f),
	ray::float2(0.04878524f, 1),
	ray::float2(0.005505024f, 0.382160827f),
	ray::float2(0.003083858f, 0.391669218f),
	ray::float2(0.00661538f, 0.490921191f),
	ray::float2(0.000900249f, 0.49850679f),
	ray::float2(0.000239042f, 0.562529458f),
	ray::float2(0.0000679821f, 0.490521275f),
	ray::float2(0.000260137f, 0.525606924f),
	ray::float2(0.002095382f, 0.332456007f),
	ray::float2(0.000378756f, 0.610056718f),
	ray::float2(0.011542062f, 0.257730557f),
	ray::float2(0.008505336f, 0.284649209f),
	ray::float2(0.002339532f, 0.358103987f),
	ray::float2(0.008953678f, 0.541032539f),
	ray::float2(0.02805926f, 0.400125682f),
	ray::float2(0.005794026f, 0.77095137f),
	ray::float2(0.001732392f, 1),
	ray::float2(0.01726392f, 0.474609615f),
	ray::float2(0.000728329f, 1),
	ray::float2(0.02363148f, 1),
	ray::float2(0.00889345f, 0.493160556f),
	ray::float2(0.001304914f, 1),
	ray::float2(0.00192188f, 1),
	ray::float2(0.01935222f, 0.407419801f),
	ray::float2(0.04748044f, 0.414529103f),
	ray::float2(0.06273436f, 0.479139899f),
	ray::float2(0.010106036f, 0.502892822f),
	ray::float2(0.006244098f, 0.490387649f),
	ray::float2(0.05095432f, 0.77095137f),
	ray::float2(0.0048657f, 0.596014835f),
	ray::float2(0.000790428f, 1),
	ray::float2(0.004913678f, 1),
	ray::float2(0.005298618f, 0.353610396f),
	ray::float2(0.010628338f, 0.695722625f),
	ray::float2(0.005128198f, 0.380012827f),
	ray::float2(0.03923218f, 0.409101295f),
	ray::float2(0.011645952f, 0.244086726f),
	ray::float2(0.03621944f, 0.368601082f),
	ray::float2(0.00396387f, 0.930769633f),
	ray::float2(0.008632844f, 0.495355626f),
	ray::float2(0.004287808f, 0.828703016f),
	ray::float2(0.001855682f, 0.388366101f),
	ray::float2(0.00399621f, 0.346997071f),
	ray::float2(0.00786954f, 0.612307841f),
	ray::float2(0.000269286f, 0.508142297f),
	ray::float2(0.0f, 0.041234838f),
	ray::float2(0.00452736f, 0.581122219f),
	ray::float2(0.004219234f, 0.404559422f),
	ray::float2(0.011270334f, 0.541876471f),
	ray::float2(0.007131702f, 0.596014835f),
	ray::float2(0.006442842f, 0.65685837f),
	ray::float2(0.000794115f, 1),
	ray::float2(0.006105202f, 0.472901056f),
	ray::float2(0.010215618f, 0.514346194f),
	ray::float2(0.001915708f, 1),
	ray::float2(0.006771836f, 0.409932584f),
	ray::float2(0.001691485f, 1),
	ray::float2(0.001886298f, 0.94454078f),
	ray::float2(0.00320641f, 1),
	ray::float2(0.009570082f, 0.90351341f),
	ray::float2(0.017150268f, 1),
	ray::float2(0.011789742f, 1),
	ray::float2(0.0f, 0.001104253f),
	ray::float2(0.009031378f, 0.459966777f),
	ray::float2(0.01779272f, 1),
	ray::float2(0.000694197f, 1),
	ray::float2(0.004880858f, 0.419956278f),
	ray::float2(0.01029147f, 0.631496413f),
	ray::float2(0.000683875f, 1),
	ray::float2(0.006194334f, 0.487817693f),
	ray::float2(0.005380284f, 0.689453539f),
	ray::float2(0.000367343f, 1),
	ray::float2(0.003435382f, 0.791362491f),
	ray::float2(0.00373529f, 0.423187627)
};

SphereMakerComponent::SphereMakerComponent() noexcept
{
}

SphereMakerComponent::~SphereMakerComponent() noexcept
{
}

void
SphereMakerComponent::onActivate() except
{
	auto diffuseMap = ray::ResManager::instance()->createTexture("dlc:EnvLighting/texture/Bricks_ao.dds", ray::GraphicsTextureDim::GraphicsTextureDim2D);
	if (!diffuseMap)
		return;

	auto normalMap = ray::ResManager::instance()->createTexture("dlc:EnvLighting/texture/Bricks_n.dds", ray::GraphicsTextureDim::GraphicsTextureDim2D);
	if (!normalMap)
		return;

	auto materialTemp = ray::ResManager::instance()->createMaterial("sys:fx/transparent_skinning0.fxml");
	if (!materialTemp)
		return;

	auto sphereMesh = std::make_shared<ray::MeshProperty>();
	sphereMesh->makeSphere(1.0, 64, 48);

	for (std::size_t i = 0; i < 10; i++)
	{
		for (std::size_t j = 0; j < 10; j++)
		{
			auto gameObject = std::make_shared<ray::GameObject>();
			gameObject->setActive(true);
			gameObject->addComponent(std::make_shared<ray::MeshComponent>(sphereMesh));
			gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(materialTemp->clone()));
			gameObject->setScale(ray::float3(0.8));
			gameObject->setTranslate(ray::float3(-10.0f + i * 2.0f, 0, j * 2.0f));

			auto material = gameObject->getComponent<ray::MeshRenderComponent>()->getMaterial();
			material->getParameter("quality")->uniform4f(ray::float4(1.0, 1.0, 0.0,0.0));
			material->getParameter("diffuse")->uniform4f(ray::float4(diff_spec_parametes[i * 10 + j].xyz(), 0.9));
			material->getParameter("specular")->uniform1f(spec_gloss_parametes[i * 10 + j].x);
			material->getParameter("shininess")->uniform1f(spec_gloss_parametes[i * 10 + j].y);
			material->getParameter("texDiffuse")->uniformTexture(diffuseMap);
			material->getParameter("texNormal")->uniformTexture(normalMap);

			_objects.push_back(gameObject);
		}
	}
}

void
SphereMakerComponent::onDeactivate() noexcept
{
}

ray::GameComponentPtr
SphereMakerComponent::clone() const noexcept
{
	return std::make_shared<SphereMakerComponent>();
}