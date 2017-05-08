// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_VARIANT_H_
#define _H_VARIANT_H_

#include <ray/math.h>

_NAME_BEGIN

class EXPORT Variant
{
public:
	enum Type
	{
		Void,
		Int,
		Float,
		Bool,
		Float4,
		String,
		Float4x4,
		Blob,
		Guid,
		Object,
		IntArray,
		FloatArray,
		BoolArray,
		Float4Array,
		StringArray,
		Float4x4Array,
	};

	Variant();
	Variant(int rhs);
	Variant(float rhs);
	Variant(bool rhs);
	Variant(const float4& v);
	Variant(const float4x4& m);
	Variant(const std::string& rhs);
	Variant(const char* chrPtr);
	Variant(const void* ptr);
	Variant(const std::vector<int>& rhs);
	Variant(const std::vector<float>& rhs);
	Variant(const std::vector<bool>& rhs);
	Variant(const std::vector<float4>& rhs);
	Variant(const std::vector<float4x4>& rhs);
	Variant(const std::vector<std::string>& rhs);
	Variant(const Variant& rhs);

	~Variant();

	void setType(Type t);
	Type getType() const;
	void clear();

	void operator=(const Variant& rhs);
	void operator=(int val);
	void operator=(float val);
	void operator=(bool val);
	void operator=(const float4& val);
	void operator=(const float4x4& val);
	void operator=(const std::string& s);
	void operator=(const char* chrPtr);
	void operator=(const void* ptr);
	void operator=(const std::vector<int>& rhs);
	void operator=(const std::vector<float>& rhs);
	void operator=(const std::vector<bool>& rhs);
	void operator=(const std::vector<float4>& rhs);
	void operator=(const std::vector<float4x4>& rhs);
	void operator=(const std::vector<std::string>& rhs);

	bool operator==(const Variant& rhs) const;
	bool operator==(int rhs) const;
	bool operator==(float rhs) const;
	bool operator==(bool rhs) const;
	bool operator==(const float4& rhs) const;
	bool operator==(const std::string& rhs) const;
	bool operator==(const char* chrPtr) const;
	bool operator==(const void* ptr) const;

	bool operator!=(const Variant& rhs) const;
	bool operator!=(int rhs) const;
	bool operator!=(float rhs) const;
	bool operator!=(bool rhs) const;
	bool operator!=(const float4& rhs) const;
	bool operator!=(const std::string& rhs) const;
	bool operator!=(const char* chrPtr) const;
	bool operator!=(const void* ptr) const;

	bool operator>(const Variant& rhs) const;
	bool operator<(const Variant& rhs) const;
	bool operator>=(const Variant& rhs) const;
	bool operator<=(const Variant& rhs) const;

	void assign(int val);
	void assign(float val);
	void assign(bool val);
	void assign(const void* ptr);
	void assign(const float4& val);
	void assign(const float4x4& val);
	void assign(const std::string& val);
	void assign(const std::vector<int>& val);
	void assign(const std::vector<float>& val);
	void assign(const std::vector<bool>& val);
	void assign(const std::vector<float4>& val);
	void assign(const std::vector<float4x4>& val);
	void assign(const std::vector<std::string>& val);

	void setInt(int val);
	void setFloat(float val);
	void setBool(bool val);
	void setFloat4(const float4& val);
	void setFloat4x4(const float4x4& val);
	void setObject(const void* ptr);
	void setString(const std::string& val);
	void setIntArray(const std::vector<int>& val);
	void setFloatArray(const std::vector<float>& val);
	void setBoolArray(const std::vector<bool>& val);
	void setFloat4Array(const std::vector<float4>& val);
	void setFloat4x4Array(const std::vector<float4x4>& val);
	void setStringArray(const std::vector<std::string>& val);

	int getInt() const;
	float getFloat() const;
	bool getBool() const;
	const std::string& getString() const;
	const float4& getFloat4() const;
	const float4x4& getFloat4x4() const;
	const void* getObject() const;
	const std::vector<int>& getIntArray() const;
	const std::vector<float>& getFloatArray() const;
	const std::vector<bool>& getBoolArray() const;
	const std::vector<float4>& getFloat4Array() const;
	const std::vector<float4x4>& getFloat4x4Array() const;
	const std::vector<std::string>& getStringArray() const;

	static std::string typeToString(Type t);
	static Type stringToType(const std::string& str);

private:
	void destroy();
	void copy(const Variant& rhs);

	Type _type;

	union
	{
		int i;
		bool b;
		float f[4];
		float4x4* matrix;
		const void* object;
		std::string* string;
		std::vector<int>* intArray;
		std::vector<float>* floatArray;
		std::vector<bool>* boolArray;
		std::vector<float4>* float4Array;
		std::vector<float4x4>* float4x4Array;
		std::vector<std::string>* stringArray;
	} _value;
};

_NAME_END

#endif