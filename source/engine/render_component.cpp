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
#include <ray/render_component.h>

_NAME_BEGIN

__ImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

RenderComponent::RenderComponent() noexcept
	: _isCastShadow(true)
	, _isReceiveShadow(true)
{
}

RenderComponent::~RenderComponent() noexcept
{
}

void
RenderComponent::setCastShadow(bool value) noexcept
{
	_isCastShadow = value;
}

bool
RenderComponent::getCastShadow() const noexcept
{
	return _isCastShadow;
}

void
RenderComponent::setReceiveShadow(bool value) noexcept
{
	_isReceiveShadow = value;
}

bool
RenderComponent::getReceiveShadow() const noexcept
{
	return _isReceiveShadow;
}

void
RenderComponent::addPreRenderListener(std::function<void(RenderPipeline&)> func) noexcept
{
	assert(!_onPreRender.find(func));
	_onPreRender.attach(func);
}

void
RenderComponent::removePreRenderListener(std::function<void(RenderPipeline&)> func) noexcept
{
	assert(_onPreRender.find(func));
	_onPreRender.remove(func);
}

void
RenderComponent::addPostRenderListener(std::function<void(RenderPipeline&)> func) noexcept
{
	assert(!_onPostRender.find(func));
	_onPostRender.attach(func);
}

void
RenderComponent::removePostRenderListener(std::function<void(RenderPipeline&)> func) noexcept
{
	assert(_onPostRender.find(func));
	_onPostRender.remove(func);
}

void
RenderComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	reader >> make_archive(_isCastShadow, "castshadow");
	reader >> make_archive(_isReceiveShadow, "receiveshadow");
}

void
RenderComponent::save(oarchive& write) noexcept
{
	GameComponent::save(write);

	write << make_archive(_isCastShadow, "castshadow");
	write << make_archive(_isReceiveShadow, "receiveshadow");
}

void
RenderComponent::onRenderObjectPre(RenderPipeline& pipeline) noexcept
{
	_onPreRender.run(pipeline);
}

void
RenderComponent::onRenderObjectPost(RenderPipeline& pipeline) noexcept
{
	_onPostRender.run(pipeline);
}

_NAME_END