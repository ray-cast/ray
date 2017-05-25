// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/ioserver.h>
#include <ray/iolistener.h>
#include <ray/fstream.h>

_NAME_BEGIN

__ImplementSingleton(IoServer)

IoServer::IoServer() noexcept
	: _enablePackage(false)
{
}

IoServer::~IoServer() noexcept
{
}

IoServer&
IoServer::addIoListener(IoListenerPtr& listener) noexcept
{
	if (_ioListener.end() == std::find(_ioListener.begin(), _ioListener.end(), listener))
		_ioListener.push_back(listener);

	return *this;
}

IoServer&
IoServer::removeIoListener(IoListenerPtr& listener) noexcept
{
	auto it = std::find(_ioListener.begin(), _ioListener.end(), listener);
	if (it != _ioListener.end())
		_ioListener.erase(it);

	return *this;
}

IoServer&
IoServer::mountArchives() noexcept
{
	_enablePackage = true;

	this->setstate(ios_base::goodbit);
	return *this;
}

IoServer&
IoServer::unmountArchives() noexcept
{
	_enablePackage = false;

	this->setstate(ios_base::goodbit);
	return *this;
}

IoServer&
IoServer::addAssign(const IoAssign& assign) noexcept
{
	auto path = assign.path();
	if (*path.rbegin() != '\\' &&
		*path.rbegin() != '/')
	{
		path += SEPARATOR;
	}

	auto& entry = _assignTable[assign.name()];
	if (!entry.empty())
	{
		if (entry == assign.path())
			return *this;
		else
		{
			this->setstate(ios_base::failbit);
			return *this;
		}
	}

	_assignTable[assign.name()] = path;

	this->setstate(ios_base::goodbit);
	return *this;
}

IoServer&
IoServer::removeAssign(const util::string& name) noexcept
{
	auto it = _assignTable.find(name);
	if (it != _assignTable.end())
	{
		_assignTable.erase(it);

		this->setstate(ios_base::goodbit);
		return *this;
	}

	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::getAssign(const util::string& name, util::string& path) noexcept
{
	path = _assignTable[name];
	if (path.empty())
		this->setstate(ios_base::failbit);
	else
		this->setstate(ios_base::goodbit);
	return *this;
}

IoServer&
IoServer::getResolveAssign(const util::string& url, util::string& resolvePath) noexcept
{
	util::string result = url;

	std::size_t index = url.find_first_of(":", 0);
	if (index > 1)
	{
		util::string path;
		util::string assign = url.substr(0, index);

		bool success = this->getAssign(assign, path);
		if (success)
		{
			result.replace(result.begin(), result.begin() + index + 1, path);
			resolvePath = result;

			this->setstate(ios_base::goodbit);
			return *this;
		}
	}

	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFile(StreamReaderPtr& stream, const util::string& path, open_mode mode) noexcept
{
	bool result = this->openFileFromFileSystem(stream, path, mode);
	if (!result)
		result = this->openFileFromDisk(stream, path, mode);
	return *this;
}

IoServer&
IoServer::openFile(StreamReaderPtr& stream, util::string::const_pointer path, open_mode mode) noexcept
{
	bool result = this->openFileFromFileSystem(stream, path, mode);
	if (!result)
		result = this->openFileFromDisk(stream, path, mode);
	return *this;
}

IoServer&
IoServer::openFileFromFileSystem(StreamReaderPtr& stream, const util::string& path, open_mode mode) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFileFromFileSystem(StreamReaderPtr& stream, util::string::const_pointer path, open_mode mode) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFileFromDisk(StreamReaderPtr& result, util::string::const_pointer path, open_mode mode) noexcept
{
	return this->openFileFromDisk(result, std::string(path), mode);
}

IoServer&
IoServer::openFileFromDisk(StreamReaderPtr& result, const util::string& path, open_mode mode) noexcept
{
	for (auto& listener : _ioListener)
		listener->onMessage("loading resource : " + path);

	util::string resolvePath;
	this->getResolveAssign(path, resolvePath);

	if (resolvePath.empty())
		resolvePath = path;

	if (this->existsFileFromDisk(resolvePath))
	{
		auto stream = std::make_shared<fstream>();
		stream->setOpenMode(mode);
		if (stream->open(resolvePath))
		{
			result = stream;
			this->setstate(ios_base::goodbit);
			return *this;
		}
	}

	for (auto& assign : _assignTable)
	{
		resolvePath = assign.second + path;

		if (this->existsFileFromDisk(resolvePath))
		{
			auto stream = std::make_shared<fstream>();
			stream->setOpenMode(mode);
			if (stream->open(resolvePath))
			{
				result = stream;
				this->setstate(ios_base::goodbit);
				return *this;
			}
		}
	}

	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFile(StreamWritePtr& stream, const util::string& path, open_mode mode) noexcept
{
	bool result = this->openFileFromFileSystem(stream, path, mode);
	if (!result)
		result = this->openFileFromDisk(stream, path, mode);
	return *this;
}

IoServer&
IoServer::openFileFromFileSystem(StreamWritePtr& stream, const util::string& path, open_mode mode) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFileFromDisk(StreamWritePtr& result, const util::string& path, open_mode mode) noexcept
{
	for (auto& listener : _ioListener)
		listener->onMessage("export resource : " + path);

	util::string resolvePath;
	this->getResolveAssign(path, resolvePath);

	if (resolvePath.empty())
		resolvePath = path;

	auto stream = std::make_shared<fstream>();
	stream->setOpenMode(mode);
	if (stream->open(resolvePath))
	{
		result = stream;
		this->setstate(ios_base::goodbit);
		return *this;
	}

	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFile(StreamPtr& stream, const util::string& path, open_mode mode) noexcept
{
	bool result = this->openFileFromFileSystem(stream, path, mode);
	if (!result)
		result = this->openFileFromDisk(stream, path, mode);
	return *this;
}

IoServer&
IoServer::openFileFromFileSystem(StreamPtr& stream, const util::string& path, open_mode mode) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::openFileFromDisk(StreamPtr& result, const util::string& path, open_mode mode) noexcept
{
	assert(false);

	for (auto& listener : _ioListener)
		listener->onMessage("loading resource : " + path);

	util::string resolvePath;
	this->getResolveAssign(path, resolvePath);

	if (resolvePath.empty())
		resolvePath = path;

	if (!this->existsFileFromDisk(resolvePath))
	{
		this->setstate(ios_base::failbit);
		return *this;
	}

	auto stream = std::make_shared<fstream>();
	stream->setOpenMode(mode);

	if (stream->open(resolvePath))
	{
		result = stream;
		this->clear(ios_base::goodbit);
		return *this;
	}

	return *this;
}

IoServer&
IoServer::deleteFile(const util::string& path) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::existsFile(const util::string& path) noexcept
{
	bool result = this->existsFileFromFileSystem(path);
	if (!result)
	{
		result = this->existsFileFromDisk(path);
	}

	return *this;
}

IoServer&
IoServer::existsFileFromFileSystem(const util::string& path) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::existsFileFromDisk(const util::string& path) noexcept
{
	util::string resolvePath;
	this->getResolveAssign(path, resolvePath);

	if (resolvePath.empty())
		resolvePath = path;

	if (faccess(resolvePath.c_str(), 0) == 0)
	{
		this->clear(ios_base::goodbit);
		return *this;
	}

	for (auto& assign : _assignTable)
	{
		resolvePath = assign.second + path;

		if (faccess(resolvePath.c_str(), 0) == 0)
		{
			this->clear(ios_base::goodbit);
			return *this;
		}
	}

	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::copyFile(const util::string& path, const util::string& to) noexcept
{
	return *this;
}

IoServer&
IoServer::createDirectory(const util::string& path) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::deleteDirectory(const util::string& path) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

IoServer&
IoServer::existsDirectory(const util::string& path) noexcept
{
	this->setstate(ios_base::failbit);
	return *this;
}

_NAME_END