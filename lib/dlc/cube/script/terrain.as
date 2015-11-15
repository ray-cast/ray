vector<float> positions = {
    -1 ,-1 ,-1 ,-1 ,-1 ,+1 ,-1 ,+1 ,-1 ,-1 ,+1 ,+1,
    +1 ,-1 ,-1 ,+1 ,-1 ,+1 ,+1 ,+1 ,-1 ,+1 ,+1 ,+1,
    -1 ,+1 ,-1 ,-1 ,+1 ,+1 ,+1 ,+1 ,-1 ,+1 ,+1 ,+1,
    -1 ,-1 ,-1 ,-1 ,-1 ,+1 ,+1 ,-1 ,-1 ,+1 ,-1 ,+1,
    -1 ,-1 ,-1 ,-1 ,+1 ,-1 ,+1 ,-1 ,-1 ,+1 ,+1 ,-1,
    -1 ,-1 ,+1 ,-1 ,+1 ,+1 ,+1 ,-1 ,+1 ,+1 ,+1 ,+1
};
vector<float> normals = {
    -1, 0, 0,
    +1, 0, 0,
     0,+1, 0,
     0,-1, 0,
     0, 0,-1,
     0, 0,+1
};
vector<float> uvs = {
    0, 0, 1, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 1, 1, 0, 1,
    0, 1, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 1, 1,
    0, 0, 0, 1, 1, 0, 1, 1,
    1, 0, 1, 1, 0, 0, 0, 1
};
vector<int> indices = {
    0, 3, 2, 0, 1, 3,
    0, 3, 1, 0, 2, 3,
    0, 3, 2, 0, 1, 3,
    0, 3, 1, 0, 2, 3,
    0, 3, 2, 0, 1, 3,
    0, 3, 1, 0, 2, 3,
};

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
    int8 x;
    int8 y;
    int8 z;
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
    private vector<TerrainEntry> _data;

    int count { get const { return _count; } }
    int length { get const { return _data.size(); } }

    void create(int mask)
    {
        _count = 0;
        _mask = mask;
        _data.resize(mask + 1);
    }

    void destroy()
    {
        //_data.clear();
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

    void get(uint index, TerrainEntry& out entry) const
    {
        entry = _data[index];
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
        _count = map._count;
        _data.swap(map._data);
    }
};

class TerrainItem
{
    int instanceID;
};

interface TerrainObject
{
    bool create(TerrainChunk@ chunk);
    bool createObject(TerrainChunk@ chunk);

    TerrainObject@ clone() const;
};

class TerrainChunk
{
    private int _x;
    private int _y;
    private int _z;
    private int _size;

    private TerrainMap _map;
    private vector<TerrainObject@> _objects;

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
    int count { get const { return _map.count; } }
    int length { get const { return _map.length; }}

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

    void get(uint index, TerrainEntry& out entry) const
    {
        _map.get(index, entry);
    }

    void addTerrainObject(TerrainObject@ object)
    {
        _objects.push_back(object);
    }

    vector<TerrainObject@>@ getTerrainObjects()
    {
        return _objects;
    }

    void createWorld(Terrain@ terrain)
    {
        auto@ objects = terrain.getTerrainObjects();
        auto size = objects.size();
        for (uint i = 0; i < size; i++)
        {
            this.addTerrainObject(objects[i].clone());
        }

        for (uint i = 0; i < size; i++)
        {
            _objects[i].create(this);
            _objects[i].createObject(this);
        }
    }
};

class Terrain : IController
{
    private int _scale = 2;
    private int _size = 32;
    private vector<TerrainChunk@> _chunks;
    private vector<TerrainObject@> _objects;

    Terrain()
    {
    }

    void addTerrainObject(TerrainObject@ object)
    {
        _objects.push_back(object);
    }

    vector<TerrainObject@>@ getTerrainObjects()
    {
        return _objects;
    }

    int chunked(float x) const
    {
        return int(floor(x / _scale / _size));
    }

    void onActivate()
    {
        vector<TerrainEntry> vectors;

        _objects.push_back(TerrainGrass());

        float3 translate = self.getTranslate();

        auto x = chunked(translate.x);
        auto y = chunked(translate.y);
        auto z = chunked(translate.z);

        TerrainChunk chunk;
        chunk.create(-1, 0, 0, _size);
        chunk.createWorld(this);

        _chunks.push_back(chunk);

        for (int i = x - 1; i < x + 1; i++)
        {
            for (int j = z - 1; j < z + 1; j++)
            {
            }
        }
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

    private vector<float3> _vertices;
    private vector<float3> _normals;
    private vector<float2> _texcoords;
    private vector<uint> _faces;

    bool create(TerrainChunk@ chunk)
    {
        int size = chunk.size;
        int half = size >> 1;

        int offsetX = chunk.x * size;
        int offsetY = chunk.z * size;

        for (int x = 0; x < chunk.size; x++)
        {
            for (int z = 0; z < chunk.size; z++)
            {
                int dx = offsetX + x;
                int dz = offsetY + z;

                float f = simplex2( dx * 0.01,  dz * 0.01, 4, 0.5, 2);
                float g = simplex2(-dx * 0.01, -dz * 0.01, 2, 0.9, 2);

                int h = int(f * (g * size + half));
                for (int y = 0; y < h; y++)
                {
                    chunk.set(x, y, z, 1);
                }
            }
        }

        return true;
    }

    bool createObject(TerrainChunk@ chunk)
    {
        auto cube = find("grass");

        int size = chunk.length;
        int count = 0;
        int total = 0;

        for (int i = 0; i < size; i++)
        {
            TerrainEntry it;
            chunk.get(i, it);
            if (it.empty())
                continue;

            if (it.y <= 0)
                continue;

            VisiableFaces faces;
            if (this.visiable(chunk, it, faces) > 0)
            {
                int dx = it.x << 1;
                int dy = it.y << 1;
                int dz = it.z << 1;

                this.makeCube(faces, dx, dy, dz, 1);
            }
        }

        int translateX = chunk.x * chunk.size << 1;
        int translateY = chunk.y * chunk.size << 1;
        int translateZ = chunk.z * chunk.size << 1;

        _object = instantiate("grass");
        auto mesh = _object.getMeshFilter().getMesh();
        mesh.setFaceArray(_faces);
        mesh.setVertexArray(_vertices);
        mesh.setNormalArray(_normals);
        mesh.computeBoundingBox();

        _object.setName("chunk_" + p + "_" + q);
        _object.setTranslate(float3(translateX, translateY, translateZ));
        _object.setActive(true);
        return true;
    }

    int visiable(TerrainChunk@ chunk, const TerrainEntry& in it, VisiableFaces& out faces)
    {
        auto f1 = chunk.get(it.x - 1, it.y, it.z);
        auto f2 = chunk.get(it.x + 1, it.y, it.z);
        auto f3 = chunk.get(it.x, it.y - 1, it.z);
        auto f4 = chunk.get(it.x, it.y + 1, it.z);
        auto f5 = chunk.get(it.x, it.y, it.z - 1);
        auto f6 = chunk.get(it.x, it.y, it.z + 1);

        f1 = (f1 != it.instanceID) ? 1 : 0;
        f2 = (f2 != it.instanceID) ? 1 : 0;
        f3 = (f3 != it.instanceID) ? 1 : 0;
        f4 = (f4 != it.instanceID) ? 1 : 0;
        f5 = (f5 != it.instanceID) ? 1 : 0;
        f6 = (f6 != it.instanceID) ? 1 : 0;

        faces.left = f1;
        faces.right = f2;
        faces.bottom = f3;
        faces.top = f4;
        faces.front = f5;
        faces.back = f6;

        return f1 + f2 + f3 + f4 + f5 + f6;
    }

    void makeCube(const VisiableFaces& faces, float x, float y, float z, float scale)
    {
        float s = 0.0625;
        float a = 0 + 1 / 2048.0;
        float b = s - 1 / 2048.0;
        vector<int> visiable = { faces.left, faces.right, faces.top, faces.bottom, faces.front, faces.back };

        for (int i = 0; i < 6; i++) {
            if (visiable[i] == 0) {
                continue;
            }
            for (int j = 0; j < 6; j++) {
                int k = indices[i * 6 + j];

                float3 v;
                v.x = x + scale * positions[i * 12 + k * 3 + 0];
                v.y = y + scale * positions[i * 12 + k * 3 + 1];
                v.z = z + scale * positions[i * 12 + k * 3 + 2];

                float3 n;
                n.x = normals[i * 3 + 0];
                n.y = normals[i * 3 + 1];
                n.z = normals[i * 3 + 2];

                float2 uv;
                uv.x = (uvs[i * 8 + k * 2 + 0] > 0 ? b : a);
                uv.y = (uvs[i * 8 + k * 2 + 1] > 0 ? b : a);

                _vertices.push_back(v);
                _normals.push_back(n);
                _texcoords.push_back(uv);
                _faces.push_back(_vertices.size() -1);
            }
        }
    }

    TerrainObject@ clone() const
    {
        return TerrainGrass();
    }
};