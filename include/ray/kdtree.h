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
#ifndef _H_KDTREE_H_
#define _H_KDTREE_H_

#include <ray/def.h>

_NAME_BEGIN

#ifndef SQ
#	define SQ(x) ((x) * (x))
#endif

#ifndef EPSILON
#	define EPSILON (0.0001f)
#endif

template<typename _Ty>
struct KdimensionData
{
	_Ty data;
};

template<>
struct KdimensionData<void>
{
};

template<typename _Tx, typename _Ty = void>
struct KdimensionNode : public KdimensionData<_Ty>
{
	_Tx pos;
	std::uint8_t split;

	KdimensionNode<_Tx, _Ty>* parent;
	KdimensionNode<_Tx, _Ty>* left;
	KdimensionNode<_Tx, _Ty>* right;

	KdimensionNode() noexcept
		: left(nullptr)
		, right(nullptr)
	{
	}

	KdimensionNode(const _Tx& pt, std::uint8_t flag) noexcept
		: pos(pt)
		, split(flag)
		, left(nullptr)
		, right(nullptr)
	{
	}

	~KdimensionNode() noexcept
	{
		if (left)
			delete left;
		if (right)
			delete right;
	}

	bool isRoot() const noexcept
	{
		return parent == nullptr;
	}

	bool isLeft() const noexcept
	{
		return parent && parent->left == this;
	}

	bool isRight() const noexcept
	{
		return parent && parent->right == this;
	}
};

template<typename _Tx, typename _Ty = void>
class KdimensionNearest final
{
public:
	KdimensionNearest() noexcept
		: _distanceSqrt(0)
		, _item(nullptr)
	{
	}

	KdimensionNearest(KdimensionNode<_Tx, _Ty>* item, float distSqrt) noexcept
		: _item(item)
		, _distanceSqrt(distSqrt)
	{
	}

	~KdimensionNearest() noexcept
	{
	}

	void setKdimensionNode(KdimensionNode<_Tx, _Ty>* node) noexcept
	{
		_item = node;
	}

	KdimensionNode<_Tx, _Ty>* getKdimensionNode() noexcept
	{
		return _item;
	}

	void setDistanceSqrt(float distSq) noexcept
	{
		_distanceSqrt = distSq;
	}

	float getDistanceSqrt() const noexcept
	{
		return _distanceSqrt;
	}

private:
	float _distanceSqrt;
	KdimensionNode<_Tx, _Ty>* _item;
};

template<typename _Tx, typename _Ty = void>
class KdimensionNearestList final
{
public:
	typedef std::vector<KdimensionNearest<_Tx, _Ty>> KdimensionNearests;

public:
	KdimensionNearestList() noexcept
	{
	}

	~KdimensionNearestList() noexcept
	{
	}

	void clear() noexcept
	{
		_iter.clear();
	}

	KdimensionNearests& iter() noexcept
	{
		return _iter;
	}

	const KdimensionNearests& iter() const noexcept
	{
		return _iter;
	}

	void insert(KdimensionNode<_Tx, _Ty>* item, float distanceSqrt) noexcept
	{
		_iter.push_back(KdimensionNearest<_Tx, _Ty>(item, distanceSqrt));
	}

	void sort() noexcept
	{
		std::sort(_iter.begin(), _iter.end(),
			[](const KdimensionNearest<_Tx, _Ty>& lhs, const KdimensionNearest<_Tx, _Ty>& rhs)
		{
			return lhs.getDistanceSqrt() < rhs.getDistanceSqrt();
		}
		);
	}

private:
	KdimensionNearests _iter;
};

template<typename _Tx, typename _Ty>
class KdimensionTreeBase
{
public:
	KdimensionTreeBase() noexcept
		: _count(0)
		, _dimension(sizeof(_Tx) / sizeof(typename _Tx::value_type))
		, _root(nullptr)
	{
	}

	~KdimensionTreeBase() noexcept
	{
		this->clear();
	}

	void clear() noexcept
	{
		if (_root)
		{
			delete _root;
			_root = nullptr;
		}

		_count = 0;

		std::memset(&_min, 0, sizeof(_min));
		std::memset(&_max, 0, sizeof(_max));
	}

	bool empty() const noexcept
	{
		return _count == 0;
	}

	void insert(KdimensionNode<_Tx, _Ty>* node)
	{
		std::uint8_t split = 0;
		auto& root = _root;
		auto next = &root;
		auto parent = root;

		while (*next)
		{
			parent = *next;
			split = ((*next)->split + 1) % _dimension;
			if (node->pos[(*next)->split] < (*next)->pos[(*next)->split])
				next = &(*next)->left;
			else
				next = &(*next)->right;
		}

		if (!*next)
		{
			node->split = split;
			*next = node;

			for (std::size_t i = 0; i < _dimension; i++)
			{
				if (node->pos[i] < _min[i])
					_min[i] = node->pos[i];
				if (node->pos[i] > _max[i])
					_max[i] = node->pos[i];
			}

			_count++;
		}
	}

	void remove_if(const _Tx& pos, std::function<bool(KdimensionNode<_Tx, _Ty>* node)> compfunc) noexcept
	{
		std::uint8_t split = 0;

		auto& root = KdimensionTreeBase<_Tx, void>::_root;
		auto next = root;

		while (next)
		{
			if (next->pos == pos && compfunc(next))
			{
				if (next->isRoot())
					_root = nullptr;
				else if (next->isLeft())
					next->parent->left = nullptr;
				else
					next->parent->right = nullptr;

				if (next->left)
				{
					this->insert(next->left);
					next->left = nullptr;
				}

				if (next->right)
				{
					this->insert(next->right);
					next->right = nullptr;
				}

				delete next;
				return;
			}

			split = (next->split + 1) % KdimensionTreeBase<_Tx, void>::_dimension;

			if (pos[next->split] < next->pos[next->split])
				next = next->left;
			else
				next = next->right;
		}
	}

	std::size_t search(KdimensionNearest<_Tx, _Ty>& result, const _Tx& pos, float range) noexcept
	{
		if (_root)
		{
			_serachPoint = pos;
			_serachRange = range;
			result.setKdimensionNode(_root);
			result.setDistanceSqrt(distanceSqrt(_root->pos, pos));
			this->nearest(result, _root);
			return 1;
		}
		return 0;
	}

	std::size_t search(KdimensionNearest<_Tx, _Ty>& result, const _Tx& pos) noexcept
	{
		return this->search(result, pos, distanceSqrt(_min, _max));
	}

	std::size_t search(KdimensionNearestList<_Tx, _Ty>& result, const _Tx& pos, float range)
	{
		_serachPoint = pos;
		_serachRange = range;
		this->nearest(result, _root);
		return result.iter().size();
	}

private:
	float distanceSqrt(const _Tx& pos1, const _Tx& pos2)
	{
		float distSq = 0;
		for (std::size_t i = 0; i < _dimension; i++)
			distSq += SQ(pos1[i] - pos2[i]);
		return distSq;
	}

	float distanceSqrt(const _Tx& pos, const _Tx& min, const _Tx& max)
	{
		float result = 0;
		for (std::size_t i = 0; i < _dimension; i++)
		{
			if (pos[i] < min[i])
				result += SQ(min[i] - pos[i]);
			else if (pos[i] > max[i])
				result += SQ(max[i] - pos[i]);
		}
		return result;
	}

	void nearest(KdimensionNearest<_Tx, _Ty>& result, KdimensionNode<_Tx, _Ty>* node) noexcept
	{
		assert(node);
		auto split = _serachPoint[node->split] - node->pos[node->split];
		auto nearerSubtree = split <= 0 ? node->left : node->right;
		auto& nearerCoord = split <= 0 ? _max[node->split] : _min[node->split];
		auto fartherSubtree = split <= 0 ? node->right : node->left;
		auto& fartherCoord = split <= 0 ? _min[node->split] : _max[node->split];
		if (nearerSubtree)
		{
			split = nearerCoord;
			nearerCoord = node->pos[node->split];
			nearest(result, nearerSubtree);
			nearerCoord = split;
		}
		float distSq = distanceSqrt(node->pos, _serachPoint);
		if (distSq < result.getDistanceSqrt() && distSq < SQ(_serachRange))
		{
			result.setKdimensionNode(node);
			result.setDistanceSqrt(distSq);
		}
		if (fartherSubtree)
		{
			split = fartherCoord;
			fartherCoord = node->pos[node->split];
			distSq = distanceSqrt(_serachPoint, _min, _max);
			if (distSq < result.getDistanceSqrt() && distSq < SQ(_serachRange))
				nearest(result, fartherSubtree);
			fartherCoord = split;
		}
	}

	bool nearest(KdimensionNearestList<_Tx, _Ty>& result, KdimensionNode<_Tx, _Ty>* node)
	{
		if (!node)
			return false;

		float distSq = distanceSqrt(node->pos, _serachPoint);
		if (distSq <= SQ(_serachRange))
			result.insert(node, distSq);

		float split = _serachPoint[node->split] - node->pos[node->split];

		bool ret = nearest(result, split <= 0.0 ? node->left : node->right);
		if (ret >= 0 && fabs(split) < _serachRange)
			ret = nearest(result, split <= 0.0 ? node->right : node->left);
		return ret;
	}

public:
	_Tx _min;
	_Tx _max;

	_Tx _serachPoint;

	float _serachRange;
	std::size_t _count;
	std::uint8_t _dimension;

	KdimensionNode<_Tx, _Ty>* _root;
};

template<typename _Tx, typename _Ty = void>
class KdimensionTree final : public KdimensionTreeBase<_Tx, _Ty>
{
public:
	void insert(const _Tx& pos, _Ty& data) noexcept
	{
		auto node = new KdimensionNode<_Tx>(pos, 0);
		node->data = data;
		KdimensionTreeBase<_Tx, _Ty>::insert(node);
	}
};

template<typename _Tx>
class KdimensionTree<_Tx, void> final : public KdimensionTreeBase<_Tx, void>
{
public:

	void insert(const _Tx& pos) noexcept
	{
		auto node = new KdimensionNode<_Tx>(pos, 0);
		KdimensionTreeBase<_Tx, void>::insert(node);
	}
};

_NAME_END

#endif