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
#ifdef _FORMAT_TYPE_

template<>
class basic_format<_FORMAT_TYPE_>
{
public:
    typedef _FORMAT_TYPE_ _Elem;
    typedef std::char_traits<_Elem> _Traits;

    typedef basic_format<_Elem, _Traits> _Myt;
    typedef std::basic_string<_Elem, _Traits> _Mystr;
    typedef std::basic_ostringstream<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>> _Myosstr;
    typedef std::basic_istringstream<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>> _Myisstr;

    basic_format(const _Mystr& _d) noexcept
        : _str(_d)
    {
    }

    template <typename T>
	_Myt& operator % (T in) noexcept
    {
        _Myosstr ss;
        ss << in;
        _chunks.push_back(ss.str());
        return *this;
    }

    // no explicit
    operator _Mystr() const noexcept
    {
        _Mystr res;

        std::size_t start = 0, last = 0;

        std::vector<_Mystr>::const_iterator chunkin = _chunks.begin();

        for (start = _str.find('%'); start != _Mystr::npos; start = _str.find('%', last))
        {
            res += _str.substr(last, start - last);

            last = start + 2;

            if (_str[start + 1] == '%')
            {
                res += '%';
                continue;
            }

            if (chunkin == _chunks.end())
                break;

            _Mystr str = *chunkin++;

            if (_str[start + 1] == 'p')
            {
                std::intptr_t ptr;
                _Myisstr is(str);
                is >> ptr;

                _Myosstr ss;

#ifdef _FORMAT_UNICODE_
                ss << L"0x" << std::hex << ptr;
#else
                ss << "0x" << std::hex << ptr;
#endif
                str = ss.str();
            }
            else if (_str[start + 1] == 'P')
            {
                std::intptr_t ptr;
                _Myisstr is(str);
                is >> ptr;

                _Myosstr ss;

#ifdef _FORMAT_UNICODE_
                ss << L"0x" << std::hex << std::uppercase << ptr;
#else
                ss << "0x" << std::hex << std::uppercase << ptr;
#endif
                str = ss.str();
            }

            res += str;
        }

        return (res += _str.substr(last));
    }

private:
    _Mystr _str;
    std::vector<_Mystr> _chunks;
};

#endif