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
// | Author: Xiemoperor <Xiemoperor@gmail.com>
// +----------------------------------------------------------------------
#ifndef _H_RENDER_STATE_H_
#define _H_RENDER_STATE_H_

#include <ray/string.h>
#include <ray/gdicmn.h>
#include <ray/dccmn.h>
#include <ray/memory.h>
#include <vector>

_NAME_BEGIN

class StateAttribute final
{
public:
	static const int unknown = -1;

	enum statecap
	{
		alpharef,
		alphafunc,
		alphatestenable,
		blendenable,
		blendcolor,
		blendsrcfactor,
		blendsrcfactoralpha,
		blenddestfactor,
		blenddestfactoralpha,
		blendop,
		blendopalpha,
		blendequationrgb,
		blendequationalpha,
		blenddest,
		blendsrc,
		ccw_stencilfail,
		ccw_stencilzfail,
		ccw_stencilpass,
		ccw_stencilfunc,
		colorwriteenable,
		colorwriteenable1,
		colorwriteenable2,
		colorwriteenable3,
		cullmode,
		fillmode,
		indexedvertexblendenable,
		linesmooth,
		linewidth,
		linefactor,
		linepattern,
		pointsize,
		pointsize_min,
		pointsize_max,
		pointspriteenable,
		pointscaleenable,
		point_smooth,
		pointscale_a,
		pointscale_b,
		pointscale_c,
		polygon_offset_factor,
		polygon_offset_units,
		polygon_offset_fill,
		polygon_offset_line,
		polygon_offset_point,
		polygon_offset_smooth,
		polygon_offset_stipple,
		srgbwriteenable,
		stencilenable,
		stencilfail,
		stencilzfail,
		stencilpass,
		stencilfunc,
		stencilref,
		stencilmask,
		stencilwritemask,
		scissortestenable,
		zenable,
		zwriteenable,
		zfunc,
		znear,
		zfar,
		_num_cap
	};

	enum stateval
	{
		off,
		on,
		protect,
		inherit,
		never,
		less,
		equal,
		lessequal,
		greater,
		notequal,
		greaterequal,
		always,
		keep,
		dec,
		inc,
		replace,
		point,
		line,
		solid,
		none,
		front,
		back,
		frontback,
		all,
		zero,
		one,
		srccolor,
		srcalpha,
		destcolor,
		destalpha,
		invsrccolor,
		invsrcalpha,
		invdestcolor,
		invdestalpha,
		additive,
		subtract,
		revsubtract,
		min,
		max,
		min_factor,
		max_factor,
		min_alpha,
		max_alpha,
		red,
		green,
		blue,
		alpha,
		_nums_val
	};

	typedef unsigned char boolean;

	class StateKeyword
	{
	public:
		explicit StateKeyword(statecap type) noexcept;
		explicit StateKeyword(const std::string& line) noexcept;
		explicit StateKeyword(const std::wstring& line) noexcept;

		void assign(statecap type) noexcept;
		void assign(const std::string& line) noexcept;
		void assign(const std::wstring& line) noexcept;

		statecap value() const noexcept;
		const char* c_str() const noexcept;
		const wchar_t* c_wcs() const noexcept;

		explicit operator statecap() const noexcept;
		explicit operator const char*() const noexcept;
		explicit operator const wchar_t*() const noexcept;
	private:
		statecap _value;
	};

	class StateValue
	{
	public:
		explicit StateValue(stateval type) noexcept;
		explicit StateValue(const std::string& line) noexcept;
		explicit StateValue(const std::wstring& line) noexcept;

		void assign(stateval type) noexcept;
		void assign(const std::string& line) noexcept;
		void assign(const std::wstring& line) noexcept;

		stateval value() const noexcept;
		const char* c_str() const noexcept;
		const wchar_t* c_wcs() const noexcept;

		explicit operator stateval() const noexcept;
		explicit operator const char*() const noexcept;
		explicit operator const wchar_t*() const noexcept;
	private:
		stateval _value;
	};
public:
	StateAttribute(const std::string& name, const std::string& value);

private:
	statecap _cap;
	stateval _val;
};

class RenderState final
{
public:
	typedef std::vector<std::shared_ptr<StateAttribute>> state_list;
	typedef state_list::iterator iterator;
	typedef state_list::const_iterator const_iterator;
	typedef state_list::reverse_iterator reverse_iterator;
	typedef state_list::const_reverse_iterator const_reverse_iterator;

public:
	RenderState() noexcept;

	void push_back(std::shared_ptr<StateAttribute> state) { _state.push_back(state); }

	bool empty() const noexcept{ return _state.empty(); }
	std::size_t size() const noexcept{ return _state.size(); }

	iterator begin() noexcept{ return _state.begin(); }
	iterator end() noexcept{ return _state.end(); }

	const_iterator begin() const noexcept{ return _state.begin(); }
	const_iterator end() const noexcept{ return _state.end(); }

	reverse_iterator rbegin() noexcept{ return _state.rbegin(); }
	reverse_iterator rend() noexcept{ return _state.rend(); }

	const_reverse_iterator rbegin() const noexcept{ return _state.rbegin(); }
	const_reverse_iterator rend() const noexcept{ return _state.rend(); }

private:
	RenderState(const RenderState& copy) = delete;
	RenderState& operator=(const RenderState& copy) = delete;

private:

	state_list _state;
};

_NAME_END

#endif