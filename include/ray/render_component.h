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
#ifndef _H_RENDER_COMPONENT_H_
#define _H_RENDER_COMPONENT_H_

#include <ray/game_component.h>
#include <ray/render_object.h>

_NAME_BEGIN

class EXPORT RenderComponent : public GameComponent, public RenderListener
{
	__DeclareSubInterface(RenderComponent, GameComponent)
public:
	RenderComponent() noexcept;
	virtual ~RenderComponent() noexcept;

	void setCastShadow(bool value) noexcept;
	bool getCastShadow() const noexcept;

	void setReceiveShadow(bool value) noexcept;
	bool getReceiveShadow() const noexcept;

	void addPreRenderListener(std::function<void(RenderPipeline&)>* listener) noexcept;
	void removePreRenderListener(std::function<void(RenderPipeline&)>* listener) noexcept;

	void addPostRenderListener(std::function<void(RenderPipeline&)>* listener) noexcept;
	void removePostRenderListener(std::function<void(RenderPipeline&)>* listener) noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

protected:
	virtual void onRenderObjectPre(RenderPipeline& pipeline) noexcept;
	virtual void onRenderObjectPost(RenderPipeline& pipeline) noexcept;

private:
	RenderComponent(const RenderComponent&) = delete;
	RenderComponent& operator=(const RenderComponent&) = delete;

protected:
	bool _isCastShadow;
	bool _isReceiveShadow;

	delegate<void(RenderPipeline&)> _onPreRender;
	delegate<void(RenderPipeline&)> _onPostRender;
};

_NAME_END

#endif