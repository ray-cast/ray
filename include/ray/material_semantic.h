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
#ifndef _H_MATERIAL_SEMANTIC_H_
#define _H_MATERIAL_SEMANTIC_H_

#include <ray/material_param.h>

_NAME_BEGIN

enum GlobalMatrixSemantic
{
    NotSemantic = -1,

    matModel,
    matModelInverse,
    matModelInverseTranspose,

    matView,
    matViewInverse,
    matViewInverseTranspose,

    matProject,
    matProjectInverse,

    matViewProject,
    matViewProjectInverse,

    matSemanticEnd
};

enum GlobalFloatSemantic
{
    CameraAperture = matSemanticEnd,
    CameraNear,
    CameraFar,

    Time,
    TimeDelta,
    TimeFps,

    LightRange,
    LightIntensity,
    LightSpotAngle,
    LightSpotInnerCone,
    LightSpotOuterCone,

    FloatEnd,
};

enum GlobalFloat3Semantic
{
    LightAmbient = FloatEnd,
    LightDiffuse,
    LightPosition,
    LightDirection,

    CameraPosition,
    CameraView,

    Float3End
};

enum GlobalFloat4Semantic
{
    Float4End = Float3End,
};

enum GlobalTexSemantic
{
    DepthMap = Float4End,
    ColorMap,
    NormalMap,

    DeferredDepthMap,
    DeferredGraphicMap,
    DeferredNormalMap,
    DeferredLightMap,

    NumSemantic,
};

class EXPORT MaterialSemantic final
{
public:
    MaterialSemantic() noexcept;
    ~MaterialSemantic() noexcept;

    void setup() noexcept;
    void close() noexcept;

    void setMatrixParam(GlobalMatrixSemantic index, const Matrix4x4& m) noexcept;
    const Matrix4x4& getMatrixParam(GlobalMatrixSemantic index) noexcept;
    std::string getMatrixParmName(GlobalMatrixSemantic index) const noexcept;
    GlobalMatrixSemantic getMatrixParamSemantic(const std::string& name) const noexcept;

    void setFloatParam(GlobalFloatSemantic index, float v) noexcept;
    float getFloatParam(GlobalFloatSemantic index) noexcept;
    std::string getFloatParmName(GlobalFloatSemantic index) const noexcept;
    GlobalFloatSemantic getFloatParamSemantic(const std::string& name) const noexcept;

    void setFloat3Param(GlobalFloat3Semantic index, const float3& v) noexcept;
    const Vector3& getFloat3Param(GlobalFloat3Semantic index) noexcept;
    std::string getFloat3ParmName(GlobalFloat3Semantic index) const noexcept;
    GlobalFloat3Semantic getFloat3ParamSemantic(const std::string& name) const noexcept;

    void setFloat4Param(GlobalFloat4Semantic index, const float4& v) noexcept;
    const Vector4& getFloat4Param(GlobalFloat4Semantic index) noexcept;
    std::string getFloat4ParmName(GlobalFloat4Semantic index) const noexcept;
    GlobalFloat4Semantic getFloat4ParamSemantic(const std::string& name) const noexcept;

    void setTexParam(GlobalTexSemantic index, TexturePtr v) noexcept;
    TexturePtr getTexParam(GlobalTexSemantic index) noexcept;
    std::string getTexParmName(GlobalTexSemantic index) const noexcept;
    GlobalTexSemantic getTexParamSemantic(const std::string& name) const noexcept;

    MaterialParamPtr getParamPointer(const std::string& name) const noexcept;

private:

    MaterialParams _semantics;
};

_NAME_END

#endif