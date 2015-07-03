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
#include <ray/ogl_shader.h>
#include <ray/ogl_texture.h>
#include <ray/ogl_buffer.h>

_NAME_BEGIN

OGLRenderer::OGLRenderer() noexcept
    : _constantBuffers(512)
    , _maxTextureUnits(32)
    , _maxViewports(4)
    , _state(GL_NONE)
    , _renderTarget(GL_NONE)
    , _defaultVAO(GL_NONE)
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
    _depthState.depthBias = 0;
    _depthState.depthBiasEnable = false;
    _depthState.depthSlopScaleBias = 0;
    _rasterState.cullMode = GPU_CULL_NONE;
    _rasterState.fillMode = GPU_SOLID_MODE;
    _rasterState.multisampleEnable = false;
    _rasterState.scissorTestEnable = false;
    _rasterState.pointSizeEnable = false;
    _stencilState.stencilEnable = false;
    _stencilState.stencilFunc = GPU_ALWAYS;
    _stencilState.stencilFail = STENCILOP_ZERO;
    _stencilState.stencilZFail = STENCILOP_ZERO;
    _stencilState.stencilPass = STENCILOP_ZERO;
    _stencilState.stencilTwoEnable = false;
    _clearState.clearFlags = ClearFlags::CLEAR_NONE;
    _clearState.clearColor = Vector4(0, 0, 0, 0);
    _clearState.clearDepth = 0.0;
    _clearState.clearStencil = 0.0;

    _textureUnits.resize(_maxTextureUnits);
    _viewport.resize(_maxViewports);
}

OGLRenderer::~OGLRenderer() noexcept
{
    this->close();
}

bool
OGLRenderer::open(RenderWindowPtr window) except
{
    this->setRenderWindow(window);

    initDebugControl();
    initCommandList();
    initStateSystem();

    return true;
}

void
OGLRenderer::close() noexcept
{
    if (_renderBuffer)
    {
        _renderBuffer.reset();
        _renderBuffer = nullptr;
    }

    if (_shaderObject)
    {
        _shaderObject.reset();
        _shaderObject = nullptr;
    }

    if (_renderTarget)
    {
        _renderTarget.reset();
        _renderTarget = nullptr;
    }

    if (_multiRenderTarget)
    {
        _multiRenderTarget.reset();
        _multiRenderTarget = nullptr;
    }

    if (_defaultVAO != GL_NONE)
    {
        glDeleteVertexArrays(1, &_defaultVAO);
        _defaultVAO = GL_NONE;
    }

    if (_glcontext)
    {
        _glcontext.reset();
        _glcontext = nullptr;
    }
}

void
OGLRenderer::renderBegin() noexcept
{
}

void
OGLRenderer::renderEnd() noexcept
{
    assert(_glcontext);

    glDepthMask(GL_TRUE);
    _glcontext->present();
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
OGLRenderer::clear(ClearFlags flags, const Color4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
    if (flags & ClearFlags::CLEAR_DEPTH)
    {
        GLfloat f = depth;
        glClearBufferfv(GL_DEPTH, 0, &f);
    }

    if (flags & ClearFlags::CLEAR_STENCIL)
    {
        GLint s = stencil;
        glClearBufferiv(GL_STENCIL, 0, &s);
    }

    if (flags & ClearFlags::CLEAR_COLOR)
    {
        glClearBufferfv(GL_COLOR, i, color.ptr());
    }
}

void
OGLRenderer::setViewport(const Viewport& view, std::size_t i) noexcept
{
    if (_viewport[i] != view)
    {
        if (OGLExtenstion::isSupport(ARB_viewport_array))
        {
            glViewportIndexedf(i, view.left, view.top, view.width, view.height);
        }
        else
        {
            glViewport(view.left, view.top, view.width, view.height);
        }

        _viewport[i] = view;
    }
}

const Viewport&
OGLRenderer::getViewport(std::size_t i) const noexcept
{
    return _viewport[i];
}

void
OGLRenderer::setRenderWindow(RenderWindowPtr window) noexcept
{
    if (_glcontext != window)
    {
        window->bind();
        _glcontext = window;
    }
}

RenderWindowPtr
OGLRenderer::getRenderWindow() const noexcept
{
    return _glcontext;
}

void
OGLRenderer::setSwapInterval(SwapInterval interval) noexcept
{
    assert(_glcontext);
    _glcontext->setSwapInterval(interval);
}

SwapInterval
OGLRenderer::getSwapInterval() const noexcept
{
    assert(_glcontext);
    return _glcontext->getSwapInterval();
}

void
OGLRenderer::setRenderState(RenderStatePtr state) noexcept
{
    auto instance = state->getInstanceID();
    if (_state != instance)
    {
        auto& blendState = state->getBlendState();
        auto& rasterState = state->getRasterState();
        auto& depthState = state->getDepthState();
        auto& stencilState = state->getStencilState();
        auto& clearState = state->getClearState();

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

            _rasterState.cullMode = rasterState.cullMode;
        }

#if !defined(EGLAPI)
        if (_rasterState.fillMode != rasterState.fillMode)
        {
            GLenum mode = OGLTypes::asFillMode(rasterState.fillMode);
            glPolygonMode(GL_FRONT_AND_BACK, mode);
            _rasterState.fillMode = rasterState.fillMode;
        }
#endif

        if (_rasterState.scissorTestEnable != rasterState.scissorTestEnable)
        {
            if (rasterState.scissorTestEnable)
                glEnable(GL_SCISSOR_TEST);
            else
                glDisable(GL_SCISSOR_TEST);

            _rasterState.scissorTestEnable = rasterState.scissorTestEnable;
        }

        if (_rasterState.pointSizeEnable != rasterState.pointSizeEnable)
        {
            if (rasterState.pointSizeEnable)
                glEnable(GL_PROGRAM_POINT_SIZE);
            else
                glDisable(GL_PROGRAM_POINT_SIZE);

            _rasterState.pointSizeEnable = rasterState.pointSizeEnable;
        }

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

        if (depthState.depthBiasEnable)
        {
            if (!_depthState.depthBiasEnable)
            {
                glEnable(GL_POLYGON_OFFSET_FILL);
                _depthState.depthBiasEnable = true;
            }

            if (_depthState.depthBias != depthState.depthBias ||
                _depthState.depthSlopScaleBias != depthState.depthSlopScaleBias)
            {
                glPolygonOffset(depthState.depthSlopScaleBias, depthState.depthBias);
                _depthState.depthBias = depthState.depthBias;
                _depthState.depthSlopScaleBias = depthState.depthSlopScaleBias;
            }
        }
        else
        {
            if (_depthState.depthBiasEnable)
            {
                glDisable(GL_POLYGON_OFFSET_FILL);
                _depthState.depthBiasEnable = false;
            }
        }

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

        _state = instance;
    }
}

RenderStatePtr
OGLRenderer::getRenderState() const noexcept
{
    return RenderState::getInstance(_state);
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
    if (_renderBuffer != buffer)
    {
        auto vb = std::dynamic_pointer_cast<OGLVertexBuffer>(buffer->getVertexBuffer());
        auto ib = std::dynamic_pointer_cast<OGLIndexBuffer>(buffer->getIndexBuffer());

        if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
        {
            if (vb)
            {
                auto bindlessVbo = vb->getInstanceAddr();
                auto vertexSize = vb->getVertexDataSize();

                GLuint64 offset = 0;

                for (auto& it : vb->getVertexComponents())
                {
                    glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.getVertexAttrib(), bindlessVbo + offset, vertexSize - offset);

                    offset += it.getVertexSize();
                }
            }

            if (ib)
            {
                auto bindlessIbo = ib->getInstanceAddr();
                auto indexDataSize = ib->getIndexDataSize();

                glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, bindlessIbo, indexDataSize);
            }
        }
        else if (OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
        {
            if (vb)
            {
                auto vbo = vb->getInstanceID();

                GLuint offset = 0;

                for (auto& it : vb->getVertexComponents())
                {
                    glBindVertexBuffer(it.getVertexAttrib(), vbo, offset, vb->getVertexSize());

                    offset += it.getVertexSize();
                }
            }

            if (ib)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());
            }
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, vb->getInstanceID());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());

            for (auto& it : vb->getVertexComponents())
            {
                glVertexAttribPointer(it.getVertexAttrib(), it.getVertexCount(), OGLTypes::asOGLVertexFormat(it.getVertexFormat()), GL_FALSE, vb->getVertexSize(), (void*)it.getVertexSize());
            }
        }

        _renderBuffer = buffer;
    }
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
    auto buffer = renderBuffer;

    auto vb = std::dynamic_pointer_cast<OGLVertexBuffer>(buffer->getVertexBuffer());
    auto ib = std::dynamic_pointer_cast<OGLIndexBuffer>(buffer->getIndexBuffer());

    if (vb)
    {
        auto vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
        auto vertexCount = vb->getVertexCount();
        auto vertexSize = vb->getVertexDataSize();

        glBindBuffer(GL_ARRAY_BUFFER, vb->getInstanceID());
        glBufferData(GL_ARRAY_BUFFER, vb->getVertexSize(), vb->data(), vertexUsage);
    }

    if (ib)
    {
        auto indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
        auto indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
        auto indexCount = ib->getIndexCount();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->getIndexDataSize(), ib->data(), indexUsage);
    }
}

void
OGLRenderer::drawRenderBuffer(const Renderable& renderable) noexcept
{
    assert(_renderBuffer);

    auto buffer = _renderBuffer;

    assert(buffer->getNumVertices() >= renderable.startVertice + renderable.numVertices);

    GLenum drawType = OGLTypes::asOGLVertexType(renderable.type);

    auto ib = buffer->getIndexBuffer();
    if (ib)
    {
        assert(ib->getIndexCount() >= renderable.startIndice + renderable.numIndices);

        GLenum indexType = OGLTypes::asOGLIndexType(ib->getIndexType());

#if !defined(EGLAPI)
        if (renderable.numInstances > 0)
            glDrawElementsInstancedBaseVertex(drawType, renderable.numIndices, indexType, (char*)nullptr + renderable.startIndice, renderable.numInstances, renderable.startVertice);
        else
            glDrawElementsBaseVertex(drawType, renderable.numIndices, indexType, (char*)(nullptr) + renderable.startIndice, renderable.startVertice);
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

RenderBufferPtr
OGLRenderer::getRenderBuffer() const noexcept
{
    return _renderBuffer;
}

void
OGLRenderer::setRenderTarget(RenderTargetPtr target) noexcept
{
    assert(target);

    auto framebuffer = std::dynamic_pointer_cast<OGLRenderTarget>(target)->getInstanceID();
    if (_framebuffer != framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        auto format = target->getTexFormat();
        if (format == PixelFormat::SR8G8B8 ||
            format == PixelFormat::SR8G8B8A8 ||
            format == PixelFormat::SRGB ||
            format == PixelFormat::SRGBA)
        {
            if (!_rasterState.srgbEnable)
            {
                glEnable(GL_FRAMEBUFFER_SRGB);
                _rasterState.srgbEnable = true;
            }
        }
        else
        {
            if (_rasterState.srgbEnable)
            {
                glDisable(GL_FRAMEBUFFER_SRGB);
                _rasterState.srgbEnable = false;
            }
        }

        this->clear(target->getClearFlags(), target->getClearColor(), target->getClearDepth(), target->getClearStencil());

        this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()));

        _framebuffer = framebuffer;
        _renderTarget = target;
        _multiRenderTarget = nullptr;
    }
}

void
OGLRenderer::copyRenderTarget(RenderTargetPtr src, const Viewport& v1, RenderTargetPtr dest, const Viewport& v2) noexcept
{
    assert(src);

    auto srcTarget = std::dynamic_pointer_cast<OGLRenderTarget>(src)->getInstanceID();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

    if (dest)
    {
        auto destTarget = std::dynamic_pointer_cast<OGLRenderTarget>(dest)->getInstanceID();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
    }
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    _renderTarget = GL_NONE;
}

RenderTargetPtr
OGLRenderer::getRenderTarget() const noexcept
{
    return _renderTarget;
}

MultiRenderTargetPtr
OGLRenderer::getMultiRenderTarget() const noexcept
{
    return _multiRenderTarget;
}

void
OGLRenderer::readRenderTarget(RenderTargetPtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    assert(target && w && h && data);

    auto framebuffer = std::dynamic_pointer_cast<OGLRenderTarget>(target)->getInstanceID();
    if (_framebuffer != framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        _framebuffer = framebuffer;
    }

    GLenum format = OGLTypes::asOGLFormat(pfd);
    GLenum type = OGLTypes::asOGLType(pfd);

    glReadPixels(0, 0, w, h, format, type, data);
}

void
OGLRenderer::setMultiRenderTarget(MultiRenderTargetPtr target) noexcept
{
    auto framebuffer = std::dynamic_pointer_cast<OGLMultiRenderTarget>(target)->getInstanceID();
    if (_framebuffer != framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        auto& targets = target->getRenderTargets();
        for (std::size_t index = 0; index < targets.size(); index++)
        {
            auto target = targets[index];

            this->clear(
                target->getClearFlags(),
                target->getClearColor(),
                target->getClearDepth(),
                target->getClearStencil(),
                index
                );
            this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()), index);
        }

        _framebuffer = framebuffer;

        _renderTarget = nullptr;
        _multiRenderTarget = target;
    }
}

void
OGLRenderer::setShaderObject(ShaderObjectPtr shader) noexcept
{
    assert(shader);

    auto program = std::dynamic_pointer_cast<OGLShaderObject>(shader)->getInstanceID();
    if (_program != program)
    {
        glUseProgram(program);
        _program = program;
        _shaderObject = shader;
    }

    for (auto& it : shader->getActiveUniforms())
    {
        this->setShaderUniform(it, it->getValue());
    }
}

ShaderObjectPtr
OGLRenderer::getShaderObject() const noexcept
{
    return _shaderObject;
}

bool
OGLRenderer::createShaderVariant(ShaderVariant& constant) noexcept
{
    if (constant.getType() != ShaderVariantType::SPT_BUFFER)
        return true;

    OGLShaderVariant buffer;

    glGenBuffers(1, &buffer.ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer.ubo);
    glBufferData(GL_UNIFORM_BUFFER, constant.getSize(), 0, GL_DYNAMIC_DRAW);

    if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        glGetNamedBufferParameterui64vNV(buffer.ubo, GL_BUFFER_GPU_ADDRESS_NV, &buffer.bindlessUbo);
        glMakeNamedBufferResidentNV(buffer.ubo, GL_READ_ONLY);
    }

    _constantBuffers[constant.getInstanceID()] = buffer;

    return true;
}

void
OGLRenderer::destroyShaderVariant(ShaderVariant& constant) noexcept
{
    auto index = constant.getInstanceID();
    if (index != 0)
    {
        auto& buffer = _constantBuffers[index];

        if (buffer.ubo)
        {
            glDeleteBuffers(1, &buffer.ubo);
            buffer.ubo = 0;
        }
    }
}

void
OGLRenderer::updateShaderVariant(ShaderVariantPtr constant) noexcept
{
    std::vector<char> _data;
    _data.resize(constant->getSize());

    std::size_t offset = 0;

    for (auto& it : constant->getParameters())
    {
        switch (it->getType())
        {
        case SPT_BOOL:
        {
            auto value = it->getBool();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_INT:
        {
            auto value = it->getInt();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_INT2:
        {
            auto value = it->getInt2();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT:
        {
            auto value = it->getFloat();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT2:
        {
            auto value = it->getFloat2();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT3:
        {
            auto value = it->getFloat3();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT4:
        {
            auto value = it->getFloat4();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT3X3:
        {
            auto value = it->getFloat3x3();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        case SPT_FLOAT4X4:
        {
            auto value = it->getFloat4x4();
            std::memcpy(&_data[offset], &value, it->getSize());
        }
        break;
        default:
            assert(false);
        }

        offset += it->getSize();
    }

    auto& buffer = _constantBuffers[constant->getInstanceID()];

    if (OGLExtenstion::isSupport(ARB_direct_state_access))
    {
        glNamedBufferSubDataEXT(buffer.ubo, 0, _data.size(), _data.data());
    }
    else
    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer.ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _data.size(), _data.data());
    }
}

void
OGLRenderer::setShaderUniform(ShaderUniformPtr uniform, TexturePtr texture) noexcept
{
    assert(uniform);
    assert(uniform->getBindingPoint() < _maxTextureUnits);

    auto _texture = std::dynamic_pointer_cast<OGLTexture>(texture);

    auto location = uniform->getLocation();
    auto program = uniform->getBindingProgram();
    auto unit = uniform->getBindingPoint();
    auto target = OGLTypes::asOGLTarget(texture->getTexDim());
    auto textureAddr = _texture->getInstanceAddr();
    auto textureID = _texture->getInstanceID();

#if !defined(EGLAPI)
    if (OGLExtenstion::isSupport(ARB_bindless_texture))
    {
        glProgramUniformHandleui64ARB(program, location, textureAddr);
    }
    else if (OGLExtenstion::isSupport(ARB_direct_state_access))
    {
        glProgramUniform1i(program, location, unit);
        glBindTextureUnit(GL_TEXTURE0 + unit, textureID);
    }
    else
#endif
    {
        glProgramUniform1i(program, location, unit);

        if (_textureUnits[unit] != textureID)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(target, textureID);

            _textureUnits[unit] = textureID;
        }
    }
}

void
OGLRenderer::setShaderUniform(ShaderUniformPtr uniform, ShaderVariantPtr constant) noexcept
{
    assert(constant && uniform);
    assert(uniform->getValue());

    auto type = uniform->getValue()->getType();
    auto location = uniform->getLocation();
    auto program = uniform->getBindingProgram();

    if (constant->isSemantic())
    {
        uniform->needUpdate(true);
    }

    switch (type)
    {
    case ShaderVariantType::SPT_BOOL:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform1i(program, location, uniform->getValue()->getBool());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_INT:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform1i(program, location, uniform->getValue()->getInt());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_INT2:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform2iv(program, location, 1, uniform->getValue()->getInt2().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform1f(program, location, uniform->getValue()->getFloat());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT2:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform2fv(program, location, 1, uniform->getValue()->getFloat2().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT3:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform3fv(program, location, 1, uniform->getValue()->getFloat3().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT4:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform4fv(program, location, 1, uniform->getValue()->getFloat4().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT3X3:
    {
        if (uniform->needUpdate())
        {
            glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat3x3().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT4X4:
    {
        if (uniform->needUpdate())
        {
            glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat4x4().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT_ARRAY:
    {
        if (uniform->needUpdate())
        {
            glProgramUniform1fv(program, location, uniform->getValue()->getFloatArray().size(), uniform->getValue()->getFloatArray().data());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_TEXTURE:
    {
        if (uniform->needUpdate())
        {
            auto texture = uniform->getValue()->getTexture();
            assert(texture);

            this->setShaderUniform(uniform, texture);
            if (OGLExtenstion::isSupport(ARB_bindless_texture))
            {
                uniform->needUpdate(false);
            }
        }

        break;
    }
    case ShaderVariantType::SPT_BUFFER:
    {
        if (uniform->needUpdate())
        {
            this->updateShaderVariant(constant);
            uniform->needUpdate(false);
        }

        auto index = constant->getInstanceID();
        if (index != 0)
        {
            auto& buffer = _constantBuffers[index];

            if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
            {
                glBindBufferRange(GL_UNIFORM_BUFFER, location, buffer.ubo, 0, constant->getSize());
            }
            else
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, location, buffer.ubo);
            }
        }
    }
    default:
        break;
    }
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
        assert(false);
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

    std::cerr << "message : " << message << std::endl;
}

void
OGLRenderer::initCommandList() noexcept
{
    OGLExtenstion::initCommandListNV();

    s_nvcmdlist_headerSizes[NVTokenTerminate::ID] = sizeof(NVTokenTerminate);
    s_nvcmdlist_headerSizes[NVTokenNop::ID] = sizeof(NVTokenNop);
    s_nvcmdlist_headerSizes[NVTokenDrawElems::ID] = sizeof(NVTokenDrawElems);
    s_nvcmdlist_headerSizes[NVTokenDrawArrays::ID] = sizeof(NVTokenDrawArrays);
    s_nvcmdlist_headerSizes[NVTokenDrawElemsStrip::ID] = sizeof(NVTokenDrawElemsStrip);
    s_nvcmdlist_headerSizes[NVTokenDrawArraysStrip::ID] = sizeof(NVTokenDrawArraysStrip);
    s_nvcmdlist_headerSizes[NVTokenDrawElemsInstanced::ID] = sizeof(NVTokenDrawElemsInstanced);
    s_nvcmdlist_headerSizes[NVTokenDrawArraysInstanced::ID] = sizeof(NVTokenDrawArraysInstanced);
    s_nvcmdlist_headerSizes[NVTokenVbo::ID] = sizeof(NVTokenVbo);
    s_nvcmdlist_headerSizes[NVTokenIbo::ID] = sizeof(NVTokenIbo);
    s_nvcmdlist_headerSizes[NVTokenUbo::ID] = sizeof(NVTokenUbo);
    s_nvcmdlist_headerSizes[NVTokenLineWidth::ID] = sizeof(NVTokenLineWidth);
    s_nvcmdlist_headerSizes[NVTokenPolygonOffset::ID] = sizeof(NVTokenPolygonOffset);
    s_nvcmdlist_headerSizes[NVTokenScissor::ID] = sizeof(NVTokenScissor);
    s_nvcmdlist_headerSizes[NVTokenBlendColor::ID] = sizeof(NVTokenBlendColor);
    s_nvcmdlist_headerSizes[NVTokenViewport::ID] = sizeof(NVTokenViewport);
    s_nvcmdlist_headerSizes[NVTokenAlphaRef::ID] = sizeof(NVTokenAlphaRef);
    s_nvcmdlist_headerSizes[NVTokenStencilRef::ID] = sizeof(NVTokenStencilRef);
    s_nvcmdlist_headerSizes[NVTokenFrontFace::ID] = sizeof(NVTokenFrontFace);

    for (int i = 0; i < GL_MAX_COMMANDS_NV; i++)
    {
        GLuint sz = s_nvcmdlist_headerSizes[i];
        assert(sz);
    }

    s_nvcmdlist_bindless = OGLExtenstion::isSupport(ARB_bindless_texture);

    if (OGLExtenstion::isSupport(NV_command_list))
    {
        for (int i = 0; i < GL_MAX_COMMANDS_NV; i++)
        {
            s_nvcmdlist_header[i] = glGetCommandHeaderNV(i, s_nvcmdlist_headerSizes[i]);
        }

        s_nvcmdlist_stages[NVTOKEN_STAGE_VERTEX] = glGetStageIndexNV(GL_VERTEX_SHADER);
        s_nvcmdlist_stages[NVTOKEN_STAGE_TESS_CONTROL] = glGetStageIndexNV(GL_TESS_CONTROL_SHADER);
        s_nvcmdlist_stages[NVTOKEN_STAGE_TESS_EVALUATION] = glGetStageIndexNV(GL_TESS_EVALUATION_SHADER);
        s_nvcmdlist_stages[NVTOKEN_STAGE_GEOMETRY] = glGetStageIndexNV(GL_GEOMETRY_SHADER);
        s_nvcmdlist_stages[NVTOKEN_STAGE_FRAGMENT] = glGetStageIndexNV(GL_FRAGMENT_SHADER);
    }
    else
    {
        for (int i = 0; i < GL_MAX_COMMANDS_NV; i++)
        {
            s_nvcmdlist_header[i] = nvtokenHeaderSW(i, s_nvcmdlist_headerSizes[i]);
        }

        for (int i = 0; i < NVTOKEN_STAGES; i++)
        {
            s_nvcmdlist_stages[i] = i;
        }
    }

    if (OGLExtenstion::isSupport(NV_command_list))
    {
        glCreateStatesNV(1, &_stateObjDraw);
        glCreateStatesNV(1, &_stateObjDrawGeo);

        glGenBuffers(1, &_tokenBuffer);
        glCreateCommandListsNV(1, &_tokenCmdList);
    }
}

void
OGLRenderer::initDebugControl() noexcept
{
#ifdef _DEBUG
    // 131184 memory info
    // 131185 memory allocation info
    GLuint ids[] =
    {
        131184,
        131185,
        131076,
        131169
    };

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallBack, this);

    // enable all
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    // disable ids
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 4, ids, GL_FALSE);
#endif
}

void
OGLRenderer::initStateSystem() noexcept
{
    _stateSystem.init();
    _stateSystem.generate(1, &_stateIdDraw);
    _stateSystem.generate(1, &_stateIdDrawGeo);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glFrontFace(GL_CW);

    if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        if (!_defaultVAO)
            glGenVertexArrays(1, &_defaultVAO);
        glBindVertexArray(_defaultVAO);

        glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
        glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
        glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);

        glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0);

        glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, (GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
        glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, (GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
        glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, (GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
    }

    if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_UNIFORM_BUFFER_UNIFIED_NV);
    }

    if (OGLExtenstion::isSupport(ARB_provoking_vertex))
    {
        glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    }
}

void
OGLRenderer::nvtokenGetStats(const void* stream, size_t streamSize, int stats[GL_MAX_COMMANDS_NV]) noexcept
{
    const GLubyte* current = (GLubyte*)stream;
    const GLubyte* streamEnd = current + streamSize;

    while (current < streamEnd) {
        const GLuint*             header = (const GLuint*)current;

        GLenum type = nvtokenHeaderCommand(*header);
        stats[type]++;

        current += s_nvcmdlist_headerSizes[type];
    }
}

GLenum
OGLRenderer::nvtokenDrawCommandSequence(const void* stream, size_t streamSize, GLenum mode, GLenum type, const State* state) noexcept
{
    const GLubyte* current = (GLubyte*)stream;
    const GLubyte* streamEnd = current + streamSize;

    GLenum modeStrip = mode;
    if (mode == GL_LINES)
        modeStrip = GL_LINE_STRIP;
    else if (mode == GL_TRIANGLES)
        modeStrip = GL_TRIANGLE_STRIP;
    else if (mode == GL_QUADS)
        modeStrip = GL_QUAD_STRIP;
    else if (mode == GL_LINES_ADJACENCY)
        modeStrip = GL_LINE_STRIP_ADJACENCY;
    else if (mode == GL_TRIANGLES_ADJACENCY)
        modeStrip = GL_TRIANGLE_STRIP_ADJACENCY;

    GLenum modeSpecial = mode;
    if (mode == GL_LINES)
        modeSpecial = GL_LINE_LOOP;
    else if (mode == GL_TRIANGLES)
        modeSpecial = GL_TRIANGLE_FAN;

    while (current < streamEnd)
    {
        const GLuint* header = (const GLuint*)current;

        GLenum cmdtype = nvtokenHeaderCommand(*header);

        // if you always use emulation on non-native tokens you can use
        // cmdtype = nvtokenHeaderCommandSW(header->encoded)
        switch (cmdtype)
        {
        case GL_TERMINATE_SEQUENCE_COMMAND_NV:
        {
            return type;
        }
        break;
        case GL_NOP_COMMAND_NV:
        {
        }
        break;
        case GL_DRAW_ELEMENTS_COMMAND_NV:
        {
            const DrawElementsCommandNV* cmd = (const DrawElementsCommandNV*)current;
            glDrawElementsBaseVertex(mode, cmd->count, type, (const GLvoid*)(cmd->firstIndex * sizeof(GLuint)), cmd->baseVertex);
        }
        break;
        case GL_DRAW_ARRAYS_COMMAND_NV:
        {
            const DrawArraysCommandNV* cmd = (const DrawArraysCommandNV*)current;
            glDrawArrays(mode, cmd->first, cmd->count);
        }
        break;
        case GL_DRAW_ELEMENTS_STRIP_COMMAND_NV:
        {
            const DrawElementsCommandNV* cmd = (const DrawElementsCommandNV*)current;
            glDrawElementsBaseVertex(modeStrip, cmd->count, type, (const GLvoid*)(cmd->firstIndex * sizeof(GLuint)), cmd->baseVertex);
        }
        break;
        case GL_DRAW_ARRAYS_STRIP_COMMAND_NV:
        {
            const DrawArraysCommandNV* cmd = (const DrawArraysCommandNV*)current;
            glDrawArrays(modeStrip, cmd->first, cmd->count);
        }
        break;
        case GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV:
        {
            const DrawElementsInstancedCommandNV* cmd = (const DrawElementsInstancedCommandNV*)current;
            assert(cmd->mode == mode || cmd->mode == modeStrip || cmd->mode == modeSpecial);

            glDrawElementsIndirect(cmd->mode, type, &cmd->count);
        }
        break;
        case GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV:
        {
            const DrawArraysInstancedCommandNV* cmd = (const DrawArraysInstancedCommandNV*)current;
            assert(cmd->mode == mode || cmd->mode == modeStrip || cmd->mode == modeSpecial);

            glDrawArraysIndirect(cmd->mode, &cmd->count);
        }
        break;
        case GL_ELEMENT_ADDRESS_COMMAND_NV:
        {
            if (s_nvcmdlist_bindless)
            {
                const ElementAddressCommandNV* cmd = (const ElementAddressCommandNV*)current;
                type = cmd->typeSizeInByte == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
                glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, GLuint64(cmd->addressLo) | (GLuint64(cmd->addressHi) << 32), 0x7FFFFFFF);
            }
            else
            {
                const ElementAddressCommandEMU* cmd = (const ElementAddressCommandEMU*)current;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmd->buffer);
            }
        }
        break;
        case GL_ATTRIBUTE_ADDRESS_COMMAND_NV:
        {
            if (s_nvcmdlist_bindless)
            {
                const AttributeAddressCommandNV* cmd = (const AttributeAddressCommandNV*)current;
                glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, cmd->index, GLuint64(cmd->addressLo) | (GLuint64(cmd->addressHi) << 32), 0x7FFFFFFF);
            }
            else
            {
                const AttributeAddressCommandEMU* cmd = (const AttributeAddressCommandEMU*)current;
                glBindVertexBuffer(cmd->index, cmd->buffer, cmd->offset, state->vertexformat.bindings[cmd->index].stride);
            }
        }
        break;
        case GL_UNIFORM_ADDRESS_COMMAND_NV:
        {
            if (s_nvcmdlist_bindless)
            {
                const UniformAddressCommandNV* cmd = (const UniformAddressCommandNV*)current;
                glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, cmd->index, GLuint64(cmd->addressLo) | (GLuint64(cmd->addressHi) << 32), 0x10000);
            }
            else
            {
                const UniformAddressCommandEMU* cmd = (const UniformAddressCommandEMU*)current;
                glBindBufferRange(GL_UNIFORM_BUFFER, cmd->index, cmd->buffer, cmd->offset256 * 256, cmd->size4 * 4);
            }
        }
        break;
        case GL_BLEND_COLOR_COMMAND_NV:
        {
            const BlendColorCommandNV* cmd = (const BlendColorCommandNV*)current;
            glBlendColor(cmd->red, cmd->green, cmd->blue, cmd->alpha);
        }
        break;
        case GL_STENCIL_REF_COMMAND_NV:
        {
            const StencilRefCommandNV* cmd = (const StencilRefCommandNV*)current;
            glStencilFuncSeparate(GL_FRONT, state->stencil.funcs[Faces::FACE_FRONT].func, cmd->frontStencilRef, state->stencil.funcs[Faces::FACE_FRONT].mask);
            glStencilFuncSeparate(GL_BACK, state->stencil.funcs[Faces::FACE_BACK].func, cmd->backStencilRef, state->stencil.funcs[Faces::FACE_BACK].mask);
        }
        break;

        case GL_LINE_WIDTH_COMMAND_NV:
        {
            const LineWidthCommandNV* cmd = (const LineWidthCommandNV*)current;
            glLineWidth(cmd->lineWidth);
        }
        break;
        case GL_POLYGON_OFFSET_COMMAND_NV:
        {
            const PolygonOffsetCommandNV* cmd = (const PolygonOffsetCommandNV*)current;
            glPolygonOffset(cmd->scale, cmd->bias);
        }
        break;
        case GL_ALPHA_REF_COMMAND_NV:
        {
            const AlphaRefCommandNV* cmd = (const AlphaRefCommandNV*)current;
            glAlphaFunc(state->alpha.mode, cmd->alphaRef);
        }
        break;
        case GL_VIEWPORT_COMMAND_NV:
        {
            const ViewportCommandNV* cmd = (const ViewportCommandNV*)current;
            glViewport(cmd->x, cmd->y, cmd->width, cmd->height);
        }
        break;
        case GL_SCISSOR_COMMAND_NV:
        {
            const ScissorCommandNV* cmd = (const ScissorCommandNV*)current;
            glScissor(cmd->x, cmd->y, cmd->width, cmd->height);
        }
        break;
        case GL_FRONT_FACE_COMMAND_NV:
        {
            FrontFaceCommandNV* cmd = (FrontFaceCommandNV*)current;
            glFrontFace(cmd->frontFace ? GL_CW : GL_CCW);
        }
        break;
        }

        GLuint tokenSize = s_nvcmdlist_headerSizes[cmdtype];
        assert(tokenSize);

        current += tokenSize;
    }

    return type;
}

void
OGLRenderer::nvtokenDrawCommandState() noexcept
{
    int lastfbo = ~0;

    auto tokens = (const char*)_tokenData.data();
    auto count = _tokenSequenceEmu.offsets.size();
    auto states = _tokenSequenceEmu.states.data();
    auto fbos = _tokenSequenceEmu.fbos.data();

    StateSystem::StateID lastID = 0;

    GLenum type = GL_UNSIGNED_SHORT;

    for (GLuint i = 0; i < count; i++)
    {
        auto ID = states[i];
        auto state = &_stateSystem.get(states[i]);

        GLuint fbo;
        if (fbos[i])
            fbo = fbos[i];
        else
            fbo = state->fbo.fboDraw;

        if (fbo != lastfbo)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            lastfbo = fbo;
        }

        if (ID != lastID)
        {
            _stateSystem.apply(ID, lastID);
            lastID = ID;
        }

        std::size_t offset = _tokenSequenceEmu.offsets[i];
        std::size_t size = _tokenSequenceEmu.sizes[i];

        assert(size + offset < _tokenData.size());

        type = nvtokenDrawCommandSequence(&tokens[offset], size, state->basePrimitiveMode, type, state);
    }
}

_NAME_END