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

#include <GL/glew.h>

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
GuiTexture::createManual(int _width, int _height, TextureUsage _usage, MyGUI::PixelFormat _format)
{
	createManual(_width, _height, _usage, _format, 0);
}

void 
GuiTexture::createManual(int width, int height, TextureUsage usage, MyGUI::PixelFormat _format, void* _data)
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
	/*if (_renderTarget == nullptr)
		_renderTarget = new GuiRenderTexture(_texture);
	return _renderTarget;*/
	assert(nullptr);
	return nullptr;
}

TexturePtr
GuiTexture::getTexture() const noexcept
{
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

GuiRenderTexture::GuiRenderTexture(unsigned int _texture) 
	: mTextureID(_texture)
	, mWidth(0)
	, mHeight(0)
	, mFBOID(0)
	, mRBOID(0)
{
	int miplevel = 0;
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &mWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &mHeight);
	glBindTexture(GL_TEXTURE_2D, 0);

	_renderTargetInfo.maximumDepth = 1.0f;
	_renderTargetInfo.hOffset = 0;
	_renderTargetInfo.vOffset = 0;
	_renderTargetInfo.aspectCoef = float(mHeight) / float(mWidth);
	_renderTargetInfo.pixScaleX = 1.0f / float(mWidth);
	_renderTargetInfo.pixScaleY = 1.0f / float(mHeight);

	// create a framebuffer object, you need to delete them when program exits.
	glGenFramebuffersEXT(1, &mFBOID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBOID);

	// create a renderbuffer object to store depth info
	// NOTE: A depth renderable image should be attached the FBO for depth test.
	// If we don't attach a depth renderable image to the FBO, then
	// the rendering output will be corrupted because of missing depth test.
	// If you also need stencil test for your rendering, then you must
	// attach additional image to the stencil attachement point, too.
	glGenRenderbuffersEXT(1, &mRBOID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRBOID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mWidth, mHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTextureID, 0);

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRBOID);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GuiRenderTexture::~GuiRenderTexture()
{
	if (mFBOID != 0)
	{
		glDeleteFramebuffersEXT(1, &mFBOID);
		mFBOID = 0;
	}
	if (mRBOID != 0)
	{
		glDeleteRenderbuffersEXT(1, &mRBOID);
		mRBOID = 0;
	}
}

void
GuiRenderTexture::begin()
{
	glGetIntegerv(GL_VIEWPORT, mSavedViewport); // save current viewport

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBOID);

	glViewport(0, 0, mWidth, mHeight);

	Gui::GuiRenderer::getInstance().begin();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void 
GuiRenderTexture::end()
{
	Gui::GuiRenderer::getInstance().end();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // unbind

	glViewport(mSavedViewport[0], mSavedViewport[1], mSavedViewport[2], mSavedViewport[3]); // restore old viewport
}

void 
GuiRenderTexture::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count)
{
	Gui::GuiRenderer::getInstance().doRenderRTT(_buffer, _texture, _count);
}

const MyGUI::RenderTargetInfo& 
GuiRenderTexture::getInfo()
{
	return _renderTargetInfo;
}

_NAME_END
