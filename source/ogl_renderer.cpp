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
    if (_defaultVAO)
    {
        glDeleteVertexArrays(1, &_defaultVAO);
        _defaultVAO = 0;
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
    //glEnable(GL_FRAMEBUFFER_SRGB);

    if (_NV_vertex_buffer_unified_memory)
    {
        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_UNIFORM_BUFFER_UNIFIED_NV);
    }
}

void
OGLRenderer::renderEnd() noexcept
{
    _renderBuffer = nullptr;
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

    if (_ARB_vertex_attrib_binding)
    {
        if (!_defaultVAO)
            glGenVertexArrays(1, &_defaultVAO);
        glBindVertexArray(_defaultVAO);
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

bool
OGLRenderer::createConstantBuffer(ShaderConstantBuffer& constant) noexcept
{
    OGLConstantBuffer buffer;

    glBindBuffer(GL_UNIFORM_BUFFER, buffer.ubo);
    //glBufferData(GL_UNIFORM_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _constantBuffers[buffer.ubo] = buffer;

    return true;
}

void
OGLRenderer::destroyConstantBuffer(ShaderConstantBuffer& constant) noexcept
{
    auto index = constant.getUserData();
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
OGLRenderer::setShaderConstantBuffer(ShaderConstantBufferPtr buffer) noexcept
{
    auto& args = buffer->getShaderParamArgs();

    for (auto& it : args)
    {
        auto type = it.value->getType();
        auto location = it.uniform->location;

        switch (type)
        {
        case ShaderParamType::SPT_BOOL:
        {
            glUniform1i(location, (int)it.value->getBool());
            break;
        }
        case ShaderParamType::SPT_INT:
        {
            auto i1 = it.uniform->param.getInt();
            auto i2 = it.value->getInt();
            if (i1 != i2)
            {
                glUniform1i(location, (int)it.value->getInt());
                it.uniform->param.assign(i2);
            }
            break;
        }
        case ShaderParamType::SPT_INT2:
        {
            glUniform2iv(location, 1, it.value->getInt2().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT:
        {
            auto f1 = it.uniform->param.getFloat();
            auto f2 = it.value->getFloat();
            if (f1 != f2)
            {
                glUniform1f(location, f2);
                it.uniform->param.assign(f2);
            }
            break;
        }
        case ShaderParamType::SPT_FLOAT2:
        {
            glUniform2fv(location, 1, it.value->getFloat2().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT3:
        {
            glUniform3fv(location, 1, it.value->getFloat3().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT4:
        {
            glUniform4fv(location, 1, it.value->getFloat4().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT3X3:
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, it.value->getFloat3x3().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT4X4:
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, it.value->getFloat4x4().ptr());
            break;
        }
        case ShaderParamType::SPT_FLOAT_ARRAY:
        {
            glUniform1fv(location, it.value->getFloatArray().size(), it.value->getFloatArray().data());
            break;
        }
        case ShaderParamType::SPT_TEXTURE:
        {
            auto texture = it.value->getTexture();
            if (texture)
                this->setTexture(*texture, it.uniform);
            break;
        }
        default:
            break;
        }
    }
}

bool
OGLRenderer::createRenderBuffer(RenderBuffer& buffer) noexcept
{
    auto vb = buffer.getVertexBuffer();
    auto ib = buffer.getIndexBuffer();

    OGLVertexArray renderBuffer;

    if (!_ARB_vertex_attrib_binding)
    {
        if (_ARB_vertex_array_object)
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

            if (_ARB_vertex_attrib_binding)
            {
                glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
                glVertexAttribFormat(it.getVertexAttrib(), it.getVertexSize(), type, GL_FALSE, 0);
                glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());

                glBindVertexBuffer(it.getVertexAttrib(), renderBuffer.vbo, offset, vb->getVertexByteSize());
            }
            else
            {
                if (_ARB_vertex_array_object)
                {
                    glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
                    glVertexAttribPointer(it.getVertexAttrib(), it.getVertexSize(), type, GL_FALSE, vb->getVertexByteSize(), (void*)offset);
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

        if (_NV_vertex_buffer_unified_memory)
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

        if (_NV_vertex_buffer_unified_memory)
        {
            glGetNamedBufferParameterui64vNV(renderBuffer.ibo, GL_BUFFER_GPU_ADDRESS_NV, &renderBuffer.bindlessIbo);
            glMakeNamedBufferResidentNV(renderBuffer.ibo, GL_READ_ONLY);
        }
    }

    if (_renderBuffers.size() < renderBuffer.vbo)
    {
        _renderBuffers.resize(renderBuffer.vbo * 2);
    }

    _renderBuffers[renderBuffer.vbo] = renderBuffer;

    buffer.setUserData(renderBuffer.vbo);

    return true;
}

void
OGLRenderer::destroyRenderBuffer(RenderBuffer& renderBuffer) noexcept
{
    auto index = renderBuffer.getUserData();
    if (index != 0)
    {
        auto& buffer = _renderBuffers[index];

        if (buffer.vao)
        {
            glDeleteVertexArrays(1, &buffer.vao);
            buffer.vao = 0;
        }

        if (buffer.vbo)
        {
            glDeleteBuffers(1, &buffer.vbo);
            buffer.vbo = 0;
        }

        if (buffer.ibo)
        {
            glDeleteBuffers(1, &buffer.ibo);
            buffer.ibo = 0;
        }
    }
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
    assert(renderBuffer->getUserData());

    if (_renderBuffer != renderBuffer)
    {
        auto buffer = _renderBuffers[renderBuffer->getUserData()];

        if (_NV_vertex_buffer_unified_memory)
        {
            if (buffer.bindlessVbo)
            {
                for (auto& it : buffer.attribs)
                {
                    glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.attrib, buffer.bindlessVbo + it.offset, buffer.vertexSize - it.offset);
                }
            }

            if (buffer.bindlessIbo)
            {
                glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, buffer.bindlessIbo, buffer.indexSize);
            }
        }
        else if (_ARB_vertex_attrib_binding)
        {
            for (auto& it : buffer.attribs)
            {
                glBindVertexBuffer(it.attrib, buffer.vbo, it.offset, it.stride);
            }

            if (buffer.ibo)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
            }
        }
        else if (_ARB_vertex_array_object)
        {
            assert(buffer.vao);
            glBindVertexArray(buffer.vao);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);

            for (auto& it : buffer.attribs)
            {
                glEnableVertexAttribArray(it.attrib);
                glVertexAttribPointer(it.attrib, it.size, it.type, GL_FALSE, it.stride, (void*)it.offset);
            }
        }

        _renderBuffer = renderBuffer;
    }
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
    assert(renderBuffer->getUserData());

    auto vb = renderBuffer->getVertexBuffer();
    auto ib = renderBuffer->getIndexBuffer();
    auto buffer = _renderBuffers[renderBuffer->getUserData()];

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
    auto buffer = _renderBuffers[_renderBuffer->getUserData()];

    assert(buffer.vertexCount >= renderable.startVertice + renderable.numVertices);

    GLenum drawType = OGLTypes::asOGLVertexType(renderable.type);

    if (buffer.indexCount)
    {
        assert(buffer.indexCount >= renderable.startIndice + renderable.numIndices);

        GLenum indexType = buffer.indexType;

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

    if (_textures.size() < instance.texture)
    {
        _renderBuffers.resize(instance.texture * 2);
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

void
OGLRenderer::setTexture(const Texture& texture, ShaderUniformPtr uniform) noexcept
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

void
OGLRenderer::initCommandList() noexcept
{
    initInternals(_NV_command_list, _ARB_bindless_texture);

    _stateSystem.init();
    _stateSystem.generate(1, &_stateIdDraw);
    _stateSystem.generate(1, &_stateIdDrawGeo);

    if (_NV_command_list)
    {
        glCreateStatesNV(1, &_stateObjDraw);
        glCreateStatesNV(1, &_stateObjDrawGeo);

        glGenBuffers(1, &_tokenBuffer);
        glCreateCommandListsNV(1, &_tokenCmdList);
    }
}

void
OGLRenderer::initInternals(bool hwsupport, bool bindlessSupport) noexcept
{
    assert(!hwsupport || (hwsupport && bindlessSupport));

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

    s_nvcmdlist_bindless = bindlessSupport;

    if (hwsupport)
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
OGLRenderer::nvtokenDrawCommandSequence(const void* stream, size_t streamSize, GLenum mode, GLenum type, const StateSystem::State* state) noexcept
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
        switch (cmdtype) {
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
            const ElementAddressCommandNV* cmd = (const ElementAddressCommandNV*)current;
            type = cmd->typeSizeInByte == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
            if (s_nvcmdlist_bindless)
            {
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
            glStencilFuncSeparate(GL_FRONT, state->stencil.funcs[StateSystem::FACE_FRONT].func, cmd->frontStencilRef, state->stencil.funcs[StateSystem::FACE_FRONT].mask);
            glStencilFuncSeparate(GL_BACK, state->stencil.funcs[StateSystem::FACE_BACK].func, cmd->backStencilRef, state->stencil.funcs[StateSystem::FACE_BACK].mask);
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

    StateSystem::StateID lastID = StateSystem::INVALID_ID;

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

        _stateSystem.applyGL(ID, lastID, true);

        lastID = ID;

        std::size_t offset = _tokenSequenceEmu.offsets[i];
        std::size_t size = _tokenSequenceEmu.sizes[i];

        assert(size + offset < _tokenData.size());

        type = nvtokenDrawCommandSequence(&tokens[offset], size, state->basePrimitiveMode, type, state);
    }
}

_NAME_END