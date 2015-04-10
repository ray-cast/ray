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
#include <ray/script_bind_string.h>
#include <ray/script_system.h>

#include <sstream>
#include <iostream>
#include <angelscript.h>

_NAME_BEGIN

#undef new

std::string StringFactory(std::size_t length, const char *s)
{
    return std::string(s, length);
}

void ConstructString(std::string *thisPointer)
{
    new(thisPointer) std::string();
}

void CopyConstructString(const std::string &other, std::string *thisPointer)
{
    new(thisPointer) std::string(other);
}

void DestructString(std::string *thisPointer)
{
    thisPointer->~basic_string();
}

static std::string &AddAssignStringToString(const std::string &str, std::string &dest)
{
    dest += str;
    return dest;
}

static bool StringIsEmpty(const std::string &str)
{
    return str.empty();
}

static std::string &AssignUInt64ToString(std::uint32_t i, std::string &dest)
{
    std::ostringstream stream;
    stream << i;
    dest = stream.str();
    return dest;
}

static std::string &AddAssignUInt64ToString(std::uint32_t i, std::string &dest)
{
    std::ostringstream stream;
    stream << i;
    dest += stream.str();
    return dest;
}

static std::string AddStringUInt64(const std::string &str, std::uint32_t i)
{
    std::ostringstream stream;
    stream << i;
    return str + stream.str();
}

static std::string AddInt64String(std::int64_t i, const std::string &str)
{
    std::ostringstream stream;
    stream << i;
    return stream.str() + str;
}

static std::string& AssignInt64ToString(std::int64_t i, std::string &dest)
{
    std::ostringstream stream;
    stream << i;
    dest = stream.str();
    return dest;
}

static std::string& AddAssignInt64ToString(std::int64_t i, std::string &dest)
{
    std::ostringstream stream;
    stream << i;
    dest += stream.str();
    return dest;
}

static std::string AddStringInt64(const std::string &str, std::int64_t i)
{
    std::ostringstream stream;
    stream << i;
    return str + stream.str();
}

static std::string AddUInt64String(std::uint32_t i, const std::string &str)
{
    std::ostringstream stream;
    stream << i;
    return stream.str() + str;
}

static std::string &AssignDoubleToString(double f, std::string& dest)
{
    std::ostringstream stream;
    stream << f;
    dest = stream.str();
    return dest;
}

static std::string &AddAssignDoubleToString(double f, std::string& dest)
{
    std::ostringstream stream;
    stream << f;
    dest += stream.str();
    return dest;
}

static std::string& AssignFloatToString(float f, std::string& dest)
{
    std::ostringstream stream;
    stream << f;
    dest = stream.str();
    return dest;
}

static std::string &AddAssignFloatToString(float f, std::string& dest)
{
    std::ostringstream stream;
    stream << f;
    dest += stream.str();
    return dest;
}

static std::string &AssignBoolToString(bool b, std::string& dest)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    dest = stream.str();
    return dest;
}

static std::string &AddAssignBoolToString(bool b, std::string& dest)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    dest += stream.str();
    return dest;
}

static std::string AddStringString(const std::string& str, const std::string& dest)
{
    return str + dest;
}

static std::string AddStringDouble(const std::string&str, double f)
{
    std::ostringstream stream;
    stream << f;
    return str + stream.str();
}

static std::string AddDoubleString(double f, const std::string &str)
{
    std::ostringstream stream;
    stream << f;
    return stream.str() + str;
}

static std::string AddStringFloat(const std::string &str, float f)
{
    std::ostringstream stream;
    stream << f;
    return str + stream.str();
}

static std::string AddFloatString(float f, const std::string &str)
{
    std::ostringstream stream;
    stream << f;
    return stream.str() + str;
}

static std::string AddStringBool(const std::string &str, bool b)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    return str + stream.str();
}

static std::string AddBoolString(bool b, const std::string &str)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    return stream.str() + str;
}

static char *StringCharAt(unsigned int i, std::string& str)
{
    return &str[i];
}

static int StringCmp(const std::string &a, const std::string &b)
{
    int cmp = 0;
    if (a < b) cmp = -1;
    else if (a > b) cmp = 1;
    return cmp;
}

static int StringFindFirst(const std::string &sub, std::size_t start, const std::string &str)
{
    return (int)str.find(sub, start);
}

static int StringFindLast(const std::string &sub, int start, const std::string &str)
{
    return (int)str.rfind(sub, (size_t)start);
}

static std::size_t StringLength(const std::string &str)
{
    return (std::size_t)str.length();
}

static void StringResize(std::size_t l, std::string &str)
{
    str.resize(l);
}

static std::string formatInt(std::int64_t value, const std::string &options, std::size_t width)
{
    bool leftJustify = options.find("l") != std::string::npos;
    bool padWithZero = options.find("0") != std::string::npos;
    bool alwaysSign = options.find("+") != std::string::npos;
    bool spaceOnSign = options.find(" ") != std::string::npos;
    bool hexSmall = options.find("h") != std::string::npos;
    bool hexLarge = options.find("H") != std::string::npos;

    std::string fmt = "%";
    if (leftJustify) fmt += "-";
    if (alwaysSign) fmt += "+";
    if (spaceOnSign) fmt += " ";
    if (padWithZero) fmt += "0";

#ifdef _WIN32
    fmt += "*I64";
#else
#ifdef _LP64
    fmt += "*l";
#else
    fmt += "*ll";
#endif
#endif

    if (hexSmall) fmt += "x";
    else if (hexLarge) fmt += "X";
    else fmt += "d";

    std::string buf;
    buf.resize(width + 30);
#if _MSC_VER >= 1400 && !defined(__S3E__)
    // MSVC 8.0 / 2005 or newer
    sprintf_s(&buf[0], buf.size(), fmt.c_str(), width, value);
#else
    sprintf(&buf[0], fmt.c_str(), width, value);
#endif
    buf.resize(strlen(&buf[0]));

    return buf;
}

static std::string formatFloat(double value, const std::string &options, std::size_t width, std::size_t precision)
{
    bool leftJustify = options.find("l") != std::string::npos;
    bool padWithZero = options.find("0") != std::string::npos;
    bool alwaysSign = options.find("+") != std::string::npos;
    bool spaceOnSign = options.find(" ") != std::string::npos;
    bool expSmall = options.find("e") != std::string::npos;
    bool expLarge = options.find("E") != std::string::npos;

    std::string fmt = "%";
    if (leftJustify) fmt += "-";
    if (alwaysSign) fmt += "+";
    if (spaceOnSign) fmt += " ";
    if (padWithZero) fmt += "0";

    fmt += "*.*";

    if (expSmall) fmt += "e";
    else if (expLarge) fmt += "E";
    else fmt += "f";

    std::string buf;
    buf.resize(width + precision + 50);
#if _MSC_VER >= 1400 && !defined(__S3E__)
    // MSVC 8.0 / 2005 or newer
    sprintf_s(&buf[0], buf.size(), fmt.c_str(), width, precision, value);
#else
    sprintf(&buf[0], fmt.c_str(), width, precision, value);
#endif
    buf.resize(strlen(&buf[0]));

    return buf;
}

static std::int64_t parseInt(const std::string &val, std::size_t base, std::size_t *byteCount)
{
    // Only accept base 10 and 16
    if (base != 10 && base != 16)
    {
        if (byteCount) *byteCount = 0;
        return 0;
    }

    const char *end = &val[0];

    // Determine the sign
    bool sign = false;
    if (*end == '-')
    {
        sign = true;
        end++;
    }
    else if (*end == '+')
        end++;

    std::int64_t res = 0;
    if (base == 10)
    {
        while (*end >= '0' && *end <= '9')
        {
            res *= 10;
            res += *end++ - '0';
        }
    }
    else if (base == 16)
    {
        while ((*end >= '0' && *end <= '9') ||
            (*end >= 'a' && *end <= 'f') ||
            (*end >= 'A' && *end <= 'F'))
        {
            res *= 16;
            if (*end >= '0' && *end <= '9')
                res += *end++ - '0';
            else if (*end >= 'a' && *end <= 'f')
                res += *end++ - 'a' + 10;
            else if (*end >= 'A' && *end <= 'F')
                res += *end++ - 'A' + 10;
        }
    }

    if (byteCount)
        *byteCount = std::size_t(size_t(end - val.c_str()));

    if (sign)
        res = -res;

    return res;
}

double parseFloat(const std::string &val, std::size_t *byteCount)
{
    char *end;

    // WinCE doesn't have setlocale. Some quick testing on my current platform
    // still manages to parse the numbers such as "3.14" even if the decimal for the
    // locale is ",".
#if !defined(_WIN32_WCE) && !defined(ANDROID) && !defined(__psp2__)
    // Set the locale to C so that we are guaranteed to parse the float value correctly
    char *orig = setlocale(LC_NUMERIC, 0);
    setlocale(LC_NUMERIC, "C");
#endif

    double res = strtod(val.c_str(), &end);

#if !defined(_WIN32_WCE) && !defined(ANDROID) && !defined(__psp2__)
    // Restore the locale
    setlocale(LC_NUMERIC, orig);
#endif

    if (byteCount)
        *byteCount = std::size_t(size_t(end - val.c_str()));

    return res;
}

static std::string StringSubString(std::size_t start, int count, const std::string &str)
{
    // Check for out-of-bounds
    std::string ret;
    if (start < str.length() && count != 0)
        ret = str.substr(start, count);

    return ret;
}

static bool StringEquals(const std::string& lhs, const std::string& rhs)
{
    return lhs == rhs;
}

template<typename T>
void PrintString(const T& string) noexcept
{
    std::cout << string;
}

void
ScriptBindString::setup(asIScriptEngine* engine) noexcept
{
    int r;

    r = engine->RegisterObjectType("string", sizeof(std::string), asOBJ_VALUE | asGetTypeTraits<std::string>()); assert(r >= 0);
    r = engine->RegisterStringFactory("string", asFUNCTION(StringFactory), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(CopyConstructString), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructString), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "string& opAssign(const string &in)", asMETHODPR(std::string, operator =, (const std::string&), std::string&), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAssign(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAssign(float)", asFUNCTION(AssignFloatToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAssign(int64)", asFUNCTION(AssignInt64ToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAssign(uint64)", asFUNCTION(AssignUInt64ToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAssign(bool)", asFUNCTION(AssignBoolToString), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "string& opAddAssign(const string &in)", asFUNCTION(AddAssignStringToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAddAssign(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAddAssign(float)", asFUNCTION(AddAssignFloatToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAddAssign(int64)", asFUNCTION(AddAssignInt64ToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAddAssign(uint64)", asFUNCTION(AddAssignUInt64ToString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string& opAddAssign(bool)", asFUNCTION(AddAssignBoolToString), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "string opAdd(const string& in) const", asFUNCTION(AddStringString), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd(double) const", asFUNCTION(AddStringDouble), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd(float) const", asFUNCTION(AddStringFloat), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd(int64) const", asFUNCTION(AddStringInt64), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd(uint64) const", asFUNCTION(AddStringUInt64), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd(bool) const", asFUNCTION(AddStringBool), asCALL_CDECL_OBJFIRST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "string opAdd_r(double) const", asFUNCTION(AddDoubleString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd_r(float) const", asFUNCTION(AddFloatString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd_r(int64) const", asFUNCTION(AddInt64String), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd_r(uint64) const", asFUNCTION(AddUInt64String), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string opAdd_r(bool) const", asFUNCTION(AddBoolString), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTIONPR(StringEquals, (const std::string&, const std::string &), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmp), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "uint8& opIndex(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("string", "uint length() const", asFUNCTION(StringLength), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "void resize(uint)", asFUNCTION(StringResize), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "uint get_length() const", asFUNCTION(StringLength), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "void set_length(uint)", asFUNCTION(StringResize), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "bool isEmpty() const", asFUNCTION(StringIsEmpty), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "string substr(uint start = 0, int count = -1) const", asFUNCTION(StringSubString), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "uint size() const", asFUNCTION(StringLength), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "bool empty() const", asFUNCTION(StringIsEmpty), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "int find(const string &in, uint start = 0) const", asFUNCTION(StringFindFirst), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "int rfind(const string &in, int start = -1) const", asFUNCTION(StringFindLast), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "int findFirst(const string &in, uint start = 0) const", asFUNCTION(StringFindFirst), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("string", "int findLast(const string &in, int start = -1) const", asFUNCTION(StringFindLast), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string formatInt(int64 val, const string &in options, uint width = 0)", asFUNCTION(formatInt), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string formatFloat(double val, const string &in options, uint width = 0, uint precision = 0)", asFUNCTION(formatFloat), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("int64 parseInt(const string &in, uint base = 10, uint &out byteCount = 0)", asFUNCTION(parseInt), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double parseFloat(const string &in, uint &out byteCount = 0)", asFUNCTION(parseFloat), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("void print(const int& in)", asFUNCTION(PrintString<int>), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void print(const float& in)", asFUNCTION(PrintString<float>), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void print(const double& in)", asFUNCTION(PrintString<double>), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(PrintString<std::string>), asCALL_CDECL); assert(r >= 0);
}

_NAME_END