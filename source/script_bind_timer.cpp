// +----------------------------------------------------------------------
// | Project : Xiemoperor.
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
#include <ray/script_bind_timer.h>
#include <ray/timer.h>

#include <angelscript.h>

_NAME_BEGIN

ScriptBindTimer::ScriptBindTimer() noexcept
	: _timer(nullptr)
{
}

ScriptBindTimer::~ScriptBindTimer() noexcept
{
}

void
ScriptBindTimer::setup(asIScriptEngine* _engine) noexcept
{
	int r = 0;

	r = _engine->RegisterObjectType("Timer", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	r = _engine->RegisterGlobalProperty("Timer timer", this); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_startTime() const", asMETHOD(ScriptBindTimer, startTime), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_elapsed() const", asMETHOD(ScriptBindTimer, elapsed), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_appTime() const", asMETHOD(ScriptBindTimer, appTime), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_delta() const", asMETHOD(ScriptBindTimer, delta), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_vsync() const", asMETHOD(ScriptBindTimer, vsync), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_fps() const", asMETHOD(ScriptBindTimer, fps), asCALL_THISCALL); assert(r >= 0);
	r = _engine->RegisterObjectMethod("Timer", "float get_averageFps() const", asMETHOD(ScriptBindTimer, averageFps), asCALL_THISCALL); assert(r >= 0);
}

void
ScriptBindTimer::setTimer(TimerPtr timer) noexcept
{
	_timer = timer;
}

float
ScriptBindTimer::startTime() noexcept
{
	assert(_timer);
	return _timer->startTime();
}

float
ScriptBindTimer::elapsed() noexcept
{
	assert(_timer);
	return _timer->elapsed();
}

float
ScriptBindTimer::appTime() noexcept
{
	assert(_timer);
	return _timer->appTime();
}

float
ScriptBindTimer::delta() noexcept
{
	assert(_timer);
	return _timer->delta();
}

float
ScriptBindTimer::vsync() noexcept
{
	assert(_timer);
	return _timer->vsync();
}

float
ScriptBindTimer::fps() noexcept
{
	assert(_timer);
	return _timer->fps();
}

float
ScriptBindTimer::averageFps() noexcept
{
	assert(_timer);
	return _timer->averageFps();
}

void
ScriptBindTimer::onFrameEnd() noexcept
{
	assert(_timer);
}

_NAME_END