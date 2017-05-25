// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/graphics_sampler.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsSampler, GraphicsChild, "GraphicsSampler")

GraphicsSamplerDesc::GraphicsSamplerDesc() noexcept
	: _wrap(GraphicsSamplerWrap::GraphicsSamplerWrapRepeat)
	, _anis(GraphicsSamplerAnis::GraphicsSamplerAnis0)
	, _filter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest)
{
}

GraphicsSamplerDesc::~GraphicsSamplerDesc() noexcept
{
}

void
GraphicsSamplerDesc::setSamplerWrap(GraphicsSamplerWrap wrap) noexcept
{
	_wrap = wrap;
}

void
GraphicsSamplerDesc::setSamplerFilter(GraphicsSamplerFilter filter) noexcept
{
	_filter = filter;
}

void
GraphicsSamplerDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
{
	_anis = anis;
}

GraphicsSamplerWrap
GraphicsSamplerDesc::getSamplerWrap() const noexcept
{
	return _wrap;
}

GraphicsSamplerFilter
GraphicsSamplerDesc::getSamplerFilter() const noexcept
{
	return _filter;
}

GraphicsSamplerAnis
GraphicsSamplerDesc::getSamplerAnis() const noexcept
{
	return _anis;
}

GraphicsSampler::GraphicsSampler() noexcept
{
}

GraphicsSampler::~GraphicsSampler() noexcept
{
}

_NAME_END