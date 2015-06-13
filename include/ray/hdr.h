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
#ifndef _H_HDR_H_
#define _H_HDR_H_

#include <ray/post_process.h>

_NAME_BEGIN

class EXPORT HDR final : public RenderPostProcess
{
public:
    struct EXPORT Setting
    {
        float lumAdapt;
        float lumKey;
        float lumMax;

        float exposure;
        float exposureBias;

        float vignetteEnable;
        float vignetteInner;
        float vignetteOuter;

        Setting() noexcept;
    };

public:
    HDR() except;
    ~HDR() noexcept;

    void setSetting(const Setting& setting) noexcept;
    const Setting& getSetting() const noexcept;

    void render(RenderPipeline& pipeline, RenderTexturePtr source) noexcept;

private:

    void sample4(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
    void sample8(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
    void sampleLog(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;

    void blurh(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
    void blurv(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;

    void measureLuminance(RenderPipeline& pipeline, RenderTexturePtr source);

    void generateBloom(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest);
    void generateToneMapping(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr bloom);

    void onActivate() except;
    void onDeactivate() except;

private:

    Setting _setting;

    RenderTexturePtr _texSample4;
    RenderTexturePtr _texSample8;
    RenderTexturePtr _texSampleLog;

    RenderTexturePtr _texBloom;

    MaterialPtr _hdr;

    MaterialPassPtr _sample4;
    MaterialPassPtr _sample8;
    MaterialPassPtr _samplelog;
    MaterialPassPtr _bloom;
    MaterialPassPtr _blurh;
    MaterialPassPtr _blurv;
    MaterialPassPtr _tone;

    ShaderParamPtr _toneSource;
    ShaderParamPtr _toneBloom;
    ShaderParamPtr _toneLumAve;
    ShaderParamPtr _toneLumKey;
    ShaderParamPtr _toneLumMax;
    ShaderParamPtr _toneExposure;
    ShaderParamPtr _toneExposureBias;
    ShaderParamPtr _toneVignetteEnable;
    ShaderParamPtr _toneVignetteInner;
    ShaderParamPtr _toneVignetteOuter;

    std::vector<float> _data;
};

_NAME_END

#endif