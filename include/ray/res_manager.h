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
#ifndef _H_RES_MANAGER_H_
#define _H_RES_MANAGER_H_

#include <ray/res_loader.h>
#include <ray/game_types.h>
#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT ResManager final
{
	__DeclareSingleton(ResManager)
public:
	ResManager() noexcept;
	~ResManager() noexcept;
	
	MaterialPtr createMaterial(const std::string& name) noexcept;
	GameObjectPtr createGameObject(const std::string& name, const std::string& anim = "") noexcept;

	template<typename T>
	std::shared_ptr<T> find(const std::string& name) noexcept
	{
		return ResLoader<T>::find(name);
	}

	GraphicsTexturePtr createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerWrap warp = GraphicsSamplerWrap::GraphicsSamplerWrapRepeat) noexcept;
	GraphicsTexturePtr getTexture(const std::string& name) noexcept;

	void destroyTexture(GraphicsTexturePtr texture) noexcept;
	void destroyTexture(const std::string& name) noexcept;

private:
	bool loadModel(const std::string& filename, ResLoader<Model>& model) noexcept;

	void createMeshes(const Model& model, GameObjectPtr& meshes) noexcept;
	void createBones(const Model& model, GameObjects& bones) noexcept;
	void createAnimation(const Model& model, GameObjectPtr& gameObject, GameObjects& bones, const std::string& file) noexcept;
	void createMaterials(const Model& model, Materials& materials) noexcept;
	void createRigidbodys(const Model& model, GameObjects& rigidbodys) noexcept;
	void createRigidbodyToBone(const Model& model, GameObjects& rigidbodys, GameObjects& bones);
	void createJoints(const Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept;

private:
	MaterialPtr _buildDefaultMaterials(const MaterialProperty& material, const std::string& file, const std::string& directory) noexcept;

private:
	ResManager(const ResManager&) = delete;
	ResManager& operator=(const ResManager&) = delete;

private:
	std::map<std::string, GraphicsTexturePtr> _textures;
};

_NAME_END

#endif