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
#include <ray/script/libscript_function.h>
#include <ray/script/libscript_table.h>
#include <ray/platform/except.h>

_NAME_BEGIN

Function::Function(Stack& stack) : Value(stack), _nil(nullptr), _pusher(stack.getInterface())
{
}

Function::Function(Value& value) : Value(value), _nil(nullptr), _pusher(value.getInterface())
{
}

Function::Function(Function& copy) : Value(copy), _nil(nullptr), _pusher(copy.getInterface())
{
}

Function::~Function()
{
    delete _nil;
    while (_result.size())
    {
        delete _result.back();
        _result.pop_back();
    }
}

Table Function::getReuslt()
{
    newtable();
    Stack stack(_c_state);
    Table t = stack;

    for (int i = 1; i <= resultCount(); ++i)
    {
        t.set(i, this->operator[](i));
    }

    return t;
}

Value& Function::operator[](int i)
{
    if (i == 0 || i > _resultNum)
    {
        if (!_nil)
        {
            pushnil();
            _nil = new Value(getInterface());
        }
        return *_nil;
    }
    return *_result[_resultNum - i];
}

void Function::pcall()
{
    if (Stack::pcall(_pusher.count(), /*LUA_MULTRET*/-1, 0))
    {
        throw failure(Stack(_c_state).tostring(-1));
    }

    int resultNum = gettop() - _beform_stack_size;

    std::size_t resultValueIdx = -1;

    _resultNum = resultNum;

    // Reverse storage

    while (resultValueIdx++, resultNum--)
    {
        if (_result.size() <= resultValueIdx)
        {
            _result.push_back(new Value(this->getInterface()));
        }
        else
        {
            _result[resultValueIdx]->reset();
        }
    }
}

_NAME_END