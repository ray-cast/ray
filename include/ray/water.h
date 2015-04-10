#ifndef _H_WATER_H_
#define _H_WATER_H_
/*
#include <rayCore/state.h>
#include <rayCore/texture2d.h>
#include <rayResources/image.h>
#include <rayTerrain/height_map.h>
#include <rayPlugins/draw_frustum.h>

namespace ray
{
    class Water
    {
        enum
        {
            LEFT,
            RIGHT,
            TOP,
            BOTTOM,
        };

    public:
        Water(StateMachine* state);

        void init(const Vector3f& pos, int size);

        void dropStone(int x, int y, int stoneSize, float StoneWeight);

        void render(const Frustum& f);

        bool computeNormalMap();

        bool computeVertexMap();

        bool computeTexcoordMap();

        inline Vector2f getTexture(int x, int y)
        {
            assert(0 <= x && x < size_);
            assert(0 <= y && y < size_);

            return Vector2f((float)x / size_, (float)y / size_);
        }

        inline Vector3f getPos(int x, int y)
        {
            assert(0 <= x && x < size_);
            assert(0 <= y && y < size_);

            float posX = x * 100.0f;
            float posZ = y * 100.0f;

            return Vector3f(posX, currentRipple_(x, y), posZ);
        }

        inline Vector3f& getNormal(int x, int y)
        {
            assert(0 <= x && x < size_);
            assert(0 <= y && y < size_);

            return pNormalTable_[x + y* size_];
        }

    private:
        void rippleSpread();

        float ripple(float dist_to_waveCenter, float waveCenterAmplitude, float velocity, float time);

        Array2d<float, Array::BYTE, 1, BYTE> currentRipple_;
        Array2d<float, Array::BYTE, 1, BYTE> lastRipple_;
        Vector3f* pVertexTable_;
        Vector3f* pNormalTable_;
        Vector2f* pTexcoordTable_;
        const Frustum *pFrustum_;
        StateMachine *pMachine_;
        Vector3f position_;
        Texture2D texture_;
        int size_;
        HeightMap *map_;
    };
}*/

#endif