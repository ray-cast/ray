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
#include <ray/err.h>

_NAME_BEGIN

template<>
std::vector<std::string> basic_error<std::string>::_table =
{
    "Successful operation",
    "Unable to remove custom importer: I can't find you ...",
    "No suitable reader found for the file format of stream .",
    "Unknown bitdepth in file.",
    "Unknown encoding in file.",
    "The file extension is already in use",
    "Encoding doesn't match bitdepth.",
    "invalid image",
    "Image width too large",
    "Image height too large",
    "File size too large",
    "Callbacks is Invalid.",
    "Memory allocation failure",
    "Stream operator failure",
    "Calling a didn't inherit the function",
    "This stream is not buf"
};

template<>
std::vector<std::wstring> basic_error<std::wstring>::_table =
{
    L"Successful operation",
    L"Unable to remove custom importer: I can't find you ...",
    L"No suitable reader found for the file format of stream .",
    L"Unknown bitdepth in file.",
    L"Unknown encoding in file.",
    L"The file extension is already in use",
    L"Encoding doesn't match bitdepth.",
    L"invalid image",
    L"Image width too large",
    L"Image height too large",
    L"File size too large",
    L"Callbacks is Invalid.",
    L"Memory allocation failure",
    L"Stream operator failure",
    L"Calling a didn't inherit the function",
    L"This stream is not buf"
};

error_code::error_code() noexcept
{
    this->assign(_Mybase::UNKNOWN_ERROR);
}

error_code::error_code(int_type code) noexcept
{
    this->assign(code);
}

error_code&
error_code::assign(int_type value) noexcept
{
    assert(value >= _Mybase::min());
    assert(value <= _Mybase::max());
    _Myval = value;
    return *this;
}

error_code::int_type
error_code::value() const noexcept
{
    return _Myval;
}

error_code::operator const char*() noexcept
{
    return basic_error<std::string>::get(_Myval).c_str();
}

error_code::operator const wchar_t*() noexcept
{
    return basic_error<std::wstring>::get(_Myval).c_str();
}

_NAME_END