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
#ifndef _H_SKYBOX_COMPONENT_H_
#define _H_SKYBOX_COMPONENT_H_

#include <ray/mesh_render_component.h>

_NAME_BEGIN

class EXPORT SkyboxComponent final : public MeshRenderComponent
{
	__DeclareSubClass(SkyboxComponent, MeshRenderComponent)
public:
	SkyboxComponent() noexcept;
	~SkyboxComponent() noexcept;

	void setSkyboxSize(float size) noexcept;
	float getSkyboxSize() const noexcept;

	void setSkyBox(GraphicsTexturePtr texture) noexcept;
	GraphicsTexturePtr getSkyBox() const noexcept;

	void setSkyLightDiffuse(GraphicsTexturePtr texture) noexcept;
	GraphicsTexturePtr getSkyLightDiffuse() const noexcept;

	void setSkyLightSpecular(GraphicsTexturePtr texture) noexcept;
	GraphicsTexturePtr getSkyLightSpecular() const noexcept;

	void setSkyboxEnable(bool enable) noexcept;
	bool getSkyboxEnable() const noexcept;

	void setSkyLightingEnable(bool enable) noexcept;
	bool getSkyLightingEnable() const noexcept;

	void setSkyDiffuseIntensity(float intensity) noexcept;
	float getSkyDiffuseIntensity() const noexcept;

	void setSkySpecularIntensity(float intensity) noexcept;
	float getSkySpecularIntensity() const noexcept;

	void addSkyBoxChangeListener(std::function<void()>* func) noexcept;
	void addSkyLightingDiffuseChangeListener(std::function<void()>* func) noexcept;
	void addSkyLightingSpecularChangeListener(std::function<void()>* func) noexcept;
	void addEnableSkyBoxListener(std::function<void(bool)>* func) noexcept;
	void addEnableSkyLightingListener(std::function<void(bool)>* func) noexcept;

	void removeSkyBoxChangeListener(std::function<void()>* func) noexcept;
	void removeSkyLightingDiffuseChangeListener(std::function<void()>* func) noexcept;
	void removeSkyLightingSpecularChangeListener(std::function<void()>* func) noexcept;
	void removeEnableSkyBoxListener(std::function<void(bool)>* func) noexcept;
	void removeEnableSkyLightingListener(std::function<void(bool)>* func) noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:
	bool _loadSkybox(const util::string& texture) noexcept;
	bool _loadSkyDiffuse(const util::string& texture) noexcept;
	bool _loadSkySpecular(const util::string& texture) noexcept;

	bool _buildQuadMesh(MeshProperty& mesh) noexcept;
	bool _buildQuadRenderMesh(const MeshProperty& mesh) noexcept;
	bool _buildQuadRenderObject(const MeshProperty& mesh, MaterialPtr technique) noexcept;

	bool _buildSphereMesh(MeshProperty& mesh) noexcept;
	bool _buildSphereRenderMesh(const MeshProperty& mesh) noexcept;
	bool _buildSphereRenderObject(const MeshProperty& mesh, MaterialPtr technique) noexcept;

	bool _setupSkybox() noexcept;
	bool _setupSkyLighting() noexcept;
	bool _setupMaterial() noexcept;

	void _destroySkybox() noexcept;
	void _destroySkyLighting() noexcept;
	void _destroyMaterial() noexcept;

	void _reloadSkybox(const util::string& texture) noexcept;
	void _reloadSkyDiffuse(const util::string& texture) noexcept;
	void _reloadSkySpecular(const util::string& texture) noexcept;

	void _updateMaterial() noexcept;
	void _updateTransform() noexcept;

private:
	void onActivate() noexcept;
	void onDeactivate() noexcept;

	void onMoveAfter() noexcept;

private:
	SkyboxComponent(const SkyboxComponent&) = delete;
	SkyboxComponent& operator=(const SkyboxComponent&) = delete;

private:
	bool _enableSkyBox;
	bool _enableSkyLighting;

	float _skyboxSize;

	float2 _skyLightingIntensity;

	util::string _skyMap;
	util::string _skyDiffuse;
	util::string _skySpecular;

	MaterialPtr _skyBoxMaterial;

	GraphicsTexturePtr _skyTexture;
	GraphicsTexturePtr _skyDiffTexture;
	GraphicsTexturePtr _skySpecTexture;

	GraphicsDataPtr _renderScreenQuadVbo;
	GraphicsDataPtr _renderScreenQuadIbo;

	GraphicsDataPtr _renderSphereVbo;
	GraphicsDataPtr _renderSphereIbo;

	GeometryPtr _quadObject;
	GeometryPtr _sphereObject;

	delegate<void()> _onSkyBoxChange;
	delegate<void()> _onSkyLightingDiffuseChange;
	delegate<void()> _onSkyLightingSpecularChange;
	delegate<void(bool)> _onEnableSkyBox;
	delegate<void(bool)> _onEnableSkyLighting;
};

_NAME_END

#endif