// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2017-2018.
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

#include <ray/utf8.h>
#include <ray/camera_component.h>
#include <ray/light_component.h>
#include <ray/skybox_component.h>
#include <ray/mesh_component.h>
#include <ray/mesh_render_component.h>

#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/render_system.h>

#include <ray/input.h>
#include <ray/input_feature.h>

#include <ray/game_server.h>
#include <ray/game_object_manager.h>

#include <ray/res_manager.h>
#include <ray/material.h>

#include <ray/image.h>
#include <ray/imagutil.h>
#include <ray/imagcubemap.h>

#include <ray/SH.h>

#include "first_person_camera.h"

#include "IESloader/IESloader.h"
#include "HLSLParser/HLSLParser.h"

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
	ray::float3(0.00198f, 0.00198f, 0.00203),
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
	ray::float3(0.592f,   0.4745f,  0.29803),
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
GuiControllerComponent::loadTexture(const ray::util::string& path, ray::GraphicsTexturePtr& texture, ray::util::string::pointer& error, ray::GraphicsTextureDim dim) noexcept
{
	return this->loadTexture(path.c_str(), texture, error, dim);
}

bool
GuiControllerComponent::loadTexture(ray::util::string::const_pointer path, ray::GraphicsTexturePtr& texture, ray::util::string::pointer& error, ray::GraphicsTextureDim dim) noexcept
{
	texture = ray::ResManager::instance()->getTexture(path);
	if (texture)
		return true;

	if (!ray::ResManager::instance()->createTexture(path, texture, dim))
	{
		error = "Failed to load texture";
		return false;
	}

	char drive[MAX_PATH];
	char dir[PATHLIMIT];
	char filename[PATHLIMIT];
	char ext[MAX_PATH];
	::_splitpath(path, drive, dir, filename, ext);

	auto item = std::make_shared<EditorAssetItem>();
	item->name = filename;
	item->preview = texture;
	item->value.emplace<EditorAssetItem::texture>(texture);

	_itemTextures.push_back(std::move(item));

	return true;
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
	gameObject->setName("wireframe");
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
	framebufferDesc.setWidth(textureDesc.getWidth());
	framebufferDesc.setHeight(textureDesc.getHeight());
	framebufferDesc.addColorAttachment(ray::GraphicsAttachmentBinding(std::move(renderTexture), 0, 0));
	framebufferDesc.setGraphicsFramebufferLayout(std::move(framebufferLayout));
	auto framebuffer = ray::RenderSystem::instance()->createFramebuffer(framebufferDesc);
	if (!framebuffer)
		return false;

	auto camera = std::make_shared<ray::CameraComponent>();
	camera->setAperture(60.0f);
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
	ray::GraphicsTexturePtr normalMap;

	char* error = nullptr;
	if (!this->loadTexture("dlc:common/textures/Bricks_ao.dds", diffuseMap, error))
		return false;

	if (!this->loadTexture("dlc:common/textures/Bricks_n.dds", normalMap, error))
		return false;

	ray::MaterialPtr material;
	if (!ray::ResManager::instance()->createMaterial("dlc:editor/fx/material.fxml", material))
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

			(*material).setName(buf);
			(*material)["albedo"]->uniform3f(diff_spec_parametes[i * 10 + j].xyz());
			(*material)["albedoMap"]->uniformTexture(diffuseMap);
			(*material)["albedoMapFrom"]->uniform1i(1);
			(*material)["albedoMapLoopNum"]->uniform2f(1.0f, 1.0f);

			(*material)["normalMap"]->uniformTexture(normalMap);
			(*material)["normalMapFrom"]->uniform1i(1);
			(*material)["normalMapLoopNum"]->uniform2f(1.0f, 1.0f);
			(*material)["normalMapScale"]->uniform1f(1.0f);

			(*material)["normalSubMap"]->uniformTexture(0);
			(*material)["normalSubMapFrom"]->uniform1i(0);
			(*material)["normalSubMapLoopNum"]->uniform2f(1.0f, 1.0f);
			(*material)["normalSubMapScale"]->uniform1f(1.0f);

			(*material)["smoothness"]->uniform1f(shininessParams[i * 10 + j]);
			(*material)["smoothnessMapFrom"]->uniform1i(0);
			(*material)["smoothnessMapLoopNum"]->uniform2f(1.0f, 1.0f);
			(*material)["smoothnessMapSwizzle"]->uniform4f(1.0f, 0.0f, 0.0f, 0.0f);

			(*material)["metalness"]->uniform1f(0);
			(*material)["metalnessMapFrom"]->uniform1i(0);
			(*material)["metalnessMapLoopNum"]->uniform2f(1.0f, 1.0f);
			(*material)["metalnessMapSwizzle"]->uniform4f(1.0f, 0.0f, 0.0f, 0.0f);

			(*material)["specular"]->uniform3f(0.5, 0.5, 0.5);
			(*material)["specularMapFrom"]->uniform1i(0);
			(*material)["specularMapLoopNum"]->uniform2f(1.0f, 1.0f);

			(*material)["occlusion"]->uniform1f(1.0);
			(*material)["occlusionMapFrom"]->uniform1i(0);
			(*material)["occlusionMapLoopNum"]->uniform2f(1.0f, 1.0f);

			(*material)["emissive"]->uniform3f(0.0, 0.0, 0.0);
			(*material)["emissiveMapFrom"]->uniform1i(0);
			(*material)["emissiveMapLoopNum"]->uniform2f(1.0f, 1.0f);
			(*material)["emissiveIntensity"]->uniform1f(1.0f);

			if (shininessParams[i * 10 + j] > 0.45 &&
				shininessParams[i * 10 + j] < 0.8 ||
				shininessParams[i * 10 + j] > 0.95)
			{
				(*material)["albedoMap"]->uniformTexture(nullptr);
				(*material)["albedoMapFrom"]->uniform1i(0);

				(*material)["normalMap"]->uniformTexture(nullptr);
				(*material)["normalMapFrom"]->uniform1i(0);
			}

			ray::GraphicsTextureDesc textureDesc;
			textureDesc.setWidth(128);
			textureDesc.setHeight(128);
			textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
			textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
			auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
			if (!renderTexture)
				continue;

			auto item = std::make_shared<EditorAssetItem>();
			item->name = buf;
			item->value.emplace<EditorAssetItem::material>(material);
			item->preview = renderTexture;
			this->onUpdateMaterial(*item);

			_itemMaterials.push_back(std::move(item));

			_objects.push_back(std::move(newGameObject));
		}
	}

	return true;
}

bool
GuiControllerComponent::makeSkyLighting() noexcept
{
	ray::GraphicsTexturePtr skybox;

	char* error = nullptr;
	if (!this->loadTexture("dlc:common/textures/uffizi_sky.hdr", skybox, error))
		return false;

	auto light = std::make_shared<ray::LightComponent>();
	light->setLightType(ray::LightType::LightTypeEnvironment);
	light->setLightRange(20000);
	light->setShadowMode(ray::ShadowMode::ShadowModeNone);

	auto sky = std::make_shared<ray::SkyboxComponent>();
	sky->setSkyBox(skybox);
	sky->loadSkyDiffuse("dlc:common/textures/uffizi_diff.dds");
	sky->loadSkySpecular("dlc:common/textures/uffizi_spec.dds");

	auto gameObject = std::make_shared<ray::GameObject>();
	gameObject->setName("sky");
	gameObject->setActive(true);

	gameObject->addComponent(std::move(light));
	gameObject->addComponent(std::move(sky));

	_lights.push_back(gameObject);
	return true;
}

bool
GuiControllerComponent::makeMaterialCamera() noexcept
{
	ray::GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(128);
	textureDesc.setHeight(128);
	textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
	if (!renderTexture)
		return false;

	ray::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(ray::GraphicsAttachmentLayout(0, ray::GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm));
	auto framebufferLayout = ray::RenderSystem::instance()->createFramebufferLayout(framebufferLayoutDesc);
	if (!framebufferLayout)
		return false;

	ray::GraphicsFramebufferDesc framebufferDesc;
	framebufferDesc.setWidth(textureDesc.getWidth());
	framebufferDesc.setHeight(textureDesc.getHeight());
	framebufferDesc.addColorAttachment(ray::GraphicsAttachmentBinding(std::move(renderTexture), 0, 0));
	framebufferDesc.setGraphicsFramebufferLayout(std::move(framebufferLayout));
	auto framebuffer = ray::RenderSystem::instance()->createFramebuffer(framebufferDesc);
	if (!framebuffer)
		return false;

	_previewFramebuffer = std::move(framebuffer);

	ray::MeshProperty sphereMesh;
	sphereMesh.makeSphere(1.0, 32, 24);

	_vbo = ray::ResManager::instance()->createVertexBuffer(sphereMesh, ray::ModelMakerFlagBits::ModelMakerFlagBitALL);
	_ibo = ray::ResManager::instance()->createIndexBuffer(sphereMesh);

	return true;
}

void
GuiControllerComponent::onAttachComponent(const ray::GameComponentPtr& component) except
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();

		EditorEvents delegate;

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
		delegate.onFetchMaterials = std::bind(&GuiControllerComponent::onFetchMaterials, this, std::placeholders::_1);
		delegate.onFetchTextures = std::bind(&GuiControllerComponent::onFetchTextures, this, std::placeholders::_1);

		delegate.onSeletedCamera = std::bind(&GuiControllerComponent::onSeletedCamera, this, std::placeholders::_1);
		delegate.onSeletedMesh = std::bind(&GuiControllerComponent::onSeletedMesh, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onSeletedLight = std::bind(&GuiControllerComponent::onSeletedLight, this, std::placeholders::_1);
		delegate.onSeletedLightProbe = std::bind(&GuiControllerComponent::onSeletedLightProbe, this, std::placeholders::_1);

		delegate.onTransformObject = std::bind(&GuiControllerComponent::onTransformObject, this, std::placeholders::_1, std::placeholders::_2);

		delegate.onMouseHoveringCamera = std::bind(&GuiControllerComponent::onMouseHoveringCamera, this, std::placeholders::_1, std::placeholders::_2);

		delegate.onImportIES = std::bind(&GuiControllerComponent::onImportIES, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onImportTexture = std::bind(&GuiControllerComponent::onImportTexture, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onImportMaterial = std::bind(&GuiControllerComponent::onImportMaterial, this, std::placeholders::_1, std::placeholders::_2);
		delegate.onImportModel = std::bind(&GuiControllerComponent::onImportModel, this, std::placeholders::_1, std::placeholders::_2);

		delegate.onExportIES = std::bind(&GuiControllerComponent::onExportIES, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		delegate.onExportTexture = std::bind(&GuiControllerComponent::onExportTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		delegate.onExportMaterial = std::bind(&GuiControllerComponent::onExportMaterial, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		delegate.onExportModel = std::bind(&GuiControllerComponent::onExportModel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		delegate.onUpdateMaterial = std::bind(&GuiControllerComponent::onUpdateMaterial, this, std::placeholders::_1);

		delegate.onModelPicker = std::bind(&GuiControllerComponent::onModelPicker, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		view->setEditorEvents(delegate);
	}
}

void
GuiControllerComponent::onDetachComponent(const ray::GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();

		EditorEvents delegate;
		view->setEditorEvents(delegate);
	}
}

void
GuiControllerComponent::onActivate() except
{
	this->makeCubeObject();
	this->makeMainCamera();
	this->makeMaterialCamera();
	this->makeSkyLighting();
	this->makeSphereObjects();
}

void
GuiControllerComponent::onMessage(const ray::MessagePtr& message) except
{
	if (!message->isInstanceOf<ray::InputMessage>())
		return;

	auto& event = message->downcast<ray::InputMessage>()->getEvent();
	if (!message->isInstanceOf<ray::InputMessage>())
		return;

	switch (event.event)
	{
	case ray::InputEvent::SizeChangeDPI:
	{
		if (_cameras.empty())
			return;

		auto cameraComponent = _cameras.front()->getComponent<ray::CameraComponent>();
		if (!cameraComponent)
			return;

		auto fb = cameraComponent->getFramebuffer();
		if (fb->getGraphicsFramebufferDesc().getWidth() != event.change.w ||
			fb->getGraphicsFramebufferDesc().getHeight() != event.change.h)
		{
			ray::GraphicsTextureDesc textureDesc;
			textureDesc.setWidth(event.change.w);
			textureDesc.setHeight(event.change.h);
			textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm);
			textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
			auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
			if (!renderTexture)
				return;

			ray::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(ray::GraphicsAttachmentLayout(0, ray::GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm));
			auto framebufferLayout = ray::RenderSystem::instance()->createFramebufferLayout(framebufferLayoutDesc);
			if (!framebufferLayout)
				return;

			ray::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(event.change.w);
			framebufferDesc.setHeight(event.change.h);
			framebufferDesc.addColorAttachment(ray::GraphicsAttachmentBinding(std::move(renderTexture), 0, 0));
			framebufferDesc.setGraphicsFramebufferLayout(std::move(framebufferLayout));
			auto framebuffer = ray::RenderSystem::instance()->createFramebuffer(framebufferDesc);
			if (!framebuffer)
				return;

			cameraComponent->setFramebuffer(framebuffer);
		}
	}
	break;
	}
}

bool
GuiControllerComponent::onModelPicker(float x, float y, ray::GameObject*& _selectedObject, std::size_t& _selectedSubset) noexcept
{
	auto cameraComponent = _cameras.front()->getComponent<ray::CameraComponent>();

	const auto& start = cameraComponent->getGameObject()->getTranslate();
	auto end = cameraComponent->screenToWorld(ray::float3(x, y, 1));

	ray::RaycastHit hit;
	if (ray::GameObjectManager::instance()->raycastHit(start, end, hit, [](ray::GameObject* object) { return object->getName() != "wireframe"; }))
	{
		_selectedObject = hit.object;
		_selectedSubset = hit.mesh;
		return this->onSeletedMesh(hit.object, hit.mesh);
	}

	return false;
}

bool
GuiControllerComponent::onImportIES(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	if (ray::ResManager::instance()->getTexture(path))
		return true;

	ray::StreamReaderPtr stream;
	if (!ray::IoServer::instance()->openFile(stream, path))
	{
		error = "Cannot open the file, check the spelling of the file path.";
		return false;
	}

	std::vector<char> buffer(stream->size());
	if (!stream->read(buffer.data(), buffer.size()))
	{
		error = "Failed to read file.";
		return false;
	}

	IESFileInfo info;
	IESLoadHelper helper;
	if (!helper.load(buffer.data(), buffer.size(), info))
	{
		error = "Non readable IES file.";
		return false;
	}

	std::vector<float> ies(256 * 3);
	helper.saveAs1D(info, ies.data(), 256, 3);

	std::vector<std::uint8_t> preview(64 * 64 * 3);
	helper.saveAsPreview(info, preview.data(), 64, 64, 3);

	ray::GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(256);
	textureDesc.setHeight(1);
	textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	textureDesc.setStream(ies.data());
	textureDesc.setStreamSize(ies.size());

	auto iesTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
	if (!iesTexture)
	{
		error = "Failed to create ies texture.";
		return false;
	}

	textureDesc.setWidth(64);
	textureDesc.setHeight(64);
	textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	textureDesc.setStream(preview.data());
	textureDesc.setStreamSize(preview.size());

	auto iesPreview = ray::RenderSystem::instance()->createTexture(textureDesc);
	if (!iesPreview)
	{
		error = "Failed to create ies texture.";
		return false;
	}

	char drive[MAX_PATH];
	char dir[PATHLIMIT];
	char filename[PATHLIMIT];
	char ext[MAX_PATH];
	_splitpath(path, drive, dir, filename, ext);

	auto item = std::make_shared<EditorAssetItem>();
	item->path = path;
	item->name = filename;
	item->preview = iesPreview;
	item->value.emplace<EditorAssetItem::texture>(iesTexture);

	_itemTextures.push_back(std::move(item));

	return true;
}

bool
GuiControllerComponent::onImportTexture(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	ray::GraphicsTexturePtr texture;
	return this->loadTexture(path, texture, error);
}

bool
GuiControllerComponent::onImportMaterial(ray::util::string::const_pointer fullpath, ray::util::string::pointer& error) noexcept
{
	static const char* token_from[] = { "ALBEDO_MAP_FROM", "ALBEDO_SUB_MAP_FROM", "NORMAL_MAP_FROM", "NORMAL_SUB_MAP_FROM", "SMOOTHNESS_MAP_FROM", "METALNESS_MAP_FROM", "SPECULAR_MAP_FROM", "OCCLUSION_MAP_FROM", "PARALLAX_MAP_FROM", "EMISSIVE_MAP_FROM", "CUSTOM_A_MAP_FROM", "CUSTOM_B_MAP_FROM" };
	static const char* token_from2[] = { "albedoMapFrom","albedoSubMapFrom","normalMapFrom","normalSubMapFrom","smoothnessMapFrom","metalnessMapFrom","specularMapFrom","occlusionMapFrom","parallaxMapFrom","emissiveMapFrom","customAMapFrom","customAMapFrom" };

	static const char* token_file[] = { "ALBEDO_MAP_FILE", "ALPHA_MAP_FILE", "NORMAL_MAP_FILE", "SMOOTHNESS_MAP_FILE", "METALNESS_MAP_FILE", "MELANIN_MAP_FILE", "EMISSIVE_MAP_FILE", "PARALLAX_MAP_FILE", "CUSTOM_A_MAP_FILE", "CUSTOM_B_MAP_FILE" };
	static const char* token_file2[] = { "albedoMap", "albedoSubMap", "normalMap", "normalSubMap", "smoothnessMap", "metalnessMap", "specularMap", "occlusionMap", "parallaxMap", "emissiveMap", "customAMap", "customBMap" };

	static const char* token_flip[] = { "ALBEDO_MAP_UV_FLIP", "ALBEDO_SUB_MAP_UV_FLIP", "NORMAL_MAP_UV_FLIP", "NORMAL_SUB_MAP_UV_FLIP", "SMOOTHNESS_MAP_UV_FLIP", "METALNESS_MAP_UV_FLIP", "SPECULAR_MAP_UV_FLIP", "OCCLUSION_MAP_UV_FLIP", "PARALLAX_MAP_UV_FLIP", "EMISSIVE_MAP_UV_FLIP", "CUSTOM_A_MAP_UV_FLIP", "CUSTOM_B_MAP_UV_FLIP" };
	static const char* token_flip2[] = { "albedoMapFlip", "albedoSubMapFlip", "normalMapFlip", "normalSubMapFlip", "smoothnessMapFlip", "metalnessMapFlip", "specularMapFlip", "occlusionMapFlip", "parallaxMapFlip", "emissiveMapFlip", "customAMapFlip", "customBMapFlip" };

	static const char* token_swizzle[] = { "SMOOTHNESS_MAP_SWIZZLE", "METALNESS_MAP_SWIZZLE", "SPECULAR_MAP_SWIZZLE", "OCCLUSION_MAP_SWIZZLE", "PARALLAX_MAP_SWIZZLE", "CUSTOM_A_MAP_SWIZZ" };
	static const char* token_swizzle2[] = { "smoothnessMapSwizzle", "metalnessMapSwizzle", "specularMapSwizzle", "occlusionMapSwizzle", "parallaxMapSwizzle", "customAMapSwizzle" };

	static const char* token_float[] = { "normalMapScale", "smoothness", "metalness", "occlusion", "emissiveIntensity" "customA" };
	static const char* token_float2[] = { "albedoMapLoopNum","albedoSubMapLoopNum", "normalMapLoopNum","normalSubMapLoopNum","smoothnessMapLoopNum","metalnessMapLoopNum","specularMapLoopNum","occlusionMapLoopNum","parallaxMapLoopNum","emissiveMapLoopNum","customAMapLoopNum","customBMapLoopNum" };
	static const char* token_float3[] = { "albedo", "albedoSub", "specular", "emissive", "customB" };

	char drive[MAX_PATH];
	char dir[PATHLIMIT];
	char filename[PATHLIMIT];
	char ext[MAX_PATH];
	_splitpath(fullpath, drive, dir, filename, ext);

	for (auto& it : _itemMaterials)
	{
		if (it->name == filename)
			return true;
	}

	ray::StreamReaderPtr stream;
	if (!ray::IoServer::instance()->openFile(stream, fullpath))
	{
		error = "Cannot open file, check the spelling of the file path.";
		return false;
	}

	std::vector<char> buffer(stream->size());
	if (!stream->read(buffer.data(), buffer.size()))
	{
		error = "Failed to read file.";
		return false;
	}

	HLSLParser parser(fullpath, buffer.data(), buffer.size());
	HLSLTree tree;

	if (!parser.Parse(&tree))
	{
		error = "Non readable format.";
		return false;
	}

	ray::MaterialPtr material;
	if (!ray::ResManager::instance()->createMaterial("dlc:editor/fx/material.fxml", material))
		return false;

	material->setName(filename);

	for (std::size_t i = 0; i < sizeof(token_from) / sizeof(token_from[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Int)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_from[i]) == 0)
			{
				(*material)[token_from2[i]]->uniform1i(expression->iValue[0]);
				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_swizzle) / sizeof(token_swizzle[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Int)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_swizzle[i]) == 0)
			{
				if (expression->iValue[0] == 0)
					(*material)[token_swizzle2[i]]->uniform4f(ray::float4::UnitX);
				else if (expression->iValue[0] == 0)
					(*material)[token_swizzle2[i]]->uniform4f(ray::float4::UnitY);
				else if (expression->iValue[0] == 0)
					(*material)[token_swizzle2[i]]->uniform4f(ray::float4::UnitZ);
				else if (expression->iValue[0] == 0)
					(*material)[token_swizzle2[i]]->uniform4f(ray::float4::UnitW);
				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_flip) / sizeof(token_flip[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Int)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_flip[i]) == 0)
			{
				(*material)[token_flip2[i]]->uniform1i(expression->iValue[0]);
				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_float) / sizeof(token_float[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Float)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_float[i]) == 0)
			{
				if (expression->type == HLSLBaseType::HLSLBaseType_Float)
					(*material)[token_float[i]]->uniform1f(expression->fValue[0]);
				else
					(*material)[token_float[i]]->uniform1f(expression->iValue[0]);
				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_float2) / sizeof(token_float2[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Float &&
				declaration->type.baseType != HLSLBaseType::HLSLBaseType_Float2)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_float2[i]) == 0)
			{
				if (expression->type == HLSLBaseType::HLSLBaseType_Float)
					(*material)[token_float2[i]]->uniform2f(expression->fValue[0], expression->fValue[0]);
				else if (expression->type == HLSLBaseType::HLSLBaseType_Float2)
					(*material)[token_float2[i]]->uniform2f(expression->fValue[0], expression->fValue[1]);
				else if (expression->type == HLSLBaseType::HLSLBaseType_Int)
					(*material)[token_float2[i]]->uniform2f(expression->iValue[0], expression->iValue[0]);
				else if (expression->type == HLSLBaseType::HLSLBaseType_Int2)
					(*material)[token_float2[i]]->uniform2f(expression->iValue[0], expression->iValue[1]);

				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_float3) / sizeof(token_float3[0]); i++)
	{
		auto statement = tree.GetRoot()->statement;
		while (statement)
		{
			auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
			auto expression = dynamic_cast<HLSLLiteralExpression*>(declaration->assignment);

			if (!declaration->assignment)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_Float3)
			{
				statement = statement->nextStatement;
				continue;
			}

			if (std::strcmp(declaration->name, token_float3[i]) == 0)
			{
				if (declaration->type.baseType == HLSLBaseType::HLSLBaseType_Float)
					(*material)[token_float3[i]]->uniform3f(expression->fValue[0], expression->fValue[0], expression->fValue[0]);
				else if (declaration->type.baseType == HLSLBaseType::HLSLBaseType_Float3)
					(*material)[token_float3[i]]->uniform3f(expression->fValue[0], expression->fValue[1], expression->fValue[2]);
				else if (declaration->type.baseType == HLSLBaseType::HLSLBaseType_Int)
					(*material)[token_float3[i]]->uniform3f(expression->iValue[0], expression->iValue[0], expression->iValue[0]);
				else if (declaration->type.baseType == HLSLBaseType::HLSLBaseType_Int3)
					(*material)[token_float3[i]]->uniform3f(expression->iValue[0], expression->iValue[1], expression->iValue[2]);

				break;
			}

			statement = statement->nextStatement;
		}
	}

	for (std::size_t i = 0; i < sizeof(token_file) / sizeof(token_file[0]); i++)
	{
		auto paramater = material->getParameter(token_from2[i]);
		if (!paramater)
			continue;

		if (paramater->value().getInt() == 0 || paramater->value().getInt() > 6)
			continue;

		if (paramater->value().getInt() == 1)
		{
			auto statement = tree.GetRoot()->statement;
			while (statement)
			{
				auto declaration = dynamic_cast<HLSLDeclaration*>(statement);
				auto expression = dynamic_cast<HLSLIdentifierExpression*>(declaration->assignment);

				if (!declaration->assignment)
				{
					statement = statement->nextStatement;
					continue;
				}

				if (declaration->type.baseType != HLSLBaseType::HLSLBaseType_UserDefined)
				{
					statement = statement->nextStatement;
					continue;
				}

				if (std::strcmp(declaration->name, token_file[i]) == 0)
				{
					ray::GraphicsTexturePtr texture;
					if (ray::ResManager::instance()->createTexture(ray::util::directory(fullpath) + expression->name, texture))
						material->getParameter(token_file2[i])->uniformTexture(texture);

					break;
				}

				statement = statement->nextStatement;
			}
		}
	}

	ray::GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(128);
	textureDesc.setHeight(128);
	textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
	if (!renderTexture)
		return false;

	auto item = std::make_shared<EditorAssetItem>();
	item->path = fullpath;
	item->name = material->getName();
	item->value.emplace<EditorAssetItem::material>(material);
	item->preview = renderTexture;

	this->onUpdateMaterial(*item);

	_itemMaterials.push_back(std::move(item));

	return true;
}

bool
GuiControllerComponent::onImportModel(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept
{
	try
	{
		ray::StreamReaderPtr stream;
		if (!ray::IoServer::instance()->openFile(stream, path))
		{
			error = "Cannot open file, check the spelling of the file path.";
			return false;
		}

		ray::PMXHandler header;
		if (!header.doCanRead(*stream))
		{
			error = "Non readable PMX file.";
			return false;
		}

		auto model = std::make_unique<ray::PMX>();
		if (!header.doLoad(*stream, *model))
		{
			error = "Non readable PMX file.";
			return false;
		}

		if (_models.empty())
		{
			_objects.clear();
			_itemMaterials.clear();
		}

		_cube->setScaleAll(0.0f);

		auto gameObject = std::make_shared<ray::GameObject>();

		if (model->description.japanModelLength)
		{
			char name[MAX_PATH];
			if (ray::utf16_to_utf8(model->description.japanModelName.data(), model->description.japanModelLength, name, MAX_PATH))
				gameObject->setName(name);
		}
		else if (model->description.englishModelLength)
		{
			char name[MAX_PATH];
			if (ray::utf16_to_utf8(model->description.englishModelName.data(), model->description.englishModelLength, name, MAX_PATH))
				gameObject->setName(name);
		}

		if (model->numVertices > 0 && model->numIndices > 0)
		{
			ray::PMX_Index* indicesData = model->indices.data();

			ray::Float3Array vertices(model->numVertices);
			ray::Float3Array normals(model->numVertices);
			ray::Float2Array texcoords(model->numVertices);
			ray::UintArray indices(model->numIndices);

			for (std::size_t i = 0; i < model->numVertices; i++)
			{
				const auto& v = model->vertices[i];

				vertices[i] = v.position;
				normals[i] = v.normal;
				texcoords[i] = v.coord;
			}

			for (std::uint32_t i = 0; i < model->numIndices; i++, indicesData += model->header.sizeOfIndices)
			{
				if (model->header.sizeOfIndices == 1)
					indices[i] = *(std::uint8_t*)indicesData;
				else if (model->header.sizeOfIndices == 2)
					indices[i] = *(std::uint16_t*)indicesData;
				else if (model->header.sizeOfIndices == 4)
					indices[i] = *(std::uint32_t*)indicesData;
				else
					return false;
			}

			ray::MeshSubsets subsets;
			std::uint32_t startIndices = 0;

			for (auto& it : model->materials)
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

		if (model->numMaterials)
		{
			ray::MaterialPtr materialTemp;
			if (!ray::ResManager::instance()->createMaterial("dlc:editor/fx/material.fxml", materialTemp))
				return false;

			ray::Materials materials(model->numMaterials);

			auto ShininessToSmoothness = [](float spec)
			{
				return 1.0f - pow(std::max(0.0f, 2.0f / (spec + 2)), 0.25f);
			};

			for (std::size_t i = 0; i < model->numMaterials; i++)
			{
				auto material = materialTemp->clone();

				char name[MAX_PATH];
				if (ray::utf16_to_utf8(model->materials[i].name.name, model->materials[i].name.length, name, MAX_PATH))
					material->setName(name);

				std::int16_t diffuseIndex = 0;
				if (model->header.sizeOfTexture == 1)
					diffuseIndex = (model->materials[i].TextureIndex == 255) ? -1 : model->materials[i].TextureIndex;
				else
					diffuseIndex = (model->materials[i].TextureIndex >= 65535) ? -1 : model->materials[i].TextureIndex;

				std::int16_t specularIndex = 0;
				if (model->header.sizeOfTexture == 1)
					specularIndex = (model->materials[i].SphereTextureIndex == 255) ? -1 : model->materials[i].SphereTextureIndex;
				else
					specularIndex = (model->materials[i].SphereTextureIndex >= 65535) ? -1 : model->materials[i].SphereTextureIndex;

				std::int16_t toonIndex = 0;
				if (model->header.sizeOfTexture == 1)
					toonIndex = (model->materials[i].ToonTexture == 255) ? -1 : model->materials[i].ToonTexture;
				else
					toonIndex = (model->materials[i].ToonTexture >= 65535) ? -1 : model->materials[i].ToonTexture;

				ray::GraphicsTexturePtr diffuseMap;
				ray::GraphicsTexturePtr specularMap;
				ray::GraphicsTexturePtr toonMap;

				if (diffuseIndex >= 0)
				{
					char name[PATHLIMIT];
					if (::wcstombs(name, model->textures[diffuseIndex].name, model->textures[diffuseIndex].length))
					{
						char* error = nullptr;
						this->loadTexture(ray::util::directory(path) + name, diffuseMap, error);
					}
				}

				if (specularIndex >= 0)
				{
					char name[MAX_PATH];
					if (::wcstombs(name, model->textures[specularIndex].name, model->textures[specularIndex].length) > 0)
					{
						char* error = nullptr;
						this->loadTexture(ray::util::directory(path) + name, specularMap, error);
					}
				}

				if (toonIndex >= 0)
				{
					char name[MAX_PATH];
					if (::wcstombs(name, model->textures[toonIndex].name, model->textures[toonIndex].length) > 0)
					{
						char* error = nullptr;
						this->loadTexture(ray::util::directory(path) + name, toonMap, error);
					}
				}

				(*material)["albedo"]->uniform3f(ray::math::srgb2linear(model->materials[i].Diffuse));
				(*material)["albedoMap"]->uniformTexture(diffuseMap);
				(*material)["albedoMapFrom"]->uniform1i(diffuseMap ? 1 : 0);
				(*material)["albedoMapLoopNum"]->uniform2f(1.0f, 1.0f);

				(*material)["normalMap"]->uniformTexture(0);
				(*material)["normalMapFrom"]->uniform1i(0);
				(*material)["normalMapLoopNum"]->uniform2f(1.0f, 1.0f);
				(*material)["normalMapScale"]->uniform1f(1.0f);

				(*material)["normalSubMap"]->uniformTexture(0);
				(*material)["normalSubMapFrom"]->uniform1i(0);
				(*material)["normalSubMapLoopNum"]->uniform2f(1.0f, 1.0f);
				(*material)["normalSubMapScale"]->uniform1f(1.0f);

				(*material)["smoothness"]->uniform1f(ShininessToSmoothness(model->materials[i].Shininess));
				(*material)["smoothnessMapFrom"]->uniform1i(0);
				(*material)["smoothnessMapLoopNum"]->uniform2f(1.0f, 1.0f);
				(*material)["smoothnessMapSwizzle"]->uniform4f(1.0f, 0.0f, 0.0f, 0.0f);

				(*material)["metalness"]->uniform1f(0);
				(*material)["metalnessMapFrom"]->uniform1i(0);
				(*material)["metalnessMapLoopNum"]->uniform2f(1.0f, 1.0f);
				(*material)["metalnessMapSwizzle"]->uniform4f(1.0f, 0.0f, 0.0f, 0.0f);

				(*material)["specular"]->uniform3f(0.5, 0.5, 0.5);
				(*material)["specularMapFrom"]->uniform1i(0);
				(*material)["specularMapLoopNum"]->uniform2f(1.0f, 1.0f);

				(*material)["occlusion"]->uniform1f(1.0);
				(*material)["occlusionMapFrom"]->uniform1i(0);
				(*material)["occlusionMapLoopNum"]->uniform2f(1.0f, 1.0f);

				(*material)["emissive"]->uniform3f(0.0, 0.0, 0.0);
				(*material)["emissiveMapFrom"]->uniform1i(0);
				(*material)["emissiveMapLoopNum"]->uniform2f(1.0f, 1.0f);
				(*material)["emissiveIntensity"]->uniform1f(1.0f);

				ray::GraphicsTextureDesc textureDesc;
				textureDesc.setWidth(128);
				textureDesc.setHeight(128);
				textureDesc.setTexFormat(ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
				textureDesc.setSamplerFilter(ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinear, ray::GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
				auto renderTexture = ray::RenderSystem::instance()->createTexture(textureDesc);
				if (!renderTexture)
					continue;

				auto item = std::make_shared<EditorAssetItem>();
				item->name = material->getName();
				item->value.emplace<EditorAssetItem::material>(material);
				item->preview = renderTexture;

				this->onUpdateMaterial(*item);

				_itemMaterials.push_back(std::move(item));

				materials[i] = std::move(material);
			}

			gameObject->addComponent(std::make_shared<ray::MeshRenderComponent>(std::move(materials)));
		}

		gameObject->setActive(true);

		_models.push_back(std::move(model));

		_objects.push_back(gameObject);

		return true;
	}
	catch (const std::exception&)
	{
		error = "Unkonwn error.";
		return false;
	}
}

bool
GuiControllerComponent::onExportIES(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept
{
	if (_itemTextures.size() < index)
		return false;

	auto texture = std::get<EditorAssetItem::texture>(_itemTextures[index]->value);
	if (!texture)
		return false;

	std::uint32_t w = texture->getGraphicsTextureDesc().getWidth();
	std::uint32_t h = texture->getGraphicsTextureDesc().getHeight();

	void* data;
	if (!texture->map(0, 0, w, h, 0, &data))
		return false;

	ray::image::Image image;
	if (!image.create(w, h, 1, ray::image::format_t::R32G32B32SFloat))
		return false;

	std::memcpy((void*)image.data(), data, image.size());

	return image.save(path);
}

bool
GuiControllerComponent::onExportTexture(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept
{
	if (_itemTextures.size() < index)
		return false;

	auto texture = std::get<EditorAssetItem::texture>(_itemTextures[index]->value);
	if (!texture)
		return false;

	char fullpath[PATHLIMIT];
	std::strcpy(fullpath, path);
	std::strcat(fullpath, _itemTextures[index]->name.c_str());

	std::uint32_t w = texture->getGraphicsTextureDesc().getWidth();
	std::uint32_t h = texture->getGraphicsTextureDesc().getHeight();

	auto format = texture->getGraphicsTextureDesc().getTexFormat();
	if (format == ray::GraphicsFormat::GraphicsFormatR32G32B32SFloat)
	{
		ray::image::Image image;
		if (!image.create(w, h, 1, ray::image::format_t::R32G32B32SFloat))
			return false;

		void* data;
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		std::strcat(fullpath, ".hdr");

		return image.save(fullpath, "hdr");
	}
	else if (format == ray::GraphicsFormat::GraphicsFormatR8G8B8UNorm)
	{
		ray::image::Image image;
		if (!image.create(w, h, 1, ray::image::format_t::R8G8B8UNorm))
			return false;

		void* data;
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		std::strcat(fullpath, ".tga");

		return image.save(fullpath, "tga");
	}
	else if (format == ray::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm)
	{
		ray::image::Image image;
		if (!image.create(w, h, 1, ray::image::format_t::R8G8B8A8UNorm))
			return false;

		void* data;
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		std::strcat(fullpath, ".tga");

		return image.save(path, "tga");
	}
	else if (format == ray::GraphicsFormat::GraphicsFormatB8G8R8A8UNorm)
	{
		ray::image::Image image;
		if (!image.create(w, h, 1, ray::image::format_t::R8G8B8A8UNorm))
			return false;

		void* data;
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		std::strcat(fullpath, ".tga");

		return image.save(fullpath, "tga");
	}
	else if (format == ray::GraphicsFormat::GraphicsFormatB8G8R8UNorm)
	{
		ray::image::Image image;
		if (!image.create(w, h, 1, ray::image::format_t::R8G8B8UNorm))
			return false;

		void* data;
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		std::strcat(fullpath, ".tga");

		return image.save(fullpath, "tga");
	}

	return false;
}

bool
GuiControllerComponent::onExportMaterial(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept
{
	if (_itemMaterials.size() < index)
	{
		error = "Out of range";
		return false;
	}

	char fullpath[PATHLIMIT];
	std::strcpy(fullpath, path);
	std::strcat(fullpath, _itemMaterials[index]->name.c_str());
	std::strcat(fullpath, ".fx");

	ray::StreamWritePtr stream;
	if (!ray::IoServer::instance()->openFile(stream, fullpath))
	{
		error = "Failed to open file";
		return false;
	}

	auto& material = *std::get<EditorAssetItem::material>(_itemMaterials[index]->value);

	(*stream) << "#define ALBEDO_MAP_FROM " << material["albedoMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define ALBEDO_MAP_UV_FLIP " << material["albedoMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define ALBEDO_MAP_APPLY_SCALE " << "1\r\n";
	(*stream) << "#define ALBEDO_MAP_APPLY_DIFFUSE " << "0\r\n";
	(*stream) << "#define ALBEDO_MAP_APPLY_MORPH_COLOR " << "0\r\n";

	auto albedoMap = material["albedoMap"]->value().getTexture();
	auto albedoMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == albedoMap; });
	if (albedoMapIter != _itemTextures.end())
		(*stream) << R"(#define ALBEDO_MAP_FILE ")" << (*albedoMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define ALBEDO_MAP_FILE "albedo.png")" << "\r\n\r\n";

	(*stream) << "const float3 albedo = float3(" << material["albedo"]->value().getFloat3() << ");\r\n";
	(*stream) << "const float2 albedoMapLoopNum = float2(" << material["albedoMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define ALBEDO_SUB_ENABLE " << material["albedoSubType"]->value().getInt() << "\r\n";
	(*stream) << "#define ALBEDO_SUB_MAP_FROM " << material["albedoSubMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define ALBEDO_SUB_MAP_UV_FLIP " << material["albedoSubMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define ALBEDO_SUB_MAP_APPLY_SCALE " << "1\r\n";

	auto albedoSubMap = material["albedoSubMap"]->value().getTexture();
	auto albedoSubMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == albedoSubMap; });
	if (albedoSubMapIter != _itemTextures.end())
		(*stream) << R"(#define ALBEDO_SUB_MAP_FILE ")" << (*albedoSubMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define ALBEDO_SUB_MAP_FILE "albedoSub.png")" << "\r\n\r\n";

	(*stream) << "const float3 albedoSub = float3(" << material["albedoSub"]->value().getFloat3() << ");\r\n";
	(*stream) << "const float2 albedoSubMapLoopNum = float2(" << material["albedoSubMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define ALPHA_MAP_FROM 3\r\n";
	(*stream) << "#define ALPHA_MAP_UV_FLIP 0\r\n";
	(*stream) << "#define ALPHA_MAP_SWIZZLE 3\r\n";
	(*stream) << R"(#define ALPHA_MAP_FILE "alpha.png")" << "\r\n\r\n";

	(*stream) << "const float alpha = 1.0;\r\n";
	(*stream) << "const float2 alphaMapLoopNum = 1.0;\r\n\r\n";

	(*stream) << "#define NORMAL_MAP_FROM " << material["normalMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define NORMAL_MAP_UV_FLIP " << material["normalMapFlip"]->value().getInt() << "\r\n";

	auto normalMap = material["normalMap"]->value().getTexture();
	auto normalMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == normalMap; });
	if (normalMapIter != _itemTextures.end())
		(*stream) << R"(#define NORMAL_MAP_FILE ")" << (*normalMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define NORMAL_MAP_FILE "normal.png")" << "\r\n\r\n";

	(*stream) << "const float normalMapScale = " << material["normalMapScale"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 normalMapLoopNum = float2(" << material["normalMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define NORMAL_SUB_MAP_FROM " << material["normalSubMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define NORMAL_SUB_MAP_UV_FLIP " << material["normalSubMapFlip"]->value().getInt() << "\r\n";

	auto normalSubMap = material["normalSubMap"]->value().getTexture();
	auto normalSubMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == normalSubMap; });
	if (normalSubMapIter != _itemTextures.end())
		(*stream) << R"(#define NORMAL_SUB_MAP_FILE ")" << (*normalSubMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define NORMAL_SUB_MAP_FILE "normalSub.png")" << "\r\n\r\n";

	(*stream) << "const float normalSubMapScale = " << material["normalSubMapScale"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 normalSubMapLoopNum = float2(" << material["normalSubMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define SMOOTHNESS_MAP_FROM " << material["smoothnessMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define SMOOTHNESS_MAP_UV_FLIP " << material["smoothnessMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define SMOOTHNESS_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["smoothnessMapSwizzle"]->value().getFloat4()) << "\r\n";
	(*stream) << "#define SMOOTHNESS_MAP_APPLY_SCALE 1\r\n";

	auto smoothnessMap = material["smoothnessMap"]->value().getTexture();
	auto smoothnessMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == smoothnessMap; });
	if (smoothnessMapIter != _itemTextures.end())
		(*stream) << R"(#define SMOOTHNESS_MAP_FILE ")" << (*smoothnessMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define SMOOTHNESS_MAP_FILE "smoothness.png")" << "\r\n\r\n";

	(*stream) << "const float smoothness = " << material["smoothness"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 smoothnessMapLoopNum = float2(" << material["smoothnessMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define METALNESS_MAP_FROM " << material["metalnessMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define METALNESS_MAP_UV_FLIP " << material["metalnessMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define METALNESS_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["metalnessMapSwizzle"]->value().getFloat4()) << "\r\n";
	(*stream) << "#define METALNESS_MAP_APPLY_SCALE 1\r\n";

	auto metalnessMap = material["metalnessMap"]->value().getTexture();
	auto metalnessMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == metalnessMap; });
	if (metalnessMapIter != _itemTextures.end())
		(*stream) << R"(#define METALNESS_MAP_FILE ")" << (*metalnessMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define METALNESS_MAP_FILE "metalness.png")" << "\r\n\r\n";

	(*stream) << "const float metalness = " << material["metalness"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 metalnessMapLoopNum = float2(" << material["metalnessMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define SPECULAR_MAP_FROM " << material["specularMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define SPECULAR_MAP_UV_FLIP " << material["specularMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define SPECULAR_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["specularMapSwizzle"]->value().getFloat4()) << "\r\n";
	(*stream) << "#define SPECULAR_MAP_APPLY_SCALE 1\r\n";

	auto specularMap = material["specularMap"]->value().getTexture();
	auto specularMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == specularMap; });
	if (specularMapIter != _itemTextures.end())
		(*stream) << R"(#define SPECULAR_MAP_FILE ")" << (*specularMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define SPECULAR_MAP_FILE "specular.png")" << "\r\n\r\n";

	(*stream) << "const float3 specular = float3(" << material["specular"]->value().getFloat3() << ");\r\n";
	(*stream) << "const float2 specularMapLoopNum = float2(" << material["specularMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define OCCLUSION_MAP_FROM " << material["occlusionMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define OCCLUSION_MAP_UV_FLIP " << material["occlusionMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define OCCLUSION_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["occlusionMapSwizzle"]->value().getFloat4()) << "\r\n";
	(*stream) << "#define OCCLUSION_MAP_APPLY_SCALE 1\r\n";

	auto occlusionMap = material["occlusionMap"]->value().getTexture();
	auto occlusionMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == occlusionMap; });
	if (occlusionMapIter != _itemTextures.end())
		(*stream) << R"(#define OCCLUSION_MAP_FILE ")" << (*occlusionMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define OCCLUSION_MAP_FILE "occlusion.png")" << "\r\n\r\n";

	(*stream) << "const float occlusion = " << material["occlusion"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 occlusionMapLoopNum = float2(" << material["occlusionMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define PARALLAX_MAP_FROM " << material["parallaxMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define PARALLAX_MAP_UV_FLIP " << material["parallaxMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define PARALLAX_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["parallaxMapSwizzle"]->value().getFloat4()) << "\r\n";

	auto parallaxMap = material["parallaxMap"]->value().getTexture();
	auto parallaxMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == parallaxMap; });
	if (parallaxMapIter != _itemTextures.end())
		(*stream) << R"(#define PARALLAX_MAP_FILE ")" << (*parallaxMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define PARALLAX_MAP_FILE "parallax.png")" << "\r\n\r\n";

	(*stream) << "const float parallaxMapScale = " << material["parallaxMapScale"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 parallaxMapLoopNum = float2(" << material["parallaxMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define EMISSIVE_MAP_FROM " << material["emissiveMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define EMISSIVE_MAP_UV_FLIP " << material["emissiveMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define EMISSIVE_MAP_APPLY_SCALE 1\r\n";
	(*stream) << "#define EMISSIVE_MAP_APPLY_MORPH_COLOR " << "0\r\n";
	(*stream) << "#define EMISSIVE_MAP_APPLY_MORPH_INTENSITY " << "0\r\n";
	(*stream) << "#define EMISSIVE_MAP_APPLY_BLINK 1\r\n";

	auto emissiveMap = material["emissiveMap"]->value().getTexture();
	auto emissiveMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == emissiveMap; });
	if (emissiveMapIter != _itemTextures.end())
		(*stream) << R"(#define EMISSIVE_MAP_FILE ")" << (*emissiveMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define EMISSIVE_MAP_FILE "emissive.png")" << "\r\n\r\n";

	(*stream) << "const float3 emissive = float3(" << material["emissive"]->value().getFloat3() << ");\r\n";
	(*stream) << "const float2 emissiveMapLoopNum = float2(" << material["emissiveMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define CUSTOM_A_MAP_FROM " << material["customAMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define CUSTOM_A_MAP_UV_FLIP " << material["customAMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define CUSTOM_A_MAP_COLOR_FLIP 0\r\n";
	(*stream) << "#define CUSTOM_A_MAP_SWIZZLE " << (int)ray::math::dot(ray::float4(0, 1, 2, 3), material["customAMapSwizzle"]->value().getFloat4()) << "\r\n";
	(*stream) << "#define CUSTOM_A_MAP_APPLY_SCALE 1\r\n";

	auto customAMap = material["customAMap"]->value().getTexture();
	auto customAMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == customAMap; });
	if (customAMapIter != _itemTextures.end())
		(*stream) << R"(#define CUSTOM_A_MAP_FILE ")" << (*customAMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define CUSTOM_A_MAP_FILE "customA.png")" << "\r\n\r\n";

	(*stream) << "const float customA = " << material["customA"]->value().getFloat() << ";\r\n";
	(*stream) << "const float2 customAMapLoopNum = float2(" << material["customAMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << "#define CUSTOM_B_MAP_FROM " << material["customBMapFrom"]->value().getInt() << "\r\n";
	(*stream) << "#define CUSTOM_B_MAP_UV_FLIP " << material["customBMapFlip"]->value().getInt() << "\r\n";
	(*stream) << "#define CUSTOM_B_MAP_APPLY_SCALE " << "1\r\n";
	(*stream) << "#define CUSTOM_B_MAP_APPLY_DIFFUSE " << "0\r\n";
	(*stream) << "#define CUSTOM_B_MAP_APPLY_MORPH_COLOR " << "0\r\n";

	auto customBMap = material["customBMap"]->value().getTexture();
	auto customBMapIter = std::find_if(_itemTextures.begin(), _itemTextures.end(), [&](const EditorAssetItemPtr& item) { return std::get<EditorAssetItem::texture>(item->value) == customBMap; });
	if (customBMapIter != _itemTextures.end())
		(*stream) << R"(#define CUSTOM_B_MAP_FILE ")" << (*customBMapIter)->name << R"(.tga")" << "\r\n\r\n";
	else
		(*stream) << R"(#define CUSTOM_B_MAP_FILE "customB.png")" << "\r\n\r\n";

	(*stream) << "const float3 customB = float3(" << material["customB"]->value().getFloat3() << ");\r\n";
	(*stream) << "const float2 customBMapLoopNum = float2(" << material["customBMapLoopNum"]->value().getFloat2() << ");\r\n\r\n";

	(*stream) << R"(#include "material_common_2.0.fxsub")";

	return true;
}

bool
GuiControllerComponent::onExportModel(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept
{
	if (_models.size() > index)
	{
		ray::StreamWritePtr stream;
		if (!ray::IoServer::instance()->openFile(stream, path))
			return false;

		ray::PMXHandler header;
		if (!header.doSave(*stream, *_models[index]))
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
	if (_models.empty())
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
	if (_models.empty())
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
			if (!lightPack.atlasUV(*_models[0], size, size, params.uvmapper.chart, params.uvmapper.stretch, params.uvmapper.margin, progress))
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
	if (_models.empty())
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
	if (_models.empty())
	{
		error = "Please load model before start lightmass";
		return false;
	}

	if (!_future)
	{
		if (options.uvmapper.slot > _models[0]->header.addUVCount)
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

			params.model.vertices = (std::uint8_t*)_models[0]->vertices.data();
			params.model.indices = _models[0]->indices.data();
			params.model.sizeofVertices = sizeof(ray::PMX_Vertex);
			params.model.sizeofIndices = _models[0]->header.sizeOfIndices;
			params.model.strideVertices = offsetof(ray::PMX_Vertex, position);
			params.model.strideTexcoord = options.uvmapper.slot ? offsetof(ray::PMX_Vertex, addCoord[options.uvmapper.slot - 1]) : offsetof(ray::PMX_Vertex, coord);
			params.model.numVertices = _models[0]->numVertices;
			params.model.numIndices = _models[0]->numIndices;
			params.model.subsets.resize(_models[0]->numMaterials);

			for (std::uint32_t i = 0; i < _models[0]->numMaterials; i++)
			{
				std::uint32_t offset = 0;

				for (std::uint32_t j = 0; j < i; j++)
					offset += _models[0]->materials[j].IndicesCount;

				params.model.subsets[i].emissive = ray::math::srgb2linear(_models[0]->materials[i].Ambient) * _models[0]->materials[i].Shininess;

				params.model.subsets[i].drawcall.count = _models[0]->materials[i].IndicesCount;
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
GuiControllerComponent::onFetchTextures(const EditorAssetItems*& textures) noexcept
{
	textures = &_itemTextures;
	return true;
}

bool
GuiControllerComponent::onFetchMaterials(const EditorAssetItems*& material) noexcept
{
	material = &_itemMaterials;
	return true;
}

bool
GuiControllerComponent::onSeletedCamera(const ray::GameObject* cameras) noexcept
{
	if (_cube)
		_cube->setScaleAll(0.0f);
	return true;
}

bool
GuiControllerComponent::onSeletedLight(const ray::GameObject* object) noexcept
{
	if (_cube)
		_cube->setScaleAll(0.0f);
	return true;
}

bool
GuiControllerComponent::onSeletedLightProbe(const ray::GameObject* object) noexcept
{
	if (_cube)
		_cube->setScaleAll(0.0f);
	return true;
}

bool
GuiControllerComponent::onSeletedMesh(const ray::GameObject* object, std::size_t subset) noexcept
{
	return this->onTransformObject(object, subset);
}

bool
GuiControllerComponent::onUpdateMaterial(const EditorAssetItem& item) noexcept
{
	ray::float4x4 view;
	view.makeLookAt_lh(ray::float3::UnitX * 2.1f, ray::float3::Zero, ray::float3::Up);
	ray::float4x4 proj;
	proj.makePerspective_fov_lh(60.0f, 1.0, 1.0f, 10.0f);

	auto skyComponent = _lights.front()->getComponent<ray::SkyboxComponent>();

	auto material = std::get<EditorAssetItem::material>(item.value);
	material->getParameter("matTransform")->uniform4fmat(proj * view);
	material->getParameter("envDiffuse")->uniformTexture(skyComponent->getSkyLightDiffuse());
	material->getParameter("envSpecular")->uniformTexture(skyComponent->getSkyLightSpecular());

	ray::RenderSystem::instance()->setFramebuffer(_previewFramebuffer);
	ray::RenderSystem::instance()->clearFramebuffer(0, ray::GraphicsClearFlagBits::GraphicsClearFlagColorDepthBit, ray::float4::Zero, 1.0, 0);
	ray::RenderSystem::instance()->setVertexBuffer(0, _vbo, 0);
	ray::RenderSystem::instance()->setIndexBuffer(_ibo, 0, ray::GraphicsIndexType::GraphicsIndexTypeUInt32);
	ray::RenderSystem::instance()->setMaterialPass(material->getTech("Preview")->getPass(0));
	ray::RenderSystem::instance()->drawIndexed(4416, 1, 0, 0, 0);
	ray::RenderSystem::instance()->readFramebuffer(item.preview, 0, 0, item.preview->getGraphicsTextureDesc().getWidth(), item.preview->getGraphicsTextureDesc().getHeight());

	return true;
}

bool
GuiControllerComponent::onTransformObject(const ray::GameObject* object, std::size_t subset) noexcept
{
	if (_cube)
	{
		auto meshComponent = object->getComponent<ray::MeshComponent>();
		if (meshComponent)
		{
			auto boundingBox = meshComponent->getMesh()->getMeshSubsets()[subset].boundingBox;
			boundingBox.transform(object->getTransform());

			_cube->setTranslate(boundingBox.center());
			_cube->setScale(boundingBox.size());
		}
	}

	return true;
}

void
GuiControllerComponent::onMouseHoveringCamera(const ray::float4& viewport, bool isHovering) noexcept
{
	if (!_cameras.empty())
	{
		auto cameraComponent = _cameras.front()->getComponent<FirstPersonCameraComponent>();

		if (isHovering)
			cameraComponent->setEditMode(FirstPersonCameraComponent::EditMode::ThirdEdit);
		else
			cameraComponent->setEditMode(FirstPersonCameraComponent::EditMode::FirstPerson);
	}
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
			material.ToonTexture = std::numeric_limits<std::uint8_t>::max();
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