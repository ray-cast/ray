// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#if defined(_BUILD_SCRIPT)
#include <ray/script_system.h>
#include <ray/script_object.h>

_NAME_BEGIN

__ImplementSingleton(ScriptSystem)

ScriptSystem::ScriptSystem() noexcept
	: _actor(nullptr)
	, _engine(nullptr)
{
}

ScriptSystem::~ScriptSystem() noexcept
{
	this->close();
}

void
ScriptSystem::open() noexcept
{
	assert(!_engine);

	_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	_engine->SetMessageCallback(asMETHOD(ScriptSystem, MessageCallback), this, asCALL_THISCALL);
	_engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, 1);

	_context = _engine->CreateContext();

	_bindCore = std::make_shared<ScriptBindArray>();
	_bindMath = std::make_shared<ScriptBindMath>();
	_bindString = std::make_shared<ScriptBindString>();
	_bindTimer = std::make_shared<ScriptBindTimer>();
	_bindInput = std::make_shared<ScriptBindInput>();
	_bindDisplay = std::make_shared<ScriptBindDisplay>();
	_bindActor = std::make_shared<ScriptBindActor>();

	_bindings.push_back(_bindCore);
	_bindings.push_back(_bindMath);
	_bindings.push_back(_bindString);
	_bindings.push_back(_bindTimer);
	_bindings.push_back(_bindInput);
	_bindings.push_back(_bindActor);
	_bindings.push_back(_bindDisplay);

	for (auto& it : _bindings)
	{
		it->setup(_engine);
	}
}

void
ScriptSystem::close() noexcept
{
	for (auto& it : _bindings)
	{
		it.reset();
	}

	_bindings.clear();

	for (auto& it : _objects)
	{
		auto object = it.lock();
		if (object)
		{
			object->close();
		}
	}

	if (_context)
	{
		_context->Release();
		_context = nullptr;
	}

	if (_engine)
	{
		_engine->Release();
		_engine = nullptr;
	}
}

void
ScriptSystem::setTimer(TimerPtr timer) noexcept
{
	_bindTimer->setTimer(timer);
}

void
ScriptSystem::setInput(InputPtr input) noexcept
{
	_bindInput->setInput(input);
}

void
ScriptSystem::setGameObject(GameObjectPtr object) noexcept
{
	_bindActor->setGameObject(object);
}

void
ScriptSystem::MessageCallback(const asSMessageInfo* msg, void* param)
{
	const char* type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	this->print(format("%s (%d, %d) : %s : %s\n") % msg->section % msg->row % msg->col % type % msg->message);
}

asIScriptModule*
ScriptSystem::getModule(const std::string& script)
{
	MemoryStream stream;
	IoServer::instance()->openFile(script, stream, ios_base::in);
	if (stream.is_open())
	{
		if (stream.size())
		{
			asIScriptModule* module = _engine->GetModule(nullptr, asGM_ALWAYS_CREATE);
			if (module)
			{
				module->AddScriptSection(nullptr, stream.map(), stream.size());
				if (module->Build() == asSUCCESS)
				{
					module->SetDefaultNamespace("");
					return module;
				}
			}
		}
	}

	return nullptr;
}

asIScriptEngine*
ScriptSystem::getScriptEngine() const noexcept
{
	return _engine;
}

asIScriptContext*
ScriptSystem::getScriptContext() const noexcept
{
	return _context;
}

void
ScriptSystem::print(const std::string& str) noexcept
{
	std::cout << str;
}

std::shared_ptr<ScriptObject>
ScriptSystem::createScriptObject()
{
	auto result = std::make_shared<ScriptObject>();
	_objects.push_back(result);
	return result;
}

void
ScriptSystem::onFrameBegin() noexcept
{
	for (auto& it : _bindings)
	{
		it->onFrameBegin();
	}
}

void
ScriptSystem::onFrame() noexcept
{
	for (auto& it : _bindings)
	{
		it->onFrame();
	}
}

void
ScriptSystem::onFrameEnd() noexcept
{
	for (auto& it : _bindings)
	{
		it->onFrameEnd();
	}
}

_NAME_END
#endif