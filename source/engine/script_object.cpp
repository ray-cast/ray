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
#include <ray/script_object.h>
#include <ray/script_system.h>

_NAME_BEGIN

ScriptObject::ScriptObject() noexcept
	: _module(nullptr)
	, _object(nullptr)
{
}

ScriptObject::~ScriptObject() noexcept
{
	this->close();
}

bool
ScriptObject::open(const std::string& name)
{
	assert(!_module);
	_module = ScriptSystem::instance()->getModule(name);
	return _module ? true : false;
}

void
ScriptObject::close() noexcept
{
	if (_object)
	{
		_object->Release();
		_object = nullptr;
	}
}

bool
ScriptObject::setInterface(const std::string& controller) noexcept
{
	assert(_module);
	assert(!controller.empty());

	asUINT tc = _module->GetObjectTypeCount();
	for (asUINT i = 0; i < tc; i++)
	{
		bool found = false;
		asIObjectType* type = _module->GetObjectTypeByIndex(i);
		asUINT ic = type->GetInterfaceCount();
		for (asUINT j = 0; j < ic; j++)
		{
			if (controller == type->GetInterface(j)->GetName())
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			_interface = type;
			break;
		}
	}

	return true;
}

std::string
ScriptObject::getName() const noexcept
{
	return _interface->GetName();
}

bool
ScriptObject::construct() noexcept
{
	assert(_interface);

	std::string construct = std::string(_interface->GetName()) + "@ " + _interface->GetName() + "()";
	auto factory = this->getFactoryByDecl(construct);
	if (factory)
	{
		if (this->exce(factory))
		{
			_object = *(asIScriptObject**)ScriptSystem::instance()->getScriptContext()->GetAddressOfReturnValue();
			_object->AddRef();
			return true;
		}
	}

	return false;
}

std::size_t
ScriptObject::getFactoryByDecl(const std::string& decl) noexcept
{
	assert(_interface);
	auto function = _interface->GetFactoryByDecl(decl.c_str());
	if (function)
	{
		_functions.push_back(function);
		return _functions.size();
	}
	return 0;
}

std::size_t
ScriptObject::getInterfaceByDecl(const char* decl) noexcept
{
	assert(_interface);
	auto function = _interface->GetMethodByDecl(decl);
	if (function)
	{
		_functions.push_back(function);
		return _functions.size();
	}
	return 0;
}

std::size_t
ScriptObject::getMethodByDecl(const char* decl) noexcept
{
	assert(_module);
	auto function = _module->GetFunctionByName(decl);
	if (function)
	{
		_functions.push_back(function);
		return _functions.size();
	}
	return 0;
}

int
ScriptObject::setArgByte(std::size_t arg, std::uint8_t value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgByte(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgWord(std::size_t arg, std::uint16_t value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgWord(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgDWord(std::size_t arg, std::uint32_t value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgDWord(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgQWord(std::size_t arg, std::uint64_t value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgQWord(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgFloat(std::size_t arg, float value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgFloat(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgDouble(std::size_t arg, double value) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgDouble(arg, value);
	}

	return 0;
}

int
ScriptObject::setArgAddress(std::size_t arg, void* addr) noexcept
{
	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		return ctx->SetArgAddress(arg, addr);
	}

	return 0;
}

bool
ScriptObject::exce(std::size_t index) noexcept
{
	assert(index > 0);
	assert(index <= _functions.size());

	auto ctx = ScriptSystem::instance()->getScriptContext();
	if (ctx)
	{
		if (ctx->Prepare(_functions[index - 1]) != asSUCCESS)
		{
			ScriptSystem::instance()->print("fail to prepare function");
			return false;
		}

		if (_object)
		{
			ctx->SetObject(_object);
		}

		int r = ctx->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			if (r == asEXECUTION_EXCEPTION)
			{
				std::string exception = format("Exception: %d\n") % ctx->GetExceptionString();
				std::string function = format("Function: %d\n") % ctx->GetExceptionFunction()->GetDeclaration();
				std::string line = format("Line: %d\n") % ctx->GetExceptionLineNumber();
				ScriptSystem::instance()->print(exception + function + line);
				return false;
			}
		}
	}

	return true;
}

_NAME_END
#endif