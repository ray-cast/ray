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
    __DeclareSubClass(OGLRenderer, RenderDevice)
public:
    OGLRenderer() noexcept;
    ~OGLRenderer() noexcept;

    virtual bool open(const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) noexcept;
    virtual void close() noexcept;

    virtual void clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil) noexcept;

    virtual void present(RenderCanvasPtr canvas) noexcept;

    virtual void setViewport(const Viewport& view) noexcept;

    virtual void setSwapInterval(SwapInterval interval) noexcept;
    virtual SwapInterval getSwapInterval() const noexcept;

    virtual void setBlendState(const RenderBlendState& state) noexcept;
    virtual void setRasterState(const RenderRasterState& state) noexcept;
    virtual void setDepthState(const RenderDepthState& state) noexcept;
    virtual void setStencilState(const RenderStencilState& state) noexcept;
    virtual void setRenderState(RenderStatePtr state) noexcept;

    virtual void renderBegin() noexcept;
    virtual void renderEnd() noexcept;

    virtual RenderCanvasPtr createRenderCanvas(WindHandle hwnd) noexcept;
    virtual void destroyRenderCanvas(RenderCanvasPtr canvas) noexcept;
    virtual void setRenderCanvas(RenderCanvasPtr handle) noexcept;

    virtual bool createConstantBuffer(ShaderConstantBuffer& buffer) noexcept;
    virtual void destroyConstantBuffer(ShaderConstantBuffer& buffer) noexcept;
    virtual void setShaderConstantBuffer(ShaderConstantBufferPtr buffer) noexcept;

    virtual bool createRenderBuffer(RenderBuffer& buffer) noexcept;
    virtual void destroyRenderBuffer(RenderBuffer& buffer) noexcept;
    virtual void setRenderBuffer(RenderBufferPtr buffer) noexcept;
    virtual void updateRenderBuffer(RenderBufferPtr buffer) noexcept;
    virtual void drawRenderBuffer(const Renderable& renderable) noexcept;

    virtual FramebufferPtr createFramebuffer(const FramebufferDesc& target) noexcept;
    virtual void destroyFramebuffer(FramebufferPtr target) noexcept;
    virtual void setFramebuffer(FramebufferPtr handle) noexcept;
    virtual void copyFramebuffer(FramebufferPtr srcTarget, const Viewport& src, FramebufferPtr destTarget, const Viewport& dest) noexcept;
    virtual void readFramebuffer(FramebufferPtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;

    virtual MultiFramebufferPtr createMultiFramebuffer(const MultiFramebufferDesc& desc) noexcept;
    virtual void destroyMultiFramebuffer(MultiFramebufferPtr target) noexcept;
    virtual void setMultiFramebuffer(MultiFramebufferPtr target) noexcept;

    virtual bool createTexture(Texture& texture) noexcept;
    virtual void destroyTexture(Texture& texture) noexcept;
    virtual void setTexture(const Texture& value, ShaderUniformPtr uniform) noexcept;

    virtual ShaderProgramPtr createShaderProgram(std::vector<ShaderPtr>& shaders) noexcept;
    virtual void destroyShaderProgram(ShaderProgramPtr shader) noexcept;
    virtual void setShaderProgram(ShaderProgramPtr shader) noexcept;

private:
    void initCommandList() noexcept;
    void initInternals(bool hwsupport, bool bindlessSupport) noexcept;

    void nvtokenCommandToString(GLenum type, char*& string) noexcept;
    void nvtokenGetStats(const void* stream, size_t streamSize, int stats[GL_MAX_COMMANDS_NV]) noexcept;
    void nvtokenDrawCommandState() noexcept;
    GLenum nvtokenDrawCommandSequence(const void* stream, size_t streamSize, GLenum mode, GLenum type, const StateSystem::State* state) noexcept;

    static void checkError() noexcept;
    static void checkFramebuffer() noexcept;
    static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

    static void applyTextureWrap(GLenum, TextureWrap wrap) noexcept;
    static void applyTextureFilter(GLenum target, TextureFilter filter) noexcept;
    static void applyTextureAnis(GLenum target, Anisotropy anis) noexcept;

    static void generateTexture(GLenum target, GLint internalformat, GLenum format, GLuint type, GLsizei w, GLsizei h, GLsizei depth, const char* buf) noexcept;

private:
    OGLRenderer(const OGLRenderer&) noexcept = delete;
    OGLRenderer& operator=(const OGLRenderer&) noexcept = delete;

private:

    GPUfbconfig _fbconfig;
    GPUctxconfig _ctxconfig;

    RenderBlendState _blendState;
    RenderRasterState _rasterState;
    RenderDepthState _depthState;
    RenderStencilState _stencilState;

    Viewport _viewport;

    SwapInterval _interval;

    RenderCanvasPtr _renderCanvas;
    FramebufferPtr _renderTarget;
    MultiFramebufferPtr _multiFramebuffer;
    RenderStatePtr _state;

    ShaderProgramPtr _shader;
    std::vector<OGLConstantBuffer> _constantBuffers;

    GLuint _defaultVAO;
    RenderBufferPtr _renderBuffer;
    std::vector<OGLVertexArray> _renderBuffers;

    GLint _maxTextures;
    GLint _maxTextureUnits;
    GLint _unpackAlignment;
    std::vector<GLuint> _textureUnits;
    std::vector<OGLTexture> _textures;

    // we introduce variables that track when we changed global state
    StateIncarnation  state;
    StateIncarnation  captured;

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