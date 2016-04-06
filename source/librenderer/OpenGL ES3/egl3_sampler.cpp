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
#include "egl3_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Sampler, GraphicsSampler, "EGL3Sampler")

EGL3Sampler::EGL3Sampler() noexcept
	: _sampler(GL_NONE)
{
}

EGL3Sampler::~EGL3Sampler() noexcept
{
	this->close();
}

bool
EGL3Sampler::setup(const GraphicsSamplerDesc& samplerDesc) except
{
	assert(_sampler == GL_NONE);

	glGenSamplers(1, &_sampler);
	if (_sampler == GL_NONE)
	{
		GL_PLATFORM_LOG("glGenSamplers() fail");
		return false;
	}

	GraphicsSamplerWrap wrap = samplerDesc.getSamplerWrap();
	if (GraphicsSamplerWrap::GraphicsSamplerWrapRepeat == wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge == wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (GraphicsSamplerWrap::GraphicsSamplerWrapMirror == wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}
	else
	{
		GL_PLATFORM_LOG("Invalid SamplerWrap");
		return false;
	}

	GraphicsSamplerFilter filter = samplerDesc.getSamplerFilter();
	if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterNearest)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterLinear)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapLinear)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		GL_PLATFORM_LOG("Invalid SamplerFilter");
		return false;
	}

	GraphicsSamplerAnis anis = samplerDesc.getSamplerAnis();
	if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis1)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis2)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis4)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis8)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis16)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	else
	{
		GL_PLATFORM_LOG("Invalid SamplerAnis");
		return false;
	}

	_sampleDesc = samplerDesc;
	return true;
}

void
EGL3Sampler::close() noexcept
{
	if (_sampler)
	{
		glDeleteSamplers(1, &_sampler);
		_sampler = 0;
	}
}

GLuint
EGL3Sampler::getInstanceID() noexcept
{
	return _sampler;
}

const GraphicsSamplerDesc&
EGL3Sampler::getGraphicsSamplerDesc() const noexcept
{
	return _sampleDesc;
}

void
EGL3Sampler::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Sampler::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END