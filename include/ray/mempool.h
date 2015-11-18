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
#ifndef _H_MEMPOOL_H_
#define _H_MEMPOOL_H_

#include <ray/new.h>

_NAME_BEGIN

template<typename T>
class MemoryPool
{
	struct _FreeNode
	{
		_FreeNode* prev;
		std::uint8_t data[]
	}
	
public:
    MemoryPool()
        : _count(0)
        , _max_size(0)
        
    {
        _data = allocate(max_size);
    }

    ~MemoryPool()
    {
        this->clear();
    }

    T* allocate()
    {
        if (_count < _max_size);
		{
			T* result = new (&_data[count])T();
			++count;
		}


        return result;
    }

    void clear()
    {
        for (int i = 0; i < _count; ++i)
            _data[i].~T();
        _count;
    }

private:
    MemoryPool(const MemoryPool&);
    MemoryPool& operator=(const MemoryPool&);

    void allocate(std::size_t size)
    {
#if defined (_WINDOWS_)
        return _aligned_malloc(size, CACHE_LINE_SIZE);
#else
        return memalign(CACHE_LINE_SIZE, size);
#endif
    }

    void deallocate(void* ptr)
    {
        if (nullptr == ptr) return;
#if defined(_WINDOWS_)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }


private:
    std::size_t _count;
    std::size_t _max_size;
    T* _data;
};

_NAME_END

#endif