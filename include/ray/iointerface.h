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
#ifndef _H_RES_LOADER_H_
#define _H_RES_LOADER_H_

#include <ray/platform.h>

#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <memory>

_NAME_BEGIN

class EXPORT IIResource
{
    friend class ResLoader;
public:
    IIResource() noexcept;
    virtual ~IIResource() noexcept;

    void cache(bool _cache) noexcept;
    bool cache() const noexcept;

    bool loaded() const noexcept;

    void name(const std::string& str) noexcept;
    const std::string& name() const noexcept;

    void uncache() noexcept;

    void load();

    virtual void* data() const noexcept = 0;

protected:
    virtual bool doLoad() noexcept = 0;

private:
    IIResource(const IIResource& copy) = delete;
    IIResource& operator=(const IIResource&) = delete;

private:
    bool _loaded;
    bool _cached;

    std::string _name;
};

template<typename T>
class IResource : public IIResource
{
public:
    IResource() noexcept
    {
    }

    IResource(const std::string& name, bool async = false)
    {
        if (async)
            this->asyncLoad(name);
        else
            this->syncLoad(name);
    }

    ~IResource() noexcept
    {
    }

    void* data() const noexcept
    {
        return _resource.get();
    }

private:
    bool doLoad() noexcept
    {
        if (!_resource)
        {
            _resource = std::make_unique<T>();
        }

        return _resource->load(this->name());
    }

public:
    std::unique_ptr<T> _resource;
};

template<typename T>
class Resource final
{
public:
    Resource() noexcept {}
    ~Resource() noexcept {}

    bool loaded() const noexcept
    {
        assert(_impl);
        return _impl->loaded();
    }

    T& syncLoad(const std::string& name) noexcept
    {
        auto result = ResLoader::get().find(name);
        if (!result)
        {
            _impl = std::make_shared<IResource<T>>();
            _impl->name(name);

            ResLoader::get().syncLoad(_impl);
        }
        else
        {
            _impl = result;
        }

        return this->data();
    }

    void asyncLoad(const std::string& name) noexcept
    {
        auto result = ResLoader::get().find(name);
        if (!result)
        {
            _impl = std::make_shared<IResource<T>>();
            _impl->name(name);

            ResLoader::get().asyncLoad(_impl);
        }
        else
        {
            _impl = result;
        }
    }

    T& data() const
    {
        assert(_impl);
        return *(T*)_impl->data();
    }

private:
    Resource(const Resource& copy) = delete;
    Resource& operator=(const Resource&) = delete;

private:
    std::shared_ptr<IIResource> _impl;
};

class EXPORT IoInterface final
{
    __DeclareSingleton(IoInterface)
public:
    IoInterface() noexcept;
    ~IoInterface() noexcept;

    void open() noexcept;
    void close() noexcept;

    void pause() noexcept;
    void resume() noexcept;
    bool running() noexcept;
    void uncache(const IIResource& resource);

    std::size_t size() const noexcept;

    void syncLoad(std::shared_ptr<IIResource> resource);
    void asyncLoad(std::shared_ptr<IIResource> resource);

    void attach(const std::string& path);
    void remove(const std::string& path);

private:
    void dispose();

private:
    IoInterface(const IoInterface&) noexcept = delete;
    IoInterface& operator=(const IoInterface&) noexcept = delete;

private:

    bool _isQuit;
    bool _isPause;

    std::mutex _mutex;
    std::condition_variable _dispose;
    std::queue<std::shared_ptr<IIResource>> _queue;

    std::unique_ptr<std::thread> _disposeThread;
    std::unordered_map<std::string, std::shared_ptr<IIResource>> _map_string;
};

_NAME_END

#endif