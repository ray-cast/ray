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
        float bloomFactor;
        float bloomIntensity;

        float lumAve;
        float lumKey;
        float lumExposure;

        Setting() noexcept;
    };

public:
    HDR() noexcept;
    ~HDR() noexcept;

    void setSetting(const Setting& setting) noexcept;
    const Setting& getSetting() const noexcept;

private:

    void sample4(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept;
    void sample8(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept;
    void sampleLog(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept;

    void measureLuminance(RenderPipeline& pipeline, RenderTargetPtr source) noexcept;

    void generateBloom(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept;
    void generateToneMapping(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept;

    void blurh(RenderPipeline& pipeline, RenderTargetPtr source) noexcept;
    void blurv(RenderPipeline& pipeline, RenderTargetPtr source) noexcept;

private:

    void onActivate(RenderPipeline& pipeline) except;
    void onDeactivate(RenderPipeline& pipeline) except;

    void onRender(RenderPipeline& pipeline, RenderTargetPtr source) noexcept;

private:

    float _lumAdapt;

    Setting _setting;

    RenderTargetPtr _texSample4;
    RenderTargetPtr _texSample8;
    RenderTargetPtr _texSampleLog;

    RenderTargetPtr _texBloom;

    MaterialPtr _hdr;

    MaterialPassPtr _sample4;
    MaterialPassPtr _sample8;
    MaterialPassPtr _samplelog;
    MaterialPassPtr _bloom;
    MaterialPassPtr _blurh;
    MaterialPassPtr _blurv;
    MaterialPassPtr _tone;

    ShaderVariantPtr _texSizeInv;

    ShaderVariantPtr _bloomFactor;
    ShaderVariantPtr _bloomIntensity;

    ShaderVariantPtr _toneSource;
    ShaderVariantPtr _toneBloom;
    ShaderVariantPtr _toneLumAve;
    ShaderVariantPtr _toneLumKey;
    ShaderVariantPtr _toneLumExposure;
};

_NAME_END

#endif