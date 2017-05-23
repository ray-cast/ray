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
#ifndef _H_ANIM_COMPONENT_H_
#define _H_ANIM_COMPONENT_H_

#include <ray/game_component.h>
#include <ray/anim.h>

_NAME_BEGIN

class EXPORT AnimationComponent final : public GameComponent
{
	__DeclareSubClass(AnimationComponent, GameComponent)
public:
	AnimationComponent() noexcept;
	~AnimationComponent() noexcept;

	bool play(const std::string& filename) noexcept;
	void pause() noexcept;
	void stop() noexcept;

	void enableAnimOnVisiable(bool visiableOnly) noexcept;
	bool enableAnimOnVisiable() const noexcept;

	void enablePhysics(bool physics) noexcept;
	bool enablePhysics() const noexcept;

	void setTransforms(const GameObjects& transforms) noexcept;
	void setTransforms(GameObjects&& transforms) noexcept;
	const GameObjects& getTransforms() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:
	bool _playAnimation(const std::string& filename) noexcept;
	void _updateAnimation() noexcept;
	void _destroyAnimation() noexcept;

private:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onAttachComponent(GameComponentPtr& component) noexcept;
	virtual void onDetachComponent(GameComponentPtr& component) noexcept;

	virtual void onMeshChange() noexcept;
	virtual void onMeshWillRender(const class Camera&) noexcept;

	virtual void onFrameEnd() noexcept;

private:
	bool _enableAnimation;
	bool _enableAnimOnVisableOnly;
	bool _enablePhysics;
	bool _needUpdate;

	GameObjects _transforms;
	AnimationPropertyPtr _animtion;

	std::function<void()> _onMeshChange;
	std::function<void(const Camera&)> _onMeshWillRender;
};

_NAME_END

#endif