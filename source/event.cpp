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
#include <ray/event.h>

_NAME_BEGIN

EvtHandler::EvtHandler() noexcept
    : _enableEventPosting(true)
{
}

EvtHandler::~EvtHandler() noexcept
{
}

void
EvtHandler::enableEventPosting(bool enable) noexcept
{
    _enableEventPosting = enable;
}

bool
EvtHandler::enableEventPosting() const noexcept
{
    return _enableEventPosting;
}

void
EvtHandler::sendEvent(const AppEvent& event) noexcept
{
}

void
EvtHandler::postEvent(const AppEvent& event) noexcept
{
    if (_enableEventPosting)
    {
        _mutex.lock();
        _events.push(event);
        _mutex.unlock();

        _dispose.notify_one();
    }
}

void
EvtHandler::peekEvents(AppEvent& event) noexcept
{
}

bool
EvtHandler::pollEvents(AppEvent& event) noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (lock)
    {
        if (!_events.empty())
        {
            event = _events.front();
            _events.pop();
            return true;
        }
    }

    return false;
}

bool
EvtHandler::waitEvents(AppEvent& event) noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (_events.empty())
    {
        _dispose.wait(lock);
    }

    return this->pollEvents(event);
}

bool
EvtHandler::waitEvents(AppEvent& event, int timeout) noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (_events.empty())
    {
        _dispose.wait_for(lock, std::chrono::milliseconds(timeout));
    }

    return this->pollEvents(event);
}

void
EvtHandler::flushEvent() noexcept
{
    _mutex.lock();
    _events = std::queue<AppEvent>();
    _mutex.unlock();
}

_NAME_END