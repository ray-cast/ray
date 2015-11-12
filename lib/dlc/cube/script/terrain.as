int hash_int(int key)
{
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
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
    int instanceID;

    TerrainEntry()
    {
        x = -1;
        y = -1;
        z = -1;
        instanceID = 0;
    }

    bool empty() const
    {
        return
            x == -1 ||
            y == -1 ||
            z == -1 ? true : false;
    }
};

class TerrainMap
{
    private int _mask;
    private int _count;
    private array<TerrainEntry> _data;

    int count { get const { return _count; } }
    const array<TerrainEntry>@ data { get const { return _data; } }

    void create(int mask)
    {
        _count = 0;
        _mask = mask;
        _data.resize(mask + 1);
    }

    void destroy()
    {
        array<TerrainEntry> empty;
        _data = empty;
    }

    void copy(TerrainMap& map)
    {
        _mask = map._mask;
        _data = map._data;
    }

    bool set(int x, int y, int z, int instanceID)
    {
        int index = hash(x, y, z) & _mask;

        int overwrite = 0;
        TerrainEntry entry = _data[index];
        while (!entry.empty())
        {
            if (entry.x == x && entry.y == y && entry.z == z)
            {
                overwrite = 1;
                break;
            }

            index = (index + 1) & _mask;
            entry = _data[index];
        }

        if (overwrite != 0)
        {
            if (entry.instanceID != instanceID)
            {
                entry.instanceID = instanceID;
                return true;
            }
        }
        else if (instanceID !=0)
        {
            _data[index].x = x;
            _data[index].y = y;
            _data[index].z = z;
            _data[index].instanceID = instanceID;
            _count++;
            if (_count * 2 > _mask)
            {
                grow();
            }
            return true;
        }

        return false;
    }

    int get(int x, int y, int z)
    {
        uint index = hash(x, y, z) & _mask;
        auto entry = _data[index];

        while (!entry.empty())
        {
            if (entry.x == x &&
                entry.y == y &&
                entry.z == z)
            {
                return entry.instanceID;
            }

            index = (index + 1) & _mask;
            entry = _data[index];
        }

        return 0;
    }

    void grow()
    {
        TerrainMap map;
        map.create(_mask << 1 | 1);

        int size = _data.size();
        for (auto i = 0; i < size; i++)
        {
            auto it = _data[i];
            if (!it.empty())
            {
                map.set(it.x, it.y, it.z, it.instanceID);
            }
        }

        _mask = map._mask;
        _data = map._data;
        _count = map._count;
    }
};

class TerrainItem
{
    int instanceID;
};

interface TerrainObject
{
    bool create(TerrainChunk& in chunk);

    TerrainObject@ clone() const;
};

class TerrainChunk
{
    private int _x;
    private int _y;
    private int _z;
    private int _size;

    private TerrainMap _map;

    TerrainChunk()
    {
    }

    ~TerrainChunk()
    {
    }

    int x { get const { return _x; } }
    int y { get const { return _y; } }
    int z { get const { return _z; } }
    int size { get const { return _size; } }

    const array<TerrainEntry>@ data { get const { return _map.data; } }

    int distance(int x, int y, int z)
    {
        int dx = abs(_x - x);
        int dy = abs(_y - y);
        int dz = abs(_z - z);
        return max(max(dx, dy), dz);
    }

    void create(int x, int y, int z, int size)
    {
        _x = x;
        _y = y;
        _z = z;
        _size = size;
        _map.create(0x7FFF);
    }

    bool set(int x, int y, int z, int instanceID)
    {
        return _map.set(x, y, z, instanceID);
    }

    int get(int x, int y, int z)
    {
        if (x < 0 || x >= _size) return 0;
        if (y < 0 || y >= _size) return 0;
        if (z < 0 || z >= _size) return 0;
        return _map.get(x, y, z);
    }

    void createWorld()
    {
    }
};

TerrainChunk createWorld(int size, int q, int p)
{
    TerrainChunk chunk;
    chunk.create(q, p, 0, size);
    chunk.createWorld();
    return chunk;
}

class Terrain : IController
{
    int size = 32;

    array<TerrainChunk> chunks;
    array<TerrainObject@> objects;

    Terrain()
    {
    }

    void addTerrainObject(TerrainObject@ object)
    {
        objects.insertLast(object);
    }

    array<TerrainObject@>@ getTerrainObjects()
    {
        return objects;
    }

    void onActivate()
    {
        objects.insertLast(TerrainGrass());

        chunks.push_back(createWorld(size, 5, 0));
        chunks.push_back(createWorld(size, 1, 0));
    }
};

class VisiableFaces
{
    int left;
    int right;
    int bottom;
    int top;
    int back;
    int front;
};

class TerrainGrass : TerrainObject
{
    private GameObject _object;

    TerrainGrass()
    {
    }

    ~TerrainGrass()
    {
    }

    bool create(TerrainChunk& in chunk)
    {
        int size = chunk.size;
        int half = size >> 1;

        int offsetX = chunk.x * size;
        int offsetY = chunk.y * size;

        for (int x = 0; x < chunk.size; x++)
        {
            for (int z = 0; z < chunk.size; z++)
            {
                int dx = offsetX + x;
                int dz = offsetY + z;

                float f = simplex2( dx * 0.01,  dz * 0.01, 4, 0.5, 2);
                float g = simplex2(-dx * 0.01, -dz * 0.01, 2, 0.9, 2);

                int h = int(f * g * chunk.size + half);

                float4x4 transform;

                for (int y = h - 2; y < h; y++)
                {
                    chunk.set(x, y, z, 1);
                }
            }
        }

        return true;
    }

    bool createObject(TerrainChunk& chunk)
    {
        _object = instantiate("grass");

        int size = chunk.data.length;
        CombineInstance combines(size);
        for (int i = 0; i < size; i++)
        {
            auto it = chunk.data[i];
            if (it.empty())
                continue;

            combines[i].setMesh(_object.getMeshFilter().getMesh());
            combines[i].setTransform(float3(it.x * 2, it.y * 2, it.z * 2));
        }

        int translateX = chunk.x * chunk.size << 1;
        int translateY = chunk.y * chunk.size << 1;
        int translateZ = chunk.z * chunk.size << 1;

        _object.setName("chunk_" + p + "_" + q);
        _object.getMeshFilter().setCombieInstnace(combines);
        _object.setActive(true);
        _object.setTranslate(float3(translateX, translateY, translateZ));

        return true;
    }

    TerrainObject@ clone() const
    {
        return TerrainGrass();
    }
};