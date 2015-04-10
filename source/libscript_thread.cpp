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

#include <ray/script/libscript_thread.h>

#include <ray/platform/memory.h>
#include <ray/platform/isystem.h>

_NAME_BEGIN

Thread::Thread(Stack& stack) : Value(stack)
{
    pushRef(T_Thread);
    _c_thread_state = tothread(-1); 
    pop(1);

    _pusher = std::make_shared<Pusher>(_c_thread_state);
}

Thread::Thread(Value& value) : Value(value)
{
    pushRef(T_Thread);
    _c_thread_state = tothread(-1);
    pop(1);

    _pusher = std::make_shared<Pusher>(_c_thread_state);
}

Thread::Thread(Thread& copy) : Value(copy)
{
    pushRef(T_Thread);
    _c_thread_state = tothread(-1);
    pop(1);

    _pusher = std::make_shared<Pusher>(_c_thread_state);
}

void Thread::load(const std::string& filename)
{
    isystem system(isystem::instance());

    if (!system.open(filename))
    {
        SCRIPT_EXCEPTION("Failed to open file:" + filename);
    }

    istream* stream = system.stream();

    stream->seekg(0, ios_base::end);

    auto size = (std::size_t)stream->tellg();

    stream->seekg(0, ios_base::beg);

    std::unique_ptr<char[]> buff(new char[size]);

    stream->read(buff.get(), size);

    if (loadbuffer_L(buff.get(), size, filename.c_str()))
    {
        std::string errorInfo = tostring(-1);
        pop(1);
        SCRIPT_EXCEPTION(errorInfo);
    }
}

void Thread::loadString(const std::string& str)
{
    if (loadbuffer_L(str.c_str(), str.size(), (str.substr(0, 10) + "...").c_str()))
    {
        std::string errorInfo = tostring(-1);
        pop(1);
        SCRIPT_EXCEPTION(errorInfo);
    }
}

Value Thread::newFunction(const std::string& script)
{
    if (loadbuffer_L(script.c_str(), script.size(), (script.substr(0, 10) + "...").c_str()))
    {
        std::string errorInfo = tostring(-1);
        pop(1);
        SCRIPT_EXCEPTION(errorInfo);
    }

    return _c_thread_state;
}

Value Thread::newFunction(CFunction function)
{
    pushcfunction(function);
    return _c_thread_state;
}

Value Thread::newTable()
{
    newtable();
    return _c_thread_state;
}

Value Thread::getGlobal(const std::string& name)
{
    getglobal(name.c_str());
    return _c_thread_state;
}

Table Thread::getGlobalTable()
{
    pushglobaltable();
    Value value(_c_thread_state);
    return value;
}

void Thread::resultReset()
{ 
    _pusher->reset();
    _beform_stack_size = gettop(); 
}

Stack::THREADSTATUS Thread::_resume()
{
    THREADSTATUS result = (THREADSTATUS)Stack::resume(_c_state, _pusher->count());

    return result;
}

int Thread::resultCount()
{
    return gettop() - _beform_stack_size;
}

Stack::THREADSTATUS Thread::status()
{
    return Stack(_c_thread_state).status();
}

_NAME_END