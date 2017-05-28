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
#ifndef _H_RES_MANAGER_H_
#define _H_RES_MANAGER_H_

#include <ray/res_loader.h>
#include <ray/render_types.h>
#include <ray/game_types.h>
#include <ray/modhelp.h>

_NAME_BEGIN

enum ModelMakerFlagBits
{
	ModelMakerFlagBitVertex = 0x00000001,
	ModelMakerFlagBitTexcoord = 0x00000002,
	ModelMakerFlagBitNormal = 0x00000004,
	ModelMakerFlagBitTangent = 0x00000008,
	ModelMakerFlagBitTangentQuat = 0x000000016,
	ModelMakerFlagBitBitangent = 0x00000032,
	ModelMakerFlagBitWeight = 0x00000064,
	ModelMakerFlagBitColor = 0x000000128,
	ModelMakerFlagBitALL = 0x7FFFFFFF,
};

typedef std::uint32_t ModelMakerFlags;

class EXPORT ResManager final
{
	__DeclareSingleton(ResManager)
public:
	ResManager() noexcept;
	~ResManager() noexcept;

	bool createModel(const util::string& path, ModelPtr& model) noexcept;
	bool createMaterial(const util::string& path, MaterialPtr& material) noexcept;
	bool createTexture(const util::string& path, GraphicsTexturePtr& texture, GraphicsTextureDim dim = GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerWrap warp = GraphicsSamplerWrap::GraphicsSamplerWrapRepeat) noexcept;
	bool createAnimation(const util::string& path, const GameObjects& bones, GameComponentPtr& animation) noexcept;

	bool createGameObject(const Model& model, GameObjectPtr& gameObject) noexcept;
	bool createMeshes(const Model& model, GameObjectPtr& meshes) noexcept;
	bool createBones(const Model& model, GameObjects& bones) noexcept;
	bool createMaterials(const Model& model, Materials& materials, bool skinned = true) noexcept;
	bool createRigidbodys(const Model& model, GameObjects& rigidbodys) noexcept;
	bool createRigidbodyToBone(const Model& model, const GameObjects& bones, GameObjects& rigidbodys);
	bool createJoints(const Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept;

	GraphicsDataPtr createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;
	GraphicsDataPtr createIndexBuffer(const MeshProperty& mesh) noexcept;
	GraphicsTexturePtr getTexture(const util::string& name) noexcept;

	void destroyTexture(GraphicsTexturePtr texture) noexcept;
	void destroyTexture(const util::string& name) noexcept;

private:
	MaterialPtr _buildDefaultMaterials(const MaterialProperty& material, const util::string& file, const util::string& directory) noexcept;

private:
	ResManager(const ResManager&) = delete;
	ResManager& operator=(const ResManager&) = delete;

private:
	std::map<util::string, GraphicsTexturePtr> _textures;
};

_NAME_END

#endif