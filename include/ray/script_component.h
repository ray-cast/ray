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
#ifndef _H_SCRIPT_COMPONENT_H_
#define _H_SCRIPT_COMPONENT_H_

#include <ray/game_component.h>

class asIScriptFunction;

_NAME_BEGIN

class ScriptObject;
class EXPORT ScriptComponent : public GameComponent
{
	__DeclareSubClass(ScriptComponent, GameComponent)
public:
	ScriptComponent() noexcept;
	virtual ~ScriptComponent() noexcept;

	virtual void load(iarchive& reader) noexcept;
	virtual void save(oarchive& write) noexcept;

protected:

	virtual void onActivate();
	virtual void onDeactivate();

	virtual void onMoveBefore();
	virtual void onMoveAfter();

	virtual void onFrameBegin();
	virtual void onFrame();
	virtual void onFrameEnd();

	virtual void onMessage(const GameMessage& message);

	virtual GameComponentPtr clone() const noexcept;

private:
	ScriptComponent(const ScriptComponent&) noexcept = delete;
	ScriptComponent& operator=(const ScriptComponent&) noexcept = delete;

private:

	std::shared_ptr<ScriptObject> _scriptObject;

	asIScriptFunction* _onAttach;
	asIScriptFunction* _onRemove;

	asIScriptFunction* _onActivate;
	asIScriptFunction* _onDeactivate;

	asIScriptFunction* _onFrameBegin;
	asIScriptFunction* _onFrame;
	asIScriptFunction* _onFrameEnd;

	asIScriptFunction* _onMoveAfter;
	asIScriptFunction* _onMoveBefore;

	std::map<std::string, asIScriptFunction*> _funcMap;
};

_NAME_END

#endif