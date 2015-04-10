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
#ifndef _H_ERR_H_
#define _H_ERR_H_

#include <ray/platform.h>
#include <ray/string.h>
#include <ray/memory.h>
#include <vector>

_NAME_BEGIN

template<typename _Dummy>
class basic_error
{
public:
    typedef std::size_t int_type;

    static int_type min() noexcept
    {
        return (0);
    }

    static int_type max() noexcept
    {
        return (int_type)_table.size();
    }

    static const _Dummy& get(int_type n) noexcept
    {
        return _table[n];
    }
private:

    static std::vector<_Dummy> _table;
};

template<typename T> std::vector<T> basic_error<T>::_table;

template<>
#if _UNICODE_
class basic_error<int> : public basic_error<wchar_t*>
#else
class basic_error<int> : public basic_error<char*>
#endif
{
public:
    static const int UNKNOWN_ERROR = 0;
    static const int UNREGISTERING_CUSTOM_IMPORTER = 1;
    static const int NO_SUITABLE_READER = 2;
    static const int UNKNOWN_BITDEPTH_IN_FILE = 3;
    static const int UNKNOWN_ENCODING_IN_FILE = 4;
    static const int FILE_EXTENSION_IS_ALREADY_IN_USE = 5;
    static const int ENCODING_DOESNT_MATCH_BITDEPTH = 6;
    static const int INVALID_IMAGE = 7;
    static const int IMAGE_WIDTH_TOO_LARGE = 8;
    static const int IMAGE_HEIGHT_TOO_LARGE = 9;
    static const int FILE_SIZE_TOO_LARGE = 10;
    static const int INVALID_CALLBACK_ERROR = 11;
    static const int MEMORY_ALLOCATION_ERROR = 12;
    static const int STREAM_FAILURE = 13;
    static const int DIDNT_INHERIT_THE_FUNCTION = 14;
    static const int EMPTY_STREAM = 15;
};

class EXPORT error_code : public basic_error<int>
{
public:
    typedef basic_error<int> _Mybase;

    error_code() noexcept;
    explicit error_code(int_type code) noexcept;

    error_code& assign(int_type value) noexcept;

    int_type value() const noexcept;

    explicit operator const char*() noexcept;
    explicit operator const wchar_t*() noexcept;

private:

    int_type _Myval;
};

inline error_code make_error_code(error_code::int_type _Errno = error_code::UNKNOWN_ERROR) noexcept
{    // make an error_code
    return (error_code(_Errno));
}

_NAME_END

#endif
