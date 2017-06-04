// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include "UIController.h"
#include "UIView.h"

#include <chrono>
#include <ctime>
#include <cinttypes>
#include <iomanip>
#include <sstream>

#include <ray/fcntl.h>
#include <ray/except.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/jsonreader.h>

#include "first_person_camera.h"

#include <ray/camera_component.h>
#include <ray/light_component.h>
#include <ray/mesh_component.h>
#include <ray/mesh_render_component.h>

#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/render_system.h>

#include <ray/input.h>
#include <ray/input_feature.h>
#include <ray/game_server.h>

#include <ray/res_manager.h>
#include <ray/material.h>

#include <ray/image.h>
#include <ray/imagutil.h>
#include <ray/imagcubemap.h>

#include <ray/SH.h>

__ImplementSubClass(GuiControllerComponent, ray::GameComponent, "GuiController")

ray::float3 diff_spec_parametes[] =
{
	ray::float3(0.0147f,  0.0332f,  0.064f),
	ray::float3(0.0183f,  0.0657f,  0.0248f),
	ray::float3(0.0712f,  0.0263f,  0.03f),
	ray::float3(0.314f,   0.259f,   0.156f),
	ray::float3(0.226f,   0.233f,   0.0489f),
	ray::float3(0.0864f,  0.0597f,  0.0302f),
	ray::float3(0.31f,    0.248f,   0.151f),
	ray::float3(0.0335f,  0.028f,   0.00244),
	ray::float3(0.0633f,  0.0603f,  0.0372f),
	ray::float3(0.00289f, 0.00263f, 0.00227),
	ray::float3(0.000547f,0.00133f, 0.0013f),
	ray::float3(0.0192f,  0.0156f,  0.0104f),
	ray::float3(0.00272f, 0.0022f,  0.00169),
	ray::float3(0.0173f,  0.0142f,  0.00972),
	ray::float3(0.00793f, 0.0134f,  0.0198f),
	ray::float3(0.0425f,  0.0698f,  0.095f),
	ray::float3(0.0382f,  0.0272f,  0.0119f),
	ray::float3(0.0173f,  0.0221f,  0.0302f),
	ray::float3(0.1f,     0.0613f,  0.0223f),
	ray::float3(0.0633f,  0.0452f,  0.0226f),
	ray::float3(0.0119f,  0.025f,   0.00997),
	ray::float3(0.0837f,  0.0128f,  0.00775),
	ray::float3(0.0432f,  0.0167f,  0.00699),
	ray::float3(0.00817f, 0.0063f,  0.00474),
	ray::float3(0.0299f,  0.0273f,  0.0196f),
	ray::float3(0.0916f,  0.027f,   0.00942),
	ray::float3(0.0749f,  0.0414f,  0.027f),
	ray::float3(0.012f,   0.0143f,  0.0267f),
	ray::float3(0.256f,   0.0341f,  0.0102f),
	ray::float3(0.299f,   0.249f,   0.15f),
	ray::float3(0.314f,   0.183f,   0.0874f),
	ray::float3(0.25f,    0.148f,   0.088f),
	ray::float3(0.0122f,  0.0058f,  0.00354),
	ray::float3(0.0232f,  0.0216f,  0.0349f),
	ray::float3(0.0474f,  0.0375f,  0.0302f),
	ray::float3(0.0731f,  0.0894f,  0.0271f),
	ray::float3(0.247f,   0.0676f,  0.0414f),
	ray::float3(0.296f,   0.182f,   0.12f),
	ray::float3(0.191f,   0.0204f,  0.00426),
	ray::float3(0.0515f,  0.0327f,  0.0141f),
	ray::float3(0.164f,   0.0796f,  0.0205f),
	ray::float3(0.102f,   0.0887f,  0.0573f),
	ray::float3(0.00727f, 0.0219f,  0.0132f),
	ray::float3(0.00479f, 0.0318f,  0.0267f),
	ray::float3(0.016f,   0.0701f,  0.0538f),
	ray::float3(0.0307f,  0.0267f,  0.0186f),
	ray::float3(0.0591f,  0.0204f,  0.0062f),
	ray::float3(0.152f,   0.023f,   0.00514),
	ray::float3(0.191f,   0.0302f,  0.0187f),
	ray::float3(0.0152f,  0.00973f, 0.0177f),
	ray::float3(0.069f,   0.0323f,  0.00638),
	ray::float3(0.0695f,  0.0628f,  0.0446f),
	ray::float3(0.102f,   0.036f,   0.00995),
	ray::float3(0.252f,   0.186f,   0.106f),
	ray::float3(0.0301f,  0.0257f,  0.0173f),
	ray::float3(0.236f,   0.204f,   0.127f),
	ray::float3(0.325f,   0.0469f,  0.00486),
	ray::float3(0.096f,   0.0534f,  0.0184f),
	ray::float3(0.41f,    0.124f,   0.00683),
	ray::float3(0.00198f, 0.0022f,  0.00203),
	ray::float3(0.418f,   0.0415f,  0.00847),
	ray::float3(0.181f,   0.129f,   0.0776f),
	ray::float3(0.29f,    0.161f,   0.0139f),
	ray::float3(0.189f,   0.146f,   0.0861f),
	ray::float3(0.288f,   0.18f,    0.0597f),
	ray::float3(0.146f,   0.0968f,  0.0559f),
	ray::float3(0.201f,   0.109f,   0.0599f),
	ray::float3(0.388f,   0.0835f,  0.043f),
	ray::float3(0.267f,   0.236f,   0.161f),
	ray::float3(0.0555f,  0.0578f,  0.0432f),
	ray::float3(0.0194f,  0.0152f,  0.0105f),
	ray::float3(0.0876f,  0.0322f,  0.0165f),
	ray::float3(0.00498f, 0.00255f, 0.00151),
	ray::float3(0.289f,   0.22f,    0.13f),
	ray::float3(0.0275f,  0.00723f, 0.00123),
	ray::float3(0.273f,   0.0276f,  0.0186f),
	ray::float3(0.0316f,  0.0308f,  0.0238f),
	ray::float3(0.302f,   0.0316f,  0.00636),
	ray::float3(0.132f,   0.0182f,  0.00668),
	ray::float3(0.00568f, 0.00249f, 0.00118),
	ray::float3(0.167f,   0.0245f,  0.00789),
	ray::float3(0.276f,   0.0456f,  0.0109f),
	ray::float3(0.242f,   0.0316f,  0.00946),
	ray::float3(0.161f,   0.0841f,  0.0537f),
	ray::float3(0.0146f,  0.011f,   0.00606),
	ray::float3(0.021f,   0.0162f,  0.0106f),
	ray::float3(0.562,  0.565,  0.578),
	ray::float3(0.8f,  0.8f,  0.8f),
	ray::float3(0.345f,   0.291f,   0.196f),
	ray::float3(0.303f,   0.261f,   0.178f),
	ray::float3(0.026f,   0.0172f,  0.00442),
	ray::float3(0.0708f,  0.0167f,  0.013f),
	ray::float3(0.245f,   0.053f,   0.0749f),
	ray::float3(0.00321f, 0.00218f, 0.00141),
	ray::float3(0.284f,   0.196f,   0.075f),
	ray::float3(0.317f,   0.234f,   0.107f),
	ray::float3(0.312f,   0.265f,   0.178f),
	ray::float3(0.307f,   0.118f,   0.0101f),
	ray::float3(0.293f,   0.104f,   0.0162f),
	ray::float3(0.253f,   0.187f,   0.0263f)
};

float shininessParams[] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0.945208645f, 0.458015101f, 0.910586764f, 0.583458654f, 1, 0.418942787f, 1, 1, 0.943868871f,0.48518681f,
	1, 1, 0.489177717f, 1, 1, 0.459261041f, 1, 0.382160827f, 0.391669218f, 0.490921191f,
	0.49850679f, 0.562529458f, 0.490521275f, 0.525606924f, 0.332456007f, 0.610056718f, 0.257730557f, 0.284649209f, 0.358103987f,0.541032539f,
	0.400125682f, 0.77095137f,1, 0.474609615f, 1, 1, 0.493160556f, 1, 1, 0.407419801f,
	0.414529103f, 0.479139899f, 0.502892822f, 0.490387649f, 0.77095137f, 0.596014835f, 1, 1, 0.353610396f,0.695722625f,
	0.380012827f, 0.409101295f, 0.244086726f, 0.368601082f, 0.930769633f, 0.495355626f, 0.828703016f, 0.388366101f, 0.346997071f, 0.612307841f,
	0.508142297f, 0.041234838f, 0.581122219f, 0.404559422f, 0.541876471f, 0.596014835f, 0.65685837f, 1, 0.472901056f,0.514346194f,
	1, 0.409932584f, 1, 0.94454078f, 1, 0.90351341f, 0.94, 1, 0.4504253f, 0.459966777f,
	1, 1, 0.419956278f, 0.631496413f, 1, 0.487817693f, 0.689453539f, 1, 0.791362491f, 0.423187627f
};

float metalnessParams[] =
{
	0.00121383f,  0.001264066f, 0.000666173f, 0.000782239f, 0.00103061f, 0.009314418f, 0.001512634f, 0.019185166f, 0.004406822f, 0.001797238f,
	0.001568741f, 0.00573269f,  0.005357428f, 0.00116849f,  0.000659981f,0.004742176f, 0.018918134f, 0.011137844f, 0.022533376f, 0.03561164f,
	0.003614732f, 0.003173843f, 0.00796655f, 0.015880952f, 0.017748664f, 0.00776415f, 0.04878524f, 	0.005505024f, 0.003083858f, 0.00661538f,
	0.000900249f, 0.000239042f, 0.0000679821f, 0.000260137f, 0.002095382f, 0.000378756f, 0.011542062f, 0.008505336f, 0.002339532f, 0.008953678f,
	0.02805926f, 0.005794026f, 0.001732392f, 0.01726392f, 0.000728329f,  0.02363148f, 0.00889345f, 0.001304914f, 0.00192188f, 0.01935222f,
	0.04748044f,  0.06273436f, 0.010106036f, 0.006244098f, 0.05095432f, 0.0048657f, 0.000790428f, 0.004913678f, 0.005298618f, 0.010628338f,
	0.005128198f, 0.03923218f,  0.011645952f, 0.03621944f, 0.00396387f, 0.008632844f, 0.004287808f, 0.001855682f, 0.00399621f, 0.00786954f,
	0.000269286f, 0.0f          , 0.00452736f,  0.004219234f, 0.011270334f, 0.007131702f, 0.006442842f, 0.000794115f, 0.006105202f, 0.010215618f,
	0.001915708f, 0.006771836f, 0.001691485f, 0.001886298f, 0.00320641f, 0.009570082f, 1.0f, 0.0f, 0.0f, 0.009031378f,
	0.01779272f, 0.000694197f, 	0.004880858f, 0.01029147f, 0.000683875f, 0.006194334f, 0.005380284f, 0.000367343f, 0.003435382f, 0.00373529
};

class AppMapListener : public ray::LightMapListener
{
public:
	AppMapListener() noexcept
		: _lastProgress(0)
	{
	}

	~AppMapListener() noexcept {}

	void onUvmapperStart()
	{
		_startTime = std::time(nullptr);
		std::cerr << "Calculating the Light map pack of the model : ";
		std::cerr << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	void onUvmapperProgressing(float progress)
	{
		if (_lastProgress != progress)
		{
			_lastProgress = progress;
		}
	}

	void onUvmapperEnd()
	{
		_endTime = std::time(nullptr);
		std::cerr << "Calculated the Light map pack of the model : ";
		std::cerr << std::put_time(std::localtime(&_endTime), "end time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onMessage(const ray::util::string& message) noexcept
	{
		std::cerr << message << std::endl;
	}

private:
	float _lastProgress;
	std::time_t _startTime;
	std::time_t _endTime;
};

class AppMassListener : public ray::LightMassListener
{
public:
	AppMassListener() noexcept {}
	~AppMassListener() noexcept {}

	virtual void onBakingStart() noexcept
	{
		_startTime = std::time(nullptr);
		std::cerr << "Calculating the radiosity of the model : ";
		std::cerr << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onBakingEnd() noexcept
	{
		_endTime = std::time(nullptr);
		std::cerr << "Calculated the radiosity of the model : ";
		std::cerr << std::put_time(std::localtime(&_endTime), "end time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onBakingProgressing(float progress) noexcept
	{
	}

	virtual void onMessage(const ray::util::string& message) noexcept
	{
		std::cerr << message << std::endl;
	}

private:
	std::time_t _startTime;
	std::time_t _endTime;
};

GuiControllerComponent::GuiControllerComponent() noexcept
	: _lightMapListener(std::make_shared<AppMapListener>())
	, _lightMassListener(std::make_shared<AppMassListener>())
	, _stopUVMapper(false)
	, _stopLightmass(false)
{
}

GuiControllerComponent::~GuiControllerComponent() noexcept
{
	this->onUVMapperCancel();
	this->onLightMassCancel();
}

ray::GameComponentPtr
GuiControllerComponent::clone() const noexcept
{
	return std::make_shared<GuiControllerComponent>();
}

bool
GuiControllerComponent::makeCubeObject() noexcept
{
	ray::MaterialPtr material;
	if (!ray::ResManager::instance()->createMaterial("sys:fx/wireframe.fxml", material))
		return false;

	auto cubeMesh = std::make_shared<ray::MeshProperty>();
	cubeMesh->makeCubeWireframe(1.0, 1.0, 1.0);

	material->getParameter("diffuse")->uniform4f(0.0, 0.8, 0.0, 0.9);

	auto gameObject = std::make_shared<ray::GameObject>();
	gameObject->setActive(true);
	gameObject->setScaleAll(0.0f);

	gameObject->addComponent(std::make_shared<ray::MeshComponent>(cubeMesh));
	gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(material));

	_cube = std::move(gameObject);

	return true;
}

bool
GuiControllerComponent::makeMainCamera() noexcept
{
	ray::GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(ray::Gui::getDisplaySize().x);
	textureDesc.setHeight(ray::Gui::getDisplaySize().y);
	textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
	if (!renderTexture)
		return false;

	ray::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(ray::GraphicsAttachmentLayout(0, ray::GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm));
	auto framebufferLayout = ray::RenderSystem::instance()->createFramebufferLayout(framebufferLayoutDesc);
	if (!framebufferLayout)
		return false;

	ray::GraphicsFramebufferDesc framebufferDesc;
	framebufferDesc.setWidth(ray::Gui::getDisplaySize().x);
	framebufferDesc.setHeight(ray::Gui::getDisplaySize().y);
	framebufferDesc.addColorAttachment(ray::GraphicsAttachmentBinding(std::move(renderTexture), 0, 0));
	framebufferDesc.setGraphicsFramebufferLayout(std::move(framebufferLayout));
	auto framebuffer = ray::RenderSystem::instance()->createFramebuffer(framebufferDesc);
	if (!framebuffer)
		return false;

	auto camera = std::make_shared<ray::CameraComponent>();
	camera->setAperture(65.0f);
	camera->setNear(0.1f);
	camera->setFar(20000.0f);
	camera->setFramebuffer(framebuffer);
	camera->setCameraRenderFlags(ray::CameraRenderFlagBits::CameraRenderTextureBit | ray::CameraRenderFlagBits::CameraRenderShadingBit);

	auto object = std::make_shared<ray::GameObject>();
	object->setName("main camera");
	object->addComponent(camera);
	object->addComponent(std::make_shared<FirstPersonCameraComponent>());
	object->setActive(true);
	object->setTranslate(ray::float3(32, 15, 30));
	object->setQuaternion(ray::Quaternion(ray::float3(25.0f, 226.0f, 0.0f)));

	this->getGameObject()->addChild(object);

	_cameras.push_back(std::move(object));

	return true;
}

bool
GuiControllerComponent::makeSphereObjects() noexcept
{
	ray::GraphicsTexturePtr diffuseMap;
	ray::ResManager::instance()->createTexture("dlc:common/textures/Bricks_ao.dds", diffuseMap);
	if (!diffuseMap)
		return false;

	ray::GraphicsTexturePtr normalMap;
	ray::ResManager::instance()->createTexture("dlc:common/textures/Bricks_n.dds", normalMap);
	if (!normalMap)
		return false;

	ray::MaterialPtr material;
	if (!ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml", material))
		return false;

	auto sphereMesh = std::make_shared<ray::MeshProperty>();
	sphereMesh->makeSphere(1.0, 48, 36);

	auto gameObject = std::make_shared<ray::GameObject>();
	gameObject->setActive(true);
	gameObject->setScaleAll(2.5f);

	gameObject->addComponent(std::make_shared<ray::MeshComponent>(std::move(sphereMesh)));
	gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(std::move(material)));

	for (std::uint8_t i = 0; i < 10; i++)
	{
		for (std::uint8_t j = 0; j < 10; j++)
		{
			char buf[MAX_PATH];
			std::sprintf(buf, "sphere_%u_%u", i, j);

			auto newGameObject = gameObject->clone();
			newGameObject->setName(buf);
			newGameObject->setActive(true);
			newGameObject->setTranslate(ray::float3(-25.0f + i * 5.5f, 3, -25.0f + j * 5.5f));

			auto material = newGameObject->getComponent<ray::MeshRenderComponent>()->getMaterial();

			material->getParameter("quality")->uniform4f(ray::float4(1.0, 1.0, 0.0, 0.0));
			material->getParameter("diffuse")->uniform3f(diff_spec_parametes[i * 10 + j].xyz());
			material->getParameter("metalness")->uniform1f(0);
			material->getParameter("specular")->uniform3f(0.5, 0.5, 0.5);

			if (shininessParams[i * 10 + j] > 0.45 &&
				shininessParams[i * 10 + j] < 0.8 ||
				shininessParams[i * 10 + j] > 0.95)
			{
				material->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
			}

			material->getParameter("smoothness")->uniform1f(shininessParams[i * 10 + j]);
			material->getParameter("texDiffuse")->uniformTexture(diffuseMap);
			material->getParameter("texNormal")->uniformTexture(normalMap);

			_objects.push_back(std::move(newGameObject));
		}
	}

	return true;
}

void
GuiControllerComponent::onAttachComponent(const ray::GameComponentPtr& component) except
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();

		GuiViewDelegates delegate;

		delegate.onModelImport = std::bind(&GuiControllerComponent::onModelImport, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onModelSaveAs = std::bind(&GuiControllerComponent::onModelSaveAs, this, std::placeholders::_1, std::placeholders::_2);

		delegate.onUVMapperCancel = std::bind(&GuiControllerComponent::onUVMapperCancel, this);
		delegate.onUVMapperWillStart = std::bind(&GuiControllerComponent::onUVMapperWillStart, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onUVMapperProcess = std::bind(&GuiControllerComponent::onUVMapperProcessing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		delegate.onLightMassCancel = std::bind(&GuiControllerComponent::onLightMassCancel, this);
		delegate.onLightMassWillStart = std::bind(&GuiControllerComponent::onLightMassWillStart, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onLightMassProcess = std::bind(&GuiControllerComponent::onLightMassProcessing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		delegate.onLightMassSave = std::bind(&GuiControllerComponent::onLightMassSave, this, std::placeholders::_1, std::placeholders::_2);

		delegate.onFetchCamera = std::bind(&GuiControllerComponent::onFetchCamera, this, std::placeholders::_1);
		delegate.onFetchMeshes = std::bind(&GuiControllerComponent::onFetchMeshes, this, std::placeholders::_1);
		delegate.onFetchLights = std::bind(&GuiControllerComponent::onFetchLights, this, std::placeholders::_1);
		delegate.onFetchLightProbes = std::bind(&GuiControllerComponent::onFetchLightProbes, this, std::placeholders::_1);

		delegate.onSeletedCamera = std::bind(&GuiControllerComponent::onSeletedCamera, this, std::placeholders::_1);
		delegate.onSeletedMesh = std::bind(&GuiControllerComponent::onSeletedMesh, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onSeletedLight = std::bind(&GuiControllerComponent::onSeletedLight, this, std::placeholders::_1);
		delegate.onSeletedLightProbe = std::bind(&GuiControllerComponent::onSeletedLightProbe, this, std::placeholders::_1);

		view->setGuiViewDelegates(delegate);
	}
}

void
GuiControllerComponent::onDetachComponent(const ray::GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();

		GuiViewDelegates delegate;
		view->setGuiViewDelegates(delegate);
	}
}

void
GuiControllerComponent::onActivate() except
{
	this->makeCubeObject();
	this->makeMainCamera();
	this->makeSphereObjects();
}

bool
GuiControllerComponent::onModelImport(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	try
	{
		ray::StreamReaderPtr stream;
		if (!ray::IoServer::instance()->openFile(stream, path))
		{
			error = "Failed to open file.";
			return false;
		}

		ray::PMXHandler header;
		if (!header.doCanRead(*stream))
			return false;

		_model = std::make_unique<ray::PMX>();
		if (!header.doLoad(*stream, *_model))
			return false;

		auto gameObject = std::make_shared<ray::GameObject>();

		if (_model->numVertices > 0 && _model->numIndices > 0)
		{
			ray::PMX_Index* indicesData = _model->indices.data();

			ray::Float3Array vertices(_model->numVertices);
			ray::Float3Array normals(_model->numVertices);
			ray::Float2Array texcoords(_model->numVertices);
			ray::UintArray indices(_model->numIndices);

			for (std::size_t i = 0; i < _model->numVertices; i++)
			{
				const auto& v = _model->vertices[i];

				vertices[i] = v.position;
				normals[i] = v.normal;
				texcoords[i] = v.coord;
			}

			for (std::uint32_t i = 0; i < _model->numIndices; i++, indicesData += _model->header.sizeOfIndices)
			{
				if (_model->header.sizeOfIndices == 1)
					indices[i] = *(std::uint8_t*)indicesData;
				else if (_model->header.sizeOfIndices == 2)
					indices[i] = *(std::uint16_t*)indicesData;
				else if (_model->header.sizeOfIndices == 4)
					indices[i] = *(std::uint32_t*)indicesData;
				else
					return false;
			}

			ray::MeshSubsets subsets;
			std::uint32_t startIndices = 0;

			for (auto& it : _model->materials)
			{
				subsets.push_back(ray::MeshSubset(0, startIndices, it.IndicesCount, 0, 0));
				startIndices += it.IndicesCount;
			}

			auto mesh = std::make_shared<ray::MeshProperty>();
			mesh->setVertexArray(std::move(vertices));
			mesh->setNormalArray(std::move(normals));
			mesh->setTexcoordArray(std::move(texcoords));
			mesh->setIndicesArray(std::move(indices));
			mesh->setMeshSubsets(std::move(subsets));

			gameObject->addComponent(std::make_shared<ray::MeshComponent>(std::move(mesh)));
		}

		if (_model->numMaterials)
		{
			ray::MaterialPtr materialTemp;
			if (!ray::ResManager::instance()->createMaterial("sys:fx/opacity.fxml", materialTemp))
				return false;

			ray::Materials materials(_model->numMaterials);

			auto ShininessToSmoothness = [](float spec)
			{
				return 1.0f - pow(std::max(0.0f, 2.0f / (spec + 2)), 0.25f);
			};

			for (std::size_t i = 0; i < _model->numMaterials; i++)
			{
				auto material = materialTemp->clone();

				material->getParameter("quality")->uniform4f(ray::float4(0.0, 0.0, 0.0, 0.0));
				material->getParameter("diffuse")->uniform3f(ray::math::srgb2linear(_model->materials[i].Diffuse));
				material->getParameter("metalness")->uniform1f(0.0);
				material->getParameter("specular")->uniform3f(0.5, 0.5, 0.5);
				material->getParameter("smoothness")->uniform1f(ShininessToSmoothness(_model->materials[i].Shininess));

				std::int16_t textureID = 0;
				if (_model->header.sizeOfTexture == 1)
					textureID = (_model->materials[i].TextureIndex == 255) ? -1 : _model->materials[i].TextureIndex;
				else
					textureID = (_model->materials[i].TextureIndex >= 65535) ? -1 : _model->materials[i].TextureIndex;

				if (textureID >= 0)
				{
					char name[MAX_PATH];
					::wcstombs(name, _model->textures[textureID].name, _model->textures[textureID].length);

					ray::GraphicsTexturePtr texture;
					if (ray::ResManager::instance()->createTexture(ray::util::directory(path) + name, texture))
					{
						material->getParameter("quality")->uniform4f(ray::float4(1.0, 0.0, 0.0, 0.0));
						material->getParameter("texDiffuse")->uniformTexture(texture);
					}
				}

				materials[i] = material;
			}

			gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(std::move(materials)));
		}

		gameObject->setActive(true);

		_objects.clear();
		_objects.push_back(gameObject);

		_cube->setScaleAll(0.0f);

		return true;
	}
	catch (const std::exception&)
	{
		_model.reset();
		error = "Unkonwn error.";
		return false;
	}
}

bool
GuiControllerComponent::onModelSaveAs(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	if (_model)
	{
		ray::StreamWritePtr stream;
		if (!ray::IoServer::instance()->openFile(stream, path))
			return false;

		ray::PMXHandler header;
		if (!header.doSave(*stream, *_model))
			return false;

		return true;
	}
	else
	{
		return this->onOutputSphere(path, error);
	}
}

bool
GuiControllerComponent::onUVMapperWillStart(const GuiParams& params, ray::util::string::pointer& error) noexcept
{
	if (!_model)
	{
		error = "Please load model before start uv mapper";
		return false;
	}

	return  true;
}

bool
GuiControllerComponent::onUVMapperCancel() noexcept
{
	if (_future)
	{
		_stopUVMapper = true;
		_stopLightmass = true;
		_future->wait();
		_future.reset();
	}

	return true;
}

bool
GuiControllerComponent::onUVMapperProcessing(const GuiParams& params, float& progressing, ray::util::string::pointer& error) noexcept
{
	if (!_model)
	{
		error = "Please load model before start uv mapper";
		return false;
	}

	if (!_future)
	{
		static auto progress = [&](float progress) -> HRESULT
		{
			progressing = progress;
			return _stopUVMapper ? -1 : S_OK;
		};

		_future = std::make_unique<std::future<bool>>(std::async(std::launch::async, [&]() -> bool
		{
			std::uint32_t size = 512;
			if (params.lightmass.imageSize == 1)
				size = 1024;
			else if (params.lightmass.imageSize == 2)
				size = 2048;
			else if (params.lightmass.imageSize == 3)
				size = 4096;
			else if (params.lightmass.imageSize == 4)
				size = 8192;

			ray::LightMapPack lightPack(_lightMapListener);
			if (!lightPack.atlasUV(*_model, size, size, params.uvmapper.chart, params.uvmapper.stretch, params.uvmapper.margin, progress))
				return false;

			return true;
		}));

		return true;
	}
	else
	{
		if (_future->wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout)
			return true;

		bool succeeded = _future->get();
		_future.reset();

		return false;
	}
}

bool
GuiControllerComponent::onLightMassWillStart(const GuiParams& params, ray::util::string::pointer& error) noexcept
{
	if (!_model)
	{
		error = "Please load model before start lightmass";
		return false;
	}

	return  true;
}

bool
GuiControllerComponent::onLightMassCancel() noexcept
{
	if (_future)
	{
		_stopUVMapper = true;
		_stopLightmass = true;
		_future->wait();
		_future.reset();
	}

	return true;
}

bool
GuiControllerComponent::onLightMassProcessing(const GuiParams& options, float& progressing, ray::util::string::pointer& error) noexcept
{
	if (!_model)
	{
		error = "Please load model before start lightmass";
		return false;
	}

	if (!_future)
	{
		if (options.uvmapper.slot > _model->header.addUVCount)
			return false;

		static auto progress = [&](float progress) -> bool
		{
			progressing = progress;
			return !_stopLightmass;
		};

		_future = std::make_unique<std::future<bool>>(std::async(std::launch::async, [&]() -> bool
		{
			std::uint32_t size = 512;
			if (options.lightmass.imageSize == 1)
				size = 1024;
			else if (options.lightmass.imageSize == 2)
				size = 2048;
			else if (options.lightmass.imageSize == 3)
				size = 4096;
			else if (options.lightmass.imageSize == 4)
				size = 8192;

			ray::LightMassParams params;
			params.baking.environmentColor = options.lightmass.environmentColor.xyz() * options.lightmass.environmentColor.w;
			params.baking.hemisphereFar = options.lightmass.hemisphereFar;
			params.baking.hemisphereNear = options.lightmass.hemisphereNear;
			params.baking.hemisphereSize = options.lightmass.sampleCount * 32 + 32;
			params.baking.interpolationPasses = options.lightmass.interpolationPasses;
			params.baking.interpolationThreshold = options.lightmass.interpolationThreshold;
			params.baking.listener = progress;

			params.model.vertices = (std::uint8_t*)_model->vertices.data();
			params.model.indices = _model->indices.data();
			params.model.sizeofVertices = sizeof(ray::PMX_Vertex);
			params.model.sizeofIndices = _model->header.sizeOfIndices;
			params.model.strideVertices = offsetof(ray::PMX_Vertex, position);
			params.model.strideTexcoord = options.uvmapper.slot ? offsetof(ray::PMX_Vertex, addCoord[options.uvmapper.slot - 1]) : offsetof(ray::PMX_Vertex, coord);
			params.model.numVertices = _model->numVertices;
			params.model.numIndices = _model->numIndices;
			params.model.subsets.resize(_model->numMaterials);

			for (std::uint32_t i = 0; i < _model->numMaterials; i++)
			{
				std::uint32_t offset = 0;

				for (std::uint32_t j = 0; j < i; j++)
					offset += _model->materials[j].IndicesCount;

				params.model.subsets[i].emissive = ray::math::srgb2linear(_model->materials[i].Ambient) * _model->materials[i].Shininess;

				params.model.subsets[i].drawcall.count = _model->materials[i].IndicesCount;
				params.model.subsets[i].drawcall.instanceCount = 1;
				params.model.subsets[i].drawcall.firstIndex = offset;
				params.model.subsets[i].drawcall.baseInstance = 0;
				params.model.subsets[i].drawcall.baseVertex = 0;
			}

			auto lightMass = std::make_unique<ray::LightMass>();
			lightMass->setLightMapData(std::make_shared<ray::LightMapData>(size, size, options.lightmass.enableGI ? 4 : 1));
			lightMass->setLightMassListener(_lightMassListener);

			if (!lightMass->open(params))
				return false;

			if (!lightMass->start())
				return false;

			_lightMapData = lightMass->getLightMapData();
			return true;
		}));

		return true;
	}
	else
	{
		if (_future->wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout)
			return true;

		_future.reset();

		return false;
	}
}

bool
GuiControllerComponent::onLightMassSave(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	if (!_lightMapData)
	{
		error = "Light map cannot be empty";
		return false;
	}

	auto data = _lightMapData;

	bool isGreyscale = data->channel == 1;
	bool hasAlpha = data->channel == 1 ? false : true;

	int margin = 2;

	if (margin > 0)
	{
		std::unique_ptr<float[]> lightmapTemp = std::make_unique<float[]>(data->width * data->height * data->channel);
		std::memset(lightmapTemp.get(), 0, data->width * data->height * data->channel * sizeof(float));

		for (std::uint32_t j = 0; j < std::max<std::uint32_t>(margin >> 1, 1); j++)
		{
			ray::image::smoothFilter(data->data.get(), lightmapTemp.get(), data->width, data->height, data->channel);
			ray::image::dilateFilter(lightmapTemp.get(), data->data.get(), data->width, data->height, data->channel);
		}
	}

	ray::image::format_t format = isGreyscale ? ray::image::format_t::R8SRGB : ray::image::format_t::R8G8B8SRGB;
	if (hasAlpha)
		format = ray::image::format_t::R8G8B8A8SRGB;

	ray::image::Image image;
	if (!image.create(data->width, data->height, format))
	{
		error = "Failed to create image.";
		return false;
	}

	auto temp = (std::uint8_t*)image.data();

	if (data->channel == 1)
		ray::image::r32f_to_r8uint(data->data.get(), temp, data->width, data->height, data->channel);
	else
		ray::image::rgb32f_to_rgbt8(data->data.get(), temp, data->width, data->height, data->channel);

	ray::image::flipHorizontal(temp, data->width, data->height, data->channel);

	if (!isGreyscale)
	{
		for (std::size_t i = 0; i < data->width * data->height * data->channel; i += data->channel)
			std::swap(temp[i], temp[i + 2]);
	}

	image.save(path);

	return true;
}

bool
GuiControllerComponent::onFetchCamera(const ray::GameObjects*& cameras) noexcept
{
	cameras = &_cameras;
	return false;
}

bool
GuiControllerComponent::onFetchLights(const ray::GameObjects*& lights) noexcept
{
	lights = &_lights;
	return false;
}

bool
GuiControllerComponent::onFetchLightProbes(const ray::GameObjects*& lightprobes) noexcept
{
	lightprobes = &_lightProbes;
	return false;
}

bool
GuiControllerComponent::onFetchMeshes(const ray::GameObjects*& meshes) noexcept
{
	meshes = &_objects;
	return true;
}

bool
GuiControllerComponent::onSeletedCamera(const ray::GameObject* cameras) noexcept
{
	assert(_cube);

	_cube->setScaleAll(0.0);
	return false;
}

bool
GuiControllerComponent::onSeletedLight(const ray::GameObject* object) noexcept
{
	assert(_cube);

	_cube->setScaleAll(0.0);
	return false;
}

bool
GuiControllerComponent::onSeletedLightProbe(const ray::GameObject* object) noexcept
{
	assert(_cube);

	_cube->setScaleAll(0.0);
	return false;
}

bool
GuiControllerComponent::onSeletedMesh(const ray::GameObject* object, std::size_t subset) noexcept
{
	assert(_cube);

	auto meshComponent = object->getComponent<ray::MeshComponent>();
	auto boundingBox = meshComponent->getMesh()->getMeshSubsets()[subset].boundingBox;
	boundingBox.transform(object->getTransform());

	_cube->setTranslate(boundingBox.center());
	_cube->setScale(boundingBox.size());
	return true;
}

bool
GuiControllerComponent::onOutputSphere(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	auto sphereMesh = std::make_shared<ray::MeshProperty>();
	sphereMesh->makeSphere(1.0, 48, 36);

	std::uint32_t out = 10;

	auto model = std::make_unique<ray::PMX>();
	model->header.version = 2.0;
	model->header.magic[0] = 'P';
	model->header.magic[1] = 'M';
	model->header.magic[2] = 'X';
	model->header.dataSize = 8;
	model->header.offset = 32;
	model->header.sizeOfBody = 1;
	model->header.sizeOfBone = 1;
	model->header.sizeOfIndices = 4;
	model->header.sizeOfMaterial = 1;
	model->header.sizeOfMorph = 1;
	model->header.sizeOfTexture = 1;
	model->header.encode = 0;
	model->numVertices = sphereMesh->getNumVertices() * out * out;
	model->numIndices = sphereMesh->getNumIndices() * out * out;
	model->numMaterials = out * out;
	model->vertices.resize(out * out * sphereMesh->getNumVertices());
	model->indices.resize(out * out * sphereMesh->getNumIndices() * sizeof(std::uint32_t));

	ray::PMX_Vertex* data = model->vertices.data();

	for (std::size_t i = 0; i < out; i++)
	{
		for (std::size_t j = 0; j < out; j++)
		{
			for (std::size_t k = 0; k < sphereMesh->getNumVertices(); k++)
			{
				ray::PMX_Vertex v;
				std::memset(&v, 0, sizeof(v));

				v.position = ray::float3(-25.0f + i * 5.5f, 3, -25.0f + j * 5.5f) + sphereMesh->getVertexArray()[k] * 2.5;
				v.normal = sphereMesh->getNormalArray()[k];
				v.coord = sphereMesh->getTexcoordArray()[k];
				v.edge = 1.0;

				*(data++) = v;
			}
		}
	}

	std::size_t startIndices = 0;
	std::uint32_t* indicesData = (std::uint32_t*)model->indices.data();

	for (std::size_t i = 0; i < out * out; i++)
	{
		for (std::size_t k = 0; k < sphereMesh->getNumIndices(); k++)
			*(indicesData++) = startIndices + sphereMesh->getIndicesArray()[k];

		startIndices += sphereMesh->getNumVertices();
	}

	ray::PMX_Name diff;
	diff.length = sizeof(L"Bricks_ao.dds") - 2;
	std::memcpy(diff.name, L"Bricks_ao.dds", diff.length);

	ray::PMX_Name spec;
	spec.length = sizeof(L"Bricks_n.dds") - 2;
	std::memcpy(spec.name, L"Bricks_n.dds", spec.length);

	model->numTextures = 2;
	model->textures.push_back(diff);
	model->textures.push_back(spec);

	auto SmoothnessToShininess = [](float smoothness)
	{
		return 2.0f / std::max(1e-4f, pow(1.0f - smoothness, 4.0f)) - 2.0f;
	};

	for (std::size_t i = 0; i < out; i++)
	{
		for (std::size_t j = 0; j < out; j++)
		{
			ray::PMX_Material material;
			std::memset(&material, 0, sizeof(material));

			std::memcpy(material.name.name, L"Material", sizeof(L"Material") - 2);
			material.name.length = sizeof(L"Material") - 2;

			std::memcpy(material.nameEng.name, L"Material", sizeof(L"Material") - 2);
			material.nameEng.length = sizeof(L"Material") - 2;

			material.Diffuse = ray::math::linear2srgb(diff_spec_parametes[i * 10 + j].xyz());
			material.Specular = ray::float3(i / 9.0f, j / 9.0f, metalnessParams[i * 10 + j]);
			material.Ambient = ray::float3(0.5, 0.5, 0.5);
			material.Shininess = SmoothnessToShininess(shininessParams[i * 10 + j]);
			material.IndicesCount = sphereMesh->getNumIndices();
			material.TextureIndex = std::numeric_limits<std::uint8_t>::max();
			material.ToneTexture = std::numeric_limits<std::uint8_t>::max();
			material.SphereTextureIndex = std::numeric_limits<std::uint8_t>::max();
			material.EdgeSize = 1;
			material.Flag = PMX_MATERIAL_DEFAULT;
			material.Opacity = 1.0;

			if (shininessParams[i * 10 + j] > 0.45 &&
				shininessParams[i * 10 + j] < 0.8 ||
				shininessParams[i * 10 + j] > 0.95)
			{
				model->materials.push_back(material);
			}
			else
			{
				material.TextureIndex = 0;
				material.SphereMode = 1;
				material.SphereTextureIndex = 1;
				model->materials.push_back(material);
			}
		}
	}

	ray::StreamWritePtr stream;
	if (!ray::IoServer::instance()->openFile(stream, path))
		return false;

	ray::PMXHandler header;
	if (!header.doSave(*stream, *model))
		return false;

	return true;
}