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
#ifndef _H_MSW_APP_H_
#define _H_MSW_APP_H_

#include <ray/appbase.h>

_NAME_BEGIN

class EXPORT MSWTopLevelApplication : public BaseTopLevelApplication
{
public:
    MSWTopLevelApplication() noexcept;
    virtual ~MSWTopLevelApplication() noexcept;

    virtual bool initialize() noexcept;

    virtual void peekEvents(AppEvent& event) noexcept;
    virtual bool pollEvents(AppEvent& event) noexcept;
    virtual bool waitEvents(AppEvent& event) noexcept;
    virtual bool waitEvents(AppEvent& event, int timeout) noexcept;
    virtual void flushEvent() noexcept;

    std::string getCurrentDirectory() noexcept;
    std::string getModuleFileName(HMODULE module) noexcept;
    std::string getModuleDirectory() noexcept;
    std::string getEnvironmentVariable(const std::string& name) noexcept;

    HMODULE loadLibrarys(const std::string& dllname) noexcept;

private:

    void update() noexcept;

private:

    HMODULE _instance;
};

_NAME_END

#endif