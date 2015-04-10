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
#ifndef _H_QUEUE_H_
#define _H_QUEUE_H_

#include <ray/platform.h>

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

_NAME_BEGIN

template<typename T>
class threadsafe_queue
{
public:
    threadsafe_queue() noexcept
    {
    }

    threadsafe_queue(threadsafe_queue const& other) noexcept
    {
        std::unique_lock<std::mutex> lock(other._mutex);
        if (lock)
        {
            _queue = other._queue;
        }
    }

    void push(T data) noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (lock)
        {
            _queue.push(data);
            _dispose.notify_one();
        }
    }

    T pop() noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (lock)
        {
            if (_queue.empty())
                return T();

            auto result = _queue.front();
            _queue.pop();
            return result;
        }

        return T();
    }

    T wait_and_pop() noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (lock)
        {
            _dispose.wait(lock, [this] { return !_queue.empty(); });
            value = _queue.front();
            _queue.pop();
            return value;
        }
    }

    bool try_pop(T& value) noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (lock)
        {
            if (_queue.empty())
                return false;

            value = _queue.front();
            _queue.pop();
            return true;
        }
    }

    bool empty() const noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (lock)
        {
            return _queue.empty();
        }

        return true;
    }

private:

    std::queue<T> _queue;

    mutable std::mutex _mutex;
    mutable std::condition_variable _dispose;
};

_NAME_END

#endif