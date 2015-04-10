int hash_int(int key)
{
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key^ (key >> 16);
    return key;
}

int hash(int x, int y, int z)
{
    x = hash_int(x);
    y = hash_int(y);
    z = hash_int(z);
    return x ^ y ^ z;
}

class TerrainEntry
{
    int x;
    int y;
    int z;
    int w;
};

class TerrainMap
{
    int _x, _y, _z;
    int _size;
    int _mask;
    array<TerrainEntry> _data;

    void create(int x, int y, int z, int mask)
    {
        _data.resize((mask + 1));
    }

    void free()
    {
        array<TerrainEntry> empty;
        _data = empty;
    }

    void copy(TerrainMap& map)
    {
        _x = map._x;
        _y = map._y;
        _z = map._z;
        _mask = map._mask;
        _data = map._data;
    }

    int set(int x, int y, int z, int w)
    {
        int index = hash(x, y, z) & _mask;
        x -= _x;
        y -= _y;
        z -= _z;

        int overwrite = 0;

        TerrainEntry entry = _data[index];
        while (entry.x != 0 && entry.y != 0 && entry.z != 0)
        {
            if (entry.x == x && entry.y == y && entry.z == z)
            {
                overwrite = 1;
            }

            index = (index + 1) & _mask;
            entry = _data[index];
        }

        if (overwrite != 0)
        {
            if (entry.w != w)
            {
                entry.w = w;
                return 1;
            }
        }
        else if (w !=0)
        {
            entry.x = x;
            entry.y = y;
            entry.z = z;
            entry.w = w;
            _size++;
            return 1;
        }

        return index;
    }

    int get(int x, int y, int z)
    {
        int index = hash(x, y, z) & _mask;
        x -= _x;
        y -= _y;
        z -= _z;
        if (x < 0 || x > 255) return 0;
        if (y < 0 || y > 255) return 0;
        if (z < 0 || z > 255) return 0;
        TerrainEntry entry = _data[index];
        while (entry.x != 0 && entry.y != 0 && entry.z != 0)
        {
            if (entry.x == x && entry.y == y && entry.z == z)
            {
                return entry.w;
            }
            index = (index + 1) & _mask;
            entry = _data[index];
        }
        return 0;
    }
};

class TerrainChunk
{
    int _q;
    int _p;
    int _size;

    TerrainMap _map;
    GameObject cube;
    GameObject group;

    TerrainChunk()
    {
        cube = find("cube");
    }

    int getX()
    {
        return _q;
    }

    int getY()
    {
        return _p;
    }

    void createClouds(int size, int p, int q)
    {
        int pad = 1;
        for (int dx = -pad; dx < size + pad; dx++)
        {
            for (int dz = -pad; dz < size + pad; dz++)
            {
                for (int y = 64; y < 72; y++)
                {
                    if (simplex3(x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        int flag = 1;
                        if (dx < 0 || dz < 0 ||
                            dx >= size || dz >= size)
                        {
                            flag = -1;
                        }
                    }
                }
            }
        }
    }

    void createWorld(int size, int p, int q)
    {
        int index = 0;
        int half = size / 2;

        CombineInstance combines(size * size * size);

        for (int x = 0; x < size; x++)
        {
            for (int z = 0; z < size; z++)
            {
                int dx = q * size + x;
                int dz = p * size + z;

                float f = simplex2( dx * 0.01,  dz * 0.01, 4, 0.5, 2);
                float g = simplex2(-dx * 0.01, -dz * 0.01, 2, 0.9, 2);

                int h = int(f * g * size + half);

                float4x4 transform;

                for (int y = h - 2; y < h; y++)
                {
                    combines[index].mesh = cube.mesh.mesh;
                    combines[index].transform = transform.makeTransform(float3(dx * 2, y * 2, dz * 2));
                    index++;
                }
            }
        }

        _q = q;
        _p = p;
        _size = size;

        group = cube.clone();
        group.name = "chunk_" + p + "_" + q;
        group.mesh.mesh.setCombieInstnace(combines);
        group.active = true;
    }

    int distance(int q, int p)
    {
        int dq = abs(_q - q);
        int dp = abs(_p - p);
        return max(dq, dp);
    }
};

TerrainChunk createWorld(int size, int q, int p)
{
    TerrainChunk chunk;
    chunk.createWorld(size, q, p);
    return chunk;
}

class Terrain : IController
{
    GameObject self;
    GameObject group;

    int size = 32;

    array<TerrainChunk> chunks;

    Terrain(GameObject& in ptr)
    {
        self = ptr;
    }

    void onActivate()
    {
        for (int i = -2; i < 2; i++)
        {
            for (int j = -2; j < 2; j++)
            {
                chunks.push_back(createWorld(size, i, j));
            }
        }
    }
}