/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#ifndef MYGUI_OPENGL3_TEXTURE_H_
#define MYGUI_OPENGL3_TEXTURE_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderFormat.h"

#include <ray/gui_imageloader.h>

_NAME_BEGIN

class OpenGL3RTTexture : public MyGUI::IRenderTarget
{
public:
	OpenGL3RTTexture(unsigned int _texture);
	virtual ~OpenGL3RTTexture();

	virtual void begin();
	virtual void end();

	virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);

	virtual const MyGUI::RenderTargetInfo& getInfo()
	{
		return mRenderTargetInfo;
	}

private:
	MyGUI::RenderTargetInfo mRenderTargetInfo;
	unsigned int mTextureID;
	int mWidth;
	int mHeight;

	int mSavedViewport[4];

	unsigned int mFBOID;
	unsigned int mRBOID;
};

class OpenGL3Texture : public MyGUI::ITexture
{
public:
	OpenGL3Texture(const std::string& name, Gui::GuiImageLoader* loader);
	virtual ~OpenGL3Texture();

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

/*internal:*/
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
	Gui::GuiImageLoader* mImageLoader;
	OpenGL3RTTexture* mRenderTarget;
};

_NAME_END

#endif
