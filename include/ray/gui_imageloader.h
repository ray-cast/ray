/*!
	@file
	@author		George Evmenov
	@date		09/2009
	@module
*/

#ifndef MYGUI_OPENGL3_IMAGE_LOADER_H_
#define MYGUI_OPENGL3_IMAGE_LOADER_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_RenderFormat.h"
#include <ray/platform.h>

_NAME_BEGIN

class EXPORT GuiImageLoader
{
public:
	GuiImageLoader() {}
	virtual ~GuiImageLoader() {}

	virtual void* loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename);
	virtual void saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename);
};

_NAME_END

#endif
