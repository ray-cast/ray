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

_NAME_BEGIN

__ImplementSubClass(OGLRenderer, RenderDevice)

OGLRenderer::OGLRenderer() noexcept
    : _viewport(0, 0, 0, 0)
    , _renderBuffers(512)
    , _constantBuffers(512)
    , _maxTextures(100)
    , _maxTextureUnits(0)
    , _defaultVAO(0)
{
    _fbconfig.redSize = 8;
    _fbconfig.greenSize = 8;
    _fbconfig.blueSize = 8;
    _fbconfig.alphaSize = 8;
    _fbconfig.bufferSize = 32;
    _fbconfig.depthSize = 24;
    _fbconfig.stencilSize = 8;
    _fbconfig.accumSize = 32;
    _fbconfig.accumRedSize = 8;
    _fbconfig.accumGreenSize = 8;
    _fbconfig.accumBlueSize = 8;
    _fbconfig.accumAlphaSize = 8;
    _fbconfig.stereo = 0;
    _fbconfig.samples = 1;
    _fbconfig.doubleBuffer = 1;

    _ctxconfig.major = 3;
    _ctxconfig.minor = 3;
    _ctxconfig.release = 0;
    _ctxconfig.robustness = 0;
    _ctxconfig.share = nullptr;
    _ctxconfig.profile = GPU_GL_CORE_PROFILE;
    _ctxconfig.forward = 0;
    _ctxconfig.multithread = false;
    _ctxconfig.hwnd = nullptr;

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

    _textures.resize(_maxTextures);
}

OGLRenderer::~OGLRenderer() noexcept
{
    this->close();
}

bool
OGLRenderer::open(WindHandle hwnd) except
{
    if ((_ctxconfig.major < 1 || _ctxconfig.minor < 0) ||
        (_ctxconfig.major == 1 && _ctxconfig.minor > 5) ||
        (_ctxconfig.major == 2 && _ctxconfig.minor > 1) ||
        (_ctxconfig.major == 3 && _ctxconfig.minor > 3))
    {
        return false;
    }

    if (_ctxconfig.profile)
    {
        if (_ctxconfig.profile != GPU_GL_CORE_PROFILE &&
            _ctxconfig.profile != GPU_GL_COMPAT_PROFILE)
        {
            return false;
        }

        if (_ctxconfig.major < 3 || (_ctxconfig.major == 3 && _ctxconfig.minor < 2))
        {
            return false;
        }
    }

    if (_ctxconfig.forward && _ctxconfig.major < 3)
    {
        return false;
    }

    if (_ctxconfig.robustness)
    {
        return false;
    }

    if (_ctxconfig.release)
    {
        return false;
    }

    if (hwnd)
    {
        _ctxconfig.hwnd = hwnd;
    }

    _glcontext = std::make_shared<OGLCanvas>();
    _glcontext->setup(_fbconfig, _ctxconfig);

    initDebugControl();
    initCommandList();
    initStateSystem();

    return true;
}

void
OGLRenderer::close() noexcept
{
    for (auto& it : _textures)
    {
        if (it.texture != GL_NONE)
        {
            glDeleteTextures(1, &it.texture);
            it.texture = GL_NONE;
        }
    }

    for (auto& it : _renderBuffers)
    {
        if (it.vao != GL_NONE)
        {
            glDeleteVertexArrays(1, &it.vao);
            it.vao = GL_NONE;
        }

        if (it.vbo != GL_NONE)
        {
            glDeleteBuffers(1, &it.vbo);
            it.vbo = GL_NONE;
        }

        if (it.ibo != GL_NONE)
        {
            glDeleteBuffers(1, &it.ibo);
            it.ibo = GL_NONE;
        }
    }

    for (auto& it : _renderTextures)
    {
        if (it.fbo != GL_NONE)
        {
            glDeleteFramebuffers(1, &it.fbo);
            it.fbo = GL_NONE;
        }
    }

    for (auto& it : _multiRenderTextures)
    {
        if (it.fbo != GL_NONE)
        {
            glDeleteFramebuffers(1, &it.fbo);
            it.fbo = GL_NONE;
        }
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
OGLRenderer::clear(std::size_t i, ClearFlags flags, const Color4& color, float depth, std::int32_t stencil) noexcept
{
    if (flags & ClearFlags::CLEAR_DEPTH)
    {
        GLfloat f = depth;
        glClearBufferfv(GL_DEPTH, i, &f);
    }

    if (flags & ClearFlags::CLEAR_STENCIL)
    {
        GLint s = stencil;
        glClearBufferiv(GL_STENCIL, i, &s);
    }

    if (flags & ClearFlags::CLEAR_DEPTH_STENCIL)
    {
        GLfloat f = depth;
        GLint s = stencil;
        glClearBufferfi(GL_DEPTH_STENCIL, i, f, s);
    }

    if (flags & ClearFlags::CLEAR_COLOR)
    {
        glClearBufferfv(GL_COLOR, i, color.ptr());
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
OGLRenderer::setClearState(const RenderClearState& state) noexcept
{
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
        auto& clearState = state->getClearState();

        this->setBlendState(blendState);
        this->setRasterState(rasterState);
        this->setDepthState(depthState);
        this->setStencilState(stencilState);
        this->setClearState(clearState);

        _state = state;
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

    _renderBuffer = nullptr;
    _renderTexture = nullptr;
    _multiRenderTexture = nullptr;
    _shader = nullptr;

    if (!_depthState.depthWriteMask)
    {
        glDepthMask(GL_TRUE);
        _depthState.depthWriteMask = true;
    }

    _glcontext->present();
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
    assert(_glcontext);

    if (_interval != interval)
    {
        _glcontext->setSwapInterval(interval);
        _interval = interval;
    }
}

SwapInterval
OGLRenderer::getSwapInterval() const noexcept
{
    return _interval;
}

bool
OGLRenderer::createRenderBuffer(RenderBuffer& buffer) noexcept
{
    auto vb = buffer.getVertexBuffer();
    auto ib = buffer.getIndexBuffer();

    OGLVertexArray renderBuffer;

    if (!OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
    {
        if (OGLExtenstion::isSupport(ARB_vertex_array_object))
        {
            glGenVertexArrays(1, &renderBuffer.vao);
            glBindVertexArray(renderBuffer.vao);
        }
    }

    if (vb)
    {
        renderBuffer.vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
        renderBuffer.vertexCount = vb->getVertexCount();
        renderBuffer.vertexSize = vb->getVertexSize() * renderBuffer.vertexCount;
        renderBuffer.vertexByteSize = vb->getVertexByteSize();

        glGenBuffers(1, &renderBuffer.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, renderBuffer.vbo);
        glBufferData(GL_ARRAY_BUFFER, renderBuffer.vertexSize, vb->data(), renderBuffer.vertexUsage);

        GLuint offset = 0;

        auto& components = vb->getVertexComponents();
        for (auto& it : components)
        {
            auto type = OGLTypes::asOGLVertexDataType(it.getVertexDataType());
            if (type == GL_INVALID_ENUM)
            {
                glDeleteBuffers(1, &renderBuffer.vbo);
                if (renderBuffer.vao)
                {
                    glDeleteVertexArrays(1, &renderBuffer.vao);
                }
                assert(false);
                return false;
            }

            if (OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
            {
                /*glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
                glVertexAttribFormat(it.getVertexAttrib(), it.getVertexSize(), type, GL_FALSE, 0);
                glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());*/

                glBindVertexBuffer(it.getVertexAttrib(), renderBuffer.vbo, offset, vb->getVertexByteSize());
            }
            else
            {
                if (OGLExtenstion::isSupport(ARB_vertex_array_object))
                {
                    glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
                    glVertexAttribPointer(it.getVertexAttrib(), it.getVertexSize(), type, GL_FALSE, vb->getVertexByteSize(), (const char*)nullptr + offset);
                }
            }

            OGLVertexAttrib attrib;

            attrib.attrib = it.getVertexAttrib();
            attrib.offset = offset;
            attrib.size = it.getVertexSize();
            attrib.stride = vb->getVertexByteSize();
            attrib.type = type;

            renderBuffer.attribs.push_back(attrib);

            offset += it.getVertexByteSize();
        }

        if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
        {
            glGetNamedBufferParameterui64vNV(renderBuffer.vbo, GL_BUFFER_GPU_ADDRESS_NV, &renderBuffer.bindlessVbo);
            glMakeNamedBufferResidentNV(renderBuffer.vbo, GL_READ_ONLY);
        }
    }

    if (ib)
    {
        renderBuffer.indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
        renderBuffer.indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
        renderBuffer.indexCount = ib->getIndexCount();

        renderBuffer.indexSize = 0;
        if (renderBuffer.indexType == GL_UNSIGNED_SHORT)
        {
            renderBuffer.indexSize = sizeof(std::int16_t) * ib->getIndexCount();
        }
        else if (renderBuffer.indexType == GL_UNSIGNED_INT)
        {
            renderBuffer.indexSize = sizeof(std::int32_t) * ib->getIndexCount();
        }
        else
        {
            assert(false);
        }

        glGenBuffers(1, &renderBuffer.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.indexSize, ib->data(), renderBuffer.indexUsage);

        if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
        {
            glGetNamedBufferParameterui64vNV(renderBuffer.ibo, GL_BUFFER_GPU_ADDRESS_NV, &renderBuffer.bindlessIbo);
            glMakeNamedBufferResidentNV(renderBuffer.ibo, GL_READ_ONLY);
        }
    }

    auto instanceID = buffer.getInstanceID();

    if (_renderBuffers.size() < instanceID)
    {
        _renderBuffers.resize(instanceID);
    }

    _renderBuffers[instanceID] = renderBuffer;

    return true;
}

void
OGLRenderer::destroyRenderBuffer(RenderBuffer& buffer) noexcept
{
    auto instanceID = buffer.getInstanceID();
    if (instanceID != 0)
    {
        auto& buffer = _renderBuffers[instanceID];

        if (buffer.vao != GL_NONE)
        {
            glDeleteVertexArrays(1, &buffer.vao);
            buffer.vao = GL_NONE;
        }

        if (buffer.vbo != GL_NONE)
        {
            glDeleteBuffers(1, &buffer.vbo);
            buffer.vbo = GL_NONE;
        }

        if (buffer.ibo != GL_NONE)
        {
            glDeleteBuffers(1, &buffer.ibo);
            buffer.ibo = GL_NONE;
        }
    }
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
    if (_renderBuffer != buffer)
    {
        auto instanceID = buffer->getInstanceID();
        auto& renderBuffer = _renderBuffers[instanceID];

        if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
        {
            if (renderBuffer.bindlessVbo)
            {
                for (auto& it : renderBuffer.attribs)
                {
                    glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.attrib, renderBuffer.bindlessVbo + it.offset, renderBuffer.vertexSize - it.offset);
                }
            }

            if (renderBuffer.bindlessIbo)
            {
                glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, renderBuffer.bindlessIbo, renderBuffer.indexSize);
            }
        }
        else if (OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
        {
            for (auto& it : renderBuffer.attribs)
            {
                glBindVertexBuffer(it.attrib, renderBuffer.vbo, it.offset, it.stride);
            }

            if (renderBuffer.ibo)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);
            }
        }
        else if (OGLExtenstion::isSupport(ARB_vertex_array_object))
        {
            assert(renderBuffer.vao);
            glBindVertexArray(renderBuffer.vao);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, renderBuffer.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);

            for (auto& it : renderBuffer.attribs)
            {
                glEnableVertexAttribArray(it.attrib);
                glVertexAttribPointer(it.attrib, it.size, it.type, GL_FALSE, it.stride, (void*)it.offset);
            }
        }

        _renderBuffer = buffer;
    }
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
    assert(renderBuffer->getUserData());

    auto vb = renderBuffer->getVertexBuffer();
    auto ib = renderBuffer->getIndexBuffer();
    auto buffer = _renderBuffers[renderBuffer->getInstanceID()];

    if (vb)
    {
        assert(buffer.vbo);

        buffer.vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
        buffer.vertexCount = vb->getVertexCount();
        buffer.vertexSize = vb->getVertexSize() * buffer.vertexCount;

        glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer.vertexSize, vb->data(), buffer.vertexUsage);
    }

    if (ib)
    {
        assert(buffer.ibo);

        buffer.indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
        buffer.indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
        buffer.indexCount = ib->getIndexCount();

        buffer.indexSize = 0;
        if (buffer.indexType == GL_UNSIGNED_SHORT)
        {
            buffer.indexSize = sizeof(std::int16_t) * ib->getIndexCount();
        }
        else if (buffer.indexType == GL_UNSIGNED_INT)
        {
            buffer.indexSize = sizeof(std::int32_t) * ib->getIndexCount();
        }
        else
        {
            assert(false);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.indexSize, ib->data(), buffer.indexUsage);
    }
}

void
OGLRenderer::drawRenderBuffer(const Renderable& renderable) noexcept
{
    assert(_renderBuffer);

    auto vb = _renderBuffer->getVertexBuffer();
    auto ib = _renderBuffer->getIndexBuffer();
    auto buffer = _renderBuffers[_renderBuffer->getInstanceID()];

    assert(buffer.vertexCount >= renderable.startVertice + renderable.numVertices);

    GLenum drawType = OGLTypes::asOGLVertexType(renderable.type);

    if (buffer.indexCount)
    {
        assert(buffer.indexCount >= renderable.startIndice + renderable.numIndices);

        GLenum indexType = buffer.indexType;

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

bool
OGLRenderer::createTexture(Texture& texture) noexcept
{
    OGLTexture instance;

    auto instanceID = texture.getInstanceID();
    auto target = OGLTypes::asOGLTarget(texture.getTexDim());
    auto format = OGLTypes::asOGLFormat(texture.getTexFormat());
    auto type = OGLTypes::asOGLType(texture.getTexFormat());
    auto internalFormat = OGLTypes::asOGLInternalformat(texture.getTexFormat());
    auto buf = texture.getStream();

    glGenTextures(1, &instance.texture);
    glBindTexture(target, instance.texture);

    // set unpack alignment to one byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLint level = (GLint)texture.getLevel();

    GLsizei w = (GLsizei)texture.getWidth();
    GLsizei h = (GLsizei)texture.getHeight();
    GLsizei depth = (GLsizei)texture.getDepth();

    texture.setUserData(instance.texture);

    applyTextureWrap(target, texture.getTexWrap());
    applyTextureFilter(target, texture.getTexFilter());
    applyTextureAnis(target, texture.getTexAnisotropy());

    switch (target)
    {
    case GL_TEXTURE_2D:
    {
        if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
            format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
            format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
        {
            int size = w * h;
            glCompressedTexImage2D(target, level, internalFormat, w, h, 0, size, buf);
        }
        else
        {
            if (texture.isMultiSample())
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, GL_FALSE);
            else
                glTexImage2D(target, level, internalFormat, w, h, 0, format, type, buf);
        }
    }
    break;
    case GL_TEXTURE_2D_ARRAY:
    {
        if (texture.isMultiSample())
            glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, level, internalFormat, w, h, depth, GL_FALSE);
        else
            glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, 0);
    }
    break;
    case GL_TEXTURE_3D:
    {
        glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, 0);
    }
    break;
    case GL_TEXTURE_CUBE_MAP:
    {
        if (texture.isMultiSample())
        {
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, 0, format, type, buf);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, 0, format, type, buf);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, 0, format, type, buf);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, 0, format, type, buf);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, 0, format, type, buf);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, 0, format, type, buf);
        }
    }
    break;
    default:
        break;
    }

    // restore old unpack alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, _unpackAlignment);

    if (OGLExtenstion::isSupport(ARB_bindless_texture))
    {
        instance.resident = glGetTextureHandleARB(instance.texture);
        glMakeTextureHandleResidentARB(instance.resident);
    }

    if (_textures.size() < instanceID)
    {
        _renderBuffers.resize(instanceID * 2);
    }

    _textures[instanceID] = instance;

    return true;
}

void
OGLRenderer::destroyTexture(Texture& texture) noexcept
{
    auto instance = texture.getInstanceID();
    if (instance)
    {
        auto handle = _textures[instance].texture;
        if (handle == 0)
            return;

        _textures[instance].texture = 0;
        _textures[instance].resident = 0;

        glDeleteTextures(1, &handle);
    }
}

void
OGLRenderer::setTexture(const Texture& texture, ShaderUniformPtr uniform) noexcept
{
    assert(uniform);
    assert(uniform->getBindingPoint() < _maxTextureUnits);

    auto location = uniform->getLocation();
    auto unit = uniform->getBindingPoint();
    auto target = OGLTypes::asOGLTarget(texture.getTexDim());
    auto instance = texture.getInstanceID();
    auto handle = _textures[instance].texture;
    auto resident = _textures[instance].resident;

#if !defined(EGLAPI)
    if (OGLExtenstion::isSupport(ARB_bindless_texture))
    {
        glUniformHandleui64ARB(location, resident);
    }
    else if (OGLExtenstion::isSupport(EXT_direct_state_access))
    {
        glUniform1i(location, unit);
        glBindMultiTextureEXT(GL_TEXTURE0 + unit, target, handle);
    }
    else
#endif
    {
        glUniform1i(location, unit);

        if (_textureUnits[unit] != handle)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(target, handle);

            _textureUnits[unit] = handle;
        }
    }
}

bool
OGLRenderer::createRenderTexture(RenderTexture& target) noexcept
{
    OGLRenderTexture instance;

    glGenFramebuffers(1, &instance.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, instance.fbo);

    auto instanceID = target.getInstanceID();

    auto sharedDepthTarget = target.getSharedDepthTexture();
    auto sharedStencilTarget = target.getSharedStencilTexture();

    if (sharedDepthTarget == sharedStencilTarget)
    {
        if (sharedDepthTarget)
            this->bindRenderTexture(sharedDepthTarget, Attachment::DEPTH_STENCIL);
    }
    else
    {
        if (sharedDepthTarget)
        {
            this->bindRenderTexture(sharedDepthTarget, Attachment::DEPTH);
        }

        if (sharedStencilTarget)
        {
            this->bindRenderTexture(sharedStencilTarget, Attachment::STENCIL);
        }
    }

    auto resolveFormat = target.getTexFormat();

    if (resolveFormat == PixelFormat::DEPTH24_STENCIL8 || resolveFormat == PixelFormat::DEPTH32_STENCIL8)
    {
        this->bindRenderTexture(target.shared_from_this(), Attachment::DEPTH_STENCIL);
    }
    else if (resolveFormat == PixelFormat::DEPTH_COMPONENT16 || resolveFormat == PixelFormat::DEPTH_COMPONENT24 || resolveFormat == PixelFormat::DEPTH_COMPONENT32)
    {
        this->bindRenderTexture(target.shared_from_this(), Attachment::DEPTH);
    }
    else if (resolveFormat == PixelFormat::STENCIL8)
    {
        this->bindRenderTexture(target.shared_from_this(), Attachment::STENCIL);
    }
    else
    {
        this->bindRenderTexture(target.shared_from_this(), Attachment::COLOR0);
    }

    if (OGLExtenstion::isSupport(EXT_direct_state_access))
        glFramebufferDrawBufferEXT(instance.fbo, GL_COLOR_ATTACHMENT0);
    else
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

    _renderTexture = nullptr;
    _multiRenderTexture = nullptr;

    if (_renderTextures.size() <= instanceID)
    {
        _renderTextures.resize(instanceID * 2);
    }

    _renderTextures[instanceID] = instance;

#if defined(_DEBUG)
    this->checkFramebuffer();
#endif

    return true;
}

void
OGLRenderer::destroyRenderTexture(RenderTexture& target) noexcept
{
    auto instanceID = target.getInstanceID();
    auto framebuffer = _renderTextures[instanceID].fbo;
    if (framebuffer != GL_NONE)
    {
        glDeleteBuffers(1, &framebuffer);
        _renderTextures[instanceID].fbo = GL_NONE;
    }
}

void
OGLRenderer::bindRenderTexture(RenderTexturePtr target, Attachment attachment) noexcept
{
    auto attribindex = OGLTypes::asOGLAttachment(attachment);

    auto texture = target->getResolveTexture();
    auto instanceID = texture->getInstanceID();
    auto handle = _textures[instanceID].texture;

    switch (target->getTexDim())
    {
    case TextureDim::DIM_2D:
    {
        if (target->isMultiSample())
            glFramebufferTexture2D(GL_FRAMEBUFFER, attribindex, GL_TEXTURE_2D_MULTISAMPLE, handle, 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, attribindex, GL_TEXTURE_2D, handle, 0);
        break;
    }
    case TextureDim::DIM_2D_ARRAY:
    {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, attribindex, handle, 0, 0);
        break;
    }
#if !defined(EGLAPI)
    case TextureDim::DIM_3D:
    {
        glFramebufferTexture3D(GL_FRAMEBUFFER, attribindex, GL_TEXTURE_3D, handle, 0, 0);
        break;
    }
#endif
    }
}

void
OGLRenderer::setRenderTexture(RenderTexturePtr target) noexcept
{
    assert(target);

    if (_renderTexture != target)
    {
        auto instanceID = target->getInstanceID();

        glBindFramebuffer(GL_FRAMEBUFFER, _renderTextures[instanceID].fbo);

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

        _renderTexture = target;
        _multiRenderTexture = nullptr;
    }

    this->clear(target->getClearFlags(), target->getClearColor(), target->getClearDepth(), target->getClearStencil());
    this->setViewport(target->getViewport());
}

void
OGLRenderer::copyRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
{
    assert(src);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _renderTextures[src->getInstanceID()].fbo);

    if (dest)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderTextures[dest->getInstanceID()].fbo);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    _renderTexture = nullptr;
    _multiRenderTexture = nullptr;
}

void
OGLRenderer::readRenderTexture(RenderTexturePtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    assert(target && w && h && data);

    if (_renderTexture != target)
    {
        auto instanceID = target->getInstanceID();
        glBindFramebuffer(GL_FRAMEBUFFER, _renderTextures[instanceID].fbo);
    }

    GLenum format = OGLTypes::asOGLFormat(pfd);
    GLenum type = OGLTypes::asOGLType(pfd);

    glReadPixels(0, 0, w, h, format, type, data);

    _renderTexture = target;
    _multiRenderTexture = nullptr;
}

bool
OGLRenderer::createMultiRenderTexture(MultiRenderTexture& target) noexcept
{
    OGLRenderTexture instance;

    auto instanceID = target.getInstanceID();

    glGenFramebuffers(1, &instance.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, instance.fbo);

    std::size_t count = 0;
    GLenum  draw[Attachment::COLOR15];

    for (auto& it : target.getRenderTextures())
    {
        TexturePtr texture = it.texture->getResolveTexture();
        if (!texture)
            continue;

        GLenum attachment = OGLTypes::asOGLAttachment(it.location);

        auto handle = _textures[texture->getInstanceID()].texture;

        auto dim = texture->getTexDim();
        if (dim == TextureDim::DIM_2D)
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0);
        else if (dim == TextureDim::DIM_2D_ARRAY)
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, handle, 0, 0);
#if !defined(EGLAPI)
        else if (dim == TextureDim::DIM_3D)
            glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, handle, 0, 0);
#endif
        else
            assert(false);

        if (attachment != GL_DEPTH_ATTACHMENT &&
            attachment != GL_DEPTH_STENCIL_ATTACHMENT &&
            attachment != GL_STENCIL_ATTACHMENT)
        {
            draw[count++] = attachment;
        }
    }

    if (OGLExtenstion::isSupport(EXT_direct_state_access))
        glFramebufferDrawBuffersEXT(instance.fbo, count, draw);
    else
        glDrawBuffers(count, draw);

    _renderTexture = nullptr;
    _multiRenderTexture = nullptr;

    if (_multiRenderTextures.size() <= instanceID)
    {
        _multiRenderTextures.resize(instanceID * 2);
    }

    _multiRenderTextures[instanceID] = instance;

    return true;
}

void
OGLRenderer::destroyMultiRenderTexture(MultiRenderTexture& target) noexcept
{
    auto instanceID = target.getInstanceID();
    auto& framebuffer = _multiRenderTextures[instanceID].fbo;
    if (framebuffer != GL_NONE)
    {
        glDeleteBuffers(GL_FRAMEBUFFER, &framebuffer);
        framebuffer = GL_NONE;
    }
}

void
OGLRenderer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
    if (_multiRenderTexture != target)
    {
        auto instanceID = target->getInstanceID();
        auto framebuffer = _multiRenderTextures[instanceID].fbo;

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        _multiRenderTexture = target;
        _renderTexture = 0;
    }

    this->setViewport(target->getViewport());

    auto& targets = target->getRenderTextures();
    for (auto& it : targets)
    {
        auto attachment = it.location;
        if (attachment == Attachment::DEPTH ||
            attachment == Attachment::STENCIL ||
            attachment == Attachment::DEPTH_STENCIL)
        {
            continue;
        }

        auto index = attachment - Attachment::COLOR0;

        this->clear(index,
            it.texture->getClearFlags(),
            it.texture->getClearColor(),
            it.texture->getClearDepth(),
            it.texture->getClearStencil()
            );
    }
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
        if (oglShaderObject)
        {
            oglShaderObject->bind();
        }

        _shader = shader;
    }

    for (auto& it : shader->getActiveUniforms())
    {
        this->setShaderVariant(it->getValue(), it);
    }
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

    if (OGLExtenstion::isSupport(EXT_direct_state_access))
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
OGLRenderer::setShaderVariant(ShaderVariantPtr constant, ShaderUniformPtr uniform) noexcept
{
    assert(uniform->getValue());

    auto type = uniform->getValue()->getType();
    auto location = uniform->getLocation();

    switch (type)
    {
    case ShaderVariantType::SPT_BOOL:
    {
        if (uniform->needUpdate())
        {
            glUniform1i(location, uniform->getValue()->getBool());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_INT:
    {
        if (uniform->needUpdate())
        {
            glUniform1i(location, uniform->getValue()->getInt());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_INT2:
    {
        if (uniform->needUpdate())
        {
            glUniform2iv(location, 1, uniform->getValue()->getInt2().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT:
    {
        if (uniform->needUpdate())
        {
            glUniform1f(location, uniform->getValue()->getFloat());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT2:
    {
        if (uniform->needUpdate())
        {
            glUniform2fv(location, 1, uniform->getValue()->getFloat2().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT3:
    {
        if (uniform->needUpdate())
        {
            glUniform3fv(location, 1, uniform->getValue()->getFloat3().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT4:
    {
        if (uniform->needUpdate())
        {
            glUniform4fv(location, 1, uniform->getValue()->getFloat4().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT3X3:
    {
        if (uniform->needUpdate())
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, uniform->getValue()->getFloat3x3().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT4X4:
    {
        if (uniform->needUpdate())
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, uniform->getValue()->getFloat4x4().ptr());
            uniform->needUpdate(false);
        }
        break;
    }
    case ShaderVariantType::SPT_FLOAT_ARRAY:
    {
        if (uniform->needUpdate())
        {
            glUniform1fv(location, uniform->getValue()->getFloatArray().size(), uniform->getValue()->getFloatArray().data());
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

            this->setTexture(*texture, uniform);
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
OGLRenderer::applyTextureWrap(GLenum target, TextureWrap wrap) noexcept
{
    if (TextureWrap::REPEAT & wrap)
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }
    else if (TextureWrap::CLAMP_TO_EDGE & wrap)
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
#endif
}

void
OGLRenderer::initStateSystem() noexcept
{
    _stateSystem.init();
    _stateSystem.generate(1, &_stateIdDraw);
    _stateSystem.generate(1, &_stateIdDrawGeo);

    if (OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
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

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &_unpackAlignment);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);

    _textureUnits.resize(_maxTextureUnits);
}

void
OGLRenderer::nvtokenCommandToString(GLenum type, char*& string) noexcept
{
    switch (type)
    {
    case GL_NOP_COMMAND_NV:
        string = "GL_NOP_COMMAND_NV";
    case GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV:
        string = "GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV";
    case GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV:
        string = "GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV";
    case GL_ELEMENT_ADDRESS_COMMAND_NV:
        string = "GL_ELEMENT_ADDRESS_COMMAND_NV";
    case GL_ATTRIBUTE_ADDRESS_COMMAND_NV:
        string = "GL_ATTRIBUTE_ADDRESS_COMMAND_NV";
    case GL_UNIFORM_ADDRESS_COMMAND_NV:
        string = "GL_UNIFORM_ADDRESS_COMMAND_NV";
    case GL_BLEND_COLOR_COMMAND_NV:
        string = "GL_BLEND_COLOR_COMMAND_NV";
    case GL_STENCIL_REF_COMMAND_NV:
        string = "GL_STENCIL_REF_COMMAND_NV";
    case GL_TERMINATE_SEQUENCE_COMMAND_NV:
        string = "GL_TERMINATE_SEQUENCE_COMMAND_NV";
    case GL_LINE_WIDTH_COMMAND_NV:
        string = "GL_LINE_WIDTH_COMMAND_NV";
    case GL_POLYGON_OFFSET_COMMAND_NV:
        string = "GL_POLYGON_OFFSET_COMMAND_NV";
    case GL_ALPHA_REF_COMMAND_NV:
        string = "GL_ALPHA_REF_COMMAND_NV";
    case GL_VIEWPORT_COMMAND_NV:
        string = "GL_VIEWPORT_COMMAND_NV";
    case GL_SCISSOR_COMMAND_NV:
        string = "GL_SCISSOR_COMMAND_NV";
    case GL_DRAW_ELEMENTS_COMMAND_NV:
        string = "GL_DRAW_ELEMENTS_COMMAND_NV";
    case GL_DRAW_ARRAYS_COMMAND_NV:
        string = "GL_DRAW_ARRAYS_COMMAND_NV";
    case GL_DRAW_ELEMENTS_STRIP_COMMAND_NV:
        string = "GL_DRAW_ELEMENTS_STRIP_COMMAND_NV";
    case GL_DRAW_ARRAYS_STRIP_COMMAND_NV:
        string = "GL_DRAW_ARRAYS_STRIP_COMMAND_NV";
    default:
        string = nullptr;
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
    auto count = _tokenSequence.offsets.size();
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

        _stateSystem.apply(ID, lastID);

        lastID = ID;

        std::size_t offset = _tokenSequenceEmu.offsets[i];
        std::size_t size = _tokenSequenceEmu.sizes[i];

        assert(size + offset < _tokenData.size());

        type = nvtokenDrawCommandSequence(&tokens[offset], size, state->basePrimitiveMode, type, state);
    }
}

_NAME_END