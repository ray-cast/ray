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
#ifndef _H_ARRAY_H_
#define _H_ARRAY_H_

#include <ray/mathfwd.h>
#include <vector>

_NAME_BEGIN
/*
enum DATATYPE
{
    BYTE,
    SHORT,
    INT,
    SIZE_T,
    UNSIGNED_BYTE,
    UNSIGNED_SHORT,
    UNSIGNED_INT,
    UINT32,
    FLOAT,
    DOUBLE,
    COLOR3,
    COLOR3F,
    COLOR3D,
    COLOR4,
    COLOR4F,
    COLOR4D,
    VECTOR2,
    VECTOR2F,
    VECTOR2D,
    VECTOR2I,
    VECTOR2UB,
    VECTOR3,
    VECTOR3F,
    VECTOR3D,
    VECTOR3I,
    VECTOR3UB,
    VECTOR4,
    VECTOR4F,
    VECTOR4D,
    VECTOR4UB,
    MATRIX2X2,
    MATRIX3X3,
    MATRIX4X4
};

class IArray
{
public:
    virtual DATATYPE getArrayType() const = 0;
    virtual std::uint8_t getDataSize()  const = 0;
    virtual DATATYPE  getDataType()  const = 0;
    virtual std::size_t getDataByte() const = 0;

    virtual const void* data() const = 0;

    virtual std::size_t size() const = 0;
};

template<typename T, DATATYPE arrayType, std::uint8_t dataSize, DATATYPE dataType>
class Array : public IArray, public std::vector<T>
{
public:
    virtual ~Array() noexcept
        {}

    DATATYPE getArrayType() const  noexcept
        { return arrayType; }

    std::uint8_t getDataSize()  const noexcept
        { return dataSize; }

    DATATYPE  getDataType()  const noexcept
        { return dataType; }

    std::size_t getDataByte() const noexcept
        { return sizeof(T)* this->size(); }

    const T* getDataPointer() const noexcept
        { return (const T*)std::vector<T>::data(); }

    const void* data() const noexcept
        { return std::vector<T>::data(); }

    std::size_t size() const noexcept
        { return std::vector<T>::size(); }

    void resize(std::size_t size) noexcept
        { std::vector<T>::resize(size); }
};

typedef Array<char, BYTE, 1, BYTE>                      ByteArray;
typedef Array<short, SHORT, 1, SHORT>                   ShortArray;
typedef Array<int, INT, 1, INT>                         IntArray;
typedef Array<unsigned char, UNSIGNED_BYTE, 1, UNSIGNED_BYTE>   UByteArray;
typedef Array<unsigned short, UNSIGNED_SHORT, 1, UNSIGNED_SHORT> UShortArray;
typedef Array<unsigned int, UNSIGNED_INT, 1, UNSIGNED_INT>   UIntArray;
typedef Array<float, FLOAT, 1, FLOAT>                   FloatArray;
typedef Array<double, DOUBLE, 1, DOUBLE>                DoubleArray;

// array
typedef Array<Vector2, VECTOR2, 2, FLOAT> Vector2Array;
typedef Array<Vector2f, VECTOR2F, 2, FLOAT> Vector2fArray;
typedef Array<Vector2d, VECTOR2D, 2, DOUBLE> Vector2dArray;

typedef Array<Vector3, VECTOR3, 3, FLOAT> Vector3Array;
typedef Array<Vector3f, VECTOR3F, 3, FLOAT> Vector3fArray;
typedef Array<Vector3d, VECTOR3D, 3, DOUBLE> Vector3dArray;

typedef Array<Vector4, VECTOR4, 4, FLOAT> Vector4Array;
typedef Array<Vector4f, VECTOR4F, 4, FLOAT> Vector4fArray;
typedef Array<Vector4d, VECTOR4D, 4, DOUBLE> Vector4dArray;

typedef Array<Color3, COLOR3, 3, FLOAT> Color3Array;
typedef Array<Color3f, COLOR3F, 3, FLOAT> Color3fArray;
typedef Array<Color3d, COLOR3D, 3, DOUBLE> Color3dArray;

typedef Array<Color3, COLOR3, 3, FLOAT> Color3Array;
typedef Array<Color3f, COLOR3F, 3, FLOAT> Color3fArray;
typedef Array<Color3d, COLOR3D, 3, DOUBLE> Color3dArray;

typedef Array<Color4, COLOR4, 4, FLOAT> Color4Array;
typedef Array<Color4f, COLOR4F, 4, FLOAT> Color4fArray;
typedef Array<Color4d, COLOR4D, 4, DOUBLE> Color4dArray;

typedef Array<Color4, COLOR4, 4, FLOAT> Color4Array;
typedef Array<Color4f, COLOR4F, 4, FLOAT> Color4fArray;
typedef Array<Color4d, COLOR4D, 4, DOUBLE> Color4dArray;

typedef Array<std::uint32_t, UINT32, 1, INT> FaceArray;
*/
_NAME_END

#endif