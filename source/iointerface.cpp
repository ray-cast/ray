// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/iointerface.h>

_NAME_BEGIN

__ImplementSingleton(IoInterface);

IIResource::IIResource() noexcept
    : _cached(true)
    , _loaded(false)
{
}

IIResource::~IIResource() noexcept
{
}

void
IIResource::cache(bool _cache) noexcept
{
    _cached = _cache;
}

bool
IIResource::cache() const noexcept
{
    return _cached;
}

void
IIResource::uncache() noexcept
{
    IoInterface::instance()->uncache(*this);
}

void
IIResource::load()
{
    if (this->doLoad())
    {
        _loaded = true;
    }
}

bool
IIResource::loaded() const noexcept
{
    return _loaded;
}

void
IIResource::name(const std::string& str) noexcept
{
    _name = str;
}

const std::string&
IIResource::name() const noexcept
{
    return _name;
}

IoInterface::IoInterface() noexcept
    : _isPause(false)
    , _isQuit(false)
{
}

IoInterface::~IoInterface() noexcept
{
    this->close();
}

void
IoInterface::pause() noexcept
{
    _isPause = true;
}

void
IoInterface::resume() noexcept
{
    _isPause = false;
}

bool
IoInterface::running() noexcept
{
    return _isPause;
}

void
IoInterface::syncLoad(std::shared_ptr<IIResource> resource)
{
    assert(resource);
    resource->load();
    if (resource->loaded())
    {
        if (resource->cache())
        {
            _map_string[resource->name()] = resource;
        }
    }
}

void
IoInterface::asyncLoad(std::shared_ptr<IIResource> resource)
{
    if (resource->cache())
    {
        _map_string[resource->name()] = resource;
    }

    _queue.push(resource);
    _dispose.notify_one();
}

void
IoInterface::uncache(const IIResource& res)
{
    _map_string[res.name()].reset();
}

void
IoInterface::attach(const std::string& path)
{
}

void
IoInterface::remove(const std::string& path)
{
}

void
IoInterface::open() noexcept
{
    if (!_disposeThread)
    {
        _disposeThread = std::make_unique<std::thread>(std::bind(&IoInterface::dispose, this));
    }
}

void
IoInterface::close() noexcept
{
    _isQuit = true;
    if (_disposeThread)
    {
        _dispose.notify_one();
        _disposeThread->join();
        _disposeThread.reset();
    }
}

std::size_t
IoInterface::size() const noexcept
{
    return _queue.size();
}

void
IoInterface::dispose()
{
    try
    {
        while (!_isQuit)
        {
            if (!_isPause)
            {
                std::unique_lock<std::mutex> _lock(_mutex);
                if (_queue.empty())
                {
                    _dispose.wait(_lock);
                }

                if (!_queue.empty())
                {
                    auto res = _queue.front();
                    _queue.pop();

                    if (res)
                    {
                        this->syncLoad(res);
                    }
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::microseconds(60));
            }
        }
    }
    catch (...)
    {
        _isQuit = true;
    }
}

_NAME_END