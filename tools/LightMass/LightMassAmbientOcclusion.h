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
#ifndef _H_LIGHTMASS_AO_H_
#define _H_LIGHTMASS_AO_H_

#include "LightMassBaking.h"

_NAME_BEGIN

class LightBakingAO final : public LightMassBaking
{
public:
	LightBakingAO() noexcept;
	LightBakingAO(const LightBakingParams& params);
	~LightBakingAO() noexcept;

	bool open(const LightBakingParams& params) noexcept;
	void close() noexcept;

private:
	bool setupContext(const LightModelData& params) noexcept;

	bool doSampleHemisphere(const Viewportt<int>& viewport, const float4x4& mvp);
	void computeBoundingBox(const LightModelData& params, ray::BoundingBox& boundingBox, std::uint32_t firstFace, std::size_t faceCount) noexcept;

private:
	struct GLContext
	{
		GLContext() noexcept;
		~GLContext() noexcept;

		std::uint32_t vs;
		std::uint32_t fs;
		std::uint32_t program;

		std::uint32_t mvp;

		std::uint32_t lightmap;
		std::uint32_t vao, vbo, ibo;
	};

	struct GLDrawElementsIndirectCommand
	{
		std::uint32_t count;
		std::uint32_t instanceCount;
		std::uint32_t firstIndex;
		std::uint32_t baseVertex;
		std::uint32_t baseInstance;
		std::uint32_t faceType;
	};

	std::unique_ptr<GLContext> _glcontext;

	std::vector<BoundingBox> _boundingBoxs;
	std::vector<GLDrawElementsIndirectCommand> _drawcalls;

	std::function<bool(float progress)> _progress;
};

_NAME_END

#endif