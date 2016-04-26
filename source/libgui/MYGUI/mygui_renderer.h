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
#ifndef _H_GUI_RENDERER_H_
#define _H_GUI_RENDERER_H_

#include "mygui_types.h"

_NAME_BEGIN

class MyGuiRenderer : public MyGUI::RenderManager, public MyGUI::IRenderTarget
{
public:
	MyGuiRenderer() noexcept;
	~MyGuiRenderer() noexcept;

	void open() except;
	void close() noexcept;

	void setImageLoader(GuiImageLoaderPtr loader) noexcept;
	GuiImageLoaderPtr getImageLoader() const noexcept;

	static MyGuiRenderer& getInstance() noexcept;
	static MyGuiRenderer* getInstancePtr() noexcept;

	void doRenderRTT(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) noexcept;
	void drawOneFrame(float delta) noexcept;

	void setViewport(int _width, int _height) noexcept;
	void getViewport(int& w, int& h) noexcept;

	virtual const MyGUI::IntSize& getViewSize() const noexcept;
	virtual MyGUI::VertexColourType getVertexFormat() noexcept;
	virtual bool isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage) noexcept;

	virtual MyGUI::IVertexBuffer* createVertexBuffer() noexcept;
	virtual void destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer) noexcept;

	virtual MyGUI::ITexture* createTexture(const std::string& _name) noexcept;
	virtual void destroyTexture(MyGUI::ITexture* _texture) noexcept;
	virtual MyGUI::ITexture* getTexture(const std::string& _name) noexcept;

	virtual void begin() noexcept;
	virtual void end() noexcept;
	virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) noexcept;
	virtual const MyGUI::RenderTargetInfo& getInfo() noexcept;

private:
	void destroyAllResources() noexcept;

private:
	typedef std::map<std::string, std::unique_ptr<MyGUI::ITexture>> MapTexture;

	bool _isUpdate;
	bool _isInitialise;

	MyGUI::IntSize _viewport;
	MyGUI::VertexColourType _vertexFormat;
	MyGUI::RenderTargetInfo _info;

	MaterialPtr _material;
	MaterialTechPtr _materialTech;
	MaterialParamPtr _materialScaleY;
	MaterialParamPtr _materialDecal;

	MapTexture _textures;
	GuiImageLoaderPtr _imageLoader;
};

_NAME_END

#endif
