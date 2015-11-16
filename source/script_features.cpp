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
#include <ray/script_features.h>
#include <ray/script_system.h>
#include <ray/script_component.h>

_NAME_BEGIN

ScriptFeatures::ScriptFeatures() noexcept
	: _scriptManager(nullptr)
{
}

ScriptFeatures::~ScriptFeatures() noexcept
{
	ScriptSystem::instance()->close();
}

GameComponentPtr
ScriptFeatures::onSerialization(iarchive& reader) except
{
	auto component = reader.getString("name");
	if (component == "script")
		return instanceScript(reader);

	return nullptr;
}

void
ScriptFeatures::onActivate() except
{
	ScriptSystem::instance()->open();
	ScriptSystem::instance()->setTimer(this->getGameServer()->getTimer());
	ScriptSystem::instance()->setInput(this->getGameServer()->getFeature<InputFeature>()->getInput());
	ScriptSystem::instance()->setRenderSystem(this->getGameServer()->getFeature<RenderFeatures>()->getRenderSystem());
	//ScriptSystem::instance()->setGuiSystem(_guiSystem);
}

void
ScriptFeatures::onDeactivate() except
{
	ScriptSystem::instance()->close();
}

void
ScriptFeatures::onFrameBegin() noexcept
{
	ScriptSystem::instance()->onFrameBegin();
}

void
ScriptFeatures::onFrame() noexcept
{
	ScriptSystem::instance()->onFrame();
}

void
ScriptFeatures::onFrameEnd() noexcept
{
	ScriptSystem::instance()->onFrameEnd();
}

GameComponentPtr
ScriptFeatures::instanceScript(iarchive& reader) except
{
	if (reader.setToFirstChild())
	{
		auto script = std::make_shared<ScriptComponent>();

		do
		{
			auto key = reader.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					if (it == "name")
					{
						script->setName(reader.getString(it));
					}
				}
			}
		} while (reader.setToNextChild());

		return script;
	}

	return nullptr;
}

_NAME_END
#endif