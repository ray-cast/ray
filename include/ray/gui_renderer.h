/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#ifndef MYGUI_OPENGL3_RENDER_MANAGER_H_
#define MYGUI_OPENGL3_RENDER_MANAGER_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_RenderFormat.h"
#include "MyGUI_IVertexBuffer.h"
#include "MyGUI_RenderManager.h"

#include <ray/platform.h>
#include <ray/gui_imageloader.h>

_NAME_BEGIN

class GuiRenderer : public MyGUI::RenderManager, public MyGUI::IRenderTarget
{
public:
	GuiRenderer();

	void initialise();
	void shutdown();

	void setImageLoader(Gui::GuiImageLoader* loader) noexcept;
	Gui::GuiImageLoader* getImageLoader() const noexcept;

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
	MyGUI::IntSize mViewSize;
	bool mUpdate;
	MyGUI::VertexColourType mVertexFormat;
	MyGUI::RenderTargetInfo mInfo;
	unsigned int mProgramID;
	unsigned int mReferenceCount;
	int mYScaleUniformLocation;

	typedef std::map<std::string, MyGUI::ITexture*> MapTexture;
	MapTexture mTextures;
	Gui::GuiImageLoader* mImageLoader;
	bool mPboIsSupported;
        
	bool mIsInitialise;
};

_NAME_END

#endif
