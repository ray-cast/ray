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

#include "MyGUI_Prerequest.h"
#include "MyGUI_RenderFormat.h"
#include "MyGUI_IVertexBuffer.h"
#include "MyGUI_RenderManager.h"

#include <ray/gui_imageloader.h>

_NAME_BEGIN

namespace Gui
{
	class GuiRenderer : public MyGUI::RenderManager, public MyGUI::IRenderTarget
	{
	public:
		GuiRenderer();

		void initialise();
		void shutdown();

		void setImageLoader(Gui::GuiImageLoaderPtr loader) noexcept;
		Gui::GuiImageLoaderPtr getImageLoader() const noexcept;

		static GuiRenderer& getInstance();
		static GuiRenderer* getInstancePtr();

		virtual const MyGUI::IntSize& getViewSize() const;
		virtual MyGUI::VertexColourType getVertexFormat();
		virtual bool isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage);
		virtual MyGUI::IVertexBuffer* createVertexBuffer();
		virtual void destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer);
		virtual MyGUI::ITexture* createTexture(const std::string& _name);
		virtual void destroyTexture(MyGUI::ITexture* _texture);
		virtual MyGUI::ITexture* getTexture(const std::string& _name);
		virtual void begin();
		virtual void end();
		virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
		virtual const MyGUI::RenderTargetInfo& getInfo();
		void doRenderRTT(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
		void drawOneFrame();
		void setViewSize(int _width, int _height);
		bool isPixelBufferObjectSupported() const;
		unsigned int createShaderProgram(void);

	private:
		void destroyAllResources();

	private:
		typedef std::map<std::string, MyGUI::ITexture*> MapTexture;

		MyGUI::IntSize mViewSize;
		MyGUI::VertexColourType mVertexFormat;
		MyGUI::RenderTargetInfo mInfo;
		unsigned int mProgramID;
		unsigned int mReferenceCount;
		int mYScaleUniformLocation;

		MapTexture mTextures;

		bool _update;
		bool _isSupportedPbo;
		bool _isInitialise;

		Gui::GuiImageLoaderPtr _imageLoader;
	};
}

_NAME_END

#endif
