// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/graphics_view.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsRenderTexture, GraphicsResource, "GraphicsRenderTexture")
__ImplementSubInterface(GraphicsMultiRenderTexture, GraphicsResource, "GraphicsMultiRenderTexture")

GraphicsRenderTextureDesc::GraphicsRenderTextureDesc() noexcept
	: _sharedDepthTexture(0)
	, _sharedStencilTexture(0)
{
}

GraphicsRenderTextureDesc::~GraphicsRenderTextureDesc() noexcept
{
}

void 
GraphicsRenderTextureDesc::setGraphicsTexture(GraphicsTexturePtr texture) noexcept
{
	_texture = texture;
}

GraphicsTexturePtr
GraphicsRenderTextureDesc::getGraphicsTexture() const noexcept
{
	return _texture;
}

void
GraphicsRenderTextureDesc::setSharedDepthTexture(GraphicsRenderTexturePtr target) noexcept
{
	_sharedDepthTexture = target;
}

void
GraphicsRenderTextureDesc::setSharedStencilTexture(GraphicsRenderTexturePtr target) noexcept
{
	_sharedStencilTexture = target;
}

GraphicsRenderTexturePtr
GraphicsRenderTextureDesc::getSharedDepthTexture() const noexcept
{
	return _sharedDepthTexture;
}

GraphicsRenderTexturePtr
GraphicsRenderTextureDesc::getSharedStencilTexture() const noexcept
{
	return _sharedStencilTexture;
}

GraphicsMultiRenderTextureDesc::GraphicsMultiRenderTextureDesc() noexcept
{
}

GraphicsMultiRenderTextureDesc::~GraphicsMultiRenderTextureDesc() noexcept
{
}

void
GraphicsMultiRenderTextureDesc::attach(GraphicsRenderTexturePtr texture) noexcept
{
	assert(texture);
	assert(std::find(_textures.begin(), _textures.end(), texture) == _textures.end());

	_textures.push_back(texture);
}

void
GraphicsMultiRenderTextureDesc::detach(GraphicsRenderTexturePtr texture) noexcept
{
	auto it = std::find(_textures.begin(), _textures.end(), texture);
	if (it != _textures.end())
		_textures.erase(it);
}

GraphicsRenderTextures&
GraphicsMultiRenderTextureDesc::getRenderTextures() noexcept
{
	return _textures;
}

const GraphicsRenderTextures&
GraphicsMultiRenderTextureDesc::getRenderTextures() const noexcept
{
	return _textures;
}

void
GraphicsMultiRenderTextureDesc::setSharedDepthTexture(GraphicsRenderTexturePtr target) noexcept
{
	_sharedDepthTexture = target;
}

void
GraphicsMultiRenderTextureDesc::setSharedStencilTexture(GraphicsRenderTexturePtr target) noexcept
{
	_sharedStencilTexture = target;
}

GraphicsRenderTexturePtr
GraphicsMultiRenderTextureDesc::getSharedDepthTexture() const noexcept
{
	return _sharedDepthTexture;
}

GraphicsRenderTexturePtr
GraphicsMultiRenderTextureDesc::getSharedStencilTexture() const noexcept
{
	return _sharedStencilTexture;
}

GraphicsRenderTexture::GraphicsRenderTexture() noexcept
{
}

GraphicsRenderTexture::~GraphicsRenderTexture() noexcept
{
}

GraphicsMultiRenderTexture::GraphicsMultiRenderTexture() noexcept
{
}

GraphicsMultiRenderTexture::~GraphicsMultiRenderTexture() noexcept
{
}

_NAME_END