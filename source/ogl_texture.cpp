// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/ogl_texture.h>

_NAME_BEGIN

OGLTexture::OGLTexture() noexcept
    : _texture(0)
    , _textureAddr(0)
{
}

OGLTexture::~OGLTexture() noexcept
{
    this->close();
}

bool
OGLTexture::setup() except
{
    auto target = OGLTypes::asOGLTarget(this->getTexDim());
    auto format = OGLTypes::asOGLFormat(this->getTexFormat());
    auto type = OGLTypes::asOGLType(this->getTexFormat());
    auto internalFormat = OGLTypes::asOGLInternalformat(this->getTexFormat());
    auto buf = this->getStream();

    glGenTextures(1, &_texture);
    glBindTexture(target, _texture);

    GLint level = (GLint)this->getLevel();

    GLsizei w = (GLsizei)this->getWidth();
    GLsizei h = (GLsizei)this->getHeight();
    GLsizei depth = (GLsizei)this->getDepth();

    applyTextureWrap(target, this->getTexWrap());
    applyTextureFilter(target, this->getTexFilter());
    applyTextureAnis(target, this->getTexAnisotropy());

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
            if (this->isMultiSample())
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, GL_FALSE);
            else
                glTexImage2D(target, level, internalFormat, w, h, 0, format, type, buf);
        }
    }
    break;
    case GL_TEXTURE_2D_ARRAY:
    {
        if (this->isMultiSample())
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
        if (this->isMultiSample())
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

#if !defined(EGLAPI)
    if (OGLExtenstion::isSupport(ARB_bindless_texture))
    {
        _textureAddr = glGetTextureHandleARB(_texture);
        glMakeTextureHandleResidentARB(_textureAddr);
    }
#endif

    return true;
}

void
OGLTexture::close() noexcept
{
    if (_texture)
    {
        glDeleteTextures(1, &_texture);
        _texture = GL_NONE;
        _textureAddr = GL_NONE;
    }
}

GLuint
OGLTexture::getInstanceID() noexcept
{
    return _texture;
}

GLuint64
OGLTexture::getInstanceAddr() noexcept
{
    return _textureAddr;
}

void
OGLTexture::applyTextureWrap(GLenum target, TextureWrap wrap) noexcept
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
OGLTexture::applyTextureFilter(GLenum target, TextureFilter filter) noexcept
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
OGLTexture::applyTextureAnis(GLenum target, Anisotropy anis) noexcept
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

OGLRenderTarget::OGLRenderTarget() noexcept
    : _fbo(GL_NONE)
{
}

OGLRenderTarget::~OGLRenderTarget() noexcept
{
    this->close();
}

bool
OGLRenderTarget::setup() noexcept
{
    assert(!_fbo);

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    auto sharedDepthTarget = this->getSharedDepthTexture();
    auto sharedStencilTarget = this->getSharedStencilTexture();

    if (sharedDepthTarget == sharedStencilTarget)
    {
        if (sharedDepthTarget)
            this->bindRenderTarget(sharedDepthTarget->getResolveTexture(), Attachment::DEPTH_STENCIL);
    }
    else
    {
        if (sharedDepthTarget)
        {
            this->bindRenderTarget(sharedDepthTarget->getResolveTexture(), Attachment::DEPTH);
        }

        if (sharedStencilTarget)
        {
            this->bindRenderTarget(sharedStencilTarget->getResolveTexture(), Attachment::STENCIL);
        }
    }

    auto resolveFormat = this->getTexFormat();

    if (resolveFormat == PixelFormat::DEPTH24_STENCIL8 || resolveFormat == PixelFormat::DEPTH32_STENCIL8)
    {
        this->bindRenderTarget(this->getResolveTexture(), Attachment::DEPTH_STENCIL);
    }
    else if (resolveFormat == PixelFormat::DEPTH_COMPONENT16 || resolveFormat == PixelFormat::DEPTH_COMPONENT24 || resolveFormat == PixelFormat::DEPTH_COMPONENT32)
    {
        this->bindRenderTarget(this->getResolveTexture(), Attachment::DEPTH);
    }
    else if (resolveFormat == PixelFormat::STENCIL8)
    {
        this->bindRenderTarget(this->getResolveTexture(), Attachment::STENCIL);
    }
    else
    {
        this->bindRenderTarget(this->getResolveTexture(), Attachment::COLOR0);
    }

    if (OGLExtenstion::isSupport(ARB_direct_state_access))
        glNamedFramebufferDrawBuffer(_fbo, GL_COLOR_ATTACHMENT0);
    else
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

    return true;
}

void
OGLRenderTarget::close() noexcept
{
    if (_fbo != GL_NONE)
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = GL_NONE;
    }
}

void
OGLRenderTarget::bindRenderTarget(TexturePtr texture, Attachment attachment) noexcept
{
    auto attribindex = OGLTypes::asOGLAttachment(attachment);
    auto handle = std::dynamic_pointer_cast<OGLTexture>(texture)->getInstanceID();

    switch (texture->getTexDim())
    {
    case TextureDim::DIM_2D:
    {
        if (texture->isMultiSample())
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

std::size_t
OGLRenderTarget::getInstanceID() noexcept
{
    return _fbo;
}

OGLMultiRenderTarget::OGLMultiRenderTarget() noexcept
    : _fbo(GL_NONE)
{
}

OGLMultiRenderTarget::~OGLMultiRenderTarget() noexcept
{
    this->close();
}

bool
OGLMultiRenderTarget::setup() noexcept
{
    assert(GL_NONE == _fbo);

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    GLenum draw[Attachment::COLOR15];
    GLsizei count = 0;

    for (auto& it : this->getRenderTargets())
    {
        TexturePtr texture = it.texture->getResolveTexture();
        if (!texture)
            continue;

        GLenum attachment = OGLTypes::asOGLAttachment(it.location);
        GLuint handle = std::dynamic_pointer_cast<OGLTexture>(texture)->getInstanceID();

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

    if (OGLExtenstion::isSupport(ARB_direct_state_access))
        glNamedFramebufferDrawBuffers(_fbo, count, draw);
    else
        glDrawBuffers(count, draw);

    return true;
}

void
OGLMultiRenderTarget::close() noexcept
{
    if (_fbo != GL_NONE)
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = GL_NONE;
    }
}

void
OGLMultiRenderTarget::onActivate() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void
OGLMultiRenderTarget::onDeactivate() noexcept
{
}

_NAME_END