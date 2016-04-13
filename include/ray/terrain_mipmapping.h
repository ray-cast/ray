// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_MIP_MAPPING_H_
#define _H_MIP_MAPPING_H_

#include <ray/terrain_height_map.h>
#include <ray/terrain_lod.h>

_NAME_BEGIN

class Mipmapping : public TerrainLod
{
public:
	enum
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
	};

	Mipmapping();
	~Mipmapping();

	bool init(HeightMap* T);

	void render();

	std::size_t buildTrianlges();

	void setFactor(float f) { factor_ = f; }

private:
	bool recursiveTessellate(const Quadnode& node, std::uint8_t depth);

	bool isDivide(const Quadnode& node, std::uint8_t depth);

	float initVariant(std::uint8_t level, std::uint32_t x, std::uint32_t y);
	void  renderNode(const Quadnode& node, std::uint8_t level);
	void  dividNode(const Quadnode& node, std::uint8_t level);
	void  disableNode(const Quadnode& node, std::uint8_t level);
	bool  split(const Quadnode& node, std::uint8_t level);

	float factor_;
	Binary bits_;
	std::size_t _size;
	std::size_t _adjct_act[4];
	std::size_t _buildTrianlges;

	Quadnode        root_;
	std::vector<Quadnode> cur_Nodes;
	std::vector<Quadnode> next_Nodes;

	std::vector<std::uint32_t> pVariant_; //½Úµã´Ö²Ú
};

_NAME_END

#endif //_H_MIP_MAPPING_H_