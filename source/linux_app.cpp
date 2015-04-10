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
#ifdef _BUILD_PLATFORM_LINUX
#include <ray/linux_app.h>

_NAME_BEGIN

Application::Application() noexcept
{
    this->initialize(0, nullptr);
}

Application::Application(int argc, char **argv) noexcept
{
    this->initialize(argc, argv);
}

Application::~Application() noexcept
{
    if (_display)
    {
        ::XCloseDisplay(_display);
        _display = nullptr;
    }
}

void
Application::initialize(int argc, char **argv)
{
    _display = ::XOpenDisplay(NULL);
}

std::string
Application::getCurrentDirectory()
{
    char file_path[MAX_PATH];
    if (getcwd(file_path, MAX_PATH))
    {
        return std::string(file_path);
    }

    return "";
}

std::string
Application::getModuleFileName(const std::string& filename)
{
    char link_target[MAX_PATH];

    int rval = readlink(filename.c_str(), link_target, sizeof(link_target) - 1);
    if (rval == -1)
        abort();
    else
    {
        link_target[rval] = '\0';

        char* last_slash = std::strrchr(link_target, '/');
        if (last_slash == 0 || last_slash == link_target)
        {
            abort();
        }

        return std::string(link_target, last_slash - link_target);
    }
}

std::string
Application::getModuleDirectory()
{
    return getModuleFileName("/proc/self/exe");
}

std::string
Application::getEnvironmentVariable(const std::string& name)
{
}

bool
Application::poolEvents(int& exitcode) noexcept
{
}

bool
Application::waitEvents(int& exitcode) noexcept
{
    XEvent evt;

    XNextEvent(_display, &evt);

    switch (evt.type)
    {
    case KeyPress:
        return true;
    }

    return false;
}

_NAME_END

#endif