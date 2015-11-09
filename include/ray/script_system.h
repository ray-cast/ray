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
#ifndef _H_SCRIPT_MANAGER_H_
#define _H_SCRIPT_MANAGER_H_

#include <ray/game_types.h>
#include <ray/script_bind_math.h>
#include <ray/script_bind_string.h>
#include <ray/script_bind_timer.h>
#include <ray/script_bind_input.h>
#include <ray/script_bind_actor.h>
#include <ray/script_bind_display.h>
#include <ray/script_bind_core.h>

_NAME_BEGIN

class ScriptObject;
class ScriptSystem final
{
	__DeclareSingleton(ScriptSystem)
public:
	ScriptSystem() noexcept;
	~ScriptSystem() noexcept;

	void open() noexcept;
	void close() noexcept;

	void setTimer(TimerPtr timer) noexcept;
	void setInput(InputPtr input) noexcept;
	void setWindow(WindowPtr wx) noexcept;

	void print(const std::string& str) noexcept;

	std::shared_ptr<ScriptObject> createScriptObject();

	asIScriptModule* getModule(const std::string& script);
	asIScriptEngine* getScriptEngine() const noexcept;
	asIScriptContext* getScriptContext() const noexcept;

	void MessageCallback(const asSMessageInfo* msg, void* param);

	void onFrameBegin() noexcept;
	void onFrame() noexcept;
	void onFrameEnd() noexcept;

private:

	TimerPtr _timer;
	InputPtr _input;
	WindowPtr _window;
	GameObject* _actor;

	asIScriptContext* _context;
	asIScriptEngine* _engine;

	std::vector<std::weak_ptr<ScriptObject>> _objects;

	std::shared_ptr<ScriptBindCore> _bindCore;
	std::shared_ptr<ScriptBindMath> _bindMath;
	std::shared_ptr<ScriptBindString> _bindString;
	std::shared_ptr<ScriptBindTimer> _bindTimer;
	std::shared_ptr<ScriptBindInput> _bindInput;
	std::shared_ptr<ScriptBindActor> _bindActor;
	std::shared_ptr<ScriptBindDisplay> _bindDisplay;

	std::vector<std::shared_ptr<ScriptBinder>> _bindings;
};

_NAME_END

#endif