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
#ifndef _H_SSAO_H_
#define _H_SSAO_H_

#include <ray/post_process.h>

_NAME_BEGIN

class SSAO : public RenderPostProcess
{
public:
    struct Setting
    {
        float radius;
        float bias;
        float epsilon;
        float intensity;

        int numSample;

        bool blur;
        int blurRadius;
        float blurSharpness;
    };

public:
    SSAO() noexcept;
    ~SSAO() noexcept;

    void setSetting(const Setting& set) noexcept;
    const Setting& getSetting() const noexcept;

    void render(RenderPipeline* pipeline, RenderTexturePtr source) noexcept;

private:

    void computeRawAO(RenderPipeline* pipeline, RenderTexturePtr dest) noexcept;
    void blurHorizontal(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
    void blurVertical(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
    void blurDirection(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest, const int2& direction) noexcept;
    void shading(RenderPipeline* pipeline, RenderTexturePtr color, RenderTexturePtr ao) noexcept;

private:

    Setting _setting;

    RenderTexturePtr _texAmbient;
    RenderTexturePtr _texBlur;

    MaterialPtr _ambientOcclusion;

    MaterialPassPtr _ambientOcclusionPass;
    MaterialPassPtr _ambientOcclusionBlurPass;
    MaterialPassPtr _ambientOcclusionCopyPass;

    ShaderParamPtr _radius;
    ShaderParamPtr _projScale;
    ShaderParamPtr _projInfo;
    ShaderParamPtr _clipInfo;
    ShaderParamPtr _bias;
    ShaderParamPtr _epsilon;
    ShaderParamPtr _intensityDivR6;
    ShaderParamPtr _numSample;

    MaterialPassPtr _blurh;
    MaterialPassPtr _blurv;

    ShaderParamPtr _blurRadius;
    ShaderParamPtr _blurTexSource;
    ShaderParamPtr _blurFactor;
    ShaderParamPtr _blurSharpness;
    ShaderParamPtr _blurDirection;
    ShaderParamPtr _blurGaussian;

    ShaderParamPtr _copyAmbient;
};

_NAME_END

#endif