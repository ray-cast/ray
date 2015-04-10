// +---------------------------------------------------------------------
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
#include <ray/ogl_renderer.h>
#include <ray/ogl_target.h>
#include <ray/ogl_buffer.h>
#include <ray/ogl_shader.h>

_NAME_BEGIN

__ImplementSubClass(OGLRenderer, RenderDevice)

OGLRenderer::OGLRenderer() noexcept
    :_viewport(0, 0, 0, 0)
    , _maxTextures(100)
    , _maxTextureUnits(0)
{
    _blendState.blendEnable = false;
    _blendState.blendSeparateEnable = false;
    _blendState.blendOp = GPU_ADD;
    _blendState.blendSrc = GPU_ZERO;
    _blendState.blendDest = GPU_ZERO;
    _blendState.blendAlphaOp = GPU_ADD;
    _blendState.blendAlphaSrc = GPU_ZERO;
    _blendState.blendAlphaDest = GPU_ZERO;
    _blendState.colorWriteMask = GPU_COLORMASK_RGBA;
    _depthState.depthEnable = false;
    _depthState.depthWriteMask = true;
    _depthState.depthFunc = GPU_ALWAYS;
    _rasterState.cullMode = GPU_CULL_NONE;
    _rasterState.depthBias = 0;
    _rasterState.depthBiasEnable = false;
    _rasterState.slopScaleDepthBias = 0;
    _rasterState.fillMode = GPU_SOLID_MODE;
    _rasterState.multisampleEnable = false;
    _rasterState.scissorTestEnable = false;
    _stencilState.stencilEnable = false;
    _stencilState.stencilFunc = GPU_ALWAYS;
    _stencilState.stencilFail = STENCILOP_ZERO;
    _stencilState.stencilZFail = STENCILOP_ZERO;
    _stencilState.stencilPass = STENCILOP_ZERO;
    _stencilState.stencilTwoEnable = false;

    _textures.resize(_maxTextures);
}

OGLRenderer::~OGLRenderer() noexcept
{
    this->close();
}

bool
OGLRenderer::open(const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) noexcept
{
    if ((ctxconfig.major < 1 || ctxconfig.minor < 0) ||
        (ctxconfig.major == 1 && ctxconfig.minor > 5) ||
        (ctxconfig.major == 2 && ctxconfig.minor > 1) ||
        (ctxconfig.major == 3 && ctxconfig.minor > 3))
    {
        return false;
    }

    if (ctxconfig.profile)
    {
        if (ctxconfig.profile != GPU_GL_CORE_PROFILE &&
            ctxconfig.profile != GPU_GL_COMPAT_PROFILE)
        {
            return false;
        }

        if (ctxconfig.major < 3 || (ctxconfig.major == 3 && ctxconfig.minor < 2))
        {
            return false;
        }
    }

    if (ctxconfig.forward && ctxconfig.major < 3)
    {
        return false;
    }

    if (ctxconfig.robustness)
    {
        return false;
    }

    if (ctxconfig.release)
    {
        return false;
    }

    _fbconfig = fbconfig;
    _ctxconfig = ctxconfig;

    return true;
}

void
OGLRenderer::close() noexcept
{
}

void
OGLRenderer::clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil) noexcept
{
    GLbitfield mode = 0;

    if (flags & ClearFlags::CLEAR_COLOR)
    {
        mode |= GL_COLOR_BUFFER_BIT;
    }

    if (flags & ClearFlags::CLEAR_DEPTH)
    {
        mode |= GL_DEPTH_BUFFER_BIT;
    }

    if (flags & ClearFlags::CLEAR_STENCIL)
    {
        mode |= GL_STENCIL_BUFFER_BIT;
    }

    if (0 != mode)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClearDepthf(depth);
        glClearStencil(stencil);
        glClear(mode);
    }
}

void
OGLRenderer::setBlendState(const RenderBlendState& blendState) noexcept
{
    if (blendState.blendEnable)
    {
        if (!_blendState.blendEnable)
        {
            glEnable(GL_BLEND);
            _blendState.blendEnable = true;
        }

        if (blendState.blendSeparateEnable)
        {
            if (_blendState.blendSrc != blendState.blendSrc ||
                _blendState.blendDest != blendState.blendDest ||
                _blendState.blendAlphaSrc != blendState.blendAlphaSrc ||
                _blendState.blendAlphaDest != blendState.blendAlphaDest)
            {
                GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
                GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);
                GLenum sfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaSrc);
                GLenum dfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaDest);

                glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

                _blendState.blendSrc = blendState.blendSrc;
                _blendState.blendDest = blendState.blendDest;
                _blendState.blendAlphaSrc = blendState.blendAlphaSrc;
                _blendState.blendAlphaDest = blendState.blendAlphaDest;
            }

            if (_blendState.blendOp != blendState.blendOp ||
                _blendState.blendAlphaOp != blendState.blendAlphaOp)
            {
                GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
                GLenum modeAlpha = OGLTypes::asBlendOperation(blendState.blendAlphaOp);

                glBlendEquationSeparate(modeRGB, modeAlpha);

                _blendState.blendOp = blendState.blendOp;
                _blendState.blendAlphaOp = blendState.blendAlphaOp;
            }
        }
        else
        {
            if (_blendState.blendSrc != blendState.blendSrc ||
                _blendState.blendDest != blendState.blendDest)
            {
                GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
                GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);

                glBlendFunc(sfactorRGB, dfactorRGB);

                _blendState.blendSrc = blendState.blendSrc;
                _blendState.blendDest = blendState.blendDest;
            }

            if (_blendState.blendOp != blendState.blendOp)
            {
                GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
                glBlendEquation(modeRGB);

                _blendState.blendOp = blendState.blendOp;
            }
        }
    }
    else
    {
        if (_blendState.blendEnable)
        {
            glDisable(GL_BLEND);
            _blendState.blendEnable = false;
        }
    }
}

void
OGLRenderer::setRasterState(const RenderRasterState& rasterState) noexcept
{
    if (_rasterState.cullMode != rasterState.cullMode)
    {
        if (rasterState.cullMode != GPU_CULL_NONE)
        {
            GLenum mode = OGLTypes::asCullMode(rasterState.cullMode);
            glEnable(GL_CULL_FACE);
            glCullFace(mode);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    if (rasterState.depthBiasEnable)
    {
#if !defined(EGLAPI)
        if (!_rasterState.depthBiasEnable)
            glEnable(GL_POLYGON_OFFSET_EXT);
#endif

        if (_rasterState.depthBias != rasterState.depthBias ||
            _rasterState.slopScaleDepthBias != rasterState.slopScaleDepthBias)
        {
            glPolygonOffset(rasterState.slopScaleDepthBias, rasterState.depthBias);
        }
    }
    else
    {
#if !defined(EGLAPI)
        if (_rasterState.depthBiasEnable)
            glDisable(GL_POLYGON_OFFSET_EXT);
#endif
    }

#if !defined(EGLAPI)
    if (_rasterState.fillMode != rasterState.fillMode)
    {
        GLenum mode = OGLTypes::asFillMode(rasterState.fillMode);
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
#endif

    if (_rasterState.scissorTestEnable != rasterState.scissorTestEnable)
    {
        if (rasterState.scissorTestEnable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    _rasterState = rasterState;
}

void
OGLRenderer::setDepthState(const RenderDepthState& depthState) noexcept
{
    if (depthState.depthEnable)
    {
        if (!_depthState.depthEnable)
        {
            glEnable(GL_DEPTH_TEST);
            _depthState.depthEnable = true;
        }

        if (_depthState.depthFunc != depthState.depthFunc)
        {
            GLenum func = OGLTypes::asCompareFunction(depthState.depthFunc);
            glDepthFunc(func);
            _depthState.depthFunc = depthState.depthFunc;
        }
    }
    else
    {
        if (_depthState.depthEnable)
        {
            glDisable(GL_DEPTH_TEST);
            _depthState.depthEnable = false;
        }
    }

    if (_depthState.depthWriteMask != depthState.depthWriteMask)
    {
        GLboolean enable = depthState.depthWriteMask ? GL_TRUE : GL_FALSE;
        glDepthMask(enable);
        _depthState.depthWriteMask = depthState.depthWriteMask;
    }
}

void
OGLRenderer::setStencilState(const RenderStencilState& stencilState) noexcept
{
    if (stencilState.stencilEnable)
    {
        if (!_stencilState.stencilEnable)
        {
            glEnable(GL_STENCIL_TEST);
            _stencilState.stencilEnable = true;
        }

        if (stencilState.stencilTwoEnable)
        {
            _stencilState.stencilTwoEnable = true;

            if (_stencilState.stencilFunc != stencilState.stencilFunc ||
                _stencilState.stencilRef != stencilState.stencilRef ||
                _stencilState.stencilReadMask != stencilState.stencilReadMask)
            {
                GLenum frontfunc = OGLTypes::asCompareFunction(stencilState.stencilFunc);
                glStencilFuncSeparate(GL_FRONT, frontfunc, stencilState.stencilRef, stencilState.stencilReadMask);

                GLenum backfunc = OGLTypes::asCompareFunction(stencilState.stencilTwoFunc);
                glStencilFuncSeparate(GL_BACK, backfunc, stencilState.stencilRef, stencilState.stencilTwoReadMask);

                _stencilState.stencilRef = stencilState.stencilRef;
                _stencilState.stencilReadMask = stencilState.stencilReadMask;
                _stencilState.stencilTwoFunc = stencilState.stencilTwoFunc;
            }

            if (_stencilState.stencilFail != _stencilState.stencilFail ||
                _stencilState.stencilZFail != _stencilState.stencilZFail ||
                _stencilState.stencilPass != _stencilState.stencilPass)
            {
                GLenum frontfail = OGLTypes::asStencilOperation(_stencilState.stencilFail);
                GLenum frontzfail = OGLTypes::asStencilOperation(_stencilState.stencilZFail);
                GLenum frontpass = OGLTypes::asStencilOperation(_stencilState.stencilPass);
                glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

                GLenum backfail = OGLTypes::asStencilOperation(_stencilState.stencilTwoFail);
                GLenum backzfail = OGLTypes::asStencilOperation(_stencilState.stencilTwoZFail);
                GLenum backpass = OGLTypes::asStencilOperation(_stencilState.stencilTwoPass);
                glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);

                _stencilState.stencilFail = stencilState.stencilFail;
                _stencilState.stencilZFail = stencilState.stencilZFail;
                _stencilState.stencilPass = stencilState.stencilPass;
                _stencilState.stencilTwoFail = stencilState.stencilTwoFail;
                _stencilState.stencilTwoZFail = stencilState.stencilTwoZFail;
                _stencilState.stencilTwoPass = stencilState.stencilTwoPass;
            }
        }
        else
        {
            if (_stencilState.stencilFunc != stencilState.stencilFunc ||
                _stencilState.stencilRef != stencilState.stencilRef ||
                _stencilState.stencilReadMask != stencilState.stencilReadMask)
            {
                GLenum func = OGLTypes::asCompareFunction(stencilState.stencilFunc);
                glStencilFunc(func, stencilState.stencilRef, stencilState.stencilReadMask);

                _stencilState.stencilFunc = stencilState.stencilFunc;
                _stencilState.stencilRef = stencilState.stencilRef;
                _stencilState.stencilReadMask = stencilState.stencilReadMask;
            }

            if (_stencilState.stencilFail != stencilState.stencilFail ||
                _stencilState.stencilZFail != stencilState.stencilZFail ||
                _stencilState.stencilPass != stencilState.stencilPass)
            {
                GLenum fail = OGLTypes::asStencilOperation(stencilState.stencilFail);
                GLenum zfail = OGLTypes::asStencilOperation(stencilState.stencilZFail);
                GLenum pass = OGLTypes::asStencilOperation(stencilState.stencilPass);
                glStencilOp(fail, zfail, pass);

                _stencilState.stencilFail = stencilState.stencilFail;
                _stencilState.stencilZFail = stencilState.stencilZFail;
                _stencilState.stencilPass = stencilState.stencilPass;
            }
        }
    }
    else
    {
        if (_stencilState.stencilEnable)
        {
            glDisable(GL_STENCIL_TEST);
            _stencilState.stencilEnable = false;
        }
    }
}

void
OGLRenderer::setRenderState(RenderStatePtr state) noexcept
{
    if (_state != state)
    {
        auto& blendState = state->getBlendState();
        auto& rasterState = state->getRasterState();
        auto& depthState = state->getDepthState();
        auto& stencilState = state->getStencilState();

        this->setBlendState(blendState);
        this->setRasterState(rasterState);
        this->setDepthState(depthState);
        this->setStencilState(stencilState);

        _state = state;
    }
}

void
OGLRenderer::renderBegin() noexcept
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void
OGLRenderer::renderEnd() noexcept
{
}

void
OGLRenderer::present(RenderCanvasPtr canvas) noexcept
{
    canvas->present();
}

void
OGLRenderer::setViewport(const Viewport& view) noexcept
{
    if (_viewport != view)
    {
        glViewport(view.left, view.top, view.width, view.height);
        _viewport = view;
    }
}

void
OGLRenderer::setSwapInterval(SwapInterval interval) noexcept
{
    if (_interval != interval)
    {
        auto oglRenderCanvas = std::dynamic_pointer_cast<OGLCanvas>(_renderCanvas);
        if (oglRenderCanvas)
        {
            oglRenderCanvas->setSwapInterval(interval);
        }

        _interval = interval;
    }
}

SwapInterval
OGLRenderer::getSwapInterval() const noexcept
{
    return _interval;
}

RenderCanvasPtr
OGLRenderer::createRenderCanvas(WindHandle hwnd) noexcept
{
    auto canvas = std::make_shared<OGLCanvas>();
    canvas->setup(hwnd, _fbconfig, _ctxconfig);

    if (_ctxconfig.debug)
    {
        // 131184 memory info
        // 131185 memory allocation info
        GLuint ids[] =
        {
            131184,
            131185,
            131076,
            131169
        };

#if !defined(EGLAPI)
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallBack, nullptr);

        // enable all
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
        // disable 131184, 131185
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 4, ids, GL_FALSE);
#else
        glEnable(GL_DEBUG_OUTPUT_KHR);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
        //glDebugMessageCallbackKHR(debugCallBack, nullptr);

        // enable all
        glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
        // disable 131184, 131185
        glDebugMessageControlKHR(GL_DEBUG_SOURCE_API_KHR, GL_DEBUG_TYPE_OTHER_KHR, GL_DONT_CARE, 3, ids, GL_FALSE);
#endif
    }

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &_unpackAlignment);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);
    if (_textureUnits.size() != _maxTextureUnits)
    {
        _textureUnits.resize(_maxTextureUnits);
    }

    _renderCanvas = canvas;
    return canvas;
}

void
OGLRenderer::destroyRenderCanvas(RenderCanvasPtr canvas) noexcept
{
    auto oglCanvas = std::dynamic_pointer_cast<OGLCanvas>(canvas);
    if (oglCanvas)
    {
        oglCanvas->close();

        if (_renderCanvas == canvas)
            _renderCanvas = nullptr;
    }
}

void
OGLRenderer::setRenderCanvas(RenderCanvasPtr canvas) noexcept
{
    if (_renderCanvas != canvas)
    {
        auto wglCanvas = std::dynamic_pointer_cast<OGLCanvas>(canvas);
        if (wglCanvas)
        {
            wglCanvas->bind();
        }
        else
        {
            if (_renderCanvas)
            {
                std::dynamic_pointer_cast<OGLCanvas>(_renderCanvas)->unbind();
            }
        }

        _renderCanvas = canvas;
    }
}

RenderBufferPtr
OGLRenderer::createRenderBuffer(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    OGLVertexArrayPtr result;

    result = std::make_shared<OGLVertexArray>();
    result->setup(vb, ib);

    auto oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(_renderBuffer);
    if (oglVertexArray)
    {
        oglVertexArray->bind();
    }

    return result;
}

void
OGLRenderer::destroyRenderBuffer(RenderBufferPtr buffer) noexcept
{
    auto oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(buffer);
    if (oglVertexArray)
    {
        oglVertexArray->close();

        if (_renderBuffer == buffer)
            _renderBuffer = nullptr;
    }
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
    if (_renderBuffer != buffer)
    {
        auto oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(buffer);
        if (oglVertexArray)
        {
            oglVertexArray->bind();
        }
        else
        {
            if (_renderBuffer)
            {
                std::dynamic_pointer_cast<OGLVertexArray>(_renderBuffer)->unbind();
            }
        }

        _renderBuffer = buffer;
    }
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr buffer, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    auto oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(buffer);
    if (oglVertexArray)
    {
        oglVertexArray->update(vb, ib);
        oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(_renderBuffer);
        if (oglVertexArray)
        {
            oglVertexArray->bind();
        }
    }
}

void
OGLRenderer::drawRenderBuffer(const Renderable& renderable) noexcept
{
    assert(_renderBuffer);

    auto array = std::dynamic_pointer_cast<OGLVertexArray>(_renderBuffer);

    auto vb = array->getVertexBuffer();
    auto ib = array->getIndexBuffer();

    assert(vb && vb->getVertexCount() >= renderable.startVertice + renderable.numVertices);

    GLenum drawType = OGLTypes::asOGLVertexType(renderable.type);

    if (ib)
    {
        assert(ib->getIndexCount() >= renderable.startIndice + renderable.numIndices);

        GLenum indexType = ib->getIndexType();

#if !defined(EGLAPI)
        if (renderable.numInstances > 0)
            glDrawElementsInstancedBaseVertex(drawType, renderable.numIndices, indexType, (char*)nullptr + (renderable.startIndice * renderable.numIndices), renderable.numInstances, renderable.startVertice);
        else
            glDrawElementsBaseVertex(drawType, renderable.numIndices, indexType, (char*)(nullptr) + (renderable.startIndice * renderable.numIndices), renderable.startVertice);
#else
        assert(renderable.startVertice == 0);
        glDrawElements(drawType, numIndice, indexType, (char*)(nullptr) + (startIndice * numIndice));
#endif
    }
    else
    {
        if (renderable.numInstances > 0)
            glDrawArraysInstanced(drawType, renderable.startVertice, renderable.numVertices, renderable.numIndices);
        else
            glDrawArrays(drawType, renderable.startVertice, renderable.numVertices);
    }
}

FramebufferPtr
OGLRenderer::createFramebuffer(const FramebufferDesc& rt) noexcept
{
    OGLFramebufferPtr result;

    result = std::make_shared<OGLFramebuffer>();
    result->setup(rt);

    _renderTarget = nullptr;
    _multiFramebuffer = nullptr;

    return result;
}

void
OGLRenderer::destroyFramebuffer(FramebufferPtr target) noexcept
{
    auto oglFramebuffer = std::dynamic_pointer_cast<OGLFramebuffer>(target);
    if (oglFramebuffer)
    {
        oglFramebuffer->close();

        if (_renderTarget == target)
            _renderTarget = nullptr;
    }
}

void
OGLRenderer::setFramebuffer(FramebufferPtr target) noexcept
{
    if (_renderTarget != target)
    {
        if (target)
        {
            std::dynamic_pointer_cast<OGLFramebuffer>(target)->bind();
            this->clear(target->getClearFlags(), target->getClearColor(), target->getClearDepth(), target->getClearStencil());
            this->setViewport(target->getViewport());
        }
        else
        {
            if (_renderTarget)
            {
                std::dynamic_pointer_cast<OGLFramebuffer>(_renderTarget)->unbind();
            }
        }

        _renderTarget = target;
        _multiFramebuffer = nullptr;
    }
}

void
OGLRenderer::copyFramebuffer(FramebufferPtr srcRT, const Viewport& src, FramebufferPtr destRT, const Viewport& dest) noexcept
{
    auto srcTarget = std::dynamic_pointer_cast<OGLFramebuffer>(srcRT);
    if (srcTarget)
    {
        srcTarget->bitblit(src, destRT, dest);

        _renderTarget = nullptr;
        _multiFramebuffer = nullptr;
    }
}

void
OGLRenderer::readFramebuffer(FramebufferPtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    assert(target && w && h && data);

    this->setFramebuffer(target);

    GLenum format = OGLTypes::asOGLFormat(pfd);
    GLenum type = OGLTypes::asOGLType(pfd);

    glReadPixels(0, 0, w, h, format, type, data);

    _renderTarget = target;
    _multiFramebuffer = nullptr;
}

MultiFramebufferPtr
OGLRenderer::createMultiFramebuffer(const MultiFramebufferDesc& desc) noexcept
{
    OGLMultiFramebufferPtr result;

    result = std::make_shared<OGLMultiFramebuffer>();
    result->setup(desc);

    _renderTarget = nullptr;
    _multiFramebuffer = nullptr;

    return result;
}

void
OGLRenderer::destroyMultiFramebuffer(MultiFramebufferPtr target) noexcept
{
    auto oglMultiFramebuffer = std::dynamic_pointer_cast<OGLMultiFramebuffer>(target);
    if (oglMultiFramebuffer)
    {
        oglMultiFramebuffer->close();

        if (_multiFramebuffer = target)
            _multiFramebuffer = nullptr;
    }
}

void
OGLRenderer::setMultiFramebuffer(MultiFramebufferPtr target) noexcept
{
    if (_multiFramebuffer != target)
    {
        auto oglTarget = std::dynamic_pointer_cast<OGLMultiFramebuffer>(target);
        if (oglTarget)
        {
            oglTarget->bind();

            ClearFlags flags = target->getClearFlags();

            auto& drawbuffers = oglTarget->getDrawBuffers();

            for (auto& it : drawbuffers)
            {
                if (it.second == GL_DEPTH_ATTACHMENT)
                {
                    if (flags & ClearFlags::CLEAR_DEPTH)
                    {
                        GLfloat f = it.first->getClearDepth();
                        glClearBufferfv(GL_DEPTH, 0, &f);
                    }
                }
                else if (it.second == GL_STENCIL_ATTACHMENT)
                {
                    if (flags & ClearFlags::CLEAR_STENCIL)
                    {
                        GLint s = it.first->getClearStencil();
                        glClearBufferiv(GL_STENCIL, 0, &s);
                    }
                }
                else if (it.second == GL_DEPTH_STENCIL_ATTACHMENT)
                {
                    if (flags & ClearFlags::CLEAR_DEPTH_STENCIL)
                    {
                        GLfloat f = it.first->getClearDepth();
                        GLint s = it.first->getClearStencil();
                        glClearBufferfi(GL_DEPTH_STENCIL, 0, f, s);
                    }
                }
                else
                {
                    if (flags & ClearFlags::CLEAR_COLOR)
                    {
                        glClearBufferfv(GL_COLOR, it.second - GL_COLOR_ATTACHMENT0, it.first->getClearColor().ptr());
                    }
                }
            }

            this->setViewport(target->getViewport());
        }
        else
        {
            if (_multiFramebuffer)
            {
                std::dynamic_pointer_cast<OGLMultiFramebuffer>(target)->unbind();
            }
        }

        _multiFramebuffer = target;
        _renderTarget = 0;
    }
}

bool
OGLRenderer::createTexture(Texture& texture) noexcept
{
    OGLTexture instance;

    auto target = OGLTypes::asOGLTarget(texture.getTexDim());
    auto format = OGLTypes::asOGLFormat(texture.getTexFormat());
    auto type = OGLTypes::asOGLType(texture.getTexFormat());
    auto internalFormat = OGLTypes::asOGLInternalformat(texture.getTexFormat());

    glGenTextures(1, &instance.texture);
    glBindTexture(target, instance.texture);

    // set unpack alignment to one byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLsizei w = (GLsizei)texture.getWidth();
    GLsizei h = (GLsizei)texture.getHeight();
    GLsizei depth = (GLsizei)texture.getDepth();

    generateTexture(target, internalFormat, format, type, w, h, depth, (const char*)texture.getStream());

    // restore old unpack alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, _unpackAlignment);

    applyTextureWrap(target, texture.getTexWrap());
    applyTextureFilter(target, texture.getTexFilter());
    applyTextureAnis(target, texture.getTexAnisotropy());

    texture.setUserData(instance.texture);

    if (_ARB_bindless_texture)
    {
        instance.resident = glGetTextureHandleARB(instance.texture);
        glMakeTextureHandleResidentARB(instance.resident);
    }

    _textures[instance.texture] = instance;

    return true;
}

void
OGLRenderer::destroyTexture(Texture& texture) noexcept
{
    GLuint instance = texture.getUserData();
    if (instance)
    {
        glDeleteTextures(1, &instance);
        for (auto& it : _textureUnits)
        {
            if (it == instance)
                it = 0;
        }

        _textures[instance].texture = 0;
        _textures[instance].resident = 0;
    }

    texture.setUserData(0);
}

ShaderProgramPtr
OGLRenderer::createShaderProgram(std::vector<ShaderPtr>& shaders) noexcept
{
    OGLShaderProgramPtr result;

    result = std::make_shared<OGLShaderProgram>();
    for (auto& it : shaders)
    {
        result->addShader(it);
    }

    result->setup();

    return result;
}

void
OGLRenderer::destroyShaderProgram(ShaderProgramPtr shader) noexcept
{
    std::dynamic_pointer_cast<OGLShaderProgram>(shader)->close();
}

void
OGLRenderer::setShaderProgram(ShaderProgramPtr shader) noexcept
{
    if (_shader != shader)
    {
        auto oglShaderObject = std::dynamic_pointer_cast<OGLShaderProgram>(shader);
        if (shader)
        {
            if (shader)
            {
                oglShaderObject->bind();
            }
            else
            {
                if (_shader)
                {
                    std::dynamic_pointer_cast<OGLShaderProgram>(_shader)->unbind();
                }
            }
        }

        _shader = shader;
    }
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const int value) noexcept
{
    assert(uniform);
    glUniform1i(uniform->location, value);
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const int2& value) noexcept
{
    assert(uniform);
    glUniform2iv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const int3& value) noexcept
{
    assert(uniform);
    glUniform3iv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const int4& value) noexcept
{
    assert(uniform);
    glUniform4iv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const uint value) noexcept
{
    assert(uniform);
    glUniform1ui(uniform->location, value);
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const uint2& value) noexcept
{
    assert(uniform);
    glUniform2uiv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const uint3& value) noexcept
{
    assert(uniform);
    glUniform3uiv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const uint4& value) noexcept
{
    assert(uniform);
    glUniform4uiv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float value) noexcept
{
    assert(uniform);
    glUniform1f(uniform->location, value);
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float2& value) noexcept
{
    assert(uniform);
    glUniform2fv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float3& value) noexcept
{
    assert(uniform);
    glUniform3fv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float4& value) noexcept
{
    assert(uniform);
    glUniform4fv(uniform->location, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float2x2& value) noexcept
{
    assert(uniform);
    glUniformMatrix2fv(uniform->location, GL_FALSE, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float3x3& value) noexcept
{
    assert(uniform);
    glUniformMatrix3fv(uniform->location, GL_FALSE, 1, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const float4x4& value) noexcept
{
    assert(uniform);
    glUniformMatrix4fv(uniform->location, 1, GL_FALSE, value.ptr());
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const Texture& texture) noexcept
{
    assert(uniform);
    assert(uniform->unit < _maxTextureUnits);
    assert(texture.getUserData() != 0);

    auto location = uniform->location;
    auto unit = uniform->unit;
    auto target = OGLTypes::asOGLTarget(texture.getTexDim());
    auto instance = texture.getUserData();
    auto resident = _textures[instance].resident;

#if !defined(EGLAPI)
    if (_ARB_bindless_texture)
    {
        glUniformHandleui64ARB(location, _textures[instance].resident);
    }
    else if (_EXT_direct_state_access)
    {
        glUniform1i(location, unit);
        glBindMultiTextureEXT(GL_TEXTURE0 + uniform->unit, target, instance);
    }
    else
#endif
    {
        glUniform1i(location, unit);

        if (_textureUnits[location] != instance)
        {
            glActiveTexture(GL_TEXTURE0 + uniform->unit);
            glBindTexture(target, instance);

            _textureUnits[location] = instance;
        }
    }
}

void
OGLRenderer::setShaderConstant(ShaderUniformPtr uniform, const std::vector<float>& value) noexcept
{
    assert(uniform);
    glUniform1fv(uniform->location, value.size(), value.data());
}

void
OGLRenderer::generateTexture(GLenum target, GLint internalformat, GLenum format, GLuint type, GLsizei w, GLsizei h, GLsizei depth, const char* buf) noexcept
{
    GLint level = 0;

    switch (target)
    {
    case GL_TEXTURE_2D:
    {
        if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
            format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
            format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
        {
            int size = w * h;
            glCompressedTexImage2D(target, level, internalformat, w, h, 0, size, buf);
        }
        else
        {
            glTexImage2D(target, level, internalformat, w, h, 0, format, type, buf);
        }
    }
    break;
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_3D:
    {
        glTexImage3D(target, level, internalformat, w, h, depth, 0, format, type, 0);
    }
    break;
    case GL_TEXTURE_CUBE_MAP:
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalformat, w, h, 0, format, type, buf);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalformat, w, h, 0, format, type, buf);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalformat, w, h, 0, format, type, buf);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalformat, w, h, 0, format, type, buf);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalformat, w, h, 0, format, type, buf);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalformat, w, h, 0, format, type, buf);
    }
    break;
    default:
        break;
    }
}

void
OGLRenderer::applyTextureWrap(GLenum target, TextureWrap wrap) noexcept
{
    if (TextureWrap::REPEAT & wrap)
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }
    else if (TextureWrap::CLAMP_TO_EDGE  & wrap)
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else if (TextureWrap::MODE_MIRROR & wrap)
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    }
}

void
OGLRenderer::applyTextureFilter(GLenum target, TextureFilter filter) noexcept
{
    if (filter == TextureFilter::GPU_NEAREST)
    {
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else if (filter == TextureFilter::GPU_LINEAR)
    {
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_LINEAR)
    {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_NEAREST)
    {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_LINEAR)
    {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
}

void
OGLRenderer::applyTextureAnis(GLenum target, Anisotropy anis) noexcept
{
    if (anis == Anisotropy::ANISOTROPY_1)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
    else if (anis == Anisotropy::ANISOTROPY_2)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
    else if (anis == Anisotropy::ANISOTROPY_4)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
    else if (anis == Anisotropy::ANISOTROPY_8)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
    else if (anis == Anisotropy::ANISOTROPY_16)
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
}

void
OGLRenderer::checkError() noexcept
{
#ifdef _DEBUG
    switch (glGetError())
    {
    case GL_NO_ERROR:
        break;
    case GL_INVALID_ENUM:
        std::cerr << "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        std::cerr << "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        std::cerr << "GL_INVALID_OPERATION";
        break;
#if !defined(EGLAPI)
    case GL_STACK_OVERFLOW:
        std::cerr << "GL_STACK_OVERFLOW";
        break;
#endif
    case GL_OUT_OF_MEMORY:
        std::cerr << "GL_OUT_OF_MEMORY";
        break;
    };
#endif
}

void
OGLRenderer::checkFramebuffer() noexcept
{
#ifdef _DEBUG
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE != result)
    {
        switch (result)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cerr << "FBO:Incomplete attachment";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cerr << "FBO:Incomplete missing attachment";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cerr << "FBO:Unsupported";
            break;
#if !defined(EGLAPI)
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cerr << "FBO:Incomplete draw buffer";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cerr << "FBO:Incomplete read buffer";
            break;
#endif
        }
    }
#endif
}

void
OGLRenderer::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
    std::cerr << "source : ";
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cerr << "GL_DEBUG_SOURCE_API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cerr << "GL_DEBUG_SOURCE_OTHER";
        break;
    }

    std::cerr << std::endl;

    std::cerr << "type : ";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "GL_DEBUG_TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "GL_DEBUG_TYPE_OTHER";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cerr << "GL_DEBUG_TYPE_MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
        break;
    }

    std::cerr << std::endl;

    std::cerr << "id : " << id << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cerr << "notice";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "low";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "medium";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "high";
        break;
    }

    std::cerr << std::endl;

    std::cerr << "message : " << message;
}

_NAME_END