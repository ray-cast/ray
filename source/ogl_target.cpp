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
#include <ray/ogl_target.h>
#include <ray/ogl_texture.h>

_NAME_BEGIN

OGLFramebuffer::OGLFramebuffer() noexcept
    : _framebuffer(0)
    , _rbo(0)
{
}

OGLFramebuffer::~OGLFramebuffer() noexcept
{
    this->close();
}

void
OGLFramebuffer::setup(const FramebufferDesc& desc) noexcept
{
    assert(!_framebuffer && !_rbo);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    this->setWidth(desc.width);
    this->setHeight(desc.height);
    this->setDepth(desc.depth);

    this->setViewport(desc.viewport);

    this->setClearFlags(desc.clearFlags);
    this->setClearColor(desc.clearColor);
    this->setClearDepth(desc.clearDepth);
    this->setClearStencil(desc.clearStencil);

    this->setTexFilter(desc.resolveFilter);
    this->setTexFormat(desc.resolveFormat);
    this->setTexMipmap(desc.resolveMipmap);
    this->setTexDim(desc.resolveMapping);
    this->setTexOp(desc.resolveTexOp);
    this->setTexWrap(desc.resolveWrap);

    this->setSharedDepthTexture(desc.sharedDepthTexture);
    this->setSharedStencilTexture(desc.sharedStencilTexture);

    TextureDesc param;
    param.size.x = this->getWidth();
    param.size.y = this->getHeight();
    param.mipmap = this->isMipmap();
    param.format = this->getTexFormat();
    param.filter = this->getTexFilter();
    param.wrap = this->getTexWrap();
    param.texop = this->getTexOp();
    param.dim = this->getTexDim();

    auto texture = std::make_shared<Texture>();
    texture->setup(param);

    _resolveTexture = texture;

    auto sharedDepthTarget = this->getSharedDepthTexture();
    auto sharedStencilTarget = this->getSharedStencilTexture();
    if (sharedDepthTarget == sharedStencilTarget)
    {
        auto target = std::dynamic_pointer_cast<OGLFramebuffer>(sharedDepthTarget);
        if (target)
        {
            _sharedStencil = _sharedDepth = target;

            if (_sharedDepth)
            {
                this->bindTexture(target->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
            }
        }
    }
    else
    {
        if (sharedDepthTarget)
        {
            auto target = std::dynamic_pointer_cast<OGLFramebuffer>(sharedDepthTarget);
            if (target)
            {
                _sharedDepth = target;

                if (_sharedDepth)
                {
                    this->bindTexture(target->getResolveTexture(), GL_DEPTH_ATTACHMENT);
                }
            }
        }

        if (sharedStencilTarget)
        {
            auto target = std::dynamic_pointer_cast<OGLFramebuffer>(sharedStencilTarget);
            if (target)
            {
                _sharedStencil = target;

                if (_sharedStencil)
                {
                    this->bindTexture(target->getResolveTexture(), GL_STENCIL_ATTACHMENT);
                }
            }
        }
    }

    if (desc.resolveFormat == PixelFormat::DEPTH24_STENCIL8 ||
        desc.resolveFormat == PixelFormat::DEPTH32_STENCIL8)
    {
        this->bindTexture(this->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
    }
    else if (desc.resolveFormat == PixelFormat::DEPTH_COMPONENT16 ||
        desc.resolveFormat == PixelFormat::DEPTH_COMPONENT24 ||
        desc.resolveFormat == PixelFormat::DEPTH_COMPONENT32)
    {
        this->bindTexture(this->getResolveTexture(), GL_DEPTH_ATTACHMENT);
    }
    else
    {
        this->bindTexture(this->getResolveTexture(), GL_COLOR_ATTACHMENT0);
        /*if (!_sharedDepth && !_sharedStencil)
        {
            glGenRenderbuffers(1, &_rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, desc.width, desc.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
        }*/
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void
OGLFramebuffer::close() noexcept
{
    if (_resolveTexture)
    {
        _resolveTexture.reset();
        _resolveTexture = nullptr;
    }

    if (_framebuffer)
    {
        glDeleteFramebuffers(1, &_framebuffer);
        _framebuffer = 0;
    }
}

void
OGLFramebuffer::bind() noexcept
{
    GLenum  draw[1] = { GL_COLOR_ATTACHMENT0 };

    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glDrawBuffers(1, draw);
}

void
OGLFramebuffer::unbind() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
OGLFramebuffer::bitblit(const Viewport& src, FramebufferPtr target, const Viewport& dest) noexcept
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebuffer);

    if (target)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, std::dynamic_pointer_cast<OGLFramebuffer>(target)->_framebuffer);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(src.left, src.top, src.width, src.height, dest.left, dest.top, dest.width, dest.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void
OGLFramebuffer::bindTexture(TexturePtr texture, GLenum attachment) const noexcept
{
    switch (texture->getTexDim())
    {
    case TextureDim::DIM_2D:
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getUserData(), 0);
        break;
    }
    case TextureDim::DIM_2D_ARRAY:
    {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture->getUserData(), 0, 0);
        break;
    }
#if !defined(EGLAPI)
    case TextureDim::DIM_3D:
    {
        glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, texture->getUserData(), 0, 0);
        break;
    }
#endif
    case TextureDim::DIM_CUBE:
    {
    }
    }
}

OGLMultiFramebuffer::OGLMultiFramebuffer() noexcept
    : _framebuffer(0)
{
}

OGLMultiFramebuffer::~OGLMultiFramebuffer() noexcept
{
    this->close();
}

void
OGLMultiFramebuffer::setup(const MultiFramebufferDesc& desc) noexcept
{
    assert(!_framebuffer);

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    _drawbuffers.clear();

    this->setClearFlags(desc.clearFlags);
    this->setViewport(desc.viewport);

    for (auto& it : desc.mrt)
    {
        auto oglTarget = std::dynamic_pointer_cast<OGLFramebuffer>(it);
        if (!oglTarget)
            continue;

        TexturePtr texture = oglTarget->getResolveTexture();
        if (!texture)
            continue;

        GLenum attachment = OGLTypes::asOGLAttachment(oglTarget->getAttachment());

        auto dim = texture->getTexDim();
        if (dim == TextureDim::DIM_2D)
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getUserData(), 0);
        else if (dim == TextureDim::DIM_2D_ARRAY)
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture->getUserData(), 0, 0);
#if !defined(EGLAPI)
        else if (dim == TextureDim::DIM_3D)
            glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, texture->getUserData(), 0, 0);
#endif
        else
            assert(false);

        _drawbuffers.push_back(std::make_pair(it, attachment));

        this->attach(it);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
OGLMultiFramebuffer::close() noexcept
{
    if (_framebuffer)
    {
        glDeleteFramebuffers(1, &_framebuffer);
        _framebuffer = 0;
    }
}

void
OGLMultiFramebuffer::bind() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    std::size_t count = 0;
    GLenum  draw[Attachment::COLOR15];

    for (auto& it : _drawbuffers)
    {
        if (it.second != GL_DEPTH_ATTACHMENT &&
            it.second != GL_DEPTH_STENCIL_ATTACHMENT &&
            it.second != GL_STENCIL_ATTACHMENT)
        {
            draw[count++] = it.second;
        }
    }

    glDrawBuffers(count, draw);
}

void
OGLMultiFramebuffer::unbind() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const OGLMultiFramebuffer::DrawBuffers&
OGLMultiFramebuffer::getDrawBuffers() const noexcept
{
    return _drawbuffers;
}

_NAME_END