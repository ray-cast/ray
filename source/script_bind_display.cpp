// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/script_bind_display.h>
#include <ray/render_system.h>

#include <angelscript.h>

_NAME_BEGIN

ScriptBindDisplay::ScriptBindDisplay() noexcept
{
}

ScriptBindDisplay::~ScriptBindDisplay() noexcept
{
}

void
ScriptBindDisplay::setup(asIScriptEngine* engine) noexcept
{
	int r = 0;

	r = engine->RegisterEnum("Interval"); assert(r >= 0);
	r = engine->RegisterEnumValue("Interval", "free", GPU_FREE); assert(r >= 0);
	r = engine->RegisterEnumValue("Interval", "vsync", GPU_VSYNC); assert(r >= 0);
	r = engine->RegisterEnumValue("Interval", "fps30", GPU_FPS30); assert(r >= 0);
	r = engine->RegisterEnumValue("Interval", "fps15", GPU_FPS15); assert(r >= 0);

	r = engine->RegisterObjectType("Display", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	r = engine->RegisterGlobalProperty("Display display", this); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "void setSwapInterval(Interval value)", asMETHOD(ScriptBindDisplay, setSwapInterval), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "void setWindowPosition(int x, int y)", asMETHOD(ScriptBindDisplay, setWindowPosition), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "void setWindowSize(int w, int h)", asMETHOD(ScriptBindDisplay, setWindowSize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "void setWindowTitle(const string& in)", asMETHOD(ScriptBindDisplay, setWindowTitle), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "int getWindowWidth() const", asMETHOD(ScriptBindDisplay, getWindowWidth), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "int getWindowHeight() const", asMETHOD(ScriptBindDisplay, getWindowHeight), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "int getWindowPosX() const", asMETHOD(ScriptBindDisplay, getWindowPosX), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "int getWindowPosY() const", asMETHOD(ScriptBindDisplay, getWindowPosY), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Display", "string getWindowTitle() const", asMETHOD(ScriptBindDisplay, getWindowTitle), asCALL_THISCALL); assert(r >= 0);
}

void 
ScriptBindDisplay::setRenderSystem(RenderSystemPtr renderer) noexcept
{
	_renderer = renderer;
}

void
ScriptBindDisplay::setSwapInterval(int interval) noexcept
{
	auto renderer = _renderer.lock();
	assert(renderer);

	if (interval == GPU_FREE)
		renderer->setSwapInterval(GPU_FREE);
	else if (interval == GPU_VSYNC)
		renderer->setSwapInterval(GPU_VSYNC);
	else if (interval == GPU_FPS30)
		renderer->setSwapInterval(GPU_FPS30);
	else if (interval == GPU_FPS15)
		renderer->setSwapInterval(GPU_FPS15);
}

void
ScriptBindDisplay::setWindowPosition(int x, int y) noexcept
{
}

void
ScriptBindDisplay::setWindowSize(int w, int h) noexcept
{
}

void
ScriptBindDisplay::setWindowTitle(const std::string& title) noexcept
{
}

int
ScriptBindDisplay::getWindowWidth() const noexcept
{
	auto renderer = _renderer.lock();
	assert(renderer);

	std::size_t w, h;
	renderer->getWindowResolution(w, h);
	return w;
}

int
ScriptBindDisplay::getWindowHeight() const noexcept
{
	auto renderer = _renderer.lock();
	assert(renderer);

	std::size_t w, h;
	renderer->getWindowResolution(w, h);
	return h;
}

int
ScriptBindDisplay::getWindowPosX() const noexcept
{
	return 0;
}

int
ScriptBindDisplay::getWindowPosY() const noexcept
{
	return 0;
}

std::string
ScriptBindDisplay::getWindowTitle() const noexcept
{
	return "";
}

_NAME_END