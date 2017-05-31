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
#include <ray/game_component.h>

#include "UIParams.h"
#include "modpmx.h"
#include "LightMass.h"
#include "LightMapPack.h"

#include <future>

class GuiControllerComponent final : public ray::GameComponent
{
	__DeclareSubClass(GuiViewComponent, ray::GameComponent)
public:
	GuiControllerComponent() noexcept;
	~GuiControllerComponent() noexcept;

	ray::GameComponentPtr clone() const noexcept;

private:
	void onAttachComponent(ray::GameComponentPtr& component) except;
	void onDetachComponent(ray::GameComponentPtr& component) noexcept;

	void onActivate() except;

	bool onModelImport(ray::util::string::const_pointer path, ray::util::string& error) noexcept;
	bool onModelSaveAs(ray::util::string::const_pointer path, ray::util::string& error) noexcept;

	bool onUVMapperCancel() noexcept;
	bool onUVMapperWillStart(const GuiParams& params) noexcept;
	bool onUVMapperProcessing(const GuiParams& params, float& progressing) noexcept;

	bool onLightMassCancel() noexcept;
	bool onLightMassWillStart(const GuiParams& params) noexcept;
	bool onLightMassProcessing(const GuiParams& params, float& progressing) noexcept;
	bool onLightMassSave(ray::util::string::const_pointer path, ray::util::string& error) noexcept;

	bool onSaveLightMass(const ray::util::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t channel, std::uint32_t margin);
	bool onOutputSphere(ray::util::string::const_pointer path, ray::util::string& error) noexcept;

private:
	GuiControllerComponent(const GuiControllerComponent&) = delete;
	GuiControllerComponent& operator=(const GuiControllerComponent&) = delete;

private:
	bool _stopUVMapper;

	ray::GameObjects _objects;

	std::unique_ptr<ray::PMX> _model;
	std::unique_ptr<std::future<bool>> _future;
	std::unique_ptr<ray::LightMass> _lightMass;

	ray::LightMapListenerPtr _lightMapListener;
	ray::LightMassListenerPtr _lightMassListener;
};