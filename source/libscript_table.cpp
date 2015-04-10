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

#include <ray/script/libscript_table.h>

_NAME_BEGIN

Table::Table(Stack& stack) : Value(stack), _pusher(stack.getInterface())
{
}

Table::Table(Value& value) : Value(value), _pusher(value.getInterface())
{
}

Table::Table(Table& copy) : Value(copy), _pusher(copy.getInterface())
{
}
 
Value Table::at(long long key)
{ 
    pushRef(T_Table);
    pushinteger(key);
    rawget(-2);
    remove(-2);
    return this->_c_state;
}

Value Table::at(Value& key)
{
    sameThread(key);

    pushRef(T_Table);
    key.pushRef(NoneMask);
    rawget(-2);
    remove(-2);
    return this->_c_state;
}

Value Table::at(const std::string& key)
{
    pushRef(T_Table);
    pushstring(key.c_str());
    rawget(-2);
    remove(-2);
    return this->_c_state;
}

size_t Table::len()
{
    this->pushRef(T_Table);
    std::size_t s = rawlen( -1);
    pop(1);
    return s;
}

Table Table::operator[](long long key)
{
    if (pushRefSafe(T_Table))
    {
        pushinteger(key);
        rawget(-2);
        remove(-2);
    }
    else
    {
        pushnil();
    }

    Value value(getInterface());

    return value;
}

Table Table::operator[](Value& value)
{
    if (sameThreadSafe(value) && this->pushRefSafe(T_Table))
    {
        value.pushRefSafe(NoneMask);
        rawget(-2);
        remove(-2);
    }
    else
    {
        pushnil();
    }

    Value _value(getInterface());

    return _value;
}

Table Table::operator[](const char *key)
{
    if (pushRefSafe(T_Table))
    {
        pushstring(key);
        rawget(-2);
        remove(-2);
    }
    else 
    {
        pushnil();
    }

    Value value(this->getInterface());

    return value;
}

// +----------------------------------------------------------------------
// class Table::Accessor
// +----------------------------------------------------------------------

Table::Accessor::Accessor(Table& value) : _table(value), _valid(true)
{
    _cur_key = new Value(_table);
    _cur_value = new Value(_table);
    _cur_key->pushnil();
    _cur_key->reset();

    _table.pushRef(Stack::T_Table);

    this->next();
}

Table::Accessor::~Accessor()
{
    _table.pop(1);
    delete _cur_key;
    delete _cur_value;
}

bool Table::Accessor::end()
{
    return !_valid;
}

void Table::Accessor::reset()
{
    _table.pushnil();
    _cur_key->reset();
    _table.pushnil();
    _cur_value->reset();
}

void Table::Accessor::next()
{
    if (!_valid)
        SCRIPT_EXCEPTION("Invalid Accessor status");

    _cur_key->pushRefSafe(NoneMask);

    _valid = _table.next(-2) != 0;

    if (_valid) {
        _cur_value->reset();
        _cur_key->reset();
    }
}

Value& Table::Accessor::key()
{
    return *_cur_key;
}

Value& Table::Accessor::value()
{
    return *_cur_value;
}

_NAME_END