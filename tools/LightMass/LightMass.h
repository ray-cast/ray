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
#ifndef _H_LIGHT_MASS_H_
#define _H_LIGHT_MASS_H_

#include "LightMassParams.h"
#include "LightMassListener.h"

#include "modpmx.h"

_NAME_BEGIN

class LightMass
{
public:
	LightMass() noexcept;
	~LightMass() noexcept;

	bool load(const std::string& path, PMX& pmx) noexcept;
	bool save(const std::string& path, const PMX& pmx) noexcept;
	bool saveLightMass(const std::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t c, std::uint32_t margin);

	void setLightMassListener(LightMassListenerPtr pointer) noexcept;
	LightMassListenerPtr getLightMassListener() const noexcept;

	bool pack(const LightMassParams& params, PMX& pmx) noexcept;

	bool baking(const LightMassParams& params, const PMX& model) noexcept;

	void computeBoundingBox(const PMX& model, Bound& boundingBox, std::uint32_t firstFace, std::uint32_t faceCount) noexcept;

	std::uint32_t getFace(const PMX& pmx, std::size_t n) noexcept;
	std::uint32_t getFace(const PMX& pmx, std::size_t n, std::uint32_t firstIndex) noexcept;
	
	void ImageDilate(const float *image, float *outImage, int w, int h, int c) noexcept;
	void ImageSmooth(const float *image, float *outImage, int w, int h, int c) noexcept;

private:
	LightMassListenerPtr _lightMassListener;
};

_NAME_END

#endif