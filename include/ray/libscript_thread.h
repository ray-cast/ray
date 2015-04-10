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

#ifndef _H_LIBSCRIPT_THREAD_H_
#define _H_LIBSCRIPT_THREAD_H_

#include <ray/script/libscript_value.h>
#include <ray/script/libscript_table.h>
#include <ray/script/libscript_helper.h>

/// @addtogroup script
/// @{

_NAME_BEGIN

class EXPORT Thread final : public Value
{
    friend class Script;
public:
    typedef Arg Result;

    Thread(Stack& stack);
    Thread(Value& value);
    Thread(Thread& copy);

    void load(const std::string& filename);
    void loadString(const std::string& str);

    Value newFunction(const std::string& script);
    Value newFunction(CFunction function);
    Value newTable();
    Value getGlobal(const std::string& name);
    Table getGlobalTable();

    /// @brief Resume this thread and given arguments
    template<typename ... _Args>
    THREADSTATUS resume(_Args ... arg)
    {
        std::tuple<_Args ...> tuple(arg ...);
        resultReset();
        _ArgPusher<std::tuple<_Args ...>, sizeof...(_Args)>::push(*_pusher.get(), tuple);
        return _resume();
    }

    /// @brief Get the number of last results
    int resultCount();

    THREADSTATUS status();

    /// @brief Get last result's value
    /// (Index begin with 1)
    Result operator[](int i)            { Result r(_c_thread_state, i + _beform_stack_size); return r; }

    RawInterface getThreadInterface()   { return _c_thread_state; }

private:
    template<typename _Tuple, std::size_t _ArgN>
    class _ArgPusher {
    public:
        static void push(Pusher& pusher, _Tuple& tuple)
        {
            _ArgPusher<_Tuple, _ArgN - 1>::push(pusher, tuple);
            pusher.push(std::get<_ArgN - 1>(tuple));
        }
    };

    template<typename _Tuple>
    class _ArgPusher<_Tuple, 0> {
    public:
        static void push(Pusher& pusher, _Tuple& tuple) { }
    };

    void resultReset();

    THREADSTATUS _resume();

private:
    int _beform_stack_size;
    int _resultNum;
    RawInterface _c_thread_state;
    std::shared_ptr<Pusher> _pusher;
};

/// @}

_NAME_END

#endif