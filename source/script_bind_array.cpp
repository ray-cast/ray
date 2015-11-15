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
#if defined(_BUILD_SCRIPT)
#include <ray/script_bind_array.h>
#include <ray/script_array.h>

_NAME_BEGIN

// This macro is used to avoid warnings about unused variables.
// Usually where the variables are only used in debug mode.
#define UNUSED_VAR(x) (void)(x)

// Set the default memory routines
// Use the angelscript engine's memory routines by default
static asALLOCFUNC_t userAlloc = asAllocMem;
static asFREEFUNC_t  userFree = asFreeMem;

// We just define a number here that we assume nobody else is using for
// object type user data. The add-ons have reserved the numbers 1000
// through 1999 for this purpose, so we should be fine.
const asPWORD ARRAY_CACHE = 1000;

static void CleanupObjectTypeArrayCache(asIObjectType* type)
{
	SArrayCache *cache = reinterpret_cast<SArrayCache*>(type->GetUserData(ARRAY_CACHE));
	if (cache)
	{
		cache->~SArrayCache();
		userFree(cache);
	}
}

// This optional callback is called when the template type is first used by the compiler.
// It allows the application to validate if the template can be instantiated for the requested
// subtype at compile time, instead of at runtime. The output argument dontGarbageCollect
// allow the callback to tell the engine if the template instance type shouldn't be garbage collected,
// i.e. no asOBJ_GC flag.
static bool ScriptArrayTemplateCallback(asIObjectType *ot, bool &dontGarbageCollect)
{
	// Make sure the subtype can be instantiated with a default factory/constructor,
	// otherwise we won't be able to instantiate the elements.
	int typeId = ot->GetSubTypeId();
	if (typeId == asTYPEID_VOID)
		return false;
	if ((typeId & asTYPEID_MASK_OBJECT) && !(typeId & asTYPEID_OBJHANDLE))
	{
		asIObjectType *subtype = ot->GetEngine()->GetObjectTypeById(typeId);
		asDWORD flags = subtype->GetFlags();
		if ((flags & asOBJ_VALUE) && !(flags & asOBJ_POD))
		{
			// Verify that there is a default constructor
			bool found = false;
			for (asUINT n = 0; n < subtype->GetBehaviourCount(); n++)
			{
				asEBehaviours beh;
				asIScriptFunction *func = subtype->GetBehaviourByIndex(n, &beh);
				if (beh != asBEHAVE_CONSTRUCT) continue;

				if (func->GetParamCount() == 0)
				{
					// Found the default constructor
					found = true;
					break;
				}
			}

			if (!found)
			{
				// There is no default constructor
				ot->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
				return false;
			}
		}
		else if ((flags & asOBJ_REF))
		{
			bool found = false;

			// If value assignment for ref type has been disabled then the array
			// can be created if the type has a default factory function
			if (!ot->GetEngine()->GetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE))
			{
				// Verify that there is a default factory
				for (asUINT n = 0; n < subtype->GetFactoryCount(); n++)
				{
					asIScriptFunction *func = subtype->GetFactoryByIndex(n);
					if (func->GetParamCount() == 0)
					{
						// Found the default factory
						found = true;
						break;
					}
				}
			}

			if (!found)
			{
				// No default factory
				ot->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
				return false;
			}
		}

		// If the object type is not garbage collected then the array also doesn't need to be
		if (!(flags & asOBJ_GC))
			dontGarbageCollect = true;
	}
	else if (!(typeId & asTYPEID_OBJHANDLE))
	{
		// Arrays with primitives cannot form circular references,
		// thus there is no need to garbage collect them
		dontGarbageCollect = true;
	}
	else
	{
		assert(typeId & asTYPEID_OBJHANDLE);

		// It is not necessary to set the array as garbage collected for all handle types.
		// If it is possible to determine that the handle cannot refer to an object type
		// that can potentially form a circular reference with the array then it is not
		// necessary to make the array garbage collected.
		asIObjectType *subtype = ot->GetEngine()->GetObjectTypeById(typeId);
		asDWORD flags = subtype->GetFlags();
		if (!(flags & asOBJ_GC))
		{
			if ((flags & asOBJ_SCRIPT_OBJECT))
			{
				// Even if a script class is by itself not garbage collected, it is possible
				// that classes that derive from it may be, so it is not possible to know
				// that no circular reference can occur.
				if ((flags & asOBJ_NOINHERIT))
				{
					// A script class declared as final cannot be inherited from, thus
					// we can be certain that the object cannot be garbage collected.
					dontGarbageCollect = true;
				}
			}
			else
			{
				// For application registered classes we assume the application knows
				// what it is doing and don't mark the array as garbage collected unless
				// the type is also garbage collected.
				dontGarbageCollect = true;
			}
		}
	}

	// The type is ok
	return true;
}

ScriptArray::ScriptArray(asIObjectType *ot, void* buf)
{
	this->init(ot);
	this->initDataArray(ot, *(asUINT*)buf, (((asUINT*)buf) + 1));
}

ScriptArray::ScriptArray(asIObjectType* ot, asUINT length, void* defVal)
{
	this->init(ot);
	this->initData(ot, length, defVal);
}

ScriptArray::ScriptArray(const ScriptArray& other)
{
	this->init(other.objType);
	this->assgin(other);
}

ScriptArray::~ScriptArray()
{
	this->clear();

	if (objType)
		objType->Release();
}

void
ScriptArray::init(asIObjectType* type) noexcept
{
	objType = type;
	objType->AddRef();

	buffer.numElements = 0;
	buffer.maxElements = 0;

	Precache();

	_primitiveType = _subTypeId & asTYPEID_MASK_OBJECT ? false : true;
	if (!_primitiveType)
	{
		bool destroy = false;

		asIObjectType *subtype = type->GetEngine()->GetObjectTypeById(_subTypeId);
		if (subtype)
		{
			_elementSize = subtype->GetSize();
			if (_elementSize > 0)
			{
				std::size_t count = subtype->GetBehaviourCount();
				for (std::size_t i = 0; i < count; i++)
				{
					asEBehaviours beh;
					subtype->GetBehaviourByIndex(i, &beh);

					if (beh == asBEHAVE_DESTRUCT ||
						beh == asBEHAVE_ADDREF ||
						beh == asBEHAVE_RELEASE)
					{
						destroy = true;
						break;
					}
				}

				_primitiveType = destroy ? false : true;
			}
		}

		if (!_primitiveType)
			_elementSize = sizeof(asPWORD);
	}
	else
	{
		_elementSize = objType->GetEngine()->GetSizeOfPrimitiveType(_subTypeId);
	}
}

void 
ScriptArray::initData(asIObjectType* type, asUINT length, void* defaultValue) noexcept
{
	assert(CheckMaxSize(length));

	CreateBuffer(buffer, length);

	if (defaultValue)
	{
		for (asUINT n = 0; n < this->count(); n++)
			this->assgin(n, defaultValue);
	}

	if (objType->GetFlags() & asOBJ_GC)
		objType->GetEngine()->NotifyGarbageCollectorOfNewObject(this, objType);
}

void
ScriptArray::initDataArray(asIObjectType* ot, asUINT length, void* buf) noexcept
{
	assert(CheckMaxSize(length));

	asIScriptEngine* engine = ot->GetEngine();

	if ((ot->GetSubTypeId() & asTYPEID_MASK_OBJECT) == 0)
	{
		CreateBuffer(buffer, length);
		if (length > 0)
			memcpy(at(0), ((asUINT*)buf), length * _elementSize);
	}
	else if (ot->GetSubTypeId() & asTYPEID_OBJHANDLE)
	{
		CreateBuffer(buffer, length);

		if (length > 0)
			memcpy(at(0), ((asUINT*)buf), length * _elementSize);

		memset(((asUINT*)buf), 0, length * _elementSize);
	}
	else if (ot->GetSubType()->GetFlags() & asOBJ_REF)
	{
		_subTypeId |= asTYPEID_OBJHANDLE;
		CreateBuffer(buffer, length);
		_subTypeId &= ~asTYPEID_OBJHANDLE;

		if (length > 0)
			memcpy(buffer.data.data(), ((asUINT*)buf), length * _elementSize);

		memset(((asUINT*)buf), 0, length * _elementSize);
	}
	else
	{
		CreateBuffer(buffer, length);

		for (asUINT n = 0; n < length; n++)
		{
			void *obj = at(n);
			asBYTE *srcObj = (asBYTE*)buf;
			srcObj += 4 + n*ot->GetSubType()->GetSize();
			engine->AssignScriptObject(obj, srcObj, ot->GetSubType());
		}
	}

	if (objType->GetFlags() & asOBJ_GC)
		objType->GetEngine()->NotifyGarbageCollectorOfNewObject(this, objType);
}

void
ScriptArray::assgin(const ScriptArray& other)
{
	assert(&other != this);
	assert(other.GetArrayObjectType() == GetArrayObjectType());
	this->resize(other.buffer.numElements);
	this->CopyBuffer(buffer, other.buffer);
}

void 
ScriptArray::assgin(asUINT index, void* value)
{
	void* ptr = this->at(index);

	if ((_subTypeId & ~asTYPEID_MASK_SEQNBR) && !(_subTypeId & asTYPEID_OBJHANDLE))
	{
		objType->GetEngine()->AssignScriptObject(ptr, value, objType->GetSubType());
	}
	else if (_subTypeId & asTYPEID_OBJHANDLE)
	{
		void* tmp = *(void**)ptr;
		*(void**)ptr = *(void**)value;
		objType->GetEngine()->AddRefScriptObject(*(void**)value, objType->GetSubType());
		if (tmp) objType->GetEngine()->ReleaseScriptObject(tmp, objType->GetSubType());
	}
	else if (_subTypeId == asTYPEID_BOOL || _subTypeId == asTYPEID_INT8 || _subTypeId == asTYPEID_UINT8)
		*(char*)ptr = *(char*)value;
	else if (_subTypeId == asTYPEID_INT16 || _subTypeId == asTYPEID_UINT16)
		*(short*)ptr = *(short*)value;
	else if (_subTypeId == asTYPEID_INT32 || _subTypeId == asTYPEID_UINT32 || _subTypeId == asTYPEID_FLOAT || _subTypeId > asTYPEID_DOUBLE)
		*(int*)ptr = *(int*)value;
	else if (_subTypeId == asTYPEID_INT64 || _subTypeId == asTYPEID_UINT64 || _subTypeId == asTYPEID_DOUBLE)
		*(double*)ptr = *(double*)value;
	else
	{
		assert(false);
	}
}

const char*
ScriptArray::data() const
{
	return (char*)buffer.data.data();
}

void
ScriptArray::swap(ScriptArray& other)
{
	assert(&other != this);
	assert(other.GetArrayObjectType() == GetArrayObjectType());
	std::swap(buffer.numElements, other.buffer.numElements);
	std::swap(buffer.maxElements, other.buffer.maxElements);
	buffer.data.swap(other.buffer.data);
}

asUINT 
ScriptArray::count() const
{
	return buffer.numElements;
}

asUINT 
ScriptArray::size() const
{
	return _elementSize;
}

bool 
ScriptArray::empty() const
{
	return buffer.numElements == 0;
}

void 
ScriptArray::reserve(asUINT numElements)
{
	assert(CheckMaxSize(numElements));

	if (numElements > buffer.maxElements)
	{
		this->destruct(buffer, 0, buffer.numElements);

		buffer.data.resize(_elementSize * numElements);
		buffer.numElements = numElements;
		buffer.maxElements = numElements;
	}
}

void 
ScriptArray::resize(asUINT numElements)
{
	assert(CheckMaxSize(numElements));

	if (numElements > buffer.maxElements)
	{
		ArrayBuffer newBuffer;
		newBuffer.data.resize(_elementSize * numElements);
		newBuffer.numElements = numElements;
		newBuffer.maxElements = numElements;

		if (buffer.numElements > 0)
			std::memcpy(newBuffer.data.data(), buffer.data.data(), numElements * _elementSize);

		this->construct(newBuffer, buffer.numElements, numElements);

		buffer = newBuffer;
	}
}

void 
ScriptArray::insert(asUINT begin, asUINT end, void* value)
{
	assert(begin != end);
	assert(begin <= buffer.numElements);
	assert(CheckMaxSize(end));

	auto min = begin < end ? begin : end;
	auto max = begin < end ? end : begin;
	auto delta = max - min;

	if (buffer.maxElements < buffer.numElements + delta)
	{
		asUINT maxElements = buffer.maxElements + (buffer.maxElements >> 1) + delta;

		ArrayBuffer newBuffer;
		newBuffer.data.resize(_elementSize * maxElements);
		newBuffer.maxElements = maxElements;
		newBuffer.numElements = buffer.numElements + delta;		

		std::memcpy(newBuffer.data.data(), buffer.data.data(), begin * _elementSize);
		if (begin < buffer.numElements)
			std::memcpy(newBuffer.data.data() + (begin + delta)*_elementSize, buffer.data.data() + begin * _elementSize, (buffer.numElements - begin)*_elementSize);

		this->construct(newBuffer, begin, begin + delta);

		buffer = newBuffer;
	}
	else if (delta < 0)
	{
		destruct(buffer, begin, begin - delta);
		std::memmove(buffer.data.data() + begin * _elementSize, buffer.data.data() + (begin - delta)*_elementSize, (buffer.numElements - (begin - delta))*_elementSize);
		buffer.numElements += delta;
	}
	else
	{
		buffer.numElements += delta;
	}

	for (int i = 0; i < delta; i++)
	{
		this->assgin(begin, value);
	}
}

void 
ScriptArray::insert(asUINT index, void* value)
{
	assert(CheckIndex(index));
	this->insert(index, index + 1, value);
}

void 
ScriptArray::insertLast(void* value)
{
	this->insert(buffer.numElements, buffer.numElements + 1, value);
}

void 
ScriptArray::remove(asUINT index)
{
	remove(index, index + 1);
}

void 
ScriptArray::remove(asUINT begin, asUINT end)
{
	assert(begin != end);
	assert(begin < buffer.numElements);
	assert(end <= buffer.numElements);

	auto min = begin < end ? begin : end;
	auto max = begin < end ? end : begin;
	auto delta = max - min;

	if (delta > 0)
	{
		destruct(buffer, begin, begin - delta);
		std::memmove(buffer.data.data() + begin * _elementSize, buffer.data.data() + (begin - delta)*_elementSize, (buffer.numElements - (begin - delta))*_elementSize);
		buffer.numElements -= delta;
	}
}

void
ScriptArray::removeLast()
{
	remove(buffer.numElements - 1);
}

const void* 
ScriptArray::at(asUINT index) const
{
	if (index >= buffer.numElements)
	{
		asIScriptContext *ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Index out of bounds");
		return nullptr;
	}

	if (!_primitiveType && !(_subTypeId & asTYPEID_OBJHANDLE))
		return *(void**)(buffer.data.data() + _elementSize*index);
	else
		return buffer.data.data() + _elementSize*index;
}

void* 
ScriptArray::at(asUINT index)
{
	return const_cast<void*>(const_cast<const ScriptArray*>(this)->at(index));
}

void
ScriptArray::clear()
{
	destruct(buffer, 0, buffer.numElements);
	buffer.data.clear();
}

bool 
ScriptArray::Less(const void *a, const void *b, bool asc, asIScriptContext *ctx, SArrayCache *cache)
{
	if (!asc)
	{
		// Swap items
		const void *TEMP = a;
		a = b;
		b = TEMP;
	}

	if (!(_subTypeId & ~asTYPEID_MASK_SEQNBR))
	{
		// Simple compare of values
		switch (_subTypeId)
		{
#define COMPARE(T) *((T*)a) < *((T*)b)
		case asTYPEID_BOOL: return COMPARE(bool);
		case asTYPEID_INT8: return COMPARE(signed char);
		case asTYPEID_UINT8: return COMPARE(unsigned char);
		case asTYPEID_INT16: return COMPARE(signed short);
		case asTYPEID_UINT16: return COMPARE(unsigned short);
		case asTYPEID_INT32: return COMPARE(signed int);
		case asTYPEID_UINT32: return COMPARE(unsigned int);
		case asTYPEID_FLOAT: return COMPARE(float);
		case asTYPEID_DOUBLE: return COMPARE(double);
		default: return COMPARE(signed int); // All enums fall in this case
#undef COMPARE
		}
	}
	else
	{
		int r = 0;

		if (_subTypeId & asTYPEID_OBJHANDLE)
		{
			// Allow sort to work even if the array contains null handles
			if (*(void**)a == 0) return true;
			if (*(void**)b == 0) return false;
		}

		// Execute object opCmp
		if (cache && cache->cmpFunc)
		{
			// TODO: Add proper error handling
			r = ctx->Prepare(cache->cmpFunc); assert(r >= 0);

			if (_subTypeId & asTYPEID_OBJHANDLE)
			{
				r = ctx->SetObject(*((void**)a)); assert(r >= 0);
				r = ctx->SetArgObject(0, *((void**)b)); assert(r >= 0);
			}
			else
			{
				r = ctx->SetObject((void*)a); assert(r >= 0);
				r = ctx->SetArgObject(0, (void*)b); assert(r >= 0);
			}

			r = ctx->Execute();

			if (r == asEXECUTION_FINISHED)
			{
				return (int)ctx->GetReturnDWord() < 0;
			}
		}
	}

	return false;
}

void
ScriptArray::reverse()
{
	asUINT size = this->count();

	if (size >= 2)
	{
		asBYTE TEMP[16];

		for (asUINT i = 0; i < size / 2; i++)
		{
			memcpy(TEMP, GetArrayItemPointer(i), _elementSize);
			memcpy(GetArrayItemPointer(i), GetArrayItemPointer(size - i - 1), _elementSize);
			memcpy(GetArrayItemPointer(size - i - 1), TEMP, _elementSize);
		}
	}
}

bool 
ScriptArray::operator==(const ScriptArray &other) const
{
	if (objType != other.objType)
		return false;

	if (this->count() != other.count())
		return false;

	asIScriptContext *cmpContext = 0;
	bool isNested = false;

	if (_subTypeId & ~asTYPEID_MASK_SEQNBR)
	{
		// Try to reuse the active context
		cmpContext = asGetActiveContext();
		if (cmpContext)
		{
			if (cmpContext->GetEngine() == objType->GetEngine() && cmpContext->PushState() >= 0)
				isNested = true;
			else
				cmpContext = 0;
		}
		if (cmpContext == 0)
		{
			// TODO: Ideally this context would be retrieved from a pool, so we don't have to
			//       create a new one everytime. We could keep a context with the array object
			//       but that would consume a lot of resources as each context is quite heavy.
			cmpContext = objType->GetEngine()->CreateContext();
		}
	}

	// Check if all elements are equal
	bool isEqual = true;
	SArrayCache *cache = reinterpret_cast<SArrayCache*>(objType->GetUserData(ARRAY_CACHE));
	for (asUINT n = 0; n < this->count(); n++)
		if (!Equals(at(n), other.at(n), cmpContext, cache))
		{
			isEqual = false;
			break;
		}

	if (cmpContext)
	{
		if (isNested)
		{
			asEContextState state = cmpContext->GetState();
			cmpContext->PopState();
			if (state == asEXECUTION_ABORTED)
				cmpContext->Abort();
		}
		else
			cmpContext->Release();
	}

	return isEqual;
}

bool 
ScriptArray::Equals(const void *a, const void *b, asIScriptContext *ctx, SArrayCache *cache) const
{
	if (!(_subTypeId & ~asTYPEID_MASK_SEQNBR))
	{
		// Simple compare of values
		switch (_subTypeId)
		{
#define COMPARE(T) *((T*)a) == *((T*)b)
		case asTYPEID_BOOL: return COMPARE(bool);
		case asTYPEID_INT8: return COMPARE(signed char);
		case asTYPEID_UINT8: return COMPARE(unsigned char);
		case asTYPEID_INT16: return COMPARE(signed short);
		case asTYPEID_UINT16: return COMPARE(unsigned short);
		case asTYPEID_INT32: return COMPARE(signed int);
		case asTYPEID_UINT32: return COMPARE(unsigned int);
		case asTYPEID_FLOAT: return COMPARE(float);
		case asTYPEID_DOUBLE: return COMPARE(double);
		default: return COMPARE(signed int); // All enums fall here
#undef COMPARE
		}
	}
	else
	{
		int r = 0;

		if (_subTypeId & asTYPEID_OBJHANDLE)
		{
			// Allow the find to work even if the array contains null handles
			if (*(void**)a == *(void**)b) return true;
		}

		// Execute object opEquals if available
		if (cache && cache->eqFunc)
		{
			// TODO: Add proper error handling
			r = ctx->Prepare(cache->eqFunc); assert(r >= 0);

			if (_subTypeId & asTYPEID_OBJHANDLE)
			{
				r = ctx->SetObject(*((void**)a)); assert(r >= 0);
				r = ctx->SetArgObject(0, *((void**)b)); assert(r >= 0);
			}
			else
			{
				r = ctx->SetObject((void*)a); assert(r >= 0);
				r = ctx->SetArgObject(0, (void*)b); assert(r >= 0);
			}

			r = ctx->Execute();

			if (r == asEXECUTION_FINISHED)
				return ctx->GetReturnByte() != 0;

			return false;
		}

		// Execute object opCmp if available
		if (cache && cache->cmpFunc)
		{
			// TODO: Add proper error handling
			r = ctx->Prepare(cache->cmpFunc); assert(r >= 0);

			if (_subTypeId & asTYPEID_OBJHANDLE)
			{
				r = ctx->SetObject(*((void**)a)); assert(r >= 0);
				r = ctx->SetArgObject(0, *((void**)b)); assert(r >= 0);
			}
			else
			{
				r = ctx->SetObject((void*)a); assert(r >= 0);
				r = ctx->SetArgObject(0, (void*)b); assert(r >= 0);
			}

			r = ctx->Execute();

			if (r == asEXECUTION_FINISHED)
				return (int)ctx->GetReturnDWord() == 0;

			return false;
		}
	}

	return false;
}

int
ScriptArray::findByRef(void *ref) const
{
	return findByRef(0, ref);
}

int 
ScriptArray::findByRef(asUINT startAt, void *ref) const
{
	// Find the matching element by its reference
	asUINT size = this->count();
	if (_subTypeId & asTYPEID_OBJHANDLE)
	{
		// Dereference the pointer
		ref = *(void**)ref;
		for (asUINT i = startAt; i < size; i++)
		{
			if (*(void**)this->at(i) == ref)
				return i;
		}
	}
	else
	{
		// Compare the reference directly
		for (asUINT i = startAt; i < size; i++)
		{
			if (this->at(i) == ref)
				return i;
		}
	}

	return -1;
}

int 
ScriptArray::find(void *value) const
{
	return find(0, value);
}

int 
ScriptArray::find(asUINT startAt, void *value) const
{
	// Check if the subtype really supports find()
	// TODO: Can't this be done at compile time too by the template callback
	SArrayCache *cache = 0;
	if (_subTypeId & ~asTYPEID_MASK_SEQNBR)
	{
		cache = reinterpret_cast<SArrayCache*>(objType->GetUserData(ARRAY_CACHE));
		if (!cache || (cache->cmpFunc == 0 && cache->eqFunc == 0))
		{
			asIScriptContext *ctx = asGetActiveContext();
			asIObjectType* subType = objType->GetEngine()->GetObjectTypeById(_subTypeId);

			// Throw an exception
			if (ctx)
			{
				char tmp[512];

				if (cache && cache->eqFuncReturnCode == asMULTIPLE_FUNCTIONS)
#if defined(_MSC_VER) && _MSC_VER >= 1500 && !defined(__S3E__)
					sprintf_s(tmp, 512, "Type '%s' has multiple matching opEquals or opCmp methods", subType->GetName());
#else
					sprintf(tmp, "Type '%s' has multiple matching opEquals or opCmp methods", subType->GetName());
#endif
				else
#if defined(_MSC_VER) && _MSC_VER >= 1500 && !defined(__S3E__)
					sprintf_s(tmp, 512, "Type '%s' does not have a matching opEquals or opCmp method", subType->GetName());
#else
					sprintf(tmp, "Type '%s' does not have a matching opEquals or opCmp method", subType->GetName());
#endif
				ctx->SetException(tmp);
			}

			return -1;
		}
	}

	asIScriptContext *cmpContext = 0;
	bool isNested = false;

	if (_subTypeId & ~asTYPEID_MASK_SEQNBR)
	{
		// Try to reuse the active context
		cmpContext = asGetActiveContext();
		if (cmpContext)
		{
			if (cmpContext->GetEngine() == objType->GetEngine() && cmpContext->PushState() >= 0)
				isNested = true;
			else
				cmpContext = 0;
		}
		if (cmpContext == 0)
		{
			// TODO: Ideally this context would be retrieved from a pool, so we don't have to
			//       create a new one everytime. We could keep a context with the array object
			//       but that would consume a lot of resources as each context is quite heavy.
			cmpContext = objType->GetEngine()->CreateContext();
		}
	}

	// Find the matching element
	int ret = -1;
	asUINT size = this->count();

	for (asUINT i = startAt; i < size; i++)
	{
		// value passed by reference
		if (Equals(this->at(i), value, cmpContext, cache))
		{
			ret = (int)i;
			break;
		}
	}

	if (cmpContext)
	{
		if (isNested)
		{
			asEContextState state = cmpContext->GetState();
			cmpContext->PopState();
			if (state == asEXECUTION_ABORTED)
				cmpContext->Abort();
		}
		else
			cmpContext->Release();
	}

	return ret;
}

void* 
ScriptArray::GetArrayItemPointer(int index)
{
	return &buffer.data[index * _elementSize];
}

void*
ScriptArray::GetDataPointer(void *buffer)
{
	if ((_subTypeId & asTYPEID_MASK_OBJECT) && !(_subTypeId & asTYPEID_OBJHANDLE))
		return reinterpret_cast<void*>(*(size_t*)buffer);
	else
		return buffer;
}

void 
ScriptArray::sortAsc()
{
	this->sort(0, this->count(), true);
}

void 
ScriptArray::sortAsc(asUINT startAt, asUINT count)
{
	this->sort(startAt, count, true);
}

void 
ScriptArray::sortDesc()
{
	this->sort(0, this->count(), false);
}

void 
ScriptArray::sortDesc(asUINT startAt, asUINT count)
{
	this->sort(startAt, count, false);
}

void 
ScriptArray::sort(asUINT startAt, asUINT count, bool asc)
{
	// Subtype isn't primitive and doesn't have opCmp
	SArrayCache *cache = reinterpret_cast<SArrayCache*>(objType->GetUserData(ARRAY_CACHE));
	if (_subTypeId & ~asTYPEID_MASK_SEQNBR)
	{
		if (!cache || cache->cmpFunc == 0)
		{
			asIScriptContext *ctx = asGetActiveContext();
			asIObjectType* subType = objType->GetEngine()->GetObjectTypeById(_subTypeId);

			// Throw an exception
			if (ctx)
			{
				char tmp[512];

				if (cache && cache->cmpFuncReturnCode == asMULTIPLE_FUNCTIONS)
#if defined(_MSC_VER) && _MSC_VER >= 1500 && !defined(__S3E__)
					sprintf_s(tmp, 512, "Type '%s' has multiple matching opCmp methods", subType->GetName());
#else
					sprintf(tmp, "Type '%s' has multiple matching opCmp methods", subType->GetName());
#endif
				else
#if defined(_MSC_VER) && _MSC_VER >= 1500 && !defined(__S3E__)
					sprintf_s(tmp, 512, "Type '%s' does not have a matching opCmp method", subType->GetName());
#else
					sprintf(tmp, "Type '%s' does not have a matching opCmp method", subType->GetName());
#endif

				ctx->SetException(tmp);
			}

			return;
		}
	}

	// No need to sort
	if (count < 2)
	{
		return;
	}

	int start = startAt;
	int end = startAt + count;

	// Check if we could access invalid item while sorting
	if (start >= (int)buffer.numElements || end >(int)buffer.numElements)
	{
		asIScriptContext *ctx = asGetActiveContext();

		// Throw an exception
		if (ctx)
		{
			ctx->SetException("Index out of bounds");
		}

		return;
	}

	asBYTE tmp[16];
	asIScriptContext *cmpContext = 0;
	bool isNested = false;

	if (_subTypeId & ~asTYPEID_MASK_SEQNBR)
	{
		// Try to reuse the active context
		cmpContext = asGetActiveContext();
		if (cmpContext)
		{
			if (cmpContext->GetEngine() == objType->GetEngine() && cmpContext->PushState() >= 0)
				isNested = true;
			else
				cmpContext = 0;
		}
		if (cmpContext == 0)
		{
			// TODO: Ideally this context would be retrieved from a pool, so we don't have to
			//       create a new one everytime. We could keep a context with the array object
			//       but that would consume a lot of resources as each context is quite heavy.
			cmpContext = objType->GetEngine()->CreateContext();
		}
	}

	// Insertion sort
	for (int i = start + 1; i < end; i++)
	{
		memcpy(tmp, GetArrayItemPointer(i), _elementSize);

		int j = i - 1;

		while (j >= start && Less(GetDataPointer(tmp), this->at(j), asc, cmpContext, cache))
		{
			memcpy(GetArrayItemPointer(j + 1), GetArrayItemPointer(j), _elementSize);
			j--;
		}

		memcpy(GetArrayItemPointer(j + 1), tmp, _elementSize);
	}

	if (cmpContext)
	{
		if (isNested)
		{
			asEContextState state = cmpContext->GetState();
			cmpContext->PopState();
			if (state == asEXECUTION_ABORTED)
				cmpContext->Abort();
		}
		else
			cmpContext->Release();
	}
}

void
ScriptArray::construct(ArrayBuffer& buf, asUINT start, asUINT end)
{
	if (!_primitiveType && !(_subTypeId & asTYPEID_OBJHANDLE))
	{
		void** it = (void**)(buf.data.data() + start * sizeof(void*));
		void** max = (void**)(buf.data.data() + end * sizeof(void*));

		asIScriptEngine *engine = objType->GetEngine();
		asIObjectType *subType = objType->GetSubType();

		for (; it < max; it++)
		{
			*it = (void*)engine->CreateScriptObject(subType);
			if (*it == 0)
			{
				std::memset(it, 0, sizeof(void*)*(max - it));
				return;
			}
		}
	}
}

void
ScriptArray::destruct(ArrayBuffer& buf, asUINT start, asUINT end)
{
	if (!_primitiveType)
	{
		void** it = (void**)(buf.data.data() + start * sizeof(void*));
		void** max = (void**)(buf.data.data() + end   * sizeof(void*));

		asIScriptEngine* engine = objType->GetEngine();
		for (; it < max; ++it)
		{
			if (*it)
				engine->ReleaseScriptObject(*it, objType->GetSubType());
		}
	}
}

void
ScriptArray::CreateBuffer(ArrayBuffer& buf, asUINT numElements)
{
	this->reserve(numElements);
	this->construct(buf, 0, numElements);
}

void 
ScriptArray::CopyBuffer(ArrayBuffer& dst, const ArrayBuffer& src)
{
	asIScriptEngine *engine = objType->GetEngine();
	if (_subTypeId & asTYPEID_OBJHANDLE)
	{
		if (dst.numElements > 0 && src.numElements > 0)
		{
			std::size_t count = dst.numElements > src.numElements ? src.numElements : dst.numElements;

			void **max = (void**)(dst.data.data() + count * sizeof(void*));
			void **d = (void**)dst.data.data();
			void **s = (void**)src.data.data();

			for (; d < max; d++, s++)
			{
				void *tmp = *d;
				*d = *s;
				if (*d)
					engine->AddRefScriptObject(*d, objType->GetSubType());
				if (tmp)
					engine->ReleaseScriptObject(tmp, objType->GetSubType());
			}
		}
	}
	else
	{
		if (dst.numElements > 0 && src.numElements > 0)
		{
			std::size_t count = dst.numElements > src.numElements ? src.numElements : dst.numElements;
			if (!_primitiveType)
			{
				void **max = (void**)(dst.data.data() + count * sizeof(void*));
				void **d = (void**)dst.data.data();
				void **s = (void**)src.data.data();

				asIObjectType* subType = objType->GetSubType();
				for (; d < max; d++, s++)
					engine->AssignScriptObject(*d, *s, subType);
			}
			else
			{
				std::memcpy(dst.data.data(), src.data.data(), count * _elementSize);
			}
		}
	}
}

void 
ScriptArray::Precache()
{
	_subTypeId = objType->GetSubTypeId();

	// Check if it is an array of objects. Only for these do we need to cache anything
	// Type ids for primitives and enums only has the sequence number part
	if (!(_subTypeId & ~asTYPEID_MASK_SEQNBR))
		return;

	// The opCmp and opEquals methods are cached because the searching for the
	// methods is quite time consuming if a lot of array objects are created.

	// First check if a cache already exists for this array type
	SArrayCache *cache = reinterpret_cast<SArrayCache*>(objType->GetUserData(ARRAY_CACHE));
	if (cache) return;

	// We need to make sure the cache is created only once, even
	// if multiple threads reach the same point at the same time
	asAcquireExclusiveLock();

	// Now that we got the lock, we need to check again to make sure the
	// cache wasn't created while we were waiting for the lock
	cache = reinterpret_cast<SArrayCache*>(objType->GetUserData(ARRAY_CACHE));
	if (cache)
	{
		asReleaseExclusiveLock();
		return;
	}

	// Create the cache
	cache = reinterpret_cast<SArrayCache*>(userAlloc(sizeof(SArrayCache)));
	memset(cache, 0, sizeof(SArrayCache));

	// If the sub type is a handle to const, then the methods must be const too
	bool mustBeConst = (_subTypeId & asTYPEID_HANDLETOCONST) ? true : false;

	asIObjectType *subType = objType->GetEngine()->GetObjectTypeById(_subTypeId);
	if (subType)
	{
		for (asUINT i = 0; i < subType->GetMethodCount(); i++)
		{
			asIScriptFunction *func = subType->GetMethodByIndex(i);

			if (func->GetParamCount() == 1 && (!mustBeConst || func->IsReadOnly()))
			{
				asDWORD flags = 0;
				int returnTypeId = func->GetReturnTypeId(&flags);

				// The method must not return a reference
				if (flags != asTM_NONE)
					continue;

				// opCmp returns an int and opEquals returns a bool
				bool isCmp = false, isEq = false;
				if (returnTypeId == asTYPEID_INT32 && strcmp(func->GetName(), "opCmp") == 0)
					isCmp = true;
				if (returnTypeId == asTYPEID_BOOL && strcmp(func->GetName(), "opEquals") == 0)
					isEq = true;

				if (!isCmp && !isEq)
					continue;

				// The parameter must either be a reference to the subtype or a handle to the subtype
				int paramTypeId;
				func->GetParam(0, &paramTypeId, &flags);

				if ((paramTypeId & ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)) != (_subTypeId &  ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)))
					continue;

				if ((flags & asTM_INREF))
				{
					if ((paramTypeId & asTYPEID_OBJHANDLE) || (mustBeConst && !(flags & asTM_CONST)))
						continue;
				}
				else if (paramTypeId & asTYPEID_OBJHANDLE)
				{
					if (mustBeConst && !(paramTypeId & asTYPEID_HANDLETOCONST))
						continue;
				}
				else
					continue;

				if (isCmp)
				{
					if (cache->cmpFunc || cache->cmpFuncReturnCode)
					{
						cache->cmpFunc = 0;
						cache->cmpFuncReturnCode = asMULTIPLE_FUNCTIONS;
					}
					else
						cache->cmpFunc = func;
				}
				else if (isEq)
				{
					if (cache->eqFunc || cache->eqFuncReturnCode)
					{
						cache->eqFunc = 0;
						cache->eqFuncReturnCode = asMULTIPLE_FUNCTIONS;
					}
					else
						cache->eqFunc = func;
				}
			}
		}
	}

	if (cache->eqFunc == 0 && cache->eqFuncReturnCode == 0)
		cache->eqFuncReturnCode = asNO_FUNCTION;
	if (cache->cmpFunc == 0 && cache->cmpFuncReturnCode == 0)
		cache->cmpFuncReturnCode = asNO_FUNCTION;

	// Set the user data only at the end so others that retrieve it will know it is complete
	objType->SetUserData(cache, ARRAY_CACHE);

	asReleaseExclusiveLock();
}

void 
ScriptArray::EnumReferences(asIScriptEngine *engine)
{
	if (_subTypeId & asTYPEID_MASK_OBJECT)
	{
		void **d = (void**)buffer.data.data();
		for (asUINT n = 0; n < buffer.numElements; n++)
		{
			if (d[n])
				engine->GCEnumCallback(d[n]);
		}
	}
}

void 
ScriptArray::ReleaseAllHandles(asIScriptEngine *)
{
	resize(0);
}

bool
ScriptArray::CheckMaxSize(asUINT numElements)
{
	asUINT maxSize = 0xFFFFFFFFul - sizeof(ArrayBuffer) + 1;
	if (_elementSize > 0)
		maxSize /= _elementSize;

	if (numElements > maxSize)
	{
		asIScriptContext *ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Too large array size");

		return false;
	}

	return true;
}

bool
ScriptArray::CheckIndex(asUINT index)
{
	if (index >= buffer.numElements)
	{
		asIScriptContext *ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Index out of bounds");
		return false;
	}
	return true;
}

asIObjectType*
ScriptArray::GetArrayObjectType() const
{
	return objType;
}

int
ScriptArray::GetArrayTypeId() const
{
	return objType->GetTypeId();
}

int
ScriptArray::GetElementTypeId() const
{
	return _subTypeId;
}

ScriptArray*
ScriptArray::Create(asIObjectType *ot, asUINT length)
{
	ScriptArray* array = new ScriptArray(ot, length, nullptr);
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx && ctx->GetState() == asEXECUTION_EXCEPTION)
	{
		array->release();
		return nullptr;
	}

	return array;
}

ScriptArray*
ScriptArray::Create(asIObjectType *ot, void* initList)
{
	ScriptArray* array = new ScriptArray(ot, initList);
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx && ctx->GetState() == asEXECUTION_EXCEPTION)
	{
		array->release();
		return nullptr;
	}

	return array;
}

ScriptArray*
ScriptArray::Create(asIObjectType *ot, asUINT length, void *defVal)
{
	ScriptArray* array = new ScriptArray(ot, length, defVal);
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx && ctx->GetState() == asEXECUTION_EXCEPTION)
	{
		array->release();
		return nullptr;
	}

	return array;
}

ScriptArray*
ScriptArray::Create(asIObjectType *ot)
{
	return ScriptArray::Create(ot, asUINT(0));
}

void
ScriptBindArray::setup(asIScriptEngine* engine) noexcept
{
	RegisterScriptArray(engine);

	int r = 0;
	r = engine->RegisterObjectType("vector<class T>", 0, asOBJ_REF | asOBJ_GC | asOBJ_TEMPLATE); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_TEMPLATE_CALLBACK, "bool f(int&in, bool&out)", asFUNCTION(ScriptArrayTemplateCallback), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_FACTORY, "vector<T>@ f(int&in)", asFUNCTIONPR(ScriptArray::Create, (asIObjectType*), ScriptArray*), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_FACTORY, "vector<T>@ f(int&in, uint)", asFUNCTIONPR(ScriptArray::Create, (asIObjectType*, asUINT), ScriptArray*), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_FACTORY, "vector<T>@ f(int&in, uint, const T &in)", asFUNCTIONPR(ScriptArray::Create, (asIObjectType*, asUINT, void *), ScriptArray*), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(ScriptArray, refCount), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_SETGCFLAG, "void f()", asMETHOD(ScriptArray, setGC), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(ScriptArray, getGC), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(ScriptArray, EnumReferences), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(ScriptArray, ReleaseAllHandles), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_LIST_FACTORY, "vector<T>@ f(int&in type, int&in list) {repeat T}", asFUNCTIONPR(ScriptArray::Create, (asIObjectType*, void*), ScriptArray*), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptArray, addRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector<T>", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptArray, release), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("vector<T>", "T &opIndex(uint)", asMETHODPR(ScriptArray, at, (asUINT), void*), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "const T &opIndex(uint) const", asMETHODPR(ScriptArray, at, (asUINT) const, const void*), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "vector<T>& opAssign(const vector<T>& in)", asMETHODPR(ScriptArray, assgin, (const ScriptArray&), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "bool opEquals(const vector<T>& in) const", asMETHOD(ScriptArray, operator==), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("vector<T>", "void insertAt(uint, const T&in)", asMETHODPR(ScriptArray, insert, (asUINT, void*), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void removeAt(uint)", asMETHODPR(ScriptArray, remove, (asUINT), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void insertLast(const T&in)", asMETHOD(ScriptArray, insertLast), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void removeLast()", asMETHOD(ScriptArray, removeLast), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "uint length() const", asMETHOD(ScriptArray, count), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void reserve(uint)", asMETHOD(ScriptArray, reserve), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void resize(uint)", asMETHODPR(ScriptArray, resize, (asUINT), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void sortAsc()", asMETHODPR(ScriptArray, sortAsc, (), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void sortAsc(uint, uint)", asMETHODPR(ScriptArray, sortAsc, (asUINT, asUINT), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void sortDesc()", asMETHODPR(ScriptArray, sortDesc, (), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void sortDesc(uint, uint)", asMETHODPR(ScriptArray, sortDesc, (asUINT, asUINT), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void reverse()", asMETHOD(ScriptArray, reverse), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "int find(const T& in) const", asMETHODPR(ScriptArray, find, (void*) const, int), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "int find(uint, const T&in) const", asMETHODPR(ScriptArray, find, (asUINT, void*) const, int), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "int findByRef(const T&in) const", asMETHODPR(ScriptArray, findByRef, (void*) const, int), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "int findByRef(uint, const T& in) const", asMETHODPR(ScriptArray, findByRef, (asUINT, void*) const, int), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "bool isEmpty() const", asMETHOD(ScriptArray, empty), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "uint size() const", asMETHOD(ScriptArray, count), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "bool empty() const", asMETHOD(ScriptArray, empty), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void push_back(const T& in)", asMETHOD(ScriptArray, insertLast), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void pop_back()", asMETHOD(ScriptArray, removeLast), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void insert(uint, const T& in)", asMETHODPR(ScriptArray, insert, (asUINT, void*), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void erase(uint)", asMETHODPR(ScriptArray, remove, (asUINT), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void clear()", asMETHOD(ScriptArray, clear), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector<T>", "void swap(vector<T>& in)", asMETHOD(ScriptArray, swap), asCALL_THISCALL); assert(r >= 0);
}

_NAME_END
#endif