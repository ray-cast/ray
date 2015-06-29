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
#ifndef _H_DEF_H_
#define _H_DEF_H_

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <memory>
#include <vector>
#include <list>
#include <map>

#include <iostream>

#ifndef _BYTE_BITMASK
#   define _BYTE_BITMASK(Enum, Ty) typedef unsigned char Ty
#endif

#ifndef _BOOL_BITMASK
#   define _BOOL_BITMASK(Enum, Ty) typedef bool Ty
#endif

#ifndef _INT_BITMASK
#   define _INT_BITMASK(Enum, Ty) typedef int Ty
#endif

#ifndef _UINT_BITMASK
#   define _UINT_BITMASK(Enum, Ty) typedef unsigned int Ty
#endif

#if defined(__cplusplus)
#   define _NAME_BEGIN namespace ray {
#   define _NAME_END }
#   define _NAME ::ray::
#else
#   define _NAME_BEGIN
#   define _NAME_END
#   define _NAME
#endif

#if defined(_MSC_VER)
#   if !defined(_VISUAL_STUDIO_)
#       define _VISUAL_STUDIO_ _MSC_VER
#   endif

#   if defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ <= 1100
#       error "IDE not supported!"
#   elif defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ <= 1200
#       error "IDE not supported!"
#   elif defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ <= 1300
#       error "IDE not supported!"
#   elif defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ <= 1400
#       error "IDE not supported!"
#   elif defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ <= 1800
#   endif

#   if defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ >= 1400 && !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
#        define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#   endif
#endif

#if defined(_VISUAL_STUDIO_) && (_VISUAL_STUDIO_ <= 1800)
#   ifndef noexcept
#       define noexcept
#   endif
#endif

#ifndef except
#	if defined(_VISUAL_STUDIO_) && (_VISUAL_STUDIO_ <= 1800)
#       define except
#	else
#       define except noexcept(false)
#	endif
#endif

#endif