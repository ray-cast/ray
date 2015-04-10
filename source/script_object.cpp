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
#include <ray/script_object.h>
#include <ray/format.h>

#include <angelscript.h>

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
ScriptObject::setup(const std::string& name, bool _throw)
{
    assert(!_module);

    _module = ScriptSystem::instance()->getModule(name);
    if (!_module)
    {
        if (_throw)
            throw failure("Couldn't create module class for the script " + name + "\n");
        else
            ScriptSystem::instance()->print("Couldn't create module class for the script " + name + "\n");

        return false;
    }

    return true;
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
            this->_interface = type;
            break;
        }
    }

    if (!this->_interface)
    {
        ScriptSystem::instance()->print("Couldn't find the interface class for the type " + controller + "\n");
        return false;
    }

    return true;
}

bool
ScriptObject::construct(GameObjectPtr gameobj) noexcept
{
    assert(_interface);

    std::string construct = std::string(_interface->GetName()) + "@ " + _interface->GetName() + "(" + "GameObject& in" + ")";
    auto factory = _interface->GetFactoryByDecl(construct.c_str());
    if (factory)
    {
        asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
        if (ctx)
        {
            if (ctx->Prepare(factory) != asSUCCESS)
            {
                ScriptSystem::instance()->print("Fail to prepare function");
                return false;
            }

            ctx->SetArgObject(0, &gameobj);

            int r = ctx->Execute();
            if (r != asEXECUTION_FINISHED)
            {
                if (r == asEXECUTION_EXCEPTION)
                {
                    ScriptSystem::instance()->print(format("Exception: %d\n") % ctx->GetExceptionString());
                    ScriptSystem::instance()->print(format("Function: %d\n") % ctx->GetExceptionFunction()->GetDeclaration());
                    ScriptSystem::instance()->print(format("Line: %d\n") % ctx->GetExceptionLineNumber());
                    return false;
                }
            }
            else
            {
                _object = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
                _object->AddRef();
                return true;
            }
        }
    }

    return false;
}

bool
ScriptObject::destruct() noexcept
{
    return true;
}

asIScriptFunction*
ScriptObject::getInterfaceByDecl(const char* decl) noexcept
{
    assert(_module);
    assert(_interface);

    return _interface->GetMethodByDecl(decl);
}

asIScriptFunction*
ScriptObject::getMethodByDecl(const char* decl) noexcept
{
    return _module->GetFunctionByName(decl);
}

int
ScriptObject::setArgByte(std::size_t arg, std::uint8_t value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgByte(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgWord(std::size_t arg, std::uint16_t value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgWord(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgDWord(std::size_t arg, std::uint32_t value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgDWord(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgQWord(std::size_t arg, std::uint64_t value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgQWord(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgFloat(std::size_t arg, float value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgFloat(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgDouble(std::size_t arg, double value) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgDouble(arg, value);
    }

    return 0;
}

int
ScriptObject::setArgAddress(std::size_t arg, void* addr) noexcept
{
    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        return ctx->SetArgAddress(arg, addr);
    }

    return 0;
}

bool
ScriptObject::exce(asIScriptFunction* func, bool _throw)
{
    assert(func);

    asIScriptContext* ctx = ScriptSystem::instance()->getScriptContext();
    if (ctx)
    {
        if (ctx->Prepare(func) != asSUCCESS)
        {
            if (_throw)
                throw failure("fail to prepare function");
            else
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

                if (_throw)
                    throw failure(exception + function + line);
                else
                    ScriptSystem::instance()->print(exception + function + line);

                return false;
            }
        }
    }

    return true;
}

_NAME_END