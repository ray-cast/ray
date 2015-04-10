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
#include <ray/ioserver.h>
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

void
IoServer::mountArchives() noexcept
{
    _enablePackage = true;
}

void
IoServer::unmountArchives() noexcept
{
    _enablePackage = false;
}

void
IoServer::addAssign(const IoAssign& assign) noexcept
{
    auto path = assign.getPath();
    if (*path.rbegin() != '\\' &&
        *path.rbegin() != '/')
    {
        path += SEPARATOR;
    }

    _assignTable[assign.getName()] = path;
}

void
IoServer::removeAssign(const std::string& name) noexcept
{
    auto it = _assignTable.find(name);
    if (it != _assignTable.end())
    {
        _assignTable.erase(it);
    }
}

const std::string&
IoServer::getAssign(const std::string& name) const noexcept
{
    return _assignTable.at(name);
}

bool
IoServer::openFile(const std::string& path, MemoryStream& stream) noexcept
{
    bool result = this->openFileFromFileSystem(path, stream);
    if (!result)
    {
        result = this->openFileFromDisk(path, stream);
    }

    return result;
}

bool
IoServer::openFileFromFileSystem(const std::string& path, MemoryStream& stream) noexcept
{
    return false;
}

bool
IoServer::openFileFromDisk(const std::string& path, MemoryStream& stream) noexcept
{
    auto resolvePath = this->resolveAssignsInString(path);
    if (resolvePath.empty())
        return false;

    fstream fileStream;

    if (fileStream.open(resolvePath))
    {
        stream.setOpenMode(fileStream.getOpenMode());
        if (stream.open(path))
        {
            streamsize size = fileStream.size();
            if (size > 0)
            {
                stream.resize(size);
                char* buf = stream.map();
                assert(buf);
                fileStream.read(buf, size);
                stream.unmap();
                stream.clear(ios_base::goodbit);
                return  true;
            }
        }
    }

    stream.setstate(ios_base::failbit);
    return false;
}

bool
IoServer::openFile(const std::wstring& path, MemoryStream& stream) noexcept
{
    bool result = this->openFileFromFileSystem(path, stream);
    if (!result)
    {
        result = this->openFileFromDisk(path, stream);
    }

    return result;
}

bool
IoServer::openFileFromFileSystem(const std::wstring& path, MemoryStream& stream) noexcept
{
    return false;
}

bool
IoServer::openFileFromDisk(const std::wstring& path, MemoryStream& stream) noexcept
{
    fstream fileStream;

    if (fileStream.open(path))
    {
        stream.setOpenMode(fileStream.getOpenMode());
        if (fileStream.open(path))
        {
            streamsize size = fileStream.size();
            if (size > 0)
            {
                stream.resize(size);
                char* buf = stream.map();
                assert(buf);
                fileStream.read(buf, size);
                stream.unmap();
                return  true;
            }
        }
    }

    return false;
}

bool
IoServer::deleteFile(const std::string& path) noexcept
{
    return false;
}

bool
IoServer::existsFile(const std::string& path) noexcept
{
    return false;
}

bool
IoServer::createDirectory(const std::string& path) noexcept
{
    return false;
}

bool
IoServer::deleteDirectory(const std::string& path) noexcept
{
    return false;
}

bool
IoServer::existsDirectory(const std::string& path) noexcept
{
    return false;
}

std::string
IoServer::resolveAssignsInString(const std::string& url) noexcept
{
    std::string result = url;

    int index = url.find_first_of(":", 0);
    if (index > 1)
    {
        std::string assign = url.substr(0, index);
        std::string path = this->getAssign(assign);
        if (!path.empty())
        {
            result.replace(result.begin(), result.begin() + index + 1, path);
            return result;
        }
    }

    return result;
}

_NAME_END