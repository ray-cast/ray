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
#include <ray/imagcubemap.h>
#include <ray/math.h>

_NAME_BEGIN

namespace image
{
	enum
	{
		CMFT_FACE_POS_X = 0,
		CMFT_FACE_NEG_X = 1,
		CMFT_FACE_POS_Y = 2,
		CMFT_FACE_NEG_Y = 3,
		CMFT_FACE_POS_Z = 4,
		CMFT_FACE_NEG_Z = 5,
	};

	static const float s_faceUvVectors[6][3][3] =
	{
		{ // +x face
			{ 0.0f,  0.0f, -1.0f }, // u -> -z
			{ 0.0f, -1.0f,  0.0f }, // v -> -y
			{ 1.0f,  0.0f,  0.0f }, // +x face
		},
		{ // -x face
			{ 0.0f,  0.0f,  1.0f }, // u -> +z
			{ 0.0f, -1.0f,  0.0f }, // v -> -y
			{ -1.0f,  0.0f,  0.0f }, // -x face
		},
		{ // +y face
			{ 1.0f,  0.0f,  0.0f }, // u -> +x
			{ 0.0f,  0.0f,  1.0f }, // v -> +z
			{ 0.0f,  1.0f,  0.0f }, // +y face
		},
		{ // -y face
			{ 1.0f,  0.0f,  0.0f }, // u -> +x
			{ 0.0f,  0.0f, -1.0f }, // v -> -z
			{ 0.0f, -1.0f,  0.0f }, // -y face
		},
		{ // +z face
			{ 1.0f,  0.0f,  0.0f }, // u -> +x
			{ 0.0f, -1.0f,  0.0f }, // v -> -y
			{ 0.0f,  0.0f,  1.0f }, // +z face
		},
		{ // -z face
			{ -1.0f,  0.0f,  0.0f }, // u -> -x
			{ 0.0f, -1.0f,  0.0f }, // v -> -y
			{ 0.0f,  0.0f, -1.0f }, // -z face
		}
	};

	static inline void texelCoordToVec(float* _out3f, float _u, float _v, std::uint8_t _faceId)
	{
		_out3f[0] = s_faceUvVectors[_faceId][0][0] * _u + s_faceUvVectors[_faceId][1][0] * _v + s_faceUvVectors[_faceId][2][0];
		_out3f[1] = s_faceUvVectors[_faceId][0][1] * _u + s_faceUvVectors[_faceId][1][1] * _v + s_faceUvVectors[_faceId][2][1];
		_out3f[2] = s_faceUvVectors[_faceId][0][2] * _u + s_faceUvVectors[_faceId][1][2] * _v + s_faceUvVectors[_faceId][2][2];

		const float invLen = 1.0f / std::sqrt(_out3f[0] * _out3f[0] + _out3f[1] * _out3f[1] + _out3f[2] * _out3f[2]);
		_out3f[0] *= invLen;
		_out3f[1] *= invLen;
		_out3f[2] *= invLen;
	}

	static inline void latLongFromVec(float& _u, float& _v, const float _vec[3])
	{
		const float phi = atan2f(_vec[0], _vec[2]);
		const float theta = acosf(_vec[1]);

		_u = (M_PI + phi) / (M_PI * 2.0f);
		_v = theta / M_PI;
	}

	static inline void vecFromLatLong(float _vec[3], float _u, float _v)
	{
		const float phi = _u * M_TWO_PI;
		const float theta = _v * M_PI;

		_vec[0] = -std::sinf(theta) * std::sinf(phi);
		_vec[1] = std::cosf(theta);
		_vec[2] = -std::sinf(theta) * std::cosf(phi);
	}

	inline void vec3Mul(float* _result, const float* _a, float _b)
	{
		_result[0] = _a[0] * _b;
		_result[1] = _a[1] * _b;
		_result[2] = _a[2] * _b;
	}

	inline float vec3Dot(const float* _a, const float* _b)
	{
		return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2];
	}

	static inline void vecToTexelCoord(float& _u, float& _v, uint8_t& _faceIdx, const float* _vec)
	{
		const float absVec[3] =
		{
			fabsf(_vec[0]),
			fabsf(_vec[1]),
			fabsf(_vec[2]),
		};
		const float max = fmaxf(fmaxf(absVec[0], absVec[1]), absVec[2]);

		// Get face id (max component == face vector).
		if (max == absVec[0])
		{
			_faceIdx = (_vec[0] >= 0.0f) ? uint8_t(CMFT_FACE_POS_X) : uint8_t(CMFT_FACE_NEG_X);
		}
		else if (max == absVec[1])
		{
			_faceIdx = (_vec[1] >= 0.0f) ? uint8_t(CMFT_FACE_POS_Y) : uint8_t(CMFT_FACE_NEG_Y);
		}
		else //if (max == absVec[2])
		{
			_faceIdx = (_vec[2] >= 0.0f) ? uint8_t(CMFT_FACE_POS_Z) : uint8_t(CMFT_FACE_NEG_Z);
		}

		// Divide by max component.
		float faceVec[3];
		vec3Mul(faceVec, _vec, 1.0f / max);

		// Project other two components to face uv basis.
		_u = (vec3Dot(s_faceUvVectors[_faceIdx][0], faceVec) + 1.0f) * 0.5f;
		_v = (vec3Dot(s_faceUvVectors[_faceIdx][1], faceVec) + 1.0f) * 0.5f;
	}

	bool isCubemap(const Image& image) noexcept
	{
		return (6 == image.depth()) && (image.width() == image.height());
	}

	bool isLatLong(const Image& image) noexcept
	{
		return math::equal((float)image.width() / image.height(), 2.0f);
	}

	bool isHStrip(const Image& image) noexcept
	{
		return (image.width() == 6 * image.height());
	}

	bool isVStrip(const Image& image) noexcept
	{
		return (6 * image.width() == image.height());
	}

	bool isOctant(const Image& image) noexcept
	{
		return image.width() == image.height();
	}

	template<std::uint8_t face>
	void makeCubefaceFromLatLong(Image& dst, const Image& src)
	{
		assert(isLatLong(src));
		assert(dst.width() == dst.height() && dst.depth() > face);
		assert(dst.channel() == 3 || dst.channel() == 4);
		assert(dst.value_type() == image::value_t::Float && dst.type_size() == 4);
		assert(src.channel() == 3 || src.channel() == 4);
		assert(src.value_type() == image::value_t::Float && src.type_size() == 4);

		const std::uint32_t dstChannel = dst.channel();
		const std::uint32_t dstBytesPerPixel = dstChannel * dst.type_size();
		const std::uint32_t dstFaceSize = dst.width();
		const std::uint32_t dstPitch = dstFaceSize * dstBytesPerPixel;
		const std::uint32_t dstFaceDataSize = dstPitch * dstFaceSize;

		const std::uint32_t srcBytesPerPixel = src.channel() * src.type_size();
		const std::uint32_t srcPitch = src.width() * srcBytesPerPixel;

		const float srcWidthMinusOne = static_cast<float>(src.width() - 1);
		const float srcHeightMinusOne = static_cast<float>(src.height() - 1);

		const float invDstFaceSize = 1.0f / float(dstFaceSize);

		std::uint8_t* dstFaceData = (std::uint8_t*)dst.data() + face * dstFaceDataSize;

		for (std::uint32_t yy = 0; yy < dstFaceSize; ++yy)
		{
			std::uint8_t* dstRowData = (std::uint8_t*)dstFaceData + yy * dstPitch;

			for (std::uint32_t xx = 0; xx < dstFaceSize; ++xx)
			{
				const float uu = xx * invDstFaceSize * 2.0f - 1.0f;
				const float vv = yy * invDstFaceSize * 2.0f - 1.0f;

				// Get cubemap vector (x,y,z) from (u,v,faceIdx).
				float vec[3];
				texelCoordToVec(vec, uu, vv, face);

				// Convert cubemap vector (x,y,z) to latlong (u,v).
				float xSrcf;
				float ySrcf;
				latLongFromVec(xSrcf, ySrcf, vec);

				// Convert from [0..1] to [0..(size-1)] range.
				xSrcf *= srcWidthMinusOne;
				ySrcf *= srcHeightMinusOne;

				const std::uint32_t xSrc = static_cast<std::uint32_t>(xSrcf);
				const std::uint32_t ySrc = static_cast<std::uint32_t>(ySrcf);
				const float* data = (const float*)((const uint8_t*)src.data() + ySrc*srcPitch + xSrc*srcBytesPerPixel);

				float* dstColumnData = (float*)(dstRowData + xx * dstBytesPerPixel);
				dstColumnData[0] = data[0];
				dstColumnData[1] = data[1];
				dstColumnData[2] = data[2];
				if (dstChannel == 4)
					dstColumnData[3] = 1.0f;
			}
		}
	}

	template<std::uint8_t face>
	void makeCubefaceFromLatLongBilinearInterpolation(Image& dst, const Image& src)
	{
		assert(isLatLong(src));
		assert(src.channel() == 3 || src.channel() == 4);
		assert(src.value_type() == image::value_t::Float && src.type_size() == 4);

		const std::uint32_t dstChannel = dst.channel();
		const std::uint32_t dstBytesPerPixel = dstChannel * dst.type_size();
		const std::uint32_t dstFaceSize = (src.height() + 1) / 2;
		const std::uint32_t dstPitch = dstFaceSize * dstBytesPerPixel;
		const std::uint32_t dstFaceDataSize = dstPitch * dstFaceSize;

		const std::uint32_t srcBytesPerPixel = src.channel() * src.type_size();
		const std::uint32_t srcPitch = src.width() * srcBytesPerPixel;

		const float srcWidthMinusOne = float(std::int32_t(src.width() - 1));
		const float srcHeightMinusOne = float(std::int32_t(src.height() - 1));

		const float invDstFaceSize = 1.0f / float(dstFaceSize);

		std::uint8_t* dstFaceData = (std::uint8_t*)dst.data() + face * dstFaceDataSize;

		for (std::uint32_t yy = 0; yy < dstFaceSize; ++yy)
		{
			std::uint8_t* dstRowData = (std::uint8_t*)dstFaceData + yy * dstPitch;

			for (std::uint32_t xx = 0; xx < dstFaceSize; ++xx)
			{
				const float uu = xx * invDstFaceSize * 2.0f - 1.0f;
				const float vv = yy * invDstFaceSize * 2.0f - 1.0f;

				float vec[3];
				texelCoordToVec(vec, uu, vv, face);

				float xSrcf;
				float ySrcf;
				latLongFromVec(xSrcf, ySrcf, vec);

				xSrcf *= srcWidthMinusOne;
				ySrcf *= srcHeightMinusOne;

				const uint32_t x0 = static_cast<std::uint32_t>(xSrcf);
				const uint32_t y0 = static_cast<std::uint32_t>(ySrcf);
				const uint32_t x1 = std::min(x0 + 1, src.width() - 1);
				const uint32_t y1 = std::min(y0 + 1, src.height() - 1);

				const float* src0 = (const float*)((const uint8_t*)src.data() + y0*srcPitch + x0*srcBytesPerPixel);
				const float* src1 = (const float*)((const uint8_t*)src.data() + y0*srcPitch + x1*srcBytesPerPixel);
				const float* src2 = (const float*)((const uint8_t*)src.data() + y1*srcPitch + x0*srcBytesPerPixel);
				const float* src3 = (const float*)((const uint8_t*)src.data() + y1*srcPitch + x1*srcBytesPerPixel);

				const float tx = xSrcf - float(int32_t(x0));
				const float ty = ySrcf - float(int32_t(y0));
				const float invTx = 1.0f - tx;
				const float invTy = 1.0f - ty;

				float p0[3];
				float p1[3];
				float p2[3];
				float p3[3];
				vec3Mul(p0, src0, invTx*invTy);
				vec3Mul(p1, src1, tx*invTy);
				vec3Mul(p2, src2, invTx*   ty);
				vec3Mul(p3, src3, tx*   ty);

				const float rr = p0[0] + p1[0] + p2[0] + p3[0];
				const float gg = p0[1] + p1[1] + p2[1] + p3[1];
				const float bb = p0[2] + p1[2] + p2[2] + p3[2];

				float* dstColumnData = (float*)((std::uint8_t*)dstRowData + xx * dstBytesPerPixel);
				dstColumnData[0] = rr;
				dstColumnData[1] = gg;
				dstColumnData[2] = bb;
				if (dstChannel == 4)
					dstColumnData[3] = 1.0f;
			}
		}
	}

	bool makeCubemapFromLatLong(Image& dst, const Image& src, bool _useBilinearInterpolation)
	{
		assert(isLatLong(src));
		assert(src.value_type() == image::value_t::Float && src.type_size() == 4);

		const std::uint32_t dstFaceSize = (src.height() + 1) / 2;
		
		if (!dst.create(dstFaceSize, dstFaceSize, 6, image::format_t::R32G32B32SFloat))
			return false;

		try
		{
			if (_useBilinearInterpolation)
			{
				makeCubefaceFromLatLongBilinearInterpolation<0>(dst, src);
				makeCubefaceFromLatLongBilinearInterpolation<1>(dst, src);
				makeCubefaceFromLatLongBilinearInterpolation<2>(dst, src);
				makeCubefaceFromLatLongBilinearInterpolation<3>(dst, src);
				makeCubefaceFromLatLongBilinearInterpolation<4>(dst, src);
				makeCubefaceFromLatLongBilinearInterpolation<5>(dst, src);
			}
			else
			{
				makeCubefaceFromLatLong<0>(dst, src);
				makeCubefaceFromLatLong<1>(dst, src);
				makeCubefaceFromLatLong<2>(dst, src);
				makeCubefaceFromLatLong<3>(dst, src);
				makeCubefaceFromLatLong<4>(dst, src);
				makeCubefaceFromLatLong<5>(dst, src);
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool makeLatLongFromCubemap(Image& dst, const Image& src, bool _useBilinearInterpolation)
	{
		assert(isCubemap(src));

		const std::uint32_t srcChannels = src.channel();
		const std::uint32_t srcBytesPerPixel = srcChannels * src.type_size();
		const std::uint32_t srcFaceSize = src.width();
		const std::uint32_t srcPitch = srcFaceSize * srcBytesPerPixel;
		const std::uint32_t srcSizeMinOne = srcFaceSize - 1;

		if (!dst.create(srcFaceSize * 4, srcFaceSize * 2, 1, image::format_t::R32G32B32SFloat))
			return false;

		const std::uint32_t dstChannel = dst.channel();
		const std::uint32_t dstWidth = dst.width();
		const std::uint32_t dstHeight = dst.height();
		const std::uint32_t dstBytesPerPixel = dstChannel * dst.type_size();
		const std::uint32_t dstPitch = dstWidth * dstBytesPerPixel;

		const float invDstWidthf = 1.0f / float(dstWidth - 1);
		const float invDstHeightf = 1.0f / float(dstHeight - 1);

		try
		{
			if (_useBilinearInterpolation)
			{
				for (std::uint32_t yy = 0; yy < dstHeight; ++yy)
				{
					std::uint8_t* dstRowData = (std::uint8_t*)dst.data() + yy * dstPitch;

					for (std::uint32_t xx = 0; xx < dstWidth; ++xx)
					{
						const float xDst = static_cast<float>(xx) * invDstWidthf;
						const float yDst = static_cast<float>(yy) * invDstHeightf;

						float vec[3];
						vecFromLatLong(vec, xDst, yDst);

						float xSrcf;
						float ySrcf;
						std::uint8_t faceIdx;
						vecToTexelCoord(xSrcf, ySrcf, faceIdx, vec);

						xSrcf *= srcSizeMinOne;
						ySrcf *= srcSizeMinOne;

						const std::uint32_t x0 = static_cast<std::uint32_t>(xSrcf);
						const std::uint32_t y0 = static_cast<std::uint32_t>(ySrcf);
						const std::uint32_t x1 = std::min(x0 + 1, srcSizeMinOne);
						const std::uint32_t y1 = std::min(y0 + 1, srcSizeMinOne);

						const std::uint8_t* srcFaceData = (const std::uint8_t*)src.data() + faceIdx * srcPitch * srcFaceSize;
						const float *src0 = (const float*)((const std::uint8_t*)srcFaceData + y0 * srcPitch + x0 * srcBytesPerPixel);
						const float *src1 = (const float*)((const std::uint8_t*)srcFaceData + y0 * srcPitch + x1 * srcBytesPerPixel);
						const float *src2 = (const float*)((const std::uint8_t*)srcFaceData + y1 * srcPitch + x0 * srcBytesPerPixel);
						const float *src3 = (const float*)((const std::uint8_t*)srcFaceData + y1 * srcPitch + x1 * srcBytesPerPixel);

						const float tx = xSrcf - float(std::int32_t(x0));
						const float ty = ySrcf - float(std::int32_t(y0));
						const float invTx = 1.0f - tx;
						const float invTy = 1.0f - ty;

						float p0[3];
						float p1[3];
						float p2[3];
						float p3[3];
						vec3Mul(p0, src0, invTx*invTy);
						vec3Mul(p1, src1, tx*invTy);
						vec3Mul(p2, src2, invTx*   ty);
						vec3Mul(p3, src3, tx*   ty);

						const float rr = p0[0] + p1[0] + p2[0] + p3[0];
						const float gg = p0[1] + p1[1] + p2[1] + p3[1];
						const float bb = p0[2] + p1[2] + p2[2] + p3[2];

						float* dstColumnData = (float*)((std::uint8_t*)dstRowData + xx*dstBytesPerPixel);
						dstColumnData[0] = rr;
						dstColumnData[1] = gg;
						dstColumnData[2] = bb;

						if (dstChannel == 4)
							dstColumnData[3] = 1.0f;
					}
				}
			}
			else
			{
				for (std::uint32_t yy = 0; yy < dstHeight; ++yy)
				{
					std::uint8_t* dstRowData = (std::uint8_t*)dst.data() + yy * dstPitch;

					for (std::uint32_t xx = 0; xx < dstWidth; ++xx)
					{
						const float xDst = static_cast<float>(xx) * invDstWidthf;
						const float yDst = static_cast<float>(yy) * invDstHeightf;

						float vec[3];
						vecFromLatLong(vec, xDst, yDst);

						float xSrcf;
						float ySrcf;
						std::uint8_t faceIdx;
						vecToTexelCoord(xSrcf, ySrcf, faceIdx, vec);

						const std::uint32_t xSrc = static_cast<std::uint32_t>(xSrcf * srcSizeMinOne);
						const std::uint32_t ySrc = static_cast<std::uint32_t>(ySrcf * srcSizeMinOne);

						const float *data = (const float*)((const std::uint8_t*)src.data() + faceIdx * srcPitch * srcFaceSize + ySrc * srcPitch + xSrc * srcBytesPerPixel);

						float* dstColumnData = (float*)((std::uint8_t*)dstRowData + xx*dstBytesPerPixel);
						dstColumnData[0] = data[0];
						dstColumnData[1] = data[1];
						dstColumnData[2] = data[2];

						if (dstChannel == 4)
							dstColumnData[3] = 1.0f;
					}
				}
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}

_NAME_END