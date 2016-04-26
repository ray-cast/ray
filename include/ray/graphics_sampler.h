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
#ifndef _H_GRAPHICS_SAMPLER_H_
#define _H_GRAPHICS_SAMPLER_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsSamplerDesc final
{
public:
	GraphicsSamplerDesc() noexcept;
	~GraphicsSamplerDesc() noexcept;

	void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
	void setSamplerFilter(GraphicsSamplerFilter filter) noexcept;
	void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;

	GraphicsSamplerWrap getSamplerWrap() const noexcept;
	GraphicsSamplerFilter getSamplerFilter() const noexcept;
	GraphicsSamplerAnis getSamplerAnis() const noexcept;

private:
	GraphicsSamplerWrap _wrap;
	GraphicsSamplerAnis _anis;
	GraphicsSamplerFilter _filter;
};

class GraphicsSampler : public GraphicsChild
{
	__DeclareSubInterface(GraphicsSampler, GraphicsChild)
public:
	GraphicsSampler() noexcept;
	virtual ~GraphicsSampler() noexcept;

	virtual const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept = 0;

private:
	GraphicsSampler(const GraphicsSampler&) noexcept = delete;
	GraphicsSampler& operator=(const GraphicsSampler&) noexcept = delete;
};

_NAME_END

#endif