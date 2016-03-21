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
#if defined(_BUILD_MYGUI)
#include "mygui_texture.h"
#include "mygui_renderer.h"
#include "mygui_system.h"

#include <ray/graphics_view.h>

_NAME_BEGIN

using namespace MyGUI;

MyGuiTexture::MyGuiTexture(const std::string& _name, GuiImageLoaderPtr _loader) noexcept
	: _name(_name)
	, _lock(false)
	, _width(0)
	, _height(0)
	, _numElemBytes(0)
	, _dataSize(0)
	, _originalUsage(TextureUsage::Default)
	, _imageLoader(_loader)
	, _renderTarget(nullptr)
{
}

MyGuiTexture::~MyGuiTexture() noexcept
{
	this->destroy();
}

const std::string& 
MyGuiTexture::getName() const noexcept
{
	return _name;
}

void 
MyGuiTexture::setUsage(TextureUsage usage) noexcept
{
	_originalUsage = usage;
}

MyGUI::TextureUsage 
MyGuiTexture::getUsage() const noexcept
{
	return _originalUsage;
}

void
MyGuiTexture::createManual(int width, int height, TextureUsage usage, MyGUI::PixelFormat format) except
{
	createManual(width, height, usage, format, 0);
}

void 
MyGuiTexture::createManual(int width, int height, TextureUsage usage, MyGUI::PixelFormat _format, void* _data) except
{
	MYGUI_PLATFORM_ASSERT(!_texture, "Texture already exist");

	GraphicsFormat pixelFormat = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	_numElemBytes = 0;
	if (_format == MyGUI::PixelFormat::R8G8B8)
	{
		pixelFormat = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
		_numElemBytes = 3;
	}
	else if (_format == MyGUI::PixelFormat::R8G8B8A8)
	{
		pixelFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
		_numElemBytes = 4;
	}
	else
	{
		MYGUI_PLATFORM_EXCEPT("format not support");
	}

	_width = width;
	_height = height;
	_dataSize = _width * _height * _numElemBytes;

	_originalFormat = _format;
	_originalUsage = usage;

	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(_width);
	textureDesc.setHeight(_height);
	textureDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	textureDesc.setTexFormat(pixelFormat);
	textureDesc.setStream(_data);

	_texture = RenderSystem::instance()->createTexture(textureDesc);
}

void 
MyGuiTexture::destroy() noexcept
{
	if (_renderTarget != nullptr)
	{
		delete _renderTarget;
		_renderTarget = nullptr;
	}

	_width = 0;
	_height = 0;
	_lock = false;
	_dataSize = 0;
	_stream.clear();
	_numElemBytes = 0;
	_originalUsage = MyGUI::TextureUsage::Default;
	_originalFormat = MyGUI::PixelFormat::Unknow;
	_originalUsage = MyGUI::TextureUsage::Default;
}

void* 
MyGuiTexture::lock(TextureUsage _access) noexcept
{
	MYGUI_PLATFORM_ASSERT(!_lock, "Texture is locked");
	MYGUI_PLATFORM_ASSERT(_texture, "Texture is not created");
	_lock = true;
	return _stream.map();
}

void
MyGuiTexture::unlock() noexcept
{
	MYGUI_PLATFORM_ASSERT(_lock, "Texture is not locked");
	_lock = false;
}

void
MyGuiTexture::loadFromFile(const std::string& _filename)
{
	this->destroy();

	if (_imageLoader)
	{
		int width = 0;
		int height = 0;
		
		auto format = GuiImageLoader::PixelFormat::Unknow;

		if (_imageLoader->loadImage(width, height, format, _filename, _stream))
		{
			MyGUI::PixelFormat pfd = MyGUI::PixelFormat::Unknow;
			if (format == GuiImageLoader::PixelFormat::L8)
				pfd = MyGUI::PixelFormat::L8;
			else if (format == GuiImageLoader::PixelFormat::L8A8)
				pfd = MyGUI::PixelFormat::L8A8;
			else if (format == GuiImageLoader::PixelFormat::R8G8B8)
				pfd = MyGUI::PixelFormat::R8G8B8;
			else if (format == GuiImageLoader::PixelFormat::R8G8B8A8)
				pfd = MyGUI::PixelFormat::R8G8B8A8;
			else
				assert(false);

			this->createManual(width, height, TextureUsage::Static | TextureUsage::Write, pfd, _stream.map());
		}
	}
}

void 
MyGuiTexture::saveToFile(const std::string& _filename)
{
	if (_imageLoader)
	{
		void* data = this->lock(TextureUsage::Read);

		auto format = GuiImageLoader::PixelFormat::Unknow;
		if (_originalFormat == MyGUI::PixelFormat::L8)
			format = GuiImageLoader::PixelFormat::L8;
		else if (_originalFormat == MyGUI::PixelFormat::L8A8)
			format = GuiImageLoader::PixelFormat::L8A8;
		else if (_originalFormat == MyGUI::PixelFormat::R8G8B8)
			format = GuiImageLoader::PixelFormat::R8G8B8;
		else if (_originalFormat == MyGUI::PixelFormat::R8G8B8A8)
			format = GuiImageLoader::PixelFormat::R8G8B8A8;
		else
			assert(false);

		_imageLoader->saveImage(_width, _height, format, data, _filename);

		this->unlock();
	}
}

IRenderTarget* 
MyGuiTexture::getRenderTarget()
{
	assert(_texture);
	if (_renderTarget == nullptr)
		_renderTarget = new MyGuiRenderTexture(_texture);		
	return _renderTarget;
}

GraphicsTexturePtr
MyGuiTexture::getTexture() const noexcept
{
	assert(_texture);
	return _texture;
}

int 
MyGuiTexture::getWidth() noexcept
{
	return _width;
}

int 
MyGuiTexture::getHeight() noexcept
{
	return _height;
}

bool 
MyGuiTexture::isLocked() noexcept
{
	return _lock;
}

MyGUI::PixelFormat 
MyGuiTexture::getFormat() noexcept
{
	return _originalFormat;
}

TextureUsage
MyGuiTexture::getUsage() noexcept
{
	return _originalUsage;
}

std::size_t
MyGuiTexture::getNumElemBytes() noexcept
{
	return _numElemBytes;
}

MyGuiRenderTexture::MyGuiRenderTexture(GraphicsTexturePtr texture) noexcept
{
	auto& textureDes = texture->getGraphicsTextureDesc();

	_renderTargetInfo.maximumDepth = 1.0f;
	_renderTargetInfo.hOffset = 0;
	_renderTargetInfo.vOffset = 0;
	_renderTargetInfo.aspectCoef = float(textureDes.getHeight()) / float(textureDes.getWidth());
	_renderTargetInfo.pixScaleX = 1.0f / float(textureDes.getWidth());
	_renderTargetInfo.pixScaleY = 1.0f / float(textureDes.getHeight());

	GraphicsRenderTextureDesc framebufferDesc;
	framebufferDesc.attach(texture);
	_renderTexture = RenderSystem::instance()->createRenderTexture(framebufferDesc);
}

MyGuiRenderTexture::~MyGuiRenderTexture() noexcept
{
}

void
MyGuiRenderTexture::begin() noexcept
{
	RenderSystem::instance()->getRenderPipeline()->setRenderTexture(_renderTexture);
	RenderSystem::instance()->getRenderPipeline()->clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsAll, Vector4(0, 0, 0, 0), 1.0, 0);
}

void 
MyGuiRenderTexture::end() noexcept
{
	RenderSystem::instance()->getRenderPipeline()->setRenderTexture(nullptr);
}

void 
MyGuiRenderTexture::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count) noexcept
{
	MyGuiRenderer::getInstance().doRenderRTT(_buffer, _texture, _count);
}

const MyGUI::RenderTargetInfo& 
MyGuiRenderTexture::getInfo() noexcept
{
	return _renderTargetInfo;
}

_NAME_END
#endif