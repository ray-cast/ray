#include <ray/gui_imageloader.h>
#include <ray/gui_manager.h>

#include <ray/image.h>

_NAME_BEGIN

void*
GuiImageLoader::loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename)
{
	std::string fullname = GuiResManager::getInstance().getDataPath(_filename);

	ray::Image image;
	if (image.load(fullname))
	{
		_width = image.width();
		_height = image.height();

		if (image.bpp() == 32)
			_format = MyGUI::PixelFormat::R8G8B8A8;
		else if (image.bpp() == 24)
			_format = MyGUI::PixelFormat::R8G8B8;
		else
		{
			assert(false);
		}

		size_t size = image.size();
		auto result = new unsigned char[size];
		std::memcpy(result, image.data(), image.size());

		return result;
	}

	return nullptr;
}

void 
GuiImageLoader::saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename)
{

}

_NAME_END