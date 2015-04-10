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
#ifdef _BUILD_PLATFORM_WINDOWS
#include <ray/msw_app.h>
#include <ray/msw_wx.h>

#include <commctrl.h>

#define MAX_BUFFER_LENGTH 1024

_NAME_BEGIN

LRESULT CALLBACK dispose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /* Get the window data for the window */
    MSWTopLevelWindow* window = nullptr;

    auto instance = BaseTopLevelWindow::instances();
    for (auto& it : instance)
    {
        MSWTopLevelWindow* wx = dynamic_cast<MSWTopLevelWindow*>(it);
        if (wx)
        {
            if (hwnd == wx->getWindowHandle())
            {
                window = wx;
                break;
            }
        }
    }

    if (window)
    {
        switch (msg)
        {
        case WM_SETFOCUS:
        {
            AppEvent event;
            event.event = AppEvent::GetFocus;
            event.window.timestamp = ::timeGetTime();

            window->postEvent(event);
        }
        break;
        case WM_KILLFOCUS:
        {
            AppEvent event;
            event.event = AppEvent::LostFocus;
            event.window.timestamp = ::timeGetTime();

            window->postEvent(event);
        }
        break;
        case WM_ERASEBKGND:
        {
            return true;
        }
        break;
        case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }
        break;
        case WM_CLOSE:
        {
            window->close();
        }
        break;
        }
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

MSWTopLevelApplication::MSWTopLevelApplication() noexcept
{
    char buffer[MAX_BUFFER_LENGTH];

    DWORD size = ::GetModuleFileName(nullptr, buffer, 1024);
    if (size < 1024)
    {
        std::string dir = std::string(buffer, size);

        std::string::size_type pos = dir.find_last_of("\\/");
        if (pos != std::string::npos)
        {
            dir.erase(pos, dir.length() - pos);
            ::SetCurrentDirectory(dir.c_str());
        }
    }
}

MSWTopLevelApplication::~MSWTopLevelApplication() noexcept
{
}

bool
MSWTopLevelApplication::initialize() noexcept
{
    ::SetDllDirectory(".");
    ::InitCommonControls();
    ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

    WNDCLASSEX wc;

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = dispose;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ::GetModuleHandle(0);
    wc.hCursor = ::LoadCursor(0, IDC_ARROW);
    wc.hIcon = ::LoadIcon(::GetModuleHandle(0), IDI_APPLICATION);
    wc.hIconSm = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpszClassName = "AppFramework";

    if (!RegisterClassEx(&wc))
        return false;

    return true;
}

std::string
MSWTopLevelApplication::getCurrentDirectory() noexcept
{
    DWORD length = 1024;

    for (DWORD i = 0; i < 5; i++)
    {
        std::unique_ptr<char[]> buffer(new char[length]);

        DWORD size = ::GetCurrentDirectoryA(length, buffer.get());

        if (length > size)
        {
            std::string dir = std::string(buffer.get(), size);

            std::string::size_type pos = dir.find_last_of("\\/:");
            if (pos != std::string::npos)
            {
                dir.erase(pos, dir.length() - pos);
                return dir;
            }
        }
    }

    return "";
}

std::string
MSWTopLevelApplication::getModuleFileName(HMODULE hModule) noexcept
{
    DWORD length = MAX_BUFFER_LENGTH;

    for (DWORD i = 0; i < 5; i++)
    {
        std::unique_ptr<TCHAR[]> buffer(new TCHAR[length]);

        DWORD size = ::GetModuleFileName(hModule, buffer.get(), length);

        if (length > size)
            return std::string(buffer.get(), size);
    }

    return "";
}

std::string
MSWTopLevelApplication::getModuleDirectory() noexcept
{
    DWORD length = 1024;

    for (DWORD i = 0; i < 5; i++)
    {
        std::unique_ptr<char[]> buffer(new char[length]);

        DWORD size = ::GetModuleFileName(::GetModuleHandle(0), buffer.get(), length);

        if (length > size)
        {
            std::string dir = std::string(buffer.get(), size);

            std::string::size_type pos = dir.find_last_of("\\/");
            if (pos != std::string::npos)
            {
                dir.erase(pos, dir.length() - pos);
                return dir;
            }
        }
    }

    return "";
}

std::string
MSWTopLevelApplication::getEnvironmentVariable(const std::string& name) noexcept
{
    DWORD length = 1024;

    for (;;)
    {
        std::unique_ptr<char[]> buffer(new char[length]);

        DWORD size = ::GetEnvironmentVariableA(name.data(), buffer.get(), length);

        if (0 == size)
            break;

        if (length > size)
            return std::string(buffer.get(), size);
    }

    return "";
}

HMODULE
MSWTopLevelApplication::loadLibrarys(const std::string& dllname) noexcept
{
    if (dllname.empty())
        return nullptr;

    HMODULE result = nullptr;

    std::string module = getModuleFileName(nullptr);
    if (!module.empty())
    {
        module += ".local";

        if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(module.data()))
        {
            std::string var = MSWTopLevelApplication::getEnvironmentVariable("ShaderObjectFiles");

            if (!var.empty())
            {
                var += "\\Debugging Tools for Windows\\" + dllname;

                if (INVALID_FILE_ATTRIBUTES != GetFileAttributesA(var.data()))
                    result = ::LoadLibrary(var.data());
            }

            if (nullptr == result)
            {
                var = MSWTopLevelApplication::getEnvironmentVariable("ShaderObjectFiles");
                if (!var.empty())
                {
                    var += "\\Debugging Tools for Windows 64-Bit\\" + dllname;

                    if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(var.data()))
                        result = ::LoadLibrary(var.data());
                }
            }
        }
    }

    if (!result)
        result = ::LoadLibrary(dllname.data());

    return result;
}

void
MSWTopLevelApplication::peekEvents(AppEvent& event) noexcept
{
    this->update();
    return BaseTopLevelApplication::peekEvents(event);
}

bool
MSWTopLevelApplication::pollEvents(AppEvent& event) noexcept
{
    this->update();
    return BaseTopLevelApplication::pollEvents(event);
}

bool
MSWTopLevelApplication::waitEvents(AppEvent& event) noexcept
{
    this->update();
    return BaseTopLevelApplication::waitEvents(event);
}

bool
MSWTopLevelApplication::waitEvents(AppEvent& event, int timeout) noexcept
{
    this->update();
    return BaseTopLevelApplication::waitEvents(event, timeout);
}

void
MSWTopLevelApplication::flushEvent() noexcept
{
    this->update();
    BaseTopLevelApplication::flushEvent();
}

void
MSWTopLevelApplication::update() noexcept
{
    MSG _message;

    while (::PeekMessage(&_message, 0, 0, 0, PM_REMOVE | PM_QS_PAINT | PM_QS_POSTMESSAGE | PM_QS_SENDMESSAGE))
    {
        switch (_message.message)
        {
        case WM_QUIT:
        {
            AppEvent event;
            event.event = AppEvent::AppQuit;
            event.quit.timestamp = ::timeGetTime();
            event.quit.code = _message.wParam;

            this->postEvent(event);
        }
        break;
        default:
        {
            ::TranslateMessage(&_message);
            ::DispatchMessage(&_message);
        }
        }
    }

    auto instance = BaseTopLevelWindow::instances();
    for (auto& it : instance)
    {
        if (it == this)
            continue;

        AppEvent event;

        while (it->pollEvents(event))
        {
            this->postEvent(event);
        }
    }
}

_NAME_END

#endif