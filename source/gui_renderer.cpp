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
#include <ray/gui_renderer.h>
#include <ray/gui_texture.h>
#include <ray/gui_buffer.h>
#include <ray/gui_assert.h>

#include <ray/render_system.h>
#include <ray/render_pipeline_base.h>

#include "MyGUI_VertexData.h"
#include "MyGUI_Gui.h"
#include "MyGUI_Timer.h"

#include "GL/glew.h"

_NAME_BEGIN

using namespace Gui;

GuiRenderer::GuiRenderer() 
	: _update(false)
	, _imageLoader(nullptr)
	, _isSupportedPbo(false)
	, _isInitialise(false)
	, _vertexFormat(MyGUI::VertexColourType::ColourABGR)
{
}

GuiRenderer&
GuiRenderer::getInstance()
{
	return *getInstancePtr();
}

GuiRenderer*
GuiRenderer::getInstancePtr()
{
	return static_cast<GuiRenderer*>(RenderManager::getInstancePtr());
}

void 
GuiRenderer::open() except
{
	MYGUI_PLATFORM_ASSERT(!_isInitialise, getClassTypeName() << " initialised twice");
	MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

	if (GLEW_VERSION_3_0) 
	{
		_material = MaterialMaker("sys:fx/default.glsl");
		_materialPass = _material->getTech(RenderQueue::RQ_OPAQUE)->getPass("ui");
		_materialDecal = _material->getParameter("decal");
		_materialScaleY = _material->getParameter("scaleY");
		_materialScaleY->assign(1.0f);

		_isSupportedPbo = glewIsExtensionSupported("GL_EXT_pixel_buffer_object") != 0;

		_isInitialise = true;
		_update = false;

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
	}
	else
	{
		MYGUI_PLATFORM_EXCEPT(std::string("OpenGL 3.0 or newer not available, current version is ") + (const char*)glGetString(GL_VERSION));
	}
}

void 
GuiRenderer::close() noexcept
{
	MYGUI_PLATFORM_ASSERT(_isInitialise, getClassTypeName() << " is not initialised");
	MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

	destroyAllResources();
	
	_isInitialise = false;

	MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
}

void
GuiRenderer::setImageLoader(Gui::GuiImageLoaderPtr loader) noexcept
{
	_imageLoader = loader;
}

Gui::GuiImageLoaderPtr
GuiRenderer::getImageLoader() const noexcept
{
	return _imageLoader;
}

MyGUI::IVertexBuffer* 
GuiRenderer::createVertexBuffer()
{
	return new GuiVertexBuffer();
}

void 
GuiRenderer::destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer)
{
	delete _buffer;
}

void 
GuiRenderer::doRenderRTT(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count)
{
	_materialScaleY->assign(-1.0f);
	doRender(_buffer, _texture, _count);
	_materialScaleY->assign(1.0f);
}

void 
GuiRenderer::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count)
{
	GuiVertexBuffer* buffer = static_cast<GuiVertexBuffer*>(_buffer);
	unsigned int buffer_id = buffer->getBufferID();
	MYGUI_PLATFORM_ASSERT(buffer_id, "Vertex buffer is not created");

	unsigned int texture_id = 0;
	if (_texture)
	{
		GuiTexture* texture = static_cast<GuiTexture*>(_texture);
		texture_id = texture->getTextureID();
		//MYGUI_PLATFORM_ASSERT(texture_id, "Texture is not created");
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glBindVertexArray(buffer_id);

	glDrawArrays(GL_TRIANGLES, 0, _count);
}

void 
GuiRenderer::begin()
{
	RenderSystem::instance()->getRenderPipeline()->setMaterialPass(_materialPass);
	glActiveTexture(GL_TEXTURE0);
}

void
GuiRenderer::end()
{
}

const MyGUI::RenderTargetInfo& 
GuiRenderer::getInfo()
{
	return mInfo;
}

const MyGUI::IntSize& 
GuiRenderer::getViewSize() const
{
	return mViewSize;
}

MyGUI::VertexColourType 
GuiRenderer::getVertexFormat()
{
	return _vertexFormat;
}

bool 
GuiRenderer::isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage)
{
	if (_format == MyGUI::PixelFormat::R8G8B8 ||
		_format == MyGUI::PixelFormat::R8G8B8A8)
		return true;

	return false;
}

void 
GuiRenderer::drawOneFrame()
{
	MyGUI::Gui* gui = MyGUI::Gui::getInstancePtr();
	if (gui == nullptr)
		return;

	static MyGUI::Timer timer;
	static unsigned long last_time = timer.getMilliseconds();
	unsigned long now_time = timer.getMilliseconds();
	unsigned long time = now_time - last_time;

	onFrameEvent((float)((double)(time) / (double)1000));

	last_time = now_time;

	this->begin();

	onRenderToTarget(this, _update);
	
	this->end();

	_update = false;
}

void 
GuiRenderer::setViewSize(int _width, int _height)
{
	if (_height == 0)
		_height = 1;
	if (_width == 0)
		_width = 1;

	mViewSize.set(_width, _height);

	mInfo.maximumDepth = 1;
	mInfo.hOffset = 0;
	mInfo.vOffset = 0;
	mInfo.aspectCoef = float(mViewSize.height) / float(mViewSize.width);
	mInfo.pixScaleX = 1.0f / float(mViewSize.width);
	mInfo.pixScaleY = 1.0f / float(mViewSize.height);

	onResizeView(mViewSize);
	_update = true;
}

bool 
GuiRenderer::isPixelBufferObjectSupported() const
{
	return _isSupportedPbo;
}

MyGUI::ITexture* 
GuiRenderer::createTexture(const std::string& _name)
{
	auto& texture = _textures[_name];
	if (!texture)
	{
		texture = std::make_unique<GuiTexture>(_name, _imageLoader);
		return texture.get();
	}

	MYGUI_PLATFORM_LOG(Info, "Texture '" << _name << "' already exist");
	return texture.get();
}

void
GuiRenderer::destroyTexture(MyGUI::ITexture* _texture)
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

void 
GuiRenderer::setTexture(MyGUI::ITexture* texture) noexcept
{

}

MyGUI::ITexture*
GuiRenderer::getTexture(const std::string& _name)
{
	MapTexture::const_iterator item = _textures.find(_name);
	if (item == _textures.end())
		return nullptr;
	return item->second.get();
}

void 
GuiRenderer::destroyAllResources()
{
	_textures.clear();
	_material.reset();
}

_NAME_END
