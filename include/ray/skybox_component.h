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
#ifndef _H_SKYBOX_COMPONENT_H_
#define _H_SKYBOX_COMPONENT_H_

#include <ray/mesh_render_component.h>

_NAME_BEGIN

class EXPORT SkyboxComponent final : public MeshRenderComponent
{
	__DeclareSubClass(SkyboxComponent, MeshRenderComponent)
public:
	SkyboxComponent() noexcept;
	SkyboxComponent(MaterialPtr material) noexcept;
	~SkyboxComponent() noexcept;

	void setSkyLightMap(const std::string& texture) noexcept;
	const std::string& getSkyLightMap() const noexcept;

	void setSkyLightDiffuse(const std::string& diffuse) noexcept;
	const std::string& getSkyLightDiffuse() const noexcept;

	void setSkyLightSpecular(const std::string& specular) noexcept;
	const std::string& getSkyLightSpecular() const noexcept;

	void setSkyboxEnable(bool enable) noexcept;
	bool getSkyboxEnable() const noexcept;

	void setSkyLightingEnable(bool enable) noexcept;
	bool getSkyLightingEnable() const noexcept;

	void setSkyLightingIntensity(const float2& intensity) noexcept;
	const float2& getSkyLightingIntensity() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:
	bool _loadSkybox(const std::string& texture) noexcept;
	bool _loadSkyDiffuse(const std::string& texture) noexcept;
	bool _loadSkySpecular(const std::string& texture) noexcept;

	bool _buildQuadMesh(MeshProperty& mesh) noexcept;
	bool _buildQuadRenderMesh(const MeshProperty& mesh) noexcept;
	bool _buildQuadRenderObject(const MeshProperty& mesh, MaterialTechPtr technique) noexcept;

	bool _buildSphereMesh(MeshProperty& mesh) noexcept;
	bool _buildSphereRenderMesh(const MeshProperty& mesh) noexcept;
	bool _buildSphereRenderObject(const MeshProperty& mesh, MaterialTechPtr technique) noexcept;

	bool _setupSkybox() noexcept;
	bool _setupSkyLighting() noexcept;
	bool _setupMaterial() noexcept;

	void _destroySkybox() noexcept;
	void _destroySkyLighting() noexcept;
	void _destroyMaterial() noexcept;

	void _reloadSkybox(const std::string& texture) noexcept;
	void _reloadSkyDiffuse(const std::string& texture) noexcept;
	void _reloadSkySpecular(const std::string& texture) noexcept;

	void _updateMaterial() noexcept;
	void _updateTransform() noexcept;

private:
	void onActivate() noexcept;
	void onDeactivate() noexcept;

	void onFrameEnd() noexcept;

	void onMoveAfter() noexcept;

private:
	SkyboxComponent(const SkyboxComponent&) = delete;
	SkyboxComponent& operator=(const SkyboxComponent&) = delete;

private:
	bool _enableSkyBox;
	bool _enableSkyLighting;

	bool _needUpdateSkybox;
	bool _needUpdateSkyDiffuse;
	bool _needUpdateSkySpecular;

	float2 _skyLightingIntensity;

	std::string _skyMap;
	std::string _skyDiffuse;
	std::string _skySpecular;

	GraphicsTexturePtr _skyTexture;
	GraphicsTexturePtr _skyDiffTexture;
	GraphicsTexturePtr _skySpecTexture;

	RenderMeshPtr _renderScreenQuad;
	RenderMeshPtr _renderSphere;

	GeometryPtr _quadObject;
	GeometryPtr _sphereObject;
};

_NAME_END

#endif