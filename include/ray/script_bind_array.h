// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_SCRIPT_BIND_CORE_H_
#define _H_SCRIPT_BIND_CORE_H_

#include <ray/script_binder.h>

_NAME_BEGIN

struct ArrayBuffer
{
	asDWORD maxElements;
	asDWORD numElements;
	std::vector<asBYTE> data;
};

struct SArrayCache
{
	asIScriptFunction *cmpFunc;
	asIScriptFunction *eqFunc;
	int cmpFuncReturnCode;
	int eqFuncReturnCode;
};

class ScriptArray : public Reference
{
public:
	ScriptArray(asIObjectType* ot, void* initBuf); // Called from script when initialized with list
	ScriptArray(asIObjectType* ot, asUINT length, void* defVal);
	ScriptArray(const ScriptArray &other);
	virtual ~ScriptArray();

	// Type information
	asIObjectType *GetArrayObjectType() const;
	int            GetArrayTypeId() const;
	int            GetElementTypeId() const;

	asUINT count() const;
	asUINT size() const;

	bool empty() const;
	void clear();

	void reserve(asUINT numElements);
	void resize(asUINT numElements);

	void* at(asUINT index);
	const void* at(asUINT index) const;

	void assgin(const ScriptArray&);
	void assgin(asUINT index, void* value);

	void insert(asUINT index, void* value);
	void insert(asUINT begin, asUINT end, void* value);
	void insertLast(void *value);

	void remove(asUINT index);
	void remove(asUINT begin, asUINT end);
	void removeLast();

	void sortAsc();
	void sortDesc();
	void sortAsc(asUINT startAt, asUINT count);
	void sortDesc(asUINT startAt, asUINT count);
	void sort(asUINT startAt, asUINT count, bool asc);

	void reverse();

	int find(void *value) const;
	int find(asUINT startAt, void *value) const;
	int findByRef(void *ref) const;
	int findByRef(asUINT startAt, void *ref) const;

	const char* data() const;

	void swap(ScriptArray& other);

	// GC methods
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseAllHandles(asIScriptEngine *engine);

	bool operator==(const ScriptArray &) const;

	static ScriptArray* Create(asIObjectType* ot);
	static ScriptArray* Create(asIObjectType* ot, asUINT length);
	static ScriptArray* Create(asIObjectType* ot, asUINT length, void* defaultValue);
	static ScriptArray* Create(asIObjectType* ot, void* listBuffer);

private:

	void init(asIObjectType* type) noexcept;
	void initData(asIObjectType* type, asUINT length, void *defaultValue) noexcept;
	void initDataArray(asIObjectType* type, asUINT length, void* defaultValue) noexcept;

	bool  Less(const void *a, const void *b, bool asc, asIScriptContext *ctx, SArrayCache *cache);
	void *GetArrayItemPointer(int index);
	void *GetDataPointer(void *buffer);
	void  Precache();
	bool  CheckMaxSize(asUINT numElements);
	bool  CheckIndex(asUINT index);
	void  CreateBuffer(ArrayBuffer& buf, asUINT numElements);
	void  CopyBuffer(ArrayBuffer& dst, const ArrayBuffer& src);
	void  construct(ArrayBuffer& buf, asUINT start, asUINT end);
	void  destruct(ArrayBuffer& buf, asUINT start, asUINT end);
	bool  Equals(const void *a, const void *b, asIScriptContext *ctx, SArrayCache *cache) const;

private:

	asIObjectType* objType;
	int  _subTypeId;
	ArrayBuffer  buffer;
	bool _primitiveType;
	int  _elementSize;
};

class ScriptBindArray final : public ScriptBinder
{
public:
	void setup(asIScriptEngine* engine) noexcept;
};

_NAME_END

#endif