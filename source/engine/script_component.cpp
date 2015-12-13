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
#if defined(_BUILD_SCRIPT)
#include <ray/script_component.h>
#include <ray/script_object.h>
#include <ray/script_system.h>

_NAME_BEGIN

__ImplementSubClass(ScriptComponent, GameComponent, "Script")

ScriptComponent::ScriptComponent() noexcept
	: _scriptObject(nullptr)
	, _onActivate(0)
	, _onDeactivate(0)
	, _onFrameBegin(0)
	, _onFrame(0)
	, _onFrameEnd(0)
	, _onMoveAfter(0)
	, _onMoveBefore(0)
{
}

ScriptComponent::~ScriptComponent() noexcept
{
}

void 
ScriptComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);
}

void 
ScriptComponent::save(oarchive& write) noexcept
{
}

void
ScriptComponent::onActivate()
{
	_scriptObject = ScriptSystem::instance()->createScriptObject();

	if (!_scriptObject->open(this->getName()))
	{
		ScriptSystem::instance()->print("Couldn't create module class for the script " + this->getName() + "\n");
		return;
	}

	if (!_scriptObject->setInterface("IController"))
	{
		ScriptSystem::instance()->print("Couldn't find the interface class for the script" + this->getName() +"\n");
		return;
	}

	if (!_scriptObject->construct())
	{
		ScriptSystem::instance()->print("Couldn't construct the class for the script" + this->getName() + "\n");
		return;
	}

	_onActivate = _scriptObject->getInterfaceByDecl("void onActivate()");
	_onDeactivate = _scriptObject->getInterfaceByDecl("void onDeactivate()");

	_onFrameBegin = _scriptObject->getInterfaceByDecl("void onFrameBegin()");
	_onFrame = _scriptObject->getInterfaceByDecl("void onFrame()");
	_onFrameEnd = _scriptObject->getInterfaceByDecl("void onFrameEnd()");

	_onMoveBefore = _scriptObject->getInterfaceByDecl("void onMoveBefore()");
	_onMoveAfter = _scriptObject->getInterfaceByDecl("void onMoveAfter()");

	if (_onActivate)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onActivate);
	}
}

void
ScriptComponent::onDeactivate()
{
	if (_onDeactivate)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onDeactivate);
	}

	if (_scriptObject)
	{
		_scriptObject.reset();
		_scriptObject = nullptr;
	}
}

void
ScriptComponent::onFrameBegin()
{
	if (_onFrameBegin)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onFrameBegin);
	}
}

void
ScriptComponent::onFrame()
{
	if (_onFrame)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onFrame);
	}
}

void
ScriptComponent::onFrameEnd()
{
	if (_onFrameEnd)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onFrameEnd);
	}
}

void
ScriptComponent::onMoveBefore()
{
	if (_onMoveBefore)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onMoveBefore);
	}
}

void
ScriptComponent::onMoveAfter()
{
	if (_onMoveAfter)
	{
		ScriptSystem::instance()->setGameObject(this->getGameObject());
		_scriptObject->exce(_onMoveAfter);
	}
}

void
ScriptComponent::onMessage(const MessagePtr& message)
{
	/*static asIScriptFunction* myNull = (asIScriptFunction*)-1;

	assert(_scriptObject);
	auto& function = _funcMap[method];
	if (function == nullptr)
	{
		function = _scriptObject->getInterfaceByDecl(method.c_str());
		if (!function)
			function = myNull;
	}

	if (function && function != myNull)
	{
		if (data)
		{
			va_list ap;

			va_start(ap, data);

			for (;;)
			{
				Variant* arg = va_arg(ap, Variant*);
				if (arg)
				{
					switch (arg->getType())
					{
					}
				}
				else
				{
					break;
				}
			}

			va_end(ap);
		}

		_scriptObject->exce(function);
	}*/
}

GameComponentPtr
ScriptComponent::clone() const noexcept
{
	auto instance = std::make_shared<ScriptComponent>();
	instance->setName(this->getName());
	return instance;
}

_NAME_END
#endif