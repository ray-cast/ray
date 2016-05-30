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
#ifndef _H_GUI_TEXTURE_H_
#define _H_GUI_TEXTURE_H_

#include "mygui_types.h"

_NAME_BEGIN

class MyGuiRenderTexture;
class MyGuiTexture : public MyGUI::ITexture
{
public:
	MyGuiTexture(const std::string& name, GuiImageLoaderPtr loader) noexcept;
	virtual ~MyGuiTexture() noexcept;

	virtual const std::string& getName() const noexcept;

	virtual void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format) except;

	virtual void loadFromFile(const std::string& _filename);
	virtual void saveToFile(const std::string& _filename);

	virtual void destroy() noexcept;

	virtual int getWidth() noexcept;
	virtual int getHeight() noexcept;

	virtual void* lock(MyGUI::TextureUsage _access) noexcept;
	virtual void unlock() noexcept;
	virtual bool isLocked() noexcept;

	virtual MyGUI::PixelFormat getFormat() noexcept;
	virtual MyGUI::TextureUsage getUsage() noexcept;
	virtual std::size_t getNumElemBytes() noexcept;

	void setUsage(MyGUI::TextureUsage _usage) noexcept;
	MyGUI::TextureUsage getUsage() const noexcept;

	virtual MyGUI::IRenderTarget* getRenderTarget();

	GraphicsTexturePtr getTexture() const noexcept;
	MaterialPassPtr getMaterialPass() const noexcept;

	void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format, void* stream) except;

private:
	std::string _name;

	bool _lock;

	std::uint32_t _width;
	std::uint32_t _height;

	std::size_t _numElemBytes;
	std::size_t _dataSize;

	MyGUI::PixelFormat _originalFormat;
	MyGUI::TextureUsage _originalUsage;

	MaterialPtr _material;
	MaterialTechPtr _materialTech;
	MaterialParamPtr _materialScaleY;
	MaterialParamPtr _materialDecal;

	MemoryStream _stream;

	GraphicsTexturePtr _texture;
	GuiImageLoaderPtr _imageLoader;

	MyGuiRenderTexture* _renderTarget;
};

class MyGuiRenderTexture : public MyGUI::IRenderTarget
{
public:
	MyGuiRenderTexture(GraphicsTexturePtr texture) noexcept;
	virtual ~MyGuiRenderTexture() noexcept;

	virtual void begin() noexcept;
	virtual void end() noexcept;

	virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) noexcept;

	virtual const MyGUI::RenderTargetInfo& getInfo() noexcept;

private:

	GraphicsFramebufferPtr _framebuffer;
	GraphicsFramebufferLayoutPtr _framebufferLayout;
	MyGUI::RenderTargetInfo _renderTargetInfo;
};

_NAME_END

#endif
