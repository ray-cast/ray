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
#ifndef _H_RENDER_SYSTEM_H_
#define _H_RENDER_SYSTEM_H_

#include <ray/render_scene.h>
#include <ray/render_window.h>
#include <ray/render_pipeline.h>
#include <ray/timer.h>

_NAME_BEGIN

struct SimpleVertex
{
    SimpleVertex()
    {
    }

    SimpleVertex(const Vector3& v, const Vector4& col)
        :vertex(v)
        , color(col)
    {
    }

    Vector3 vertex;
    Vector4 color;
};

class EXPORT RenderSystem final
{
    __DeclareClass(RenderSystem)
public:
    RenderSystem() noexcept;
    ~RenderSystem() noexcept;

    bool setup(WindHandle win, std::size_t width, std::size_t height) except;
    void close() noexcept;

    void setSwapInterval(SwapInterval interval) noexcept;
    SwapInterval getSwapInterval() const noexcept;

    void setTimer(TimerPtr timer) noexcept;
    TimerPtr getTimer() const noexcept;

    bool addRenderScene(RenderScenePtr scene) noexcept;
    void removeRenderScene(RenderScenePtr scene) noexcept;

    void drawAABB(const Vector3& min, const Vector3& max, const Vector4& color) noexcept;
    void drawAABB(const Vector3& min, const Vector3& max, const Matrix4x4& trans, const Vector4& color) noexcept;

    void drawArc(float x, float y, float radius, float angle, float angle2, float segments) noexcept;
    void drawArcSolid(float x, float y, float radius, float angle, float angle2, float segments) noexcept;
    void drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radius, float minAngle, float maxAngle, float segments) noexcept;

    void drawCircle(float x, float  y, float radius, float segmenst) noexcept;
    void drawCircleSolid(float x, float  y, float radius, float segmenst) noexcept;

    void drawQuad(float x, float y, float w, float h, const Vector4& color) noexcept;
    void drawQuad(float dx, float dy, float dz, float x, float y, float z) noexcept;
    void drawQuad(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1 = 1, float t1 = 1) noexcept;
    void drawQuad(const Vector3& right, const Vector3& up, const Vector3& origin, int nFlipMode) noexcept;
    void drawQuad(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float ftx0, float fty0, float ftx1 = 1, float fty1 = 1) noexcept;

    void drawImage(float xpos, float ypos, float w, float h, int textureid, float s0, float t0, float s1, float t1, float r, float g, float b, float a) noexcept;
    void drawImage(float xpos, float ypos, float z, float w, float h, int textureid, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a) noexcept;
    void drawImageWithUV(float xpos, float ypos, float z, float w, float h, int texture_id, float *s, float *t, float r, float g, float b, float a) noexcept;

    void drawLine(const Vector3& pos1, const Vector3& pos2) noexcept;
    void drawLineColor(const Vector3& pos1, const Vector3& pos2, const Vector4& color) noexcept;
    void drawLineColor(const Vector3& pos1, const Vector4& color1, const Vector3 & pos2, const Vector4& color2) noexcept;
    void drawLines(const Vector3 v[], std::size_t num, const Vector4& col, int flags, float fGround) noexcept;

    void drawPoint(const Vector3& pt) noexcept;
    void drawPoint(const Vector3& pt, const Vector4& color) noexcept;
    void drawPoints(const Vector3 pt[], std::size_t num) noexcept;

    void drawText(const std::string& string) noexcept;

    void renderBegin() noexcept;
    void render() noexcept;
    void renderEnd() noexcept;

private:

    void applyTimer(TimerPtr timer) noexcept;
    void applyCamera(Camera* camera) noexcept;
    void applyEnvironment(const RenderScene& scene) noexcept;

    void renderCamera(Camera* camera) noexcept;

private:
    RenderSystem(const RenderSystem&) noexcept = delete;
    RenderSystem& operator=(const RenderSystem&) noexcept = delete;

private:

    TimerPtr _timer;

    Vector4 _globalColor;
    Matrix4x4 _orthoCamera;

    std::vector<SimpleVertex> _lines;
    std::vector<SimpleVertex> _polygons;

    bool _enableSSAO;
    bool _enableSAT;
    bool _enableSSR;
    bool _enableDOF;
    bool _enableHDR;
    bool _enableFXAA;

    MaterialPtr _lineMaterial;

    VertexBufferDataPtr _dynamicBuffers;
    RenderBufferPtr _renderBuffer;

    RenderDevicePtr _renderDevice;

    std::unique_ptr<RenderPipeline> _renderPipeline;

    std::vector<RenderScenePtr> _sceneList;
};

_NAME_END

#endif