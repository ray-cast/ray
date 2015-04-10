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
#ifndef _H_COMPUTER_H_
#define _H_COMPUTER_H_

#include <ray/platform.h>
#include <string>

_NAME_BEGIN

class Computer
{
    enum
    {
        EAX = 0,
        EBX = 1,
        ECX = 2,
        EDX = 3,

        FLAG_VENDOR = 0x0,
        FLAG_BRAND = 0x80000000,
        FLAG_MMX = 0x1,
        FLAG_SSE = 0x1,
        FLAG_AVX = 0x1,
        FLAG_AVX2 = 0x7,
        FLAG_XSAVE = 0x1,
        FLAG_EXT = 0x080000000,
        FLAG_3DNOW = 0x080000001,
        FLAG_XCRO0 = 0xd,

        BIT_MMX = 0x00800000,
        BIT_SSE = 0x02000000,
        BIT_SSE2 = 0x04000000,
        BIT_SSE3 = 0x00000001,
        BIT_SSE3S = 0x00000100,
        BIT_SSE41 = 0x00080000,
        BIT_SSE42 = 0x00100000,
        BIT_SSE4A = 0x00100000,
        BIT_SSE5 = 0x00100000,
        BIT_AVX2 = 0x00000020,
        BIT_XSAVE = 0x0000001A,
        BIT_OSXSAVE = 0x18000000,
        BIT_3DNOW = 0x80000000,
        BIT_EXT = 0x80000000,
        BIT_XCRO_X87 = 0x00000001,
        BIT_XCRO_SSE = 0x00000002,
        BIT_XCRO_YMM = 0x00000004,
        BIT_XCRO = BIT_XCRO_X87 | BIT_XCRO_SSE | BIT_XCRO_YMM,
    };

public:
    Computer();
    ~Computer();

    std::string& getVendorName();
    std::string& getBrandName();

    bool try_mmx();

    bool try_sse();
    bool try_sse2();
    bool try_sse3();
    bool try_sse3s();
    bool try_sse41();
    bool try_sse42();

    bool try_ext();
    bool try_3dnow();

    bool try_avx();
    bool try_avx2();
    bool try_xsave();
    bool try_osxsave();

    bool try_xcr0();
    bool try_xcr0_x87();
    bool try_xcr0_sse();
    bool try_xcr0_ymm();

    bool try_simd_mmx();
    bool try_simd_sse();

    bool openOSXSAVE();

private:

    bool try_sigill(bool(*pfunc)(void*), void* puserdata);
    void get_cpuid(std::int32_t info[4], std::int32_t type);

    mutable bool _mmx;
    mutable bool _sse;
    mutable bool _sse2;
    mutable bool _sse3;
    mutable bool _sse41;
    mutable bool _sse42;
    mutable bool _3dnow;
    mutable bool _ext;

    std::string _vendor_name;
    std::string _brand_name;
};

_NAME_END

#endif