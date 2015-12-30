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
#include "mygui_renderer.h"
#include "mygui_texture.h"
#include "mygui_buffer.h"

_NAME_BEGIN

MyGuiRenderer::MyGuiRenderer() noexcept
	: _imageLoader(nullptr)
	, _isInitialise(false)
	, _isUpdate(true)
	, _vertexFormat(MyGUI::VertexColourType::ColourABGR)
{
}

MyGuiRenderer::~MyGuiRenderer() noexcept
{
	this->close();
}

MyGuiRenderer&
MyGuiRenderer::getInstance() noexcept
{
	return *getInstancePtr();
}

MyGuiRenderer*
MyGuiRenderer::getInstancePtr() noexcept
{
	return static_cast<MyGuiRenderer*>(RenderManager::getInstancePtr());
}

void 
MyGuiRenderer::open() except
{
	MYGUI_PLATFORM_ASSERT(!_isInitialise, getClassTypeName() << " initialised twice");
	MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

	_material = RenderSystem::instance()->createMaterial("sys:fx/uilayout.fxml.o");
	_materialPass = _material->getTech(RenderQueue::RQ_OPAQUE)->getPass("ui");
	_materialDecal = _material->getParameter("decal");
	_materialScaleY = _material->getParameter("scaleY");
	_materialScaleY->assign(1);

	_isInitialise = true;

	MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
}

void 
MyGuiRenderer::close() noexcept
{
	if (_isInitialise)
	{
		MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

		destroyAllResources();

		_isInitialise = false;

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
	}
}

void
MyGuiRenderer::setImageLoader(GuiImageLoaderPtr loader) noexcept
{
	_imageLoader = loader;
}

GuiImageLoaderPtr
MyGuiRenderer::getImageLoader() const noexcept
{
	return _imageLoader;
}

MyGUI::IVertexBuffer* 
MyGuiRenderer::createVertexBuffer() noexcept
{
	return new MyGuiVertexBuffer();
}

void 
MyGuiRenderer::destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer) noexcept
{
	delete _buffer;
}

MyGUI::ITexture*
MyGuiRenderer::createTexture(const std::string& _name) noexcept
{
	auto& texture = _textures[_name];
	if (!texture)
	{
		texture = std::make_unique<MyGuiTexture>(_name, _imageLoader);
		return texture.get();
	}

	MYGUI_PLATFORM_LOG(Info, "Texture '" << _name << "' already exist");
	return texture.get();
}

void
MyGuiRenderer::destroyTexture(MyGUI::ITexture* _texture) noexcept
{
	if (_texture)
	{
		auto& texture = _textures[_texture->getName()];
		if (texture)
		{
			texture.reset();
			texture = nullptr;
		}
		else
		{
			MYGUI_PLATFORM_ASSERT(texture, "Texture '" << _texture->getName() << "' not found");
		}
	}
	else
	{
		MYGUI_PLATFORM_LOG(Info, "Empty texture pointer");
	}
}

MyGUI::ITexture*
MyGuiRenderer::getTexture(const std::string& _name) noexcept
{
	MapTexture::const_iterator item = _textures.find(_name);
	if (item == _textures.end())
		return nullptr;
	return item->second.get();
}

void
MyGuiRenderer::destroyAllResources() noexcept
{
	_textures.clear();
	_material.reset();
}

void 
MyGuiRenderer::doRenderRTT(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) noexcept
{
	_materialScaleY->assign(-1.0f);
	doRender(_buffer, _texture, _count);
	_materialScaleY->assign(1.0f);
}

void 
MyGuiRenderer::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) noexcept
{
	MyGuiVertexBuffer* buffer = static_cast<MyGuiVertexBuffer*>(_buffer);
	if (buffer)
	{
		MyGuiTexture* texture = static_cast<MyGuiTexture*>(_texture);
		_materialDecal->assign(texture ? texture->getTexture() : nullptr);

		auto renderBuffer = buffer->getBuffer();
		if (renderBuffer)
		{
			RenderIndirect renderable;
			renderable.numVertices = _count;
			RenderSystem::instance()->getRenderPipeline()->drawMesh(_materialPass, renderBuffer, renderable);
		}
		else
		{
			MYGUI_PLATFORM_ASSERT(renderBuffer, "render buffer is not created");
		}
	}
	else
	{
		MYGUI_PLATFORM_ASSERT(buffer, "buffer is not valid");
	}
}

void 
MyGuiRenderer::begin() noexcept
{
}

void
MyGuiRenderer::end() noexcept
{
}

const MyGUI::RenderTargetInfo& 
MyGuiRenderer::getInfo() noexcept
{
	return _info;
}

const MyGUI::IntSize& 
MyGuiRenderer::getViewSize() const noexcept
{
	return _viewport;
}

MyGUI::VertexColourType 
MyGuiRenderer::getVertexFormat() noexcept
{
	return _vertexFormat;
}

bool 
MyGuiRenderer::isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage) noexcept
{
	if (_format == MyGUI::PixelFormat::R8G8B8 ||
		_format == MyGUI::PixelFormat::R8G8B8A8)
		return true;

	return false;
}

void 
MyGuiRenderer::drawOneFrame(float delta) noexcept
{
	onFrameEvent(delta);
	onRenderToTarget(this, _isUpdate);

	_isUpdate = false;
}

void 
MyGuiRenderer::setViewport(int _width, int _height) noexcept
{
	if (_height == 0)
		_height = 1;
	if (_width == 0)
		_width = 1;

	_viewport.set(_width, _height);

	_info.maximumDepth = 1;
	_info.hOffset = 0;
	_info.vOffset = 0;
	_info.aspectCoef = float(_viewport.height) / float(_viewport.width);
	_info.pixScaleX = 1.0f / float(_viewport.width);
	_info.pixScaleY = 1.0f / float(_viewport.height);

	this->onResizeView(_viewport);

	_isUpdate = true;
}

void 
MyGuiRenderer::getViewport(int& w, int& h) noexcept
{
	w = _viewport.width;
	h = _viewport.height;
}

_NAME_END

#endif