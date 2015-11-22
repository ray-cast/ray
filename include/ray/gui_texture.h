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

#include "MyGUI_Prerequest.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderFormat.h"

#include <ray/gui_imageloader.h>

_NAME_BEGIN

class GuiRenderTexture : public MyGUI::IRenderTarget
{
public:
	GuiRenderTexture(unsigned int _texture);
	virtual ~GuiRenderTexture();

	virtual void begin();
	virtual void end();

	virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);

	virtual const MyGUI::RenderTargetInfo& getInfo();

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;

	int mSavedViewport[4];

	unsigned int mFBOID;
	unsigned int mRBOID;

	MyGUI::RenderTargetInfo _renderTargetInfo;
};

class GuiTexture : public MyGUI::ITexture
{
public:
	GuiTexture(const std::string& name, Gui::GuiImageLoaderPtr loader);
	virtual ~GuiTexture();

	virtual const std::string& getName() const;

	virtual void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format);
	virtual void loadFromFile(const std::string& _filename);
	virtual void saveToFile(const std::string& _filename);

	virtual void destroy();

	virtual int getWidth();
	virtual int getHeight();

	virtual void* lock(MyGUI::TextureUsage _access);
	virtual void unlock();
	virtual bool isLocked();

	virtual MyGUI::PixelFormat getFormat();
	virtual MyGUI::TextureUsage getUsage();
	virtual size_t getNumElemBytes();

	virtual MyGUI::IRenderTarget* getRenderTarget();

	unsigned int getTextureID() const;
	void setUsage(MyGUI::TextureUsage _usage);
	void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format, void* _data);

private:
	void _create();

private:
	std::string mName;
	int mWidth;
	int mHeight;
	int mPixelFormat;
	int mInternalPixelFormat;
	int mUsage;
	int mAccess;
	size_t mNumElemBytes;
	size_t mDataSize;
	unsigned int mTextureID;
	unsigned int mPboID;
	bool mLock;
	void* mBuffer;
	MyGUI::PixelFormat mOriginalFormat;
	MyGUI::TextureUsage mOriginalUsage;
	Gui::GuiImageLoaderPtr _imageLoader;
	GuiRenderTexture* _renderTarget;
};

_NAME_END

#endif
