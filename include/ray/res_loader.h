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
#ifndef _H_RESOURCE_H_
#define _H_RESOURCE_H_

#include <ray/iointerface.h>
#include <functional>

_NAME_BEGIN

template<typename T>
class ResLoader : public IoLoader
{
public:
	typedef std::shared_ptr<T> ResourcePtr;
	typedef std::shared_ptr<ResLoader<T>> ResLoaderPtr;
	typedef std::unordered_map<std::string, ResourcePtr> CacheResources;
	typedef std::function<bool(std::shared_ptr<T>, const std::string&)> onLoad;
	typedef std::function<void(std::shared_ptr<T>)> onLoaded;
public:
	ResLoader() noexcept
	{
	}

	virtual ~ResLoader() noexcept
	{
	}

	ResourcePtr data() const noexcept
	{
		return _resource;
	}

	bool load(const std::string& name, onLoad load, bool async = false) except
	{
		if (name.empty())
			return false;

		this->setName(name);

		_onload = load;

		auto resource = _cacheResources[name];
		if (!resource)
		{
			if (!async)
			{
				if (!IoLoader::load())
					return false;

				if (this->isCache())
					this->doCache();
			}
		}
		else
		{
			_resource = resource;
		}

		return true;
	}

	bool load(const std::string& name, onLoad load, onLoaded loaded, bool async = false) except
	{
		if (name.empty())
			return false;

		this->setName(name);

		_onload = load;
		_onloaded = loaded;

		_resource = _cacheResources[name];
		if (!_resource)
		{
			if (!async)
			{
				if (!IoLoader::load())
					return false;

				if (this->isCache())
					this->doCache();
			}
		}
		else
		{
			this->doLoaded();
		}

		return true;
	}

	static ResourcePtr find(const std::string& path)
	{
		return _cacheResources[path];
	}

	bool isLoaded() const
	{
		return _resource ? true : false;
	}

	ResourcePtr getInstance() const noexcept
	{
		return _resource;
	}

private:
	bool doLoad() except
	{
		if (!_resource)
			_resource = std::make_shared<T>();
		return _onload(_resource, this->getName());
	}

	void doLoaded() except
	{
		if (_resource && _onloaded)
			_onloaded(_resource);
	}

	void doCache() except
	{
		_cacheResources[this->getName()] = _resource;
	}

	void doUnCache() except
	{
		_cacheResources[this->getName()].reset();
	}

	std::shared_ptr<IoLoader> clone() const
	{
		auto result = std::make_shared<ResLoader<T>>();
		result->_onload = _onload;
		result->_onloaded = _onloaded;
		result->_resource = _resource;

		return result;
	}

private:
	ResLoader(const ResLoader&) noexcept = delete;
	ResLoader& operator=(const ResLoader&) noexcept = delete;

public:
	onLoad _onload;
	onLoaded _onloaded;

	ResourcePtr _resource;

	static CacheResources _cacheResources;
};

template<typename T> typename ResLoader<T>::CacheResources ResLoader<T>::_cacheResources;

_NAME_END

#endif