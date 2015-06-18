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
#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include <ray/render_types.h>

_NAME_BEGIN

enum ShaderType
{
    ST_VERTEX,
    ST_FRAGMENT,
    ST_GEOMETRY,
    ST_COMPUTE,
    ST_TESS_EVALUATION,
    ST_TESS_CONTROL,
    ST_CG_VERTEX,
    ST_CG_FRAGMENT,
};

enum ShaderVariantType
{
    SPT_NONE,
    SPT_BOOL,
    SPT_INT,
    SPT_INT2,
    SPT_FLOAT,
    SPT_FLOAT2,
    SPT_FLOAT3,
    SPT_FLOAT4,
    SPT_FLOAT3X3,
    SPT_FLOAT4X4,
    SPT_FLOAT_ARRAY,
    SPT_FLOAT2_ARRAY,
    SPT_FLOAT3_ARRAY,
    SPT_FLOAT4_ARRAY,
    SPT_TEXTURE,
    SPT_BUFFER,
};

class EXPORT ShaderVariant : public Instance<ShaderVariant>
{
public:
    ShaderVariant() noexcept;
    ShaderVariant(const std::string& name, float value) noexcept;
    ShaderVariant(const std::string& name, const float3& value) noexcept;
    ShaderVariant(const std::string& name, const Vector4& value) noexcept;
    ShaderVariant(const std::string& name, const Matrix4x4& value) noexcept;
    ShaderVariant(const std::string& name, ShaderVariantType type) noexcept;
    ~ShaderVariant() noexcept;

    void setup() noexcept;
    void close() noexcept;

    void setName(const std::string& name) noexcept;
    const std::string& getName() const noexcept;

    void setSemantic(const std::string& semantic) noexcept;
    const std::string& getSemantic() const noexcept;

    void setType(ShaderVariantType type) noexcept;
    ShaderVariantType getType() const noexcept;

    void addParameter(ShaderVariantPtr arg) noexcept;
    void removeParameter(ShaderVariantPtr arg) noexcept;
    ShaderVariantPtr getParameter(const std::string& name) const noexcept;
    const ShaderVariants& getParameters() const noexcept;

    std::size_t getSize() const noexcept;

    void assign(bool value) noexcept;
    void assign(int value) noexcept;
    void assign(const int2& value) noexcept;
    void assign(float value) noexcept;
    void assign(const float2& value) noexcept;
    void assign(const float3& value) noexcept;
    void assign(const float4& value) noexcept;
    void assign(const float3x3& value) noexcept;
    void assign(const float4x4& value) noexcept;
    void assign(const std::vector<float>& value) noexcept;
    void assign(const std::vector<float2>& value) noexcept;
    void assign(const std::vector<float3>& value) noexcept;
    void assign(const std::vector<float4>& value) noexcept;
    void assign(TexturePtr texture) noexcept;

    const bool getBool() const noexcept;
    const int getInt() const noexcept;
    const int2& getInt2() const noexcept;
    const float getFloat() const noexcept;
    const float2& getFloat2() const noexcept;
    const float3& getFloat3() const noexcept;
    const float4& getFloat4() const noexcept;
    const float3x3& getFloat3x3() const noexcept;
    const float4x4& getFloat4x4() const noexcept;
    const std::vector<float>& getFloatArray() const noexcept;
    const std::vector<float2>& getFloat2Array() const noexcept;
    const std::vector<float3>& getFloat3Array() const noexcept;
    const std::vector<float4>& getFloat4Array() const noexcept;
    TexturePtr getTexture() const noexcept;

protected:

    virtual void onChangeBefore() noexcept;
    virtual void onChangeAfter() noexcept;

private:

    std::string _name;
    std::string _semantic;

    TexturePtr _texture;

    union
    {
        bool b;
        int i[4];
        float f[4];
        float3x3* m3;
        float4x4* m4;
        std::vector<float>* farray;
        std::vector<float2>* farray2;
        std::vector<float3>* farray3;
        std::vector<float4>* farray4;
    } _value;

    ShaderVariantType _type;

    ShaderVariants _params;
};

class EXPORT ShaderParameter : public Instance<ShaderParameter>
{
public:
    ShaderParameter() noexcept;
    virtual ~ShaderParameter() noexcept;

    void setName(const std::string& name) noexcept;
    const std::string& getName() const noexcept;

    void setLocation(std::size_t location) noexcept;
    std::size_t getLocation() const noexcept;

    void needUpdate(bool update) noexcept;
    bool needUpdate() const noexcept;

private:
    bool _needUpdate;

    std::string _name;
    std::size_t _location;
};

class EXPORT ShaderAttribute : public ShaderParameter
{
};

class EXPORT ShaderUniform : public ShaderParameter
{
public:
    ShaderUniform() noexcept;
    ~ShaderUniform() noexcept;

    void setType(ShaderVariantType buffer) noexcept;
    ShaderVariantType getType() const noexcept;

    void setValue(ShaderVariantPtr buffer) noexcept;
    ShaderVariantPtr getValue() const noexcept;

    void setBindingPoint(std::size_t unit) noexcept;
    std::size_t getBindingPoint() const noexcept;

public:

    std::size_t _bindingPoint;

    ShaderVariantType _type;
    ShaderVariantPtr _value;
};

class EXPORT ShaderSubroutine : public ShaderParameter
{
public:
};

class EXPORT Shader
{
public:
    Shader() noexcept;
    Shader(ShaderType type, const std::string& code) noexcept;
    Shader(const std::string& type, const std::string& code) noexcept;
    Shader(const std::wstring& type, const std::string& code) noexcept;
    virtual ~Shader() noexcept;

    void setType(ShaderType type) noexcept;
    void setType(const std::string& type) noexcept;
    void setType(const std::wstring& type) noexcept;

    void setSource(const std::string& source) noexcept;
    void setIncludePath(const std::string& include) noexcept;

    ShaderType  getType()   const noexcept;
    const std::string& getSource() const noexcept;
    const std::string& getIncludePath() const noexcept;

private:
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

private:

    ShaderType _type;
    std::string _source;
    std::string _path;
};

class EXPORT ShaderProgram
{
public:
    ShaderProgram() noexcept;
    virtual ~ShaderProgram() noexcept;

    void addShader(ShaderPtr shader) noexcept;
    void removeShader(ShaderPtr shader) noexcept;

    Shaders& getShaders() noexcept;

    ShaderUniforms&      getActiveUniforms() noexcept;
    ShaderAttributes&    getActiveAttributes() noexcept;
    ShaderSubroutines&   getActiveSubroutines() noexcept;

protected:

    Shaders _shaders;

    ShaderUniforms      _activeUniforms;
    ShaderAttributes    _activeAttributes;
    ShaderSubroutines   _activeSubroutines;
};

class EXPORT ShaderObject final
{
public:
    ShaderObject() noexcept;
    ~ShaderObject() noexcept;

    bool setup() noexcept;
    void close() noexcept;

    void addShader(ShaderPtr shader) noexcept;
    void removeShader(ShaderPtr shader) noexcept;

    ShaderProgramPtr getShaderProgram() noexcept;

    Shaders& getShaders() noexcept;
    ShaderAttributes&    getActiveAttributes() noexcept;
    ShaderUniforms&      getActiveUniforms() noexcept;
    ShaderSubroutines&   getActiveSubroutines() noexcept;

private:

    Shaders _shaders;
    ShaderProgramPtr _program;
};

_NAME_END

#endif