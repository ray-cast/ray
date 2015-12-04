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
#ifndef _H_STRING_H_
#define _H_STRING_H_

#include <ray/platform.h>

#ifdef _UNICODE
#   include <wchar.h>
#   include <wctype.h>
#endif

#include <string>
#if defined(__GNUWIN__)
#   include <strings.h>
#endif

_NAME_BEGIN

namespace util
{

#ifdef _UNICODE
typedef wchar_t char_type;
#else
typedef char char_type;
#endif

#if _UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif

// the operation specific directory separator
#ifndef __WINDOWS__
#   define SEPARATOR '/'
#else
#   define  SEPARATOR '\\'
#endif

// maximum path length
// XXX http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html

#ifdef PATH_MAX
#    define PATHLIMIT PATH_MAX
#else
#    define PATHLIMIT 4096
#endif

#ifndef MAX_PATH
#   define MAX_PATH 4096
#endif

#ifndef __TEXT
#	ifdef  __UNICODE__
#		define __TEXT(quote) L##quote
#	else
#		define __TEXT(quote) quote         
#	endif  
#endif 

#if defined(__WINDOWS__)
EXPORT char* fullpath(char * out, const char * in, std::size_t size);
EXPORT wchar_t* fullpath(wchar_t * out, const wchar_t * in, std::size_t size);
#endif

EXPORT int stricmp(const std::string& a, const std::string& b);
EXPORT int stricmp(const std::wstring& a, const std::wstring& b);
EXPORT int stricmp(const char *s1, const char *s2);
EXPORT int stricmp(const wchar_t *s1, const wchar_t *s2);
EXPORT int strnicmp(const char *s1, const char *s2, std::size_t n);
EXPORT int strnicmp(const wchar_t *s1, const wchar_t *s2, std::size_t n);

EXPORT char* strstr(char* dest, const char * source);
EXPORT char* strcpy(char* dest, const char* source);
EXPORT char* strncpy(char* s1, const char* s2, std::size_t cnt);
EXPORT wchar_t* strcpy(wchar_t* dest, const wchar_t* source);
EXPORT wchar_t* strncpy(wchar_t* s1, const wchar_t* s2, std::size_t cnt);
EXPORT wchar_t* strstr(wchar_t * dest, const wchar_t * source);
EXPORT std::size_t strlen(const char* str);
EXPORT std::size_t strlen(const wchar_t* str);
EXPORT int strncmp(const char* s1, const char* s2, std::size_t cnt);
EXPORT int strncmp(const wchar_t* s1, const wchar_t* s2, std::size_t cnt);

EXPORT std::size_t itoa10(char* out, std::size_t max, char number);
EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned char number);
EXPORT std::size_t itoa10(char* out, std::size_t max, short number);
EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned short number);
EXPORT std::size_t itoa10(char* out, std::size_t max, int number);
EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned int number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, int number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned int number);
EXPORT std::size_t itoa10(char* out, std::size_t max, long number);
EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned long number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, long number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long number);
EXPORT std::size_t itoa10(char* out, std::size_t max, long long number);
EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned long long number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, long long number);
EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long long number);

EXPORT std::size_t ftoa10(char* out, std::size_t max, float number);
EXPORT std::size_t dtoa10(char* out, std::size_t max, double number);

EXPORT float fast_atof(const char* c);
EXPORT float fast_atof(const char* c, const char** cout);
EXPORT float fast_atof(const char** inout);
EXPORT double fast_atod(const char* c);
EXPORT double fast_atod(const char* c, const char** cout);
EXPORT double fast_atod(const char** inout);

EXPORT std::size_t strtoul10(const char* in, const char** out);
EXPORT std::size_t strtoul8(const char* in, const char** out);
EXPORT std::size_t strtoul16(const char* in, const char** out);
EXPORT std::size_t HexDigitToDecimal(char in);
EXPORT std::uint8_t HexOctetToDecimal(const char* in);
EXPORT int strtol10(const char* in, const char** out);
EXPORT std::size_t strtoul_cppstyle(const char* in, const char** out);
EXPORT std::uint64_t strtoul10_64(const char* in, const char** out, unsigned int* max_inout);

EXPORT char tolower(char in);
EXPORT char toUpper(char in);
EXPORT bool isHex(char in);
EXPORT bool isUpper(char in);
EXPORT bool isLower(char in);
EXPORT bool isSpace(char in);
EXPORT bool isSeparator(char in);
EXPORT bool isNewLine(char in);
EXPORT bool isLineEnd(char in);
EXPORT bool isNumeric(char in);
EXPORT bool isSpaceOrNewLine(char in);
EXPORT bool isEndOfStream(char* it, char* end);

EXPORT wchar_t tolower(wchar_t in);
EXPORT wchar_t toUpper(wchar_t in);
EXPORT bool isHex(wchar_t in);
EXPORT bool isUpper(wchar_t in);
EXPORT bool isLower(wchar_t in);
EXPORT bool isSpace(wchar_t in);
EXPORT bool isSeparator(wchar_t in);
EXPORT bool isNewLine(wchar_t in);
EXPORT bool isLineEnd(wchar_t in);
EXPORT bool isNumeric(wchar_t in);
EXPORT bool isSpaceOrNewLine(wchar_t in);
EXPORT bool isEndOfStream(wchar_t* it, wchar_t* end);

EXPORT bool skipSpaces(const char* in, const char** out);
EXPORT bool skipSpaces(const char** inout);
EXPORT char* skipSpaces(char* it, char* end, std::size_t& num);
EXPORT char* skipLine(char* it, char* end, std::size_t& num);
EXPORT char* skipSeparator(char* in);
EXPORT bool skipSpacesAndLineEnd(const char* in, const char** out);
EXPORT bool skipSpacesAndLineEnd(const wchar_t* in, const wchar_t** out);
EXPORT bool skipSpacesAndLineEnd(const char** inout);
EXPORT bool skipSpacesAndLineEnd(const wchar_t** inout);

EXPORT void skipToken(const char*& in);
EXPORT bool getNextLine(const char*& buffer, char out[4096]);
EXPORT bool tokenMatch(char*& in, const char* token, std::size_t len);
EXPORT bool tokenMatchI(const char*& in, const char* token, std::size_t len);
EXPORT std::string getNextToken(const char*& in);
EXPORT std::wstring getNextToken(const wchar_t*& in);

EXPORT int integer_pow(int base, int power);
EXPORT std::string directory(const std::string& path);
EXPORT std::wstring directory(const std::wstring& path);

EXPORT std::string toLower(const std::string& _input);
EXPORT std::wstring toLower(const std::wstring& _input);

EXPORT void split(std::vector<std::string>& result, const std::string& _source, const std::string& _delims = "\t\n ");
EXPORT void split(std::vector<std::wstring>& result, const std::wstring& _source, const std::wstring& _delims = L"\t\n ");

}

_NAME_END

#endif
