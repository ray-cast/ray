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

// +----------------------------------------------------------------------
#ifndef _H_OCTREE_INL_
#define _H_OCTREE_INL_

template<typename T>
OctreeNode<T>::OctreeNode(OCTREENODETYPE type)
    :_type(type)
{
}

template<typename T>
OctreeNode<T>::~OctreeNode()
{
}

template<typename T> const T&
OctreeNode<T>::value() const
{
    return _value;
}

template<typename T> T&
OctreeNode<T>::value()
{
    return _value;
}

template<typename T> void
OctreeNode<T>::setValue(const T& t)
{
    _value = t;
}

template<typename T> OctreeNode<T>&
OctreeNode<T>::operator = (const T& t)
{
    this->setValue(t);
    return *this;
}

template<typename T>
OctreeBranch<T>::OctreeBranch()
    :OctreeNode<T>(OCTREE_NODE_BRANCH)
{
    std::memset(_children, 0, sizeof(_children));
}

template<typename T>
OctreeBranch<T>::OctreeBranch(const OctreeBranch<T>& branch)
    :OctreeNode<T>(OCTREE_NODE_BRANCH)
{
    for (int_type i = 0; i < 8; ++i)
    {
        const _Mynode* node = branch.at(i);
        if (node)
            this->at(i) = const_cast<_Mynode*>(node->clone());
        else
            this->at(i) = nullptr;
    }
}

template<typename T>
OctreeBranch<T>::~OctreeBranch() noexcept
{
    Octree<T>::deleteNode(&_children[0][0][0]);
    Octree<T>::deleteNode(&_children[0][0][1]);
    Octree<T>::deleteNode(&_children[0][1][0]);
    Octree<T>::deleteNode(&_children[0][1][1]);
    Octree<T>::deleteNode(&_children[1][0][0]);
    Octree<T>::deleteNode(&_children[1][0][1]);
    Octree<T>::deleteNode(&_children[1][1][0]);
    Octree<T>::deleteNode(&_children[1][1][1]);
}

template<typename T> const OctreeNode<T>*
OctreeBranch<T>::at(int_type x, int_type y, int_type z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);

    return _children[z][y][x];
}

template<typename T> OctreeNode<T>*&
OctreeBranch<T>::at(int_type x, int_type y, int_type z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);

    return _children[z][y][x];
}

template<typename T> const OctreeNode<T>*
OctreeBranch<T>::at(int_type index) const
{
    assert(index >= 0 && index < 8);

    return *(&_children[0][0][0] + index);
}

template<typename T> OctreeNode<T>*&
OctreeBranch<T>::at(int_type index)
{
    assert(index >= 0 && index < 8);

    return *(&_children[0][0][0] + index);
}

template<typename T> const OctreeNode<T>**
OctreeBranch<T>::pointer(int_type x, int_type y, int_type z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);

    return &_children[z][y][x];
}

template<typename T> OctreeNode<T>**
OctreeBranch<T>::pointer(int_type x, int_type y, int_type z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);

    return &_children[z][y][x];
}

template<typename T> const OctreeNode<T>**
OctreeBranch<T>::pointer(int_type index) const
{
    assert(index >= 0 && index < 8);

    return &_children[0][0][0] + index;
}

template<typename T> OctreeNode<T>**
OctreeBranch<T>::pointer(int_type index)
{
    assert(index >= 0 && index < 8);

    return &_children[0][0][0] + index;
}

template<typename T> const OctreeNode<T>**
OctreeBranch<T>::next(int_type size, int_type x, int_type y, int_type z) const
{
    int_type xx = !!(x & size);
    int_type yy = !!(y & size);
    int_type zz = !!(z & size);

    return this->pointer(xx, yy, zz);
}

template<typename T> OctreeNode<T>**
OctreeBranch<T>::next(int_type size, int_type x, int_type y, int_type z)
{
    int_type xx = !!(x & size);
    int_type yy = !!(y & size);
    int_type zz = !!(z & size);

    return this->pointer(xx, yy, zz);
}

template<typename T> OctreeNode<T>*
OctreeBranch<T>::clone() const
{
    return new OctreeBranch<T>(*this);
}

template<typename T>
OctreeLeaf<T>::OctreeLeaf()
    :OctreeNode<T>(OCTREE_NODE_LEAF)
{
}

template<typename T> OctreeNode<T>*
OctreeLeaf<T>::clone() const
{
    return new OctreeLeaf<T>(*this);
}

template<typename T>
Octree<T>::Octree()
    : _root(nullptr)
    , _size(0)
{
}

template<typename T>
Octree<T>::Octree(int_type size, const T& empty)
    : _root(nullptr)
    , _size(size)
    , _empty_value(empty)
{
    assert(((size - 1 & size) & size) == 0);
}

template<typename T>
Octree<T>::Octree(OctreeNode<T>* node)
    :_deferred(true)
{
    _root = node;
}

template<typename T>
Octree<T>::~Octree() noexcept
{
    this->deleteNode(&_root);
}

template<typename T> void
Octree<T>::setRoot(OctreeNode<T>* root)
{
    assert(root);

    _root = root;
}

template<typename T> void
Octree<T>::erase(int_type x, int_type y, int_type z)
{
    assert(x >= 0 && x < (int)_size);
    assert(y >= 0 && y < (int)_size);
    assert(z >= 0 && z < (int)_size);

    this->eraseRecursive(&_root, _size, x, y, z);
}

template<typename T> void
Octree<T>::eraseRecursive(OctreeNode<T>** node, int_type size, int_type x, int_type y, int_type z)
{
    assert(node);

    if (!*node) return;

    if (size != 1)
    {
        if ((*node)->type() == OCTREE_NODE_BRANCH)
        {
            size = size >> 1;

            OctreeBranch<T>* branch = dynamic_cast<OctreeBranch<T>*>(*node);

            this->eraseRecursive(branch->next(size, x, y, z), size, x, y, z);

            if (branch->at(0)) return;
            if (branch->at(1)) return;
            if (branch->at(2)) return;
            if (branch->at(3)) return;
            if (branch->at(4)) return;
            if (branch->at(5)) return;
            if (branch->at(6)) return;
            if (branch->at(7)) return;

            this->deleteNode(node);
        }
        else if (dynamic_cast<OctreeLeaf<T>*>(*node)->value() == _empty_value)
        {
            this->deleteNode(node);
        }
        else
        {
            OctreeBranch<T>* branch = new OctreeBranch<T>;

            size = size >> 1;
            int_type index = (x & size ? 1 : 0) | (y & size ? 2 : 0) | (z & size ? 4 : 0);

            const T& value = dynamic_cast<OctreeLeaf<T>*>(*node)->value();

            try
            {
                for (int_type i = 0; i < 8; i++)
                {
                    if (i == index)
                        continue;

                    if (size == 1)
                        branch->at(i) = new OctreeLeaf<T>(value);
                    else
                        branch->at(i) = new OctreeBranch<T>(value);
                }

                this->deleteNode(node);

                *node = branch;
                node = branch->pointer(index);
            }
            catch (std::bad_alloc& e)
            {
                OctreeNode<T>* node = branch;
                this->deleteNode(&node);
                throw e;
            }
        }
    }
    else
    {
        --size;
        OctreeBranch<T>* aggregate = dynamic_cast<OctreeBranch<T>*>(*node);

        int_type xx = x & size;
        int_type yy = y & size;
        int_type zz = z & size;

        aggregate->setValue(xx, yy, zz, _empty_value);

        for (int_type i = 0; i < 1; ++i)
        {
            if (aggregate->value(i) != _empty_value)
                return;
        }

        this->deleteNode(node);
    }
}

template<typename T> OctreeNode<T>*
Octree<T>::root()
{
    return _root;
}

template<typename T> const OctreeNode<T>*
Octree<T>::root() const
{
    return _root;
}

template<typename T> const std::size_t
Octree<T>::size() const
{
    return _size;
}

template<typename T> void
Octree<T>::swap(_Myt& other)
{
    std::swap(_root, other._root);
    std::swap(_size, other._size);
}

template<typename T> OctreeNode<T>&
Octree<T>::at(int_type x, int_type y, int_type z)
{
    assert(x >= -_size * 0.5 && x < _size * 0.5);
    assert(y >= -_size * 0.5 && y < _size * 0.5);
    assert(z >= -_size * 0.5 && z < _size * 0.5);

    OctreeNode<T>** node = &_root;

    int_type size = _size;
    while (size != 1)
    {
        if (!*node)
        {
            *node = new OctreeBranch<T>;
        }
        else if ((*node)->type() == OCTREE_NODE_BRANCH)
        {
            size = size >> 1;

            node = static_cast<OctreeBranch<T>*>(*node)->next(size, x, y, z);
        }
        else
        {
            return **node;
        }
    }

    if (!*node)
        *node = new OctreeLeaf<T>;

    return **node;
}

template<typename T> const OctreeNode<T>&
Octree<T>::at(int_type x, int_type y, int_type z) const
{
    assert(x >= -_size * 0.5 && x < _size * 0.5);
    assert(y >= -_size * 0.5 && y < _size * 0.5);
    assert(z >= -_size * 0.5 && z < _size * 0.5);

    OctreeNode<T>** node = &_root;

    int_type size = _size;
    while (size != 1)
    {
        if (!*node)
        {
            *node = new OctreeBranch<T>;
        }
        else if ((*node)->type() == OCTREE_NODE_BRANCH)
        {
            size = size >> 1;

            node = static_cast<OctreeBranch<T>*>(*node)->next(size, x, y, z);
        }
        else
        {
            return **node;
        }
    }

    if (!*node)
        *node = new OctreeLeaf<T>;

    return **node;
}

template<typename T> std::size_t
Octree<T>::nodes() const
{
    std::size_t cnt = 0;

    auto eval = [&](const OctreeNode<T>*) -> bool
    {
        return (++cnt) ? true : false;
    };

    this->recursive(_root, eval);

    return cnt;
}

template<typename T> template<typename Function> void
Octree<T>::recursive(OctreeNode<T>* node, Function callback)
{
    if (!node)
        return;

    if (node->type() == OCTREE_NODE_BRANCH)
    {
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(0), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(1), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(2), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(3), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(4), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(5), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(6), callback);
        this->recursive(static_cast<OctreeBranch<T>*>(node)->at(7), callback);
    }
    else if (node->type() == OCTREE_NODE_LEAF)
    {
        callback(node);
    }
    else
    {
        assert(false);
    }
}

template<typename T> template<typename Function> void
Octree<T>::recursive(const OctreeNode<T>* node, Function callback) const
{
    if (!node)
        return;

    if (node->type() == OCTREE_NODE_BRANCH)
    {
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(0), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(1), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(2), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(3), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(4), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(5), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(6), callback);
        this->recursive(static_cast<const OctreeBranch<T>*>(node)->at(7), callback);
    }
    else if (node->type() == OCTREE_NODE_LEAF)
    {
        callback(node);
    }
    else
    {
        assert(false);
    }
}

template<typename T> OctreeNode<T>&
Octree<T>::operator() (int_type x, int_type y, int_type z)
{
    return this->at(x, y, z);
}

template<typename T> const OctreeNode<T>&
Octree<T>::operator() (int_type x, int_type y, int_type z) const
{
    return this->at(x, y, z);
}

template<typename T> void
Octree<T>::deleteNode(OctreeNode<T>** node)
{
    assert(node);
    if (*node)
    {
        delete *node;
        *node = 0;
    }
}

template<typename T>
template<typename _IStream> bool
Octree<T>::load(_IStream& in)
{
    Octree<T> tmp;

    char root;
    in.read(&root, 1);

    if (root)
        this->loadRecursive(in, &tmp._root);

    in.read(reinterpret_cast<char*>(&tmp._empty_value), sizeof(tmp._empty_value));
    in.read(reinterpret_cast<char*>(&tmp._size), sizeof(tmp._size));

    if (in.good())
    {
        this->swap(tmp);
        return true;
    }

    return false;
}

template<typename T>
template<typename _IStream> void
Octree<T>::loadRecursive(_IStream& in, OctreeNode<T>** node)
{
    assert(node);

    if (in.fail()) return;

    char type;
    in.read(&type, 1);

    switch (type)
    {
    case OCTREE_NODE_BRANCH:
    {
        OctreeBranch<T>* branch = new OctreeBranch<T>;
        *node = branch;

        char children;
        in.read(&children, 1);

        if (children & (1 << 0)) this->loadRecursive(in, &branch->at(0));
        if (children & (1 << 1)) this->loadRecursive(in, &branch->at(1));
        if (children & (1 << 2)) this->loadRecursive(in, &branch->at(2));
        if (children & (1 << 3)) this->loadRecursive(in, &branch->at(3));
        if (children & (1 << 4)) this->loadRecursive(in, &branch->at(4));
        if (children & (1 << 5)) this->loadRecursive(in, &branch->at(5));
        if (children & (1 << 6)) this->loadRecursive(in, &branch->at(6));
        if (children & (1 << 7)) this->loadRecursive(in, &branch->at(7));
    }
    break;
    case OCTREE_NODE_LEAF:
    {
        OctreeLeaf<T>* leaf = new OctreeLeaf<T>(T(0));
        *node = leaf;
        in.read(reinterpret_cast<char*>(&leaf->value()), sizeof(T));
    }
    break;
    default:

        break;
    }
}

template<typename T>
template<typename _OStream> bool
Octree<T>::save(_OStream& out)
{
    static const char one = 1;
    static const char zero = 0;

    if (!_root)
    {
        out.write(&zero, 1);
    }
    else
    {
        out.write(&one, 1);

        this->saveRecursive(out, this->root());
    }

    out.write(reinterpret_cast<const char*>(&_empty_value), sizeof(_empty_value));
    out.write(reinterpret_cast<const char*>(&_size), sizeof(_size));
}

template<typename T>
template<typename _OStream> void
Octree<T>::saveRecursive(_OStream& out, OctreeNode<T>** node)
{
    assert(node);

    if (out.fail()) return;

    char type = node->type();
    out.write(&type, 1);

    switch (type)
    {
    case OCTREE_NODE_BRANCH:
    {
        const OctreeBranch<T>* branch = static_cast<const OctreeBranch<T>*>(node);

        char children = 0;
        children |= (branch->at(0) != 0) << 0;
        children |= (branch->at(1) != 0) << 1;
        children |= (branch->at(2) != 0) << 2;
        children |= (branch->at(3) != 0) << 3;
        children |= (branch->at(4) != 0) << 4;
        children |= (branch->at(5) != 0) << 5;
        children |= (branch->at(6) != 0) << 6;
        children |= (branch->at(7) != 0) << 7;

        out.write(&children, 1);

        if (branch->at(0)) this->saveRecursive(out, branch->at(0));
        if (branch->at(1)) this->saveRecursive(out, branch->at(1));
        if (branch->at(2)) this->saveRecursive(out, branch->at(2));
        if (branch->at(3)) this->saveRecursive(out, branch->at(3));
        if (branch->at(4)) this->saveRecursive(out, branch->at(4));
        if (branch->at(5)) this->saveRecursive(out, branch->at(5));
        if (branch->at(6)) this->saveRecursive(out, branch->at(6));
        if (branch->at(7)) this->saveRecursive(out, branch->at(7));
    }
    break;
    case OCTREE_NODE_LEAF:
    {
        out.write(reinterpret_cast<const char*>(&reinterpret_cast<const OctreeLeaf<T>*>(node)->value()), sizeof(T));
    }
    break;
    default:
        break;
    }
}

#endif