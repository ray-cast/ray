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
#include <ray/iolistener.h>
#include <vector>

_NAME_BEGIN

std::vector<ioListener*> _listeners;

void
ioListener::onAttach() noexcept
{
}

void
ioListener::onRemove() noexcept
{
}

void
ioListener::onOpenBefor(const std::string& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListener::onOpenBefor(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListener::onOpenFail(const std::string& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListener::onOpenFail(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListener::onOpenSuccess(const std::string& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListener::onOpenSuccess(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
}

void
ioListenerPoster::onOpenBefor(const std::string& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenBefor(filename, mode);
    }
}

void
ioListenerPoster::onOpenBefor(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenBefor(filename, mode);
    }
}

void
ioListenerPoster::onOpenFail(const std::string& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenFail(filename, mode);
    }
}

void
ioListenerPoster::onOpenFail(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenFail(filename, mode);
    }
}

void
ioListenerPoster::onOpenSuccess(const std::string& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenSuccess(filename, mode);
    }
}

void
ioListenerPoster::onOpenSuccess(const std::wstring& filename, const ios_base::openmode mode) noexcept
{
    for (auto& it : _listeners)
    {
        it->onOpenSuccess(filename, mode);
    }
}

ioListenerRegister::ioListenerRegister() noexcept
{
}

ioListenerRegister::~ioListenerRegister() noexcept
{
    for (auto& it : _listeners)
        delete it;
}

void
ioListenerRegister::addListener(ioListener* listener) noexcept
{
    if (std::find(_listeners.begin(), _listeners.end(), listener) == _listeners.end())
    {
        listener->onAttach();
        _listeners.push_back(listener);
    }
}

void
ioListenerRegister::removeListener(const ioListener* listener) noexcept
{
    auto it = std::find(_listeners.begin(), _listeners.end(), listener);
    if (it != _listeners.end())
    {
        (*it)->onRemove();
        _listeners.erase(it);
    }
}

_NAME_END