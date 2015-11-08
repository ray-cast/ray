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
#include <ray/game_application.h>
#include <ray/game_scene.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include "terrain.h"
#include "first_person_camera.h"

class GameEngine : public ray::GameApplication
{
public:
	GameEngine() noexcept
	{
		_renderSetting.enableSSAO = true;
		_renderSetting.enableSAT = true;
		_renderSetting.enableSSSS = false;
		_renderSetting.enableFog = false;
		_renderSetting.enableFimic = true;
		_renderSetting.enableFXAA = true;
	}

	void setup(HWND hwnd, std::size_t w, std::size_t h)
	{
		auto renderer = std::make_shared<ray::RenderFeatures>();
		renderer->setRenderWindow(hwnd, w, h);
		renderer->setRenderSetting(_renderSetting);

		this->addFeatures(std::make_shared<ray::InputFeatures>());
		this->addFeatures(std::make_shared<ray::PhysicFeatures>());
		this->addFeatures(renderer);

		if (!this->openScene("dlc:cube\\scene.map"))
			throw ray::failure("App::openScene('dlc:cube\\scene.map') fail");
	}

private:

	ray::RenderSetting _renderSetting;
};

int main(int argc, char *argv[])
{
	try
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		auto window = SDL_CreateWindow("Cube World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1376, 768, SDL_WINDOW_SHOWN);

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(window, &info);

		GameEngine game;
		game.open();
		game.start();
		game.setup(info.info.win.window, 1376, 768);

		SDL_Event event;
		while (!game.isQuitRequest())
		{
			while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUIT, SDL_SYSWMEVENT))
			{
				switch (event.type)
				{
				case SDL_QUIT:
				{
					ray::AppQuitEvent quit;
					game.sendMessage(&quit);
				}
				break;
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					{
						ray::GetFocusEvent focus;
						focus.window.id = event.window.windowID;
						game.sendMessage(&focus);
					}
					break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
					{
						ray::LostFocusEvent focus;
						focus.window.id = event.window.windowID;
						game.sendMessage(&focus);
					}
					}
				}
				break;
				}
			}

			game.update();
		}
	}
	catch (const ray::exception& e)
	{
		SDL_Quit();

		std::cout << e.what();
		std::system("pause");
	}

	SDL_Quit();
	return 0;
}