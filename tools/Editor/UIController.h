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
#include <ray/game_component.h>
#include <ray/material.h>

#include "UIParams.h"
#include "modpmx.h"
#include "LightMass.h"
#include "LightMapPack.h"
#include "EditorItemTexture.h"

#include <future>

class GuiControllerComponent final : public ray::GameComponent
{
	__DeclareSubClass(GuiViewComponent, ray::GameComponent)
public:
	GuiControllerComponent() noexcept;
	~GuiControllerComponent() noexcept;

	ray::GameComponentPtr clone() const noexcept;

private:
	bool makeCubeObject() noexcept;
	bool makeMainCamera() noexcept;
	bool makeSphereObjects() noexcept;
	bool makeSkyLighting() noexcept;

private:
	void onAttachComponent(const ray::GameComponentPtr& component) except;
	void onDetachComponent(const ray::GameComponentPtr& component) noexcept;

	void onActivate() except;
	void onMessage(const ray::MessagePtr& message) except;

	bool onImportIES(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;
	bool onImportTexture(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;
	bool onImportMaterial(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;
	bool onImportModel(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;

	bool onExportIES(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept;
	bool onExportTexture(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept;
	bool onExportMaterial(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept;
	bool onExportModel(ray::util::string::const_pointer path, std::size_t index, ray::util::string::pointer& error) noexcept;

	bool onUVMapperCancel() noexcept;
	bool onUVMapperWillStart(const GuiParams& params, ray::util::string::pointer& error) noexcept;
	bool onUVMapperProcessing(const GuiParams& params, float& progressing, ray::util::string::pointer& error) noexcept;

	bool onLightMassCancel() noexcept;
	bool onLightMassWillStart(const GuiParams& params, ray::util::string::pointer& error) noexcept;
	bool onLightMassProcessing(const GuiParams& params, float& progressing, ray::util::string::pointer& error) noexcept;
	bool onLightMassSave(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;

	bool onFetchCamera(const ray::GameObjects*& objects) noexcept;
	bool onFetchMeshes(const ray::GameObjects*& objects) noexcept;
	bool onFetchLights(const ray::GameObjects*& objects) noexcept;
	bool onFetchLightProbes(const ray::GameObjects*& objects) noexcept;
	bool onFetchTextures(const ray::GraphicsTextures*& textures) noexcept;
	bool onFetchMaterials(const ray::Materials*& material) noexcept;

	bool onSeletedCamera(const ray::GameObject* object) noexcept;
	bool onSeletedLight(const ray::GameObject* object) noexcept;
	bool onSeletedLightProbe(const ray::GameObject* object) noexcept;
	bool onSeletedMesh(const ray::GameObject* object, std::size_t subset) noexcept;

	bool onTransformObject(const ray::GameObject* object, std::size_t subset) noexcept;

	bool onOutputSphere(ray::util::string::const_pointer path, ray::util::string::pointer& error) noexcept;

	void onMouseHoveringCamera(const ray::float4& viewport, bool isHovering) noexcept;

private:
	GuiControllerComponent(const GuiControllerComponent&) = delete;
	GuiControllerComponent& operator=(const GuiControllerComponent&) = delete;

private:
	bool _stopUVMapper;
	bool _stopLightmass;

	ray::GameObjects _cameras;
	ray::GameObjects _lights;
	ray::GameObjects _lightProbes;
	ray::GameObjects _objects;
	ray::Materials _materials;

	ray::GameObjectPtr _cube;

	EditorItemTextures _itemIES;

	std::vector<std::unique_ptr<ray::PMX>> _models;
	std::unique_ptr<std::future<bool>> _future;
	std::shared_ptr<ray::LightMapData> _lightMapData;

	ray::LightMapListenerPtr _lightMapListener;
	ray::LightMassListenerPtr _lightMassListener;
};