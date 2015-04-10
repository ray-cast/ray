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
#ifndef _H_LIBSCRIPT_BIND_H_
#define _H_LIBSCRIPT_BIND_H_

#include <ray/script/libscript_sys.h>

/// @addtogroup script
/// @{

#include <iostream>
_NAME_BEGIN

enum class ARGS_EVALUATE
{
    NOT_TEST,
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
};

class EXPORT ArgsIterator
{
public:
    ArgsIterator(Args args, bool reverse = false, int ignoreBottom = 0);
    Arg GetAndToNext();

    static ARGS_EVALUATE getStaticSequence();
    static ARGS_EVALUATE getConstructorSequence();
    static ARGS_EVALUATE getMethodSequence();
private:
    int _begin;
    int _end;
    int _step;
    Args _args;
};

template<typename _Class, typename ... _Args>
class BindConstructor
{
public:
    static int Constructor(RawInterface raw)
    {
        ArgsIterator argIter(raw,
            ArgsIterator::getConstructorSequence() != ARGS_EVALUATE::RIGHT_TO_LEFT, 0);
        return ConstructorCaller(raw, new _Class(static_cast<_Args>(argIter.GetAndToNext())...));
    }

    typedef _Class* (*Creator)(_Args ...);

    static int ConstructorBridge(RawInterface raw)
    {
        Stack stack(raw);

        auto w = (Wrapper<Creator>*)stack.touserdata(Stack::upvalueindex(1));

        ArgsIterator argIter(raw,
            ArgsIterator::getConstructorSequence() != ARGS_EVALUATE::RIGHT_TO_LEFT, 0);

        return ConstructorCaller(raw, w->value(static_cast<_Args>(argIter.GetAndToNext())...));
    }

private:
    static int ConstructorCaller(RawInterface raw, _Class* obj)
    {
        std::string metaname(METATABLEPREFIX);
        metaname += typeid(_Class).raw_name();

        Stack stack(raw);

        auto info = (ClassInfo<_Class>*)stack.newuserdata(sizeof(ClassInfo<_Class>));

        info->obj = obj;
        info->ref = false;
        info->readonly = false;

        stack.getfield(Stack::REGISTRYINDEX(), metaname.c_str());
        stack.setmetatable(-2);
        
        return 1;
    }
};

template<typename _Class, typename _Method>
class BindMethod
{
public:
    static int Method(RawInterface raw)
    {
        Stack stack(raw);

        auto info = (ClassInfo<_Class>*)stack.touserdata(1);
        
        Wrapper<_Method>* w = (Wrapper<_Method>*)stack.touserdata(Stack::upvalueindex(1));

        if (info->readonly)
        {
            bool readonlyMethod = stack.tointeger(Stack::upvalueindex(2)) == 1;
            if (readonlyMethod == false)
            {
                stack.error_L("Try to call readonly class's usually method");
                return 0;
            }
        }

        return BindMethod<_Class, _Method>::MethodCaller(stack, info, w->value);
    }

private:
    template <typename ... _Args>
    static int MethodCaller(Stack& stack, ClassInfo<_Class>* info, void (_Class::*method)(_Args ...))
    {
        if (info->readonly)
        {
            stack.error_L("Try to call readonly class's usually method");
            return 0;
        }

        ArgsIterator argIter(stack.getInterface(),
            ArgsIterator::getMethodSequence() == ARGS_EVALUATE::RIGHT_TO_LEFT, 1);

        (info->obj->*method)(static_cast<_Args>(argIter.GetAndToNext())...);

        return 0;
    }

    template <typename _Rt, typename ... _Args>
    static int MethodCaller(Stack& stack, ClassInfo<_Class>* info, _Rt(_Class::*method)(_Args ...))
    {
        if (info->readonly)
        {
            stack.error_L("Try to call readonly class's usually method");
            return 0;
        }

        ArgsIterator argIter(stack.getInterface(),
            ArgsIterator::getMethodSequence() == ARGS_EVALUATE::RIGHT_TO_LEFT, 1);

        _Rt rt = (info->obj->*method)(static_cast<_Args>(argIter.GetAndToNext())...);

        Pusher pusher(stack.getInterface());

        pusher.push(rt);

        return 1;
    }

    template <typename ... _Args>
    static int MethodCaller(Stack& stack, ClassInfo<_Class>* info, void (_Class::*method)(_Args ...) const)
    {
        ArgsIterator argIter(stack.getInterface(),
            ArgsIterator::getMethodSequence() == ARGS_EVALUATE::RIGHT_TO_LEFT, 1);

        (info->obj->*method)(static_cast<_Args>(argIter.GetAndToNext())...);

        return 0;
    }

    template <typename _Rt, typename ... _Args>
    static int MethodCaller(Stack& stack, ClassInfo<_Class>* info, _Rt(_Class::*method)(_Args ...) const)
    {
        ArgsIterator argIter(stack.getInterface(),
            ArgsIterator::getMethodSequence() == ARGS_EVALUATE::RIGHT_TO_LEFT, 1);

        _Rt rt = (info->obj->*method)(static_cast<_Args>(argIter.GetAndToNext())...);

        Pusher pusher(stack.getInterface());

        pusher.push(rt);

        return 1;
    }
};

template<typename _Class>
class BindDestructor
{
public:
    static int Destructor(RawInterface raw)
    {
        ArgsIterator argIter(raw,
            ArgsIterator::getConstructorSequence() != ARGS_EVALUATE::RIGHT_TO_LEFT, 0);

        Stack stack(raw);

        ClassInfo<_Class>* info = (ClassInfo<_Class>*)stack.touserdata(1);

        if (info->ref)
            return 0;

        if (info->obj)
        {
            delete info->obj;
            info->obj = nullptr;
        }

        return 0;
    }

    typedef void (*Destroyer)(_Class*);

    static int DestructorBridge(RawInterface raw)
    {
        Stack stack(raw);

        assert(stack.gettop() == 1);

        auto w = (Wrapper<Destroyer>*)stack.touserdata(Stack::upvalueindex(1));

        ClassInfo<_Class>* info = (ClassInfo<_Class>*)stack.touserdata(1);

        if (info->ref)
            return 0;

        if (info->obj)
        {
            w->value(info->obj);
            info->obj = nullptr;
        }

        return 0;
    }
};

template<typename _Class>
class BindClass final
{
public:
    BindClass(Script& script) 
        :   _script(script), 
            _metaTable(script.newTable()),
            _pusher(script.getInterface())
    {
        std::string metaname(METATABLEPREFIX);
        metaname += typeid(_Class).raw_name();

        _metaTable.set("__index", _metaTable);
        _metaTable.pushRefSafe(Stack::NoneMask);
        _metaTable.setfield(Stack::REGISTRYINDEX(), metaname.c_str());
    }

    template <typename ... _Args>
    BindClass& create(const char* name)
    {
        _script.pushcfunction(BindConstructor<_Class, _Args ...>::Constructor);
        _script.setglobal(name);
        return *this;
    }
    
    template <typename ... _Args>
    BindClass& create(const char* name, _Class* (creator)(_Args ...))
    {
        auto w =
            (Wrapper<_Class* (*)(_Args ...)>*)_script.newuserdata(sizeof(Wrapper<_Class* (*)(_Args ...)>));
        w->value = creator;
        _script.pushcclosure(BindConstructor<_Class, _Args ...>::ConstructorBridge, 1);
        _script.setglobal(name);
        return *this;
    }

    template<typename _Method>
    BindClass& method(const char* name, _Method method)
    {
        _metaTable.pushRef(Stack::T_Table);
        _script.pushstring(name);
        Wrapper<_Method>* w = (Wrapper<_Method>*)_script.newuserdata(sizeof(Wrapper<_Method>));
        w->value = method;
        _script.pushcclosure(BindMethod<_Class, _Method>::Method, 1);
        _script.rawset(-3);
        _script.pop(1);
        return *this;
    }

    BindClass& methodRaw(const char* name, int (method)(RawInterface))
    {
        _metaTable.pushRef(Stack::T_Table);
        _script.pushstring(name);
        _script.pushcfunction(method);
        _script.rawset(-3);
        _script.pop(1);
        return *this;
    }

    BindClass& destroy(const char* name)
    {
        _metaTable.pushRef(Stack::T_Table);
        _script.pushstring(name);
        _script.pushcfunction(BindDestructor<_Class>::Destructor);
        _script.rawset(-3);
        _script.pop(1);
        return *this;
    }

    BindClass& destroy(const char* name, void (destroyer)(_Class*))
    {
        _metaTable.pushRef(Stack::T_Table);
        _script.pushstring(name);
        auto w =
            (Wrapper<void(*)(_Class*)>*)_script.newuserdata(sizeof(Wrapper<void(*)(_Class*)>));
        w->value = destroyer;
        _script.pushcclosure(BindDestructor<_Class>::DestructorBridge, 1);
        _script.rawset(-3);
        _script.pop(1);
        return *this;
    }

    template<typename _Key, typename _Value>
    BindClass& set(_Key key, _Value value)
    {
        _metaTable.pushRef(Stack::T_Table);
        _pusher.push(key);
        _pusher.push(value);
        _pusher.rawset(-3);
        _pusher.pop(1);
        return *this;
    }
private:
    BindClass(const BindClass& copy) = delete;
    BindClass& operator=(const BindClass& copy) = delete;

private:
    Script& _script;
    Table _metaTable;
    Pusher _pusher;
};

class EXPORT Module final
{
public:
    Module(Script& script);
    Module(Script& script, const std::string& name);
    ~Module();

    template<typename _Key, typename _Value> 
    Module& set(_Key key, _Value value)
    {
        _pusher.push(key);
        _pusher.push(value);
        _pusher.rawset(-3);
        return *this;
    }
private:
    Module(const Module& copy) = delete;
    Module& operator=(const Module& copy) = delete;

private:
    Script& _script;
    Table _module;
    Pusher _pusher;
};

/// @}

_NAME_END

#endif