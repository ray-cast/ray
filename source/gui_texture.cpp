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
#include <ray/gui_texture.h>
#include <ray/gui_renderer.h>
#include <ray/gui_assert.h>
#include <ray/gui_system.h>
#include <ray/render_texture.h>
#include <ray/render_system.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

using namespace Gui;
using namespace MyGUI;

GuiTexture::GuiTexture(const std::string& _name, Gui::GuiImageLoaderPtr _loader) noexcept
	: _name(_name)
	, _width(0)
	, _height(0)
	, _originalUsage(TextureUsage::Default)
	, _numElemBytes(0)
	, _dataSize(0)
	, _lock(false)
	, _imageLoader(_loader)
	, _renderTarget(nullptr)
{
}

GuiTexture::~GuiTexture() noexcept
{
	this->destroy();
}

const std::string& 
GuiTexture::getName() const noexcept
{
	return _name;
}

void 
GuiTexture::setUsage(TextureUsage usage) noexcept
{
	_originalUsage = usage;
}

MyGUI::TextureUsage 
GuiTexture::getUsage() const noexcept
{
	return _originalUsage;
}

void
GuiTexture::createManual(int _width, int _height, TextureUsage _usage, MyGUI::PixelFormat _format) except
{
	createManual(_width, _height, _usage, _format, 0);
}

void 
GuiTexture::createManual(int width, int height, TextureUsage usage, MyGUI::PixelFormat _format, void* _data) except
{
	MYGUI_PLATFORM_ASSERT(!_texture, "Texture already exist");

	ray::PixelFormat pixelFormat = ray::PixelFormat::R8G8B8;
	_numElemBytes = 0;
	if (_format == MyGUI::PixelFormat::R8G8B8)
	{
		pixelFormat = ray::PixelFormat::R8G8B8;
		_numElemBytes = 3;
	}
	else if (_format == MyGUI::PixelFormat::R8G8B8A8)
	{
		pixelFormat = ray::PixelFormat::R8G8B8A8;
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

	_texture = RenderFactory::createTexture();
	_texture->setWidth(_width);
	_texture->setHeight(_height);
	_texture->setTexFilter(TextureFilter::GPU_LINEAR);
	_texture->setTexDim(TextureDim::DIM_2D);
	_texture->setTexFormat(pixelFormat);
	_texture->setStream(_data);
	_texture->setup();
}

void 
GuiTexture::destroy() noexcept
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
GuiTexture::lock(TextureUsage _access) noexcept
{
	MYGUI_PLATFORM_ASSERT(!_lock, "Texture is locked");
	MYGUI_PLATFORM_ASSERT(_texture, "Texture is not created");
	_lock = true;
	return _stream.map();
}

void
GuiTexture::unlock() noexcept
{
	MYGUI_PLATFORM_ASSERT(_lock, "Texture is not locked");
	_lock = false;
}

void
GuiTexture::loadFromFile(const std::string& _filename)
{
	this->destroy();

	if (_imageLoader)
	{
		int width = 0;
		int height = 0;
		Gui::PixelFormat format = Gui::PixelFormat::Unknow;

		if (_imageLoader->loadImage(width, height, format, _filename, _stream))
		{
			MyGUI::PixelFormat pfd = MyGUI::PixelFormat::Unknow;
			if (format == Gui::PixelFormat::L8)
				pfd = MyGUI::PixelFormat::L8;
			else if (format == Gui::PixelFormat::L8A8)
				pfd = MyGUI::PixelFormat::L8A8;
			else if (format == Gui::PixelFormat::R8G8B8)
				pfd = MyGUI::PixelFormat::R8G8B8;
			else if (format == Gui::PixelFormat::R8G8B8A8)
				pfd = MyGUI::PixelFormat::R8G8B8A8;
			else
				assert(false);

			this->createManual(width, height, TextureUsage::Static | TextureUsage::Write, pfd, _stream.map());
		}
	}
}

void 
GuiTexture::saveToFile(const std::string& _filename)
{
	if (_imageLoader)
	{
		void* data = this->lock(TextureUsage::Read);

		Gui::PixelFormat format = Gui::PixelFormat::Unknow;
		if (_originalFormat == MyGUI::PixelFormat::L8)
			format = Gui::PixelFormat::L8;
		else if (_originalFormat == MyGUI::PixelFormat::L8A8)
			format = Gui::PixelFormat::L8A8;
		else if (_originalFormat == MyGUI::PixelFormat::R8G8B8)
			format = Gui::PixelFormat::R8G8B8;
		else if (_originalFormat == MyGUI::PixelFormat::R8G8B8A8)
			format = Gui::PixelFormat::R8G8B8A8;
		else
			assert(false);

		_imageLoader->saveImage(_width, _height, format, data, _filename);

		this->unlock();
	}
}

IRenderTarget* 
GuiTexture::getRenderTarget()
{
	assert(_texture);
	if (_renderTarget == nullptr)
		_renderTarget = new GuiRenderTexture(_texture);		
	return _renderTarget;
}

TexturePtr
GuiTexture::getTexture() const noexcept
{
	assert(_texture);
	return _texture;
}

int 
GuiTexture::getWidth() noexcept
{
	return _width;
}

int 
GuiTexture::getHeight() noexcept
{
	return _height;
}

bool 
GuiTexture::isLocked() noexcept
{
	return _lock;
}

MyGUI::PixelFormat 
GuiTexture::getFormat() noexcept
{
	return _originalFormat;
}

TextureUsage GuiTexture::getUsage() noexcept
{
	return _originalUsage;
}

size_t GuiTexture::getNumElemBytes() noexcept
{
	return _numElemBytes;
}

GuiRenderTexture::GuiRenderTexture(TexturePtr texture) noexcept
{
	_renderTargetInfo.maximumDepth = 1.0f;
	_renderTargetInfo.hOffset = 0;
	_renderTargetInfo.vOffset = 0;
	_renderTargetInfo.aspectCoef = float(texture->getHeight()) / float(texture->getWidth());
	_renderTargetInfo.pixScaleX = 1.0f / float(texture->getWidth());
	_renderTargetInfo.pixScaleY = 1.0f / float(texture->getHeight());

	_renderTexture = RenderFactory::createRenderTexture();
	_renderTexture->setup(texture);
}

GuiRenderTexture::~GuiRenderTexture() noexcept
{
}

void
GuiRenderTexture::begin() noexcept
{
	RenderSystem::instance()->getRenderPipeline()->setRenderTexture(_renderTexture);
	RenderSystem::instance()->getRenderPipeline()->clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4(0, 0, 0, 0), 1.0, 0);
}

void 
GuiRenderTexture::end() noexcept
{
	RenderSystem::instance()->getRenderPipeline()->setRenderTexture(nullptr);
}

void 
GuiRenderTexture::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count) noexcept
{
	Gui::GuiRenderer::getInstance().doRenderRTT(_buffer, _texture, _count);
}

const MyGUI::RenderTargetInfo& 
GuiRenderTexture::getInfo() noexcept
{
	return _renderTargetInfo;
}

_NAME_END
