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
#ifndef _H_OGL_RENDERER_H_
#define _H_OGL_RENDERER_H_

#include <ray/ogl_state.h>
#include <ray/ogl_commandlist.h>

_NAME_BEGIN

class EXPORT OGLRenderer final : public RenderDevice
{
public:
    OGLRenderer() noexcept;
    ~OGLRenderer() noexcept;

    virtual bool open(RenderWindowPtr window) except;
    virtual void close() noexcept;

    virtual void renderBegin() noexcept;
    virtual void renderEnd() noexcept;

    virtual void clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil) noexcept;
    virtual void clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;

    virtual void setViewport(const Viewport& view, std::size_t i = 0) noexcept;
    virtual const Viewport& getViewport(std::size_t i = 0) const noexcept;

    virtual void setRenderWindow(RenderWindowPtr window) noexcept;
    virtual RenderWindowPtr getRenderWindow() const noexcept;

    virtual void setSwapInterval(SwapInterval interval) noexcept;
    virtual SwapInterval getSwapInterval() const noexcept;

    virtual void setRenderState(RenderStatePtr state) noexcept;
    virtual RenderStatePtr getRenderState() const noexcept;

    virtual void setRenderBuffer(RenderBufferPtr buffer) noexcept;
    virtual void updateRenderBuffer(RenderBufferPtr buffer) noexcept;
    virtual void drawRenderBuffer(const Renderable& renderable) noexcept;
    virtual RenderBufferPtr getRenderBuffer() const noexcept;

    virtual void setRenderTarget(RenderTargetPtr target) noexcept;
    virtual void setMultiRenderTarget(MultiRenderTargetPtr target) noexcept;
    virtual void readRenderTarget(RenderTargetPtr src, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
    virtual void copyRenderTarget(RenderTargetPtr src, const Viewport& v1, RenderTargetPtr dest, const Viewport& v2) noexcept;
    virtual RenderTargetPtr getRenderTarget() const noexcept;
    virtual MultiRenderTargetPtr getMultiRenderTarget() const noexcept;

    virtual void setShaderObject(ShaderObjectPtr shader) noexcept;
    virtual void setShaderUniform(ShaderUniformPtr uniform, TexturePtr texture) noexcept;
    virtual void setShaderUniform(ShaderUniformPtr uniform, ShaderVariantPtr constant) noexcept;
    virtual ShaderObjectPtr getShaderObject() const noexcept;

    virtual bool createShaderVariant(ShaderVariant& constant) noexcept;
    virtual void destroyShaderVariant(ShaderVariant& constant) noexcept;
    virtual void updateShaderVariant(ShaderVariantPtr constant) noexcept;

private:
    void initCommandList() noexcept;
    void initDebugControl() noexcept;
    void initStateSystem() noexcept;

    void nvtokenGetStats(const void* stream, size_t streamSize, int stats[GL_MAX_COMMANDS_NV]) noexcept;
    void nvtokenDrawCommandState() noexcept;
    GLenum nvtokenDrawCommandSequence(const void* stream, size_t streamSize, GLenum mode, GLenum type, const State* state) noexcept;

    static void checkError() noexcept;
    static void checkFramebuffer() noexcept;
    static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

private:
    OGLRenderer(const OGLRenderer&) noexcept = delete;
    OGLRenderer& operator=(const OGLRenderer&) noexcept = delete;

private:

    RenderWindowPtr _glcontext;

    GLint _maxViewports;
    std::vector<Viewport> _viewport;

    GLuint _framebuffer;
    RenderTargetPtr _renderTarget;
    MultiRenderTargetPtr _multiRenderTarget;

    GLuint _program;
    ShaderObjectPtr _shaderObject;
    std::vector<OGLShaderVariant> _constantBuffers;

    GLuint _defaultVAO;
    RenderBufferPtr _renderBuffer;

    GLint _maxTextureUnits;
    std::vector<GLint> _textureUnits;

    // we introduce variables that track when we changed global state
    RenderState::InstanceID _state;
    RenderState::InstanceID _captured;

    RenderBlendState _blendState;
    RenderRasterState _rasterState;
    RenderDepthState _depthState;
    RenderStencilState _stencilState;
    RenderClearState _clearState;

    StateSystem _stateSystem;
    StateSystem::StateID  _stateIdDraw;
    StateSystem::StateID  _stateIdDrawGeo;

    // two state objects
    GLuint _stateObjDraw;
    GLuint _stateObjDrawGeo;

    GLuint          _tokenBuffer;
    GLuint          _tokenCmdList;
    std::string     _tokenData;
    CommandSequence _tokenSequence;
    CommandSequence _tokenSequenceList;
    CommandSequence _tokenSequenceEmu;
};

_NAME_END

#endif