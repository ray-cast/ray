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
#ifndef _H_BINARY_H_
#define _H_BINARY_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class Binaryt
{
private:
	T* bits_;
	T col_;
	T row_;

	enum
	{
		bit_size = sizeof(T) * 8
	};

public:
	Binaryt() noexcept
		: bits_(0)
	{
	}

	Binaryt(std::uint32_t x, std::uint32_t y) noexcept
		: bits_(0)
	{
		this->create(x, y);
	}

	~Binaryt() noexcept
	{
		if (bits_)
		{
			delete[] bits_;
			bits_ = 0;
		}
	}

	bool create(std::uint32_t x, std::uint32_t y)
	{
		assert(!bits_);

		col_ = x;
		row_ = y;

		std::size_t size = row_ * col_ / 32;
		if (size < std::numeric_limits<std::uint32_t>::max())
		{
			std::uint32_t sz = (std::uint32_t)size;

			if (row_ * col_ % bit_size)
				sz++;

			bits_ = new T[sz];

			if (bits_)
			{
				memset(bits_, 0, sz * 4);

				return true;
			}
		}

		return false;
	}

	// 标志清零
	void reset()
	{
		assert(bits_);

		std::uint32_t size = row_ * col_ / 32;

		if (row_ * col_ % bit_size)
			size++;

		memset(bits_, 0, size * 4);
	}

	// 设置标志位
	void set(std::uint32_t x, std::uint32_t y, bool bFlog = true)
	{
		assert(bits_);
		assert(x < col_ && y < row_);

		if (bFlog)
		{
			bits_[(y*col_ + x) / bit_size] |= (1 << (y*col_ + x) % bit_size);
		}
		else
		{
			bits_[(y*col_ + x) / 32] &= ~(1 << (y*col_ + x) % 32);
		}
	}

	bool isTrue(std::uint32_t x, std::uint32_t y) const //查询该标志
	{
		return  (bits_[(y*col_ + x) / bit_size] & (1 << (y*col_ + x) % bit_size)) != 0;
	}
};

_NAME_END

#endif