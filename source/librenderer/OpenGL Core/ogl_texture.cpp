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
#include "ogl_texture.h"

_NAME_BEGIN

__ImplementSubClass(OGLTexture, GraphicsTexture, "OGLTexture")

OGLTexture::OGLTexture() noexcept
	: _texture(0)
	, _target(GL_NONE)
{
}

OGLTexture::~OGLTexture() noexcept
{
	this->close();
}

bool
OGLTexture::setup(const GraphicsTextureDesc& textureDesc) except
{
	auto target = OGLTypes::asOGLTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	auto internalFormat = OGLTypes::asOGLInternalformat(textureDesc.getTexFormat());
	auto stream = textureDesc.getStream();

	if (target == GL_INVALID_ENUM)
		return false;

	if (internalFormat == GL_INVALID_ENUM)
		return false;

	glCreateTextures(target, 1, &_texture);

	GLsizei w = (GLsizei)textureDesc.getWidth();
	GLsizei h = (GLsizei)textureDesc.getHeight();
	GLsizei depth = (GLsizei)textureDesc.getDepth();

	this->applySamplerWrap(textureDesc.getSamplerWrap());
	this->applySamplerFilter(textureDesc.getSamplerFilter());
	this->applySamplerAnis(textureDesc.getSamplerAnis());

	GLsizei level = target == GL_TEXTURE_CUBE_MAP ? 6 : 1;
	level = std::max(level, textureDesc.getMipLevel());

	if (target == GL_TEXTURE_2D)
		glTextureStorage2D(_texture, level, internalFormat, w, h);
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		glTextureStorage2DMultisample(_texture, level, internalFormat, w, h, GL_FALSE);
	else if (target == GL_TEXTURE_2D_ARRAY)
		glTextureStorage3D(_texture, level, internalFormat, w, h, depth);
	else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		glTextureStorage3DMultisample(_texture, level, internalFormat, w, h, depth, GL_FALSE);
	else if (target == GL_TEXTURE_3D)
		glTextureStorage3D(_texture, level, internalFormat, w, h, depth);
	else if (target == GL_TEXTURE_CUBE_MAP)
		glTextureStorage2D(_texture, level, internalFormat, w, h);
	else if (target == GL_TEXTURE_CUBE_MAP_ARRAY)
		glTextureStorage3D(_texture, level, internalFormat, w, h, depth);

	if (stream)
	{
		if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ||
			internalFormat == GL_COMPRESSED_RG_RGTC2 ||
			internalFormat == GL_COMPRESSED_SIGNED_RG_RGTC2)
		{
			GLsizei offset = 0;
			GLsizei blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

			for (GLint mip = 0; mip < level; mip++)
			{
				GLsizei mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

				glCompressedTextureSubImage2D(_texture, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

				w = std::max(w >> 1, 1);
				h = std::max(h >> 1, 1);

				offset += mipSize;
			}
		}
		else
		{
			GLenum format = OGLTypes::asOGLFormat(textureDesc.getTexFormat());
			GLenum type = OGLTypes::asOGLType(textureDesc.getTexFormat());
			
			switch (target)
			{
			case GL_TEXTURE_2D:
				glTextureSubImage2D(_texture, 0, 0, 0, w, h, format, type, stream);
				break;
			case GL_TEXTURE_2D_ARRAY:
				glTextureSubImage3D(_texture, 0, 0, 0, 0, w, h, depth, format, type, stream);
				break;
			case GL_TEXTURE_3D:
				glTextureSubImage3D(_texture, 0, 0, 0, 0, w, h, depth, format, type, stream);
				break;
			case GL_TEXTURE_CUBE_MAP:
				glTextureSubImage3D(_texture, 0, 0, 0, 0, w, h, 6, format, type, stream);
				break;
			break;
			default:
				assert(false);
				break;
			}
		}
	}

	if (textureDesc.isMipmap())
		glGenerateTextureMipmap(_texture);

	_target = target;
	_textureDesc = textureDesc;

	return true;
}

void
OGLTexture::close() noexcept
{
	if (_texture)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}
}

bool
OGLTexture::isMultiSample() const noexcept
{
	return _textureDesc.isMultiSample();
}

GLenum
OGLTexture::getTarget() const noexcept
{
	return _target;
}

GLuint
OGLTexture::getInstanceID() noexcept
{
	return _texture;
}

const GraphicsTextureDesc&
OGLTexture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
OGLTexture::applySamplerWrap(SamplerWrap wrap) noexcept
{
	if (SamplerWrap::Repeat == wrap)
	{
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (SamplerWrap::ClampToEdge == wrap)
	{
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (SamplerWrap::Mirror == wrap)
	{
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}
}

void
OGLTexture::applySamplerFilter(SamplerFilter filter) noexcept
{
	if (filter == SamplerFilter::Nearest)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == SamplerFilter::Linear)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == SamplerFilter::NearestMipmapLinear)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == SamplerFilter::NearestMipmapNearest)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == SamplerFilter::LinearMipmapNearest)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == SamplerFilter::LinearMipmapLinear)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
}

void
OGLTexture::applySamplerAnis(SamplerAnis anis) noexcept
{
	if (anis == SamplerAnis::Anis1)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == SamplerAnis::Anis2)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == SamplerAnis::Anis4)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == SamplerAnis::Anis8)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == SamplerAnis::Anis16)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

_NAME_END