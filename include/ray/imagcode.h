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
#ifndef _H_IMAGCODE_H_
#define _H_IMAGCODE_H_

#include <ray/imagdef.h>

_NAME_BEGIN

template<typename _Dummy>
struct image_code
{
    friend class image_error;

    static std::vector<_Dummy>_table;
    static const _Dummy& get(std::size_t n) noexcept
    {
        return _table[n];
    }
};

template<>
struct image_code<int>
{
    enum
    {
        UNREGISTERING_CUSTOM_IMPORTER,
        NO_SUITABLE_READER,
        UNKNOWN_BITDEPTH_IN_FILE,
        UNKNOWN_ENCODING_IN_FILE,
        FILE_EXTENSION_IS_ALREADY_IN_USE,
        ENCODING_DOESNT_MATCH_BITDEPTH,
        INVALID_IMAGE,
        IMAGE_WIDTH_TOO_LARGE,
        IMAGE_HEIGHT_TOO_LARGE,
        FILE_SIZE_TOO_LARGE,
    };
};

_NAME_END

#endif