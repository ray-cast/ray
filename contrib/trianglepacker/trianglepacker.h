/***********************************************************
* A single header file 3D->2D rect triangle packing lib    *
* https://github.com/ands/trianglepacker                   *
* no warranty implied | use at your own risk               *
* author: Andreas Mantler (ands) | last change: 31.08.2016 *
*                                                          *
* License:                                                 *
* This software is in the public domain.                   *
* Where that dedication is not recognized,                 *
* you are granted a perpetual, irrevocable license to copy *
* and modify this file however you want.                   *
***********************************************************/

#ifndef TRIANGLEPACKER_H
#define TRIANGLEPACKER_H

#ifndef TP_CALLOC
#define TP_CALLOC(count, size) calloc(count, size)
#endif

#ifndef TP_FREE
#define TP_FREE(ptr) free(ptr)
#endif

typedef int tp_bool;
#define TP_FALSE 0
#define TP_TRUE  1

// API

// returns TP_FALSE if triangles do not fit into the rect with the specified size, border and spacing
tp_bool tpPackIntoRect(
	const float *positions, int vertexCount,
	int width, int height, int border, int spacing,
	float *outUVs,
	float *outScale3Dto2D);

// returns number of successfully packed vertices
int tpPackWithFixedScaleIntoRect(
	const float *positions, int vertexCount, float scale3Dto2D,
	int width, int height, int border, int spacing,
	float *outUVs);

#endif // TRIANGLEPACKER_H
////////////////////// END OF HEADER //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TRIANGLEPACKER_IMPLEMENTATION
#undef TRIANGLEPACKER_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define TP_SWAP(type, a, b) { type tmp = (a); (a) = (b); (b) = tmp; }

#if defined(_MSC_VER) && !defined(__cplusplus) // TODO: specific versions only?
#define inline __inline
#endif

static inline int      tp_mini      (int     a, int     b) { return a < b ? a : b; }
static inline int      tp_maxi      (int     a, int     b) { return a > b ? a : b; }
static inline int      tp_absi      (int     a           ) { return a < 0 ? -a : a; }

typedef struct tp_vec2 { float x, y; } tp_vec2;
static inline tp_vec2  tp_v2i       (int     x, int     y) { tp_vec2 v = { (float)x, (float)y }; return v; }
static inline tp_vec2  tp_v2        (float   x, float   y) { tp_vec2 v = { x, y }; return v; }
static inline tp_vec2  tp_mul2      (tp_vec2 a, tp_vec2 b) { return tp_v2(a.x * b.x, a.y * b.y); }

typedef struct tp_vec3 { float x, y, z; } tp_vec3;
static inline tp_vec3  tp_v3        (float   x, float   y, float   z) { tp_vec3 v = { x, y, z }; return v; }
static inline tp_vec3  tp_add3      (tp_vec3 a, tp_vec3 b) { return tp_v3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline tp_vec3  tp_sub3      (tp_vec3 a, tp_vec3 b) { return tp_v3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline tp_vec3  tp_scale3    (tp_vec3 a, float   b) { return tp_v3(a.x * b, a.y * b, a.z * b); }
static inline tp_vec3  tp_div3      (tp_vec3 a, float   b) { return tp_scale3(a, 1.0f / b); }
static inline float    tp_dot3      (tp_vec3 a, tp_vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline float    tp_length3sq (tp_vec3 a           ) { return a.x * a.x + a.y * a.y + a.z * a.z; }
static inline float    tp_length3   (tp_vec3 a           ) { return sqrtf(tp_length3sq(a)); }
static inline tp_vec3  tp_normalize3(tp_vec3 a           ) { return tp_div3(a, tp_length3(a)); }

typedef struct
{
	int Aindex;
	short w, x, h, hflip;
	//       C           -
	//     * |  *        | h
	//   *   |     *     |
	// B-----+--------A  -
	// '--x--'        |
	// '-------w------'
} tp_triangle;

static int tp_triangle_cmp(const void *a, const void *b)
{
	tp_triangle *ea = (tp_triangle*)a;
	tp_triangle *eb = (tp_triangle*)b;
	int dh = eb->h - ea->h;
	return dh != 0 ? dh : (eb->w - ea->w);
}

#ifdef TP_DEBUG_OUTPUT
static void tp_line(unsigned char *data, int w, int h,
                    int x0, int y0, int x1, int y1,
                    unsigned char r, unsigned char g, unsigned char b)
{
	int dx = tp_absi(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = tp_absi(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = (dx > dy ? dx : -dy) / 2, e2;
	for(;;)
	{
		unsigned char *p = data + (y0 * w + x0) * 3;
		p[0] = r; p[1] = g; p[2] = b;

		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 <  dy) { err += dx; y0 += sy; }
	}
}

static tp_bool tp_save_bgr_tga(const char *filename, const unsigned char *image, int w, int h)
{
	unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, w & 0xff, (w >> 8) & 0xff, h & 0xff, (h >> 8) & 0xff, 24, 0 };
#if defined(_MSC_VER) && _MSC_VER >= 1400
	FILE *file;
	if (fopen_s(&file, filename, "wb") != 0) return TP_FALSE;
#else
	FILE *file = fopen(filename, "wb");
	if (!file) return TP_FALSE;
#endif
	fwrite(header, 1, sizeof(header), file);
	fwrite(image, 1, w * h * 3 , file);
	fclose(file);
	return TP_TRUE;
}
#endif

static void tp_wave_surge(int *wave, int right, int x0, int y0, int x1, int y1)
{
	int dx = tp_absi(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = tp_absi(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = (dx > dy ? dx : -dy) / 2, e2;
	for(;;)
	{
		if (right)
			wave[y0] = x0 > wave[y0] ? x0 : wave[y0];
		else
			wave[y0] = x0 < wave[y0] ? x0 : wave[y0];
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 <  dy) { err += dx; y0 += sy; }
	}
}

static int tp_wave_wash_up(int *wave, int right, int height, int y0, int x1, int y1, int spacing)
{
	int x0 = 0;
	int dx = tp_absi(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = tp_absi(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = (dx > dy ? dx : -dy) / 2, e2;
	int x = wave[y0];
	for(;;)
	{
		int xDistance = wave[y0] - x0 - x;
		for (int y = tp_maxi(y0 - spacing, 0); y <= tp_mini(y0 + spacing, height - 1); y++)
			xDistance = right ? tp_maxi(wave[y] - x0 - x, xDistance) : tp_mini(wave[y] - x0 - x, xDistance);
		if ((right && xDistance > 0) || (!right && xDistance < 0))
			x += xDistance;
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 <  dy) { err += dx; y0 += sy; }
	}
	return x;
}

int tpPackWithFixedScaleIntoRect(const float *positions, int vertexCount, float scale3Dto2D, int width, int height, int border, int spacing, float *outUVs)
{
	tp_triangle *tris = (tp_triangle*)TP_CALLOC(vertexCount / 3, sizeof(tp_triangle));
	tp_vec3 *p = (tp_vec3*)positions;
	tp_vec2 *uv = (tp_vec2*)outUVs;

	for (int i = 0; i < vertexCount / 3; i++)
	{
		tp_vec3 tp[3], tv[3];
		tp[0] = tp_scale3(p[i * 3 + 0], scale3Dto2D);
		tp[1] = tp_scale3(p[i * 3 + 1], scale3Dto2D);
		tp[2] = tp_scale3(p[i * 3 + 2], scale3Dto2D);
		tv[0] = tp_sub3(tp[1], tp[0]);
		tv[1] = tp_sub3(tp[2], tp[1]);
		tv[2] = tp_sub3(tp[0], tp[2]);
		float tvlsq[3] = { tp_length3sq(tv[0]), tp_length3sq(tv[1]), tp_length3sq(tv[2]) };

		// find long edge
		int maxi;        float maxl = tvlsq[0]; maxi = 0;
		if (tvlsq[1] > maxl) { maxl = tvlsq[1]; maxi = 1; }
		if (tvlsq[2] > maxl) { maxl = tvlsq[2]; maxi = 2; }
		int nexti = (maxi + 1) % 3;

		// measure triangle
		float w = sqrtf(maxl);
		float x = -tp_dot3(tv[maxi], tv[nexti]) / w;
		float h = tp_length3(tp_sub3(tp_add3(tv[maxi], tv[nexti]), tp_scale3(tp_normalize3(tv[maxi]), w - x)));

		// store entry
		tp_triangle *e = tris + i;
		e->Aindex = i * 3 + maxi;
		e->w = (int)ceilf(w);
		e->x = (int)ceilf(x);
		e->h = (int)ceilf(h);
		e->hflip = 0;
	}
	qsort(tris, vertexCount / 3, sizeof(tp_triangle), tp_triangle_cmp);

	tp_vec2 uvScale = tp_v2(1.0f / width, 1.0f / height);

#ifdef TP_DEBUG_OUTPUT
	unsigned char *data;
	if (uv)
		data = (unsigned char*)TP_CALLOC(width * height, 3);
#endif

	int processed;
	int *waves[2];
	waves[0] = (int*)TP_CALLOC(2 * height, sizeof(int));
	waves[1] = waves[0] + height;
	for (int i = 0; i < height; i++)
	{
		waves[0][i] = width - 1;// - border;
		waves[1][i] = border;
	}
	int pass = 0;

	int row_y = border;
	int row_h = tris[0].h;
	int vflip = 0;
	for (processed = 0; processed < vertexCount / 3; processed++)
	{
		tp_triangle *e = tris + processed;
		int ymin, ystart, yend, xmin[2], x, hflip;
retry:
		ymin = vflip ? row_y + row_h - e->h : row_y;
		ystart = vflip ? ymin + e->h : ymin;
		yend = vflip ? ymin : ymin + e->h;

		// calculate possible x values for the triangle in the current row
		hflip = processed & 1; // seems to work better than the heuristics below!?
		if (pass < 3) // left to right (first three passes)
		{
			xmin[0] = tp_wave_wash_up(waves[1], 1, height, ystart,        e->x, yend, spacing);
			xmin[1] = tp_wave_wash_up(waves[1], 1, height, ystart, e->w - e->x, yend, spacing); // flipped horizontally
			//hflip = (xmin[1] < xmin[0] || (xmin[1] == xmin[0] && e->x > e->w / 2)) ? 1 : 0;
		}
		else if (pass < 5) // right to left (last two passes)
		{ // TODO: fix spacing!
			xmin[0] = tp_wave_wash_up(waves[0], 0, height, ystart, -       e->x, yend, spacing) - e->w - 1;
			xmin[1] = tp_wave_wash_up(waves[0], 0, height, ystart, -e->w + e->x, yend, spacing) - e->w - 1; // flipped horizontally
			//hflip = (xmin[1] > xmin[0] || (xmin[1] == xmin[0] && e->x < e->w / 2)) ? 1 : 0;
		}
		else
			goto finish;
		
		// execute hflip (and choose best x)
		e->x = hflip ? e->w - e->x : e->x;
		e->hflip ^= hflip;
		x = xmin[hflip];
		
		// check if it fits into the specified rect
		// (else advance to next row or do another pass over the rect)
		if (x + e->w + border >= width || x < border)
		{
			row_y += row_h + spacing + 1; // next row
			row_h = e->h;
			if (row_y + row_h + border >= height)
			{
				++pass; // next pass
				row_y = border;
			}
			goto retry;
		}

		// found a space for the triangle. update waves
		tp_wave_surge(waves[0], 0, x        - spacing - 1, ystart, x + e->x - spacing - 1, yend); // left side
		tp_wave_surge(waves[1], 1, x + e->w + spacing + 1, ystart, x + e->x + spacing + 1, yend); // right side

		// calc & store UVs
		if (uv)
		{
#ifdef TP_DEBUG_OUTPUT
			tp_line(data, width, height, x       , ystart, x + e->w, ystart, 255, 255, 255);
			tp_line(data, width, height, x       , ystart, x + e->x, yend  , 255, 255, 255);
			tp_line(data, width, height, x + e->w, ystart, x + e->x, yend  , 255, 255, 255);
#endif
			int tri = e->Aindex - (e->Aindex % 3);
			int Ai = e->Aindex;
			int Bi = tri + ((e->Aindex + 1) % 3);
			int Ci = tri + ((e->Aindex + 2) % 3);
			if (e->hflip) TP_SWAP(int, Ai, Bi);
			uv[Ai] = tp_mul2(tp_v2i(x + e->w, ystart), uvScale);
			uv[Bi] = tp_mul2(tp_v2i(x       , ystart), uvScale);
			uv[Ci] = tp_mul2(tp_v2i(x + e->x, yend  ), uvScale);
		}
		vflip = !vflip;
	}

finish:
#ifdef TP_DEBUG_OUTPUT
	if (uv)
	{
		/*for (int i = 0; i < height; i++)
		{
			// left
			int x = waves[0][i];// +spacing;
			while (x >= 0)
				data[(i * width + x--) * 3 + 2] = 255;
			// right
			x = waves[1][i] - spacing;
			while (x < width)
				data[(i * width + x++) * 3] = 255;
		}*/
		if (tp_save_bgr_tga("debug_triangle_packing.tga", data, width, height))
			printf("Saved debug_triangle_packing.tga\n");
		TP_FREE(data);
	}
#endif

	TP_FREE(waves[0]);
	TP_FREE(tris);
	
	return processed * 3;
}

tp_bool tpPackIntoRect(const float *positions, int vertexCount, int width, int height, int border, int spacing, float *outUVs, float *outScale3Dto2D)
{
	float testScale = 1.0f;
	int processed = tpPackWithFixedScaleIntoRect(positions, vertexCount, testScale, width, height, border, spacing, 0);
	int increase = processed < vertexCount ? 0 : 1;
	float lastFitScale = 0.0f;
	float multiplicator = 0.5f;

	if (increase)
	{
		while (!(processed < vertexCount))
		{
			testScale *= 2.0f;
			//printf("inc testing scale %f\n", testScale);
			processed = tpPackWithFixedScaleIntoRect(positions, vertexCount, testScale, width, height, border, spacing, 0);
		}
		lastFitScale = testScale / 2.0f;
		//printf("inc scale %f fits\n", lastFitScale);
		multiplicator = 0.75f;
	}

	for (int j = 0; j < 16; j++)
	{
		//printf("dec multiplicator %f\n", multiplicator);
		for (int i = 0; processed < vertexCount && i < 2; i++)
		{
			testScale *= multiplicator;
			//printf("dec testing scale %f\n", testScale);
			processed = tpPackWithFixedScaleIntoRect(positions, vertexCount, testScale, width, height, border, spacing, 0);
		}
		if (!(processed < vertexCount))
		{
			processed = 0;
			//printf("scale %f fits\n", testScale);
			lastFitScale = testScale;
			testScale /= multiplicator;
			multiplicator = (multiplicator + 1.0f) * 0.5f;
		}
	}
	if (lastFitScale > 0.0f)
	{
		*outScale3Dto2D = lastFitScale;
		processed = tpPackWithFixedScaleIntoRect(positions, vertexCount, lastFitScale, width, height, border, spacing, outUVs);
		assert(processed == vertexCount);
		return TP_TRUE;
	}
	return TP_FALSE;
}

#endif // TRIANGLEPACKER_IMPLEMENTATION

