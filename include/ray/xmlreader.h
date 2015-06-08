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
#ifndef _H_XMLSTREAM_H_
#define _H_XMLSTREAM_H_

#include <ray/iostream.h>
#include <ray/math.h>

class TiXmlDocument;
class TiXmlElement;

_NAME_BEGIN

class EXPORT XMLReader final
{
public:
    XMLReader() noexcept;
    ~XMLReader() noexcept;

    bool open(istream& stream) noexcept;
    void close() noexcept;

    std::string getCurrentNodeName() const noexcept;
    std::string getCurrentNodePath() const noexcept;

    void setToNode(const std::string& path) noexcept;
    bool setToFirstChild(const std::string& name = "") noexcept;
    bool setToNextChild(const std::string& name = "") noexcept;
    bool setToParent() noexcept;

    bool hasAttr(const char* name) const noexcept;
    std::vector<std::string> getAttrs() const noexcept;

    std::string getString(const char* name) const noexcept;
    std::string getString(const std::string& name) const noexcept;

    std::string getText() const noexcept;

    int getInteger(const char* name) const noexcept;
    int getInteger(const std::string& name) const noexcept;

    bool getBoolean(const char* name) const noexcept;
    bool getBoolean(const std::string& name) const noexcept;

    float getFloat(const char* name) const noexcept;
    float getFloat(const std::string& name) const noexcept;

    float2 getFloat2(const char* name) const noexcept;
    float2 getFloat2(const std::string& name) const noexcept;

    float3 getFloat3(const char* name) const noexcept;
    float3 getFloat3(const std::string& name) const noexcept;

    float4 getFloat4(const char* name) const noexcept;
    float4 getFloat4(const std::string& name) const noexcept;

private:

    TiXmlElement* _currentNode;
    std::unique_ptr<TiXmlDocument> _document;
};

_NAME_END

#endif