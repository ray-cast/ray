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
#ifndef _H_EULER_H_
#define _H_EULER_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class EulerAnglest
{
public:
    T x, y, z;

    EulerAnglest() noexcept {};
    EulerAnglest(T xx, T yy, T zz) noexcept
        : x(xx)
		, y(yy)
        , z(zz)
    {
    }

	EulerAnglest(const Vector3t<T>& v) noexcept
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{
	}

	EulerAnglest(const Quaterniont<T>& q) noexcept
	{
		this->makeRotate(q);
	}

	EulerAnglest<T>& identity() noexcept
    {
        x = z = y = 0.0f;
    }

    EulerAnglest<T>& canonize() noexcept
    {
        x = wrapPI(x);

        if (x <-M_PI_2)
        {
            x = -M_PI - x;
            y += M_PI;
            z += M_PI;
        }
        else if (x > M_PI_2)
        {
            x = M_PI - x;
            y += M_PI;
            z += M_PI;
        }

        if (fabs(x) > M_PI_2 - 1e-4)
        {
            y += z;
            z = 0.0f;
        }
        else
        {
            z = wrapPI(z);
        }

        y = wrapPI(y);

		return *this;
    }

	EulerAnglest<T>& makeRotate(const Quaterniont<T>& q) noexcept
	{
		T sp = -2.0f * (q.y * q.z + q.w * q.x);

		if (std::fabs(sp) > 0.9999f)
		{
			this->x = M_PI_2 * sp;
			this->y = atan2(-q.x * q.z - q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
			this->z = 0.0f;
		}
		else
		{
			this->x = asin(sp);
			this->y = atan2(q.x * q.z - q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
			this->z = atan2(q.x * q.y - q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
		}

		return *this;
	}
};

_NAME_END

#endif
