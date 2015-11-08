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
#ifndef _H_OCTREE_H_
#define _H_OCTREE_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

enum
{
	INSIDE_CROSS = -1,
	OUTSIDE_OFFSET = 2
};

enum
{
	OUTSIDE_POS_X,
	OUTSIDE_NEG_X,
	OUTSIDE_POS_Y,
	OUTSIDE_NEG_Y,
	OUTSIDE_POS_Z,
	OUTSIDE_NEG_Z
};

enum
{
	FLAG_POS_X = 1 << (OUTSIDE_POS_X + 1),
	FLAG_NEG_X = 1 << (OUTSIDE_NEG_X + 1),
	FLAG_POS_Y = 1 << (OUTSIDE_POS_Y + 1),
	FLAG_NEG_Y = 1 << (OUTSIDE_NEG_Y + 1),
	FLAG_POS_Z = 1 << (OUTSIDE_POS_Z + 1),
	FLAG_NEG_Z = 1 << (OUTSIDE_NEG_Z + 1)
};

enum OCTREENODETYPE
{
	OCTREE_NODE_BRANCH,
	OCTREE_NODE_LEAF,
};

template<typename T>
class Octree;

template<typename T>
class OctreeNode
{
public:
	friend class Octree<T>;

public:
	explicit OctreeNode(OCTREENODETYPE type);

	OCTREENODETYPE type() const { return _type; }

	T& value();
	const T& value() const;

	void setValue(const T& t);

	OctreeNode& operator = (const T& t);

	virtual OctreeNode<T>* clone() const = 0;

protected:
	virtual ~OctreeNode();

private:
	OCTREENODETYPE _type;

	T _value;
};

template<typename T>
class OctreeBranch : public OctreeNode<T>
{
public:
	typedef int int_type;
	typedef OctreeNode<T> _Mynode;

public:
	OctreeBranch();
	explicit OctreeBranch(const OctreeBranch<T>& branch);

	const OctreeNode<T>* at(int_type x, int_type y, int_type z) const;
	const OctreeNode<T>* at(int_type index) const;

	OctreeNode<T>*& at(int_type x, int_type y, int_type z);
	OctreeNode<T>*& at(int_type index);

	const OctreeNode<T>** pointer(int_type x, int_type y, int_type z) const;
	const OctreeNode<T>** pointer(int_type index) const;

	OctreeNode<T>** pointer(int_type x, int_type y, int_type z);
	OctreeNode<T>** pointer(int_type index);

	const OctreeNode<T>** next(int_type size, int_type x, int_type y, int_type z) const;
	OctreeNode<T>** next(int_type size, int_type x, int_type y, int_type z);

	OctreeNode<T>* clone() const;

private:
	~OctreeBranch() noexcept;

	std::vector<T> _value;

	OctreeNode<T>* _children[2][2][2];
};

template<typename T>
class OctreeLeaf : public OctreeNode<T>
{
public:
	explicit OctreeLeaf();

	OctreeNode<T>* clone() const;

private:
	~OctreeLeaf() noexcept {};
};

template<typename _Ty>
class Octree
{
public:
	friend class OctreeBranch<_Ty>;
	typedef Octree<_Ty> _Myt;
	typedef OctreeNode<_Ty> _Mynode;

	typedef int int_type;

public:
	Octree();
	explicit Octree(int_type size, const _Ty& empty = _Ty(0));
	explicit Octree(_Mynode* node);

	virtual ~Octree() noexcept;

	_Mynode& at(int_type x, int_type y, int_type z);
	const _Mynode& at(int_type x, int_type y, int_type z) const;

	_Mynode* root();
	const _Mynode* root() const;

	void setRoot(_Mynode* root);

	void erase(int_type x, int_type y, int_type z);

	std::size_t nodes() const;

	template<typename Function>
	void recursive(_Mynode* node, Function call);

	template<typename Function>
	void recursive(const _Mynode* node, Function call) const;

	std::size_t size() const;

	void swap(_Myt& other);

	_Mynode& operator() (int_type x, int_type y, int_type z);
	const _Mynode& operator() (int_type x, int_type y, int_type z) const;

	template<typename _IStream>
	bool load(_IStream& in);

	template<typename _OStream>
	bool save(_OStream& out);

private:

	template<typename _IStream>
	void loadRecursive(_IStream& in, _Mynode** node);

	template<typename _OStream>
	void saveRecursive(_OStream& in, _Mynode** node);

	void eraseRecursive(_Mynode** node, int_type size, int_type x, int_type y, int_type z);

	static void deleteNode(_Mynode** node);

	std::size_t _node_count;

	std::vector<_Ty*> _objects;
	std::vector<_Ty*> _objects_deferred;

	_Mynode* _root;

	bool _deferred;

	int_type _size;

	_Ty _empty_value;
};

#include <ray/octree.inl>

_NAME_END

#endif