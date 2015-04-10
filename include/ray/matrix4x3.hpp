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
// | Author: Xiemoperor <Xiemoperor@gmail.com>
// +----------------------------------------------------------------------
// | @file matrix4x3.hpp
// | @brief matrix4x4 structure, including operators when compiling in C++
// +----------------------------------------------------------------------

namespace ray
{

    template<typename T=float>
    class Matrix4x3
    {
    public:
        Matrix4x3()
        {
            // empty
        }

        void zeroTranslation() { mat[3][0] = mat[3][1] = mat[3][2] = 0.0f; }
        void setTranslation(const Vector3t<T>& d) { mat[3][0] = d.x; mat[3][1] = d.y; mat[3][2] = d.z; };

        void setupTranslation(const Vector3t<T>& d)
        {
            mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
            mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
            mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;
            mat[3][0] = d.x; mat[3][1] = d.y; mat[3][2] = d.z;
        }

        void setupLocalToParent(const Vector3t<T>& pos, const EulerAnglest<T>& orient)
        {
            Matrix4x4t<T> orientMatrix;
            orientMatrix.setup(orient);

            setupLocalToParent(pos, orientMatrix);
        }

        void setupLocalToParent(const Vector3t<T>& pos, const Matrix4x4t<T>& orient)
        {
            mat[0][0] = orient(0,0);
            mat[0][1] = orient(0,1);
            mat[0][2] = orient(0,2);
            mat[1][0] = orient(1,0);
            mat[1][1] = orient(1,1);
            mat[1][2] = orient(1,2);
            mat[2][0] = orient(2,0);
            mat[2][1] = orient(2,1);
            mat[2][2] = orient(2,2);

            mat[3][0] = pos.x; mat[3][1] = pos.y; mat[3][2] = pos.z;
        }

        void setupParentToLocal(const Vector3t<T>& pos, const EulerAnglest<T>& orient)
        {
            Matrix4x4t<T> orientMatrix;
            orientMatrix.setup(orient);

            setupParentToLocal(pos, orientMatrix);
        }

        void setupParentToLocal(const Vector3t<T>& pos, const Matrix4x4t<T>& orient)
        {
            mat[0][0] = orient(0,0);
            mat[0][1] = orient(0,1);
            mat[0][2] = orient(0,2);
            mat[1][0] = orient(1,0);
            mat[1][1] = orient(1,1);
            mat[1][2] = orient(1,2);
            mat[2][0] = orient(2,0);
            mat[2][1] = orient(2,1);
            mat[2][2] = orient(2,2);

            mat[3][0] = -(pos.x * mat[0][0] + pos.y * mat[1][0] + pos.z * mat[2][0]);
            mat[3][1] = -(pos.x * mat[0][1] + pos.y * mat[1][1] + pos.z * mat[2][1]);
            mat[3][2] = -(pos.x * mat[0][2] + pos.y * mat[1][2] + pos.z * mat[2][2]);
        }

        void setupRotate(int axis, T theta)
        {
            T s, c;
            sinCos(&s, &c, theta);

            switch (axis)
            {
            case 1:
                mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = 0.0f; mat[1][1] = c;    mat[1][2] = s;
                mat[2][0] = 0.0f; mat[2][1] = -s;   mat[2][2] = c;

                break;
            case 2:
                mat[0][0] = c;    mat[0][1] = 0.0f; mat[0][2] = -s;
                mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
                mat[2][0] = s;    mat[2][1] = 0.0f; mat[2][2] = c;

                break;
            case 3:
                mat[0][0] = c;    mat[0][1] = s;    mat[0][2] = 0.0f;
                mat[1][0] = -s;   mat[1][1] = c;    mat[1][2] = 0.0f;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

                break;
            default:
                assert(false);

                break;
            }

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupRotate(const Vector3t<T> &axis, T theta)
        {
            assert(fabs(axis.length() - 1.0f) <0.01f);

            T s, c;
            sinCos(&s, &c, theta);

            value_type a = 1.0f - c;
            T ax = a * axis.x;
            T ay = a * axis.y;
            T az = a * axis.z;

            mat[0][0] = ax * axis.x + c;
            mat[0][1] = ax * axis.y + axis.z * s;
            mat[0][2] = ax * axis.z - axis.y * s;

            mat[1][0] = ay * axis.x - axis.z * s;
            mat[1][1] = ay * axis.y + c;
            mat[1][2] = ay * axis.z + axis.x * s;

            mat[2][0] = az * axis.x + axis.y * s;
            mat[2][1] = az * axis.y - axis.x * s;
            mat[2][2] = az * axis.z + c;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void fromQuaterniont(const Quaterniont<T>& q)
        {
            T ww = 2.0f * q.w;
            T xx = 2.0f * q.x;
            T yy = 2.0f * q.y;
            T zz = 2.0f * q.z;

            mat[0][0] = 1.0f - yy * q.y - zz * q.z;
            mat[0][1] = xx * q.y + ww * q.z;
            mat[0][2] = xx * q.z - ww * q.x;

            mat[1][0] = xx * q.y - ww * q.z;
            mat[1][1] = 1.0f - xx * q.x - zz * q.z;
            mat[1][2] = yy * q.z + ww * q.w;

            mat[2][0] = xx * q.z + ww * q.y;
            mat[2][1] = yy * q.z - ww * q.x;
            mat[2][2] = 1.0f - xx * q.x - yy * q.y;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupScale(const Vector3t<T> &s)
        {
            mat[0][0] = s.x; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
            mat[1][0] = 0.0f; mat[1][1] = s.y; mat[1][2] = 0.0f;
            mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = s.z;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupScaleAlongAxis(const Vector3t<T>& axis, T k)
        {
            assert(fabs(axis.length() - 1.0f) <0.01f);

            T a = k - 1.0f;
            T ax = a * axis.x;
            T ay = a * axis.y;
            T az = a * axis.z;

            mat[0][0] = ax * axis.x + 1.0f;
            mat[1][1] = ay * axis.y + 1.0f;
            mat[2][2] = az * axis.z + 1.0f;

            mat[0][1] = mat[1][0] = ax * axis.y;
            mat[0][2] = mat[2][0] = ax * axis.z;
            mat[1][2] = mat[2][1] = ay * axis.z;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupShear(int axis, T s, T t)
        {
            switch (axis)
            {
            case  1:
                mat[0][0] = 1.0f; mat[0][1] = s; mat[0][2] = t;
                mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

                break;
            case  2:
                mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = s; mat[1][1] = 1.0f; mat[1][2] = t;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

                break;
            case  3:
                mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
                mat[2][0] = s; mat[2][1] = t; mat[2][2] = 1.0f;

                break;
            default:
                assert(false);

                break;
            }

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupProject(const Vector3t<T>& n)
        {
            assert(fabs(n.length() - 1.0f) <0.01f);

            mat[0][0] = 1.0f * n.x + n.x;
            mat[1][1] = 1.0f * n.y + n.y;
            mat[2][2] = 1.0f * n.z + n.z;

            mat[0][1] = mat[1][0] = -n.x * n.y;
            mat[0][2] = mat[2][0] = -n.x * n.z;
            mat[1][2] = mat[2][1] = -n.y * n.z;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupReflect(int axis, T k)
        {
            switch (axis)
            {
            case 1:
                mat[0][0] = -1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

                mat[3][0] = 2.0f * k;
                mat[3][1] = 0.0f;
                mat[3][2] = 0.0f;

                break;
            case 2:
                mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = 0.0f; mat[1][1] = -1.0f; mat[1][2] = 0.0f;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

                mat[3][0] = 0.0f;
                mat[3][1] = 2.0f * k;
                mat[3][2] = 0.0f;

                break;
            case 3:
                mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
                mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
                mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = -1.0f;

                mat[3][0] = 0.0f;
                mat[3][1] = 0.0f;
                mat[3][2] = 2.0f * k;

                break;
            default:
                assert(false);

                break;
            }

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        void setupReflect(const Vector3t<T>& n)
        {
            assert(fabs(n.length() - 1.0f) <0.01f);

            T ax = -2.0f * n.x;
            T ay = -2.0f * n.y;
            T az = -2.0f * n.z;

            mat[0][0] = 1.0f * ax + n.x;
            mat[1][1] = 1.0f * ay + n.y;
            mat[2][2] = 1.0f * az + n.z;

            mat[0][1] = mat[1][0] = ax * n.y;
            mat[0][2] = mat[2][0] = ax * n.z;
            mat[1][2] = mat[2][1] = ay * n.z;

            mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
        }

        T determinant()
        {
            return
                mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) +
                mat[0][1] * (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) +
                mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
        }

        Matrix4x3 inverse()
        {
            T det = determinant();

            assert(fabs(det) > 0.000001f);

            T oneOverDet = 1.0f / det;

            Matrix4x3 r;

            r.mat[0][0] = (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) * oneOverDet;
            r.mat[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) * oneOverDet;
            r.mat[0][2] = (mat[0][0] * mat[1][2] - mat[0][2] * mat[1][1]) * oneOverDet;

            r.mat[1][0] = (mat[1][2] * mat[2][1] - mat[1][0] * mat[2][2]) * oneOverDet;
            r.mat[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][1]) * oneOverDet;
            r.mat[1][2] = (mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2]) * oneOverDet;

            r.mat[2][0] = (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][1]) * oneOverDet;
            r.mat[2][1] = (mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1]) * oneOverDet;
            r.mat[2][2] = (mat[0][0] * mat[1][1] - mat[0][1] * mat[1][1]) * oneOverDet;

            r.mat[3][0] = -(mat[3][0] * mat[0][0] + mat[3][1] * mat[1][0] + mat[3][2] * mat[2][0]);
            r.mat[3][1] = -(mat[3][0] * mat[0][1] + mat[3][1] * mat[1][1] + mat[3][2] * mat[2][1]);
            r.mat[3][2] = -(mat[3][0] * mat[0][2] + mat[3][1] * mat[1][2] + mat[3][2] * mat[2][2]);

            return r;
        }

        Vector3t<T> getTranslation() const { return Vector3t<T>(mat[3][0], mat[3][1], mat[3][2]); };

        Vector3t<T> getPositionFro_parentToLocalMatrix()
        {
            return Vector3t<T>(
                -(mat[3][0] * mat[0][0] + mat[3][1] * mat[0][1] + mat[3][2] * mat[0][2]),
                -(mat[3][0] * mat[1][0] + mat[3][1] * mat[1][1] + mat[3][2] * mat[1][2]),
                -(mat[3][0] * mat[2][0] + mat[3][1] * mat[1][1] + mat[3][2] * mat[2][2]));
        }

        Vector3t<T> getPositionFromLocalToParentMatrix() { return Vector3t<T>(mat[3][0], mat[3][1], mat[3][2]); }

        T mat[4][3];
    };
}