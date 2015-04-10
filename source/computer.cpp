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
#include <ray/computer.h>

#ifdef __LINUX__
#include <x86intrin.h>
#include <cpuid.h>
#else
#include <intrin.h>
#endif

#include <signal.h>
#include <csetjmp>

_NAME_BEGIN

Computer::Computer()
    :_mmx(0)
    , _sse(0)
{
}

Computer::~Computer()
{
}

static std::jmp_buf* volatile _s_jmpback = nullptr;

void catch_sigill(int sig)
{
    if (_s_jmpback)
        ::longjmp(*_s_jmpback, 1);

    ::abort();
}

bool
Computer::try_sigill(bool(*pfunc)(void*), void* puserdata)
{
    bool result = false;
    std::jmp_buf jmp;
    std::jmp_buf* poldjmp = nullptr;

    void(*old_signal)(int) = SIG_DFL;

    if (0 == setjmp(jmp))
    {
        poldjmp = _s_jmpback;

        _s_jmpback = &jmp;

        old_signal = ::signal(SIGILL, catch_sigill);

        result = pfunc(puserdata);
    }

    _s_jmpback = poldjmp;

    ::signal(SIGILL, old_signal);

    return result;
}

bool
Computer::try_simd_mmx()
{
    /*auto try_mmx_func = [](void*) -> bool
    {
        ::_mm_empty();
        return true;
    };

    return this->try_sigill(try_mmx_func, 0) ? true : false;*/
    return false;
}

bool
Computer::try_simd_sse()
{
    auto try_sse_func = [](void*) -> bool
    {
        volatile __m128 xmm1 = ::_mm_setzero_ps();

        return 0 == *(int*)&xmm1;
    };

    return this->try_sigill(try_sse_func, 0) ? true : false;
}

bool
Computer::try_mmx()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_MMX);

    if (dwBuf[EDX] & BIT_MMX)
    {
        if (this->try_simd_mmx())
            _mmx = true;
    }

    return false;
}

bool
Computer::try_sse()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE)
        return true;

    return false;
}

bool
Computer::try_sse2()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE2)
        return true;

    return false;
}

bool
Computer::try_sse3()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE3)
        return true;

    return false;
}

bool
Computer::try_sse3s()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE3S)
        return true;

    return false;
}

bool
Computer::try_sse41()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE41)
        return true;

    return false;
}

bool
Computer::try_sse42()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_SSE);

    if (dwBuf[EDX] & BIT_SSE42)
        return true;

    return false;
}

bool
Computer::try_avx()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_AVX);

    if (dwBuf[ECX] & BIT_XSAVE)
        return true;

    return false;
}

bool
Computer::try_avx2()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_AVX2);

    if (dwBuf[EBX] & BIT_AVX2)
        return true;

    return false;
}

bool
Computer::try_xsave()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_XSAVE);

    if (dwBuf[ECX] & BIT_XSAVE)
        return true;

    return false;
}

bool
Computer::try_osxsave()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_AVX);

    if (dwBuf[ECX] & BIT_OSXSAVE)
        return true;

    return false;
}

bool
Computer::try_ext()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_EXT);

    if (dwBuf[EAX] & BIT_EXT)
        return true;

    return false;
}

bool
Computer::try_3dnow()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_3DNOW);

    if (dwBuf[EDX] & BIT_3DNOW)
        return true;

    return false;
}

bool
Computer::try_xcr0()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_XCRO0);

    if (dwBuf[EDX] & BIT_XCRO)
        return true;

    return false;
}

bool
Computer::try_xcr0_x87()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_XCRO0);

    if (dwBuf[EDX] & BIT_XCRO_X87)
        return true;

    return false;
}

bool
Computer::try_xcr0_sse()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_XCRO0);

    if (dwBuf[EDX] & BIT_XCRO_SSE)
        return true;

    return false;
}

bool
Computer::try_xcr0_ymm()
{
    std::int32_t dwBuf[4] = { 0 };

    get_cpuid(dwBuf, FLAG_XCRO0);

    if (dwBuf[EDX] & BIT_XCRO_YMM)
        return true;

    return false;
}

std::string&
Computer::getVendorName()
{
    if (_vendor_name.empty())
    {
        std::int32_t name[4] = { 0 };

        get_cpuid(name, FLAG_VENDOR);

        std::int32_t vendor[5] =
        {
            name[1],
            name[3],
            name[2],
            0
        };

        _vendor_name = (char*)vendor;
    }

    return _vendor_name;
}

std::string&
Computer::getBrandName()
{
    if (_brand_name.empty())
    {
        std::int32_t name[13] = { 0 };

        get_cpuid(name, FLAG_BRAND);

        if (name[0] < 0x80000004)
            return _brand_name;

        get_cpuid(&name[0], 0x80000002);
        get_cpuid(&name[5], 0x80000002);
        get_cpuid(&name[9], 0x80000002);

        _brand_name = (char*)name;
    }

    return _brand_name;
}

bool
Computer::openOSXSAVE()
{
    if (try_xsave() && try_xcr0())
    {
#ifdef _VISUAL_STUDIO_
        //        __asm cli

        //         __asm mov eax, cr4
        //         __asm or  eax, 18
        //         __asm mov cr4, eax
#endif

        return true;
    }

    return false;
}

void
Computer::get_cpuid(std::int32_t info[4], std::int32_t type)
{
#ifdef __LINUX__
    __cpuid(info[0], info[1], info[2], info[3], type);
#else

#endif
}

_NAME_END