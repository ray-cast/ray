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
#include <ray/state.h>
#include <ray/parse.h>

#include <unordered_map>

_NAME_BEGIN

std::unordered_map<std::string, StateAttribute::statecap> _s_state_cap_map =
{
	{ "alpharef", StateAttribute::alpharef },
	{ "alphafunc", StateAttribute::alphafunc },
	{ "alphatestenable", StateAttribute::alphatestenable },
	{ "blendenable", StateAttribute::blendenable },
	{ "blendcolor", StateAttribute::blendcolor },
	{ "blendsrcfactor", StateAttribute::blendsrcfactor },
	{ "blendsrcfactoralpha", StateAttribute::blendsrcfactoralpha },
	{ "blenddestfactor", StateAttribute::blenddestfactor },
	{ "blenddestfactoralpha", StateAttribute::blenddestfactoralpha },
	{ "blendop", StateAttribute::blendop },
	{ "blendopalpha", StateAttribute::blendopalpha },
	{ "blendequationrgb", StateAttribute::blendequationrgb },
	{ "blendequationalpha", StateAttribute::blendequationalpha },
	{ "blenddest", StateAttribute::blenddest },
	{ "blendsrc", StateAttribute::blendsrc },
	{ "ccw_stencilfail", StateAttribute::ccw_stencilfail },
	{ "ccw_stencilzfail", StateAttribute::ccw_stencilzfail },
	{ "ccw_stencilpass", StateAttribute::ccw_stencilpass },
	{ "ccw_stencilfunc", StateAttribute::ccw_stencilfunc },
	{ "colorwriteenable", StateAttribute::colorwriteenable },
	{ "colorwriteenable1", StateAttribute::colorwriteenable1 },
	{ "colorwriteenable2", StateAttribute::colorwriteenable2 },
	{ "colorwriteenable3", StateAttribute::colorwriteenable3 },
	{ "cullmode", StateAttribute::cullmode },
	{ "fillmode", StateAttribute::fillmode },
	{ "indexedvertexblendenable", StateAttribute::indexedvertexblendenable },
	{ "line_smooth", StateAttribute::linesmooth },
	{ "line_width", StateAttribute::linewidth },
	{ "line_factor", StateAttribute::linefactor },
	{ "line_pattern", StateAttribute::linepattern },
	{ "pointsize", StateAttribute::pointsize },
	{ "pointsize_min", StateAttribute::pointsize_min },
	{ "pointsize_max", StateAttribute::pointsize_max },
	{ "pointspriteenable", StateAttribute::pointspriteenable },
	{ "pointscaleenable", StateAttribute::pointscaleenable },
	{ "point_smooth", StateAttribute::point_smooth },
	{ "pointscale_a", StateAttribute::pointscale_a },
	{ "pointscale_b", StateAttribute::pointscale_b },
	{ "pointscale_c", StateAttribute::pointscale_c },
	{ "polygon_offset_factor", StateAttribute::polygon_offset_factor },
	{ "polygon_offset_units", StateAttribute::polygon_offset_units },
	{ "polygon_offset_fill", StateAttribute::polygon_offset_fill },
	{ "polygon_offset_line", StateAttribute::polygon_offset_line },
	{ "polygon_offset_point", StateAttribute::polygon_offset_point },
	{ "polygon_offset_smooth", StateAttribute::polygon_offset_smooth },
	{ "polygon_offset_stipple", StateAttribute::polygon_offset_stipple },
	{ "srgbwriteenable", StateAttribute::srgbwriteenable },
	{ "stencilenable", StateAttribute::stencilenable },
	{ "stencilfail", StateAttribute::stencilfail },
	{ "stencilzfail", StateAttribute::stencilzfail },
	{ "stencilpass", StateAttribute::stencilpass },
	{ "stencilfunc", StateAttribute::stencilfunc },
	{ "stencilref", StateAttribute::stencilref },
	{ "stencilmask", StateAttribute::stencilmask },
	{ "stencilwritemask", StateAttribute::stencilwritemask },
	{ "scissortestenable", StateAttribute::scissortestenable },
	{ "zenable", StateAttribute::zenable },
	{ "zwriteenable", StateAttribute::zwriteenable },
	{ "zfunc", StateAttribute::zfunc },
	{ "znear", StateAttribute::znear },
	{ "zfar", StateAttribute::zfar }
};

std::unordered_map<std::wstring, StateAttribute::statecap> _s_state_cap_mapw =
{
	{ L"alpharef", StateAttribute::alpharef },
	{ L"alphafunc", StateAttribute::alphafunc },
	{ L"alphatestenable", StateAttribute::alphatestenable },
	{ L"blendenable", StateAttribute::blendenable },
	{ L"blendcolor", StateAttribute::blendcolor },
	{ L"blendsrcfactor", StateAttribute::blendsrcfactor },
	{ L"blendsrcfactoralpha", StateAttribute::blendsrcfactoralpha },
	{ L"blenddestfactor", StateAttribute::blenddestfactor },
	{ L"blenddestfactoralpha", StateAttribute::blenddestfactoralpha },
	{ L"blendop", StateAttribute::blendop },
	{ L"blendopalpha", StateAttribute::blendopalpha },
	{ L"blendequationrgb", StateAttribute::blendequationrgb },
	{ L"blendequationalpha", StateAttribute::blendequationalpha },
	{ L"blenddest", StateAttribute::blenddest },
	{ L"blendsrc", StateAttribute::blendsrc },
	{ L"ccw_stencilfail", StateAttribute::ccw_stencilfail },
	{ L"ccw_stencilzfail", StateAttribute::ccw_stencilzfail },
	{ L"ccw_stencilpass", StateAttribute::ccw_stencilpass },
	{ L"ccw_stencilfunc", StateAttribute::ccw_stencilfunc },
	{ L"colorwriteenable", StateAttribute::colorwriteenable },
	{ L"colorwriteenable1", StateAttribute::colorwriteenable1 },
	{ L"colorwriteenable2", StateAttribute::colorwriteenable2 },
	{ L"colorwriteenable3", StateAttribute::colorwriteenable3 },
	{ L"cullmode", StateAttribute::cullmode },
	{ L"fillmode", StateAttribute::fillmode },
	{ L"indexedvertexblendenable", StateAttribute::indexedvertexblendenable },
	{ L"line_smooth", StateAttribute::linesmooth },
	{ L"line_width", StateAttribute::linewidth },
	{ L"line_factor", StateAttribute::linefactor },
	{ L"line_pattern", StateAttribute::linepattern },
	{ L"pointsize", StateAttribute::pointsize },
	{ L"pointsize_min", StateAttribute::pointsize_min },
	{ L"pointsize_max", StateAttribute::pointsize_max },
	{ L"pointspriteenable", StateAttribute::pointspriteenable },
	{ L"pointscaleenable", StateAttribute::pointscaleenable },
	{ L"point_smooth", StateAttribute::point_smooth },
	{ L"pointscale_a", StateAttribute::pointscale_a },
	{ L"pointscale_b", StateAttribute::pointscale_b },
	{ L"pointscale_c", StateAttribute::pointscale_c },
	{ L"polygon_offset_factor", StateAttribute::polygon_offset_factor },
	{ L"polygon_offset_units", StateAttribute::polygon_offset_units },
	{ L"polygon_offset_fill", StateAttribute::polygon_offset_fill },
	{ L"polygon_offset_line", StateAttribute::polygon_offset_line },
	{ L"polygon_offset_point", StateAttribute::polygon_offset_point },
	{ L"polygon_offset_smooth", StateAttribute::polygon_offset_smooth },
	{ L"polygon_offset_stipple", StateAttribute::polygon_offset_stipple },
	{ L"srgbwriteenable", StateAttribute::srgbwriteenable },
	{ L"stencilenable", StateAttribute::stencilenable },
	{ L"stencilfail", StateAttribute::stencilfail },
	{ L"stencilzfail", StateAttribute::stencilzfail },
	{ L"stencilpass", StateAttribute::stencilpass },
	{ L"stencilfunc", StateAttribute::stencilfunc },
	{ L"stencilref", StateAttribute::stencilref },
	{ L"stencilmask", StateAttribute::stencilmask },
	{ L"stencilwritemask", StateAttribute::stencilwritemask },
	{ L"scissortestenable", StateAttribute::scissortestenable },
	{ L"zenable", StateAttribute::zenable },
	{ L"zwriteenable", StateAttribute::zwriteenable },
	{ L"zfunc", StateAttribute::zfunc },
	{ L"znear", StateAttribute::znear },
	{ L"zfar", StateAttribute::zfar }
};

StateAttribute::StateKeyword::StateKeyword(statecap type) noexcept
{
    this->assign(type);
}

StateAttribute::StateKeyword::StateKeyword(const std::string& line) noexcept
{
    this->assign(line);
}

StateAttribute::StateKeyword::StateKeyword(const std::wstring& line) noexcept
{
    this->assign(line);
}

void
StateAttribute::StateKeyword::assign(statecap value) noexcept
{
    _value = value;
}

void
StateAttribute::StateKeyword::assign(const std::string& line) noexcept
{
    _value = _s_state_cap_map[line];
}

void
StateAttribute::StateKeyword::assign(const std::wstring& line) noexcept
{
    _value = _s_state_cap_mapw[line];
}

StateAttribute::statecap
StateAttribute::StateKeyword::value() const noexcept
{
    return _value;
}

const char*
StateAttribute::StateKeyword::c_str() const noexcept
{
    switch (this->value())
    {
    case alpharef:
        return "alpharef";
    case alphafunc:
        return "alphafunc";
    case alphatestenable:
        return "alphatestenable";
    case blendenable:
        return "blendenable";
    case blendcolor:
        return "blendcolor";
    case blendsrcfactor:
        return "blendsrcfactor";
    case blendsrcfactoralpha:
        return "blendsrcfactoralpha";
    case blenddestfactor:
        return "blenddestfactor";
    case blenddestfactoralpha:
        return "blenddestfactoralpha";
    case blendop:
        return "blendop";
    case blendopalpha:
        return "blendopalpha";
    case blendequationrgb:
        return "blendequationrgb";
    case blendequationalpha:
        return "blendequationalpha";
    case blenddest:
        return "blenddest";
    case blendsrc:
        return "blendsrc";
    case ccw_stencilfail:
        return "ccw_stencilfail";
    case ccw_stencilzfail:
        return "ccw_stencilzfail";
    case ccw_stencilpass:
        return "ccw_stencilpass";
    case ccw_stencilfunc:
        return "ccw_stencilfunc";
    case colorwriteenable:
        return "colorwriteenable";
    case colorwriteenable1:
        return "colorwriteenable1";
    case colorwriteenable2:
        return "colorwriteenable2";
    case colorwriteenable3:
        return "colorwriteenable3";
    case cullmode:
        return "cullmode";
    case fillmode:
        return "fillmode";
    case indexedvertexblendenable:
        return "indexedvertexblendenable";
    case linesmooth:
        return "line_smooth";
    case linewidth:
        return "line_width";
    case linefactor:
        return "line_factor";
    case linepattern:
        return "line_pattern";
    case pointsize:
        return "pointsize";
    case pointsize_min:
        return "pointsize_min";
    case pointsize_max:
        return "pointsize_max";
    case pointspriteenable:
        return "pointspriteenable";
    case pointscaleenable:
        return "pointscaleenable";
    case point_smooth:
        return "point_smooth";
    case pointscale_a:
        return "pointscale_a";
    case pointscale_b:
        return "pointscale_b";
    case pointscale_c:
        return "pointscale_c";
    case polygon_offset_factor:
        return "polygon_offset_factor";
    case polygon_offset_units:
        return "polygon_offset_units";
    case polygon_offset_fill:
        return "polygon_offset_fill";
    case polygon_offset_line:
        return "polygon_offset_line";
    case polygon_offset_point:
        return "polygon_offset_point";
    case polygon_offset_smooth:
        return "polygon_offset_smooth";
    case polygon_offset_stipple:
        return "polygon_offset_stipple";
    case srgbwriteenable:
        return "srgbwriteenable";
    case stencilenable:
        return "stencilenable";
    case stencilfail:
        return "stencilfail";
    case stencilzfail:
        return "stencilzfail";
    case stencilpass:
        return "stencilpass";
    case stencilfunc:
        return "stencilfunc";
    case stencilref:
        return "stencilref";
    case stencilmask:
        return "stencilmask";
    case stencilwritemask:
        return "stencilwritemask";
    case scissortestenable:
        return "scissortestenable";
    case zenable:
        return "zenable";
    case zwriteenable:
        return "zwriteenable";
    case zfunc:
        return "zfunc";
    case znear:
        return "znear";
    case zfar:
        return "zfar";
    default:
        return "unknown";
    }
}

const wchar_t*
StateAttribute::StateKeyword::c_wcs() const noexcept
{
    switch (this->value())
    {
    case alpharef:
        return L"alpharef";
    case alphafunc:
        return L"alphafunc";
    case alphatestenable:
        return L"alphatestenable";
    case blendenable:
        return L"blendenable";
    case blendcolor:
        return L"blendcolor";
    case blendsrcfactor:
        return L"blendsrcfactor";
    case blendsrcfactoralpha:
        return L"blendsrcfactoralpha";
    case blenddestfactor:
        return L"blenddestfactor";
    case blenddestfactoralpha:
        return L"blenddestfactoralpha";
    case blendop:
        return L"blendop";
    case blendopalpha:
        return L"blendopalpha";
    case blendequationrgb:
        return L"blendequationrgb";
    case blendequationalpha:
        return L"blendequationalpha";
    case blenddest:
        return L"blenddest";
    case blendsrc:
        return L"blendsrc";
    case ccw_stencilfail:
        return L"ccw_stencilfail";
    case ccw_stencilzfail:
        return L"ccw_stencilzfail";
    case ccw_stencilpass:
        return L"ccw_stencilpass";
    case ccw_stencilfunc:
        return L"ccw_stencilfunc";
    case colorwriteenable:
        return L"colorwriteenable";
    case colorwriteenable1:
        return L"colorwriteenable1";
    case colorwriteenable2:
        return L"colorwriteenable2";
    case colorwriteenable3:
        return L"colorwriteenable3";
    case cullmode:
        return L"cullmode";
    case fillmode:
        return L"fillmode";
    case indexedvertexblendenable:
        return L"indexedvertexblendenable";
    case linesmooth:
        return L"line_smooth";
    case linewidth:
        return L"line_width";
    case linefactor:
        return L"line_factor";
    case linepattern:
        return L"line_pattern";
    case pointsize:
        return L"pointsize";
    case pointsize_min:
        return L"pointsize_min";
    case pointsize_max:
        return L"pointsize_max";
    case pointspriteenable:
        return L"pointspriteenable";
    case pointscaleenable:
        return L"pointscaleenable";
    case point_smooth:
        return L"point_smooth";
    case pointscale_a:
        return L"pointscale_a";
    case pointscale_b:
        return L"pointscale_b";
    case pointscale_c:
        return L"pointscale_c";
    case polygon_offset_factor:
        return L"polygon_offset_factor";
    case polygon_offset_units:
        return L"polygon_offset_units";
    case polygon_offset_fill:
        return L"polygon_offset_fill";
    case polygon_offset_line:
        return L"polygon_offset_line";
    case polygon_offset_point:
        return L"polygon_offset_point";
    case polygon_offset_smooth:
        return L"polygon_offset_smooth";
    case polygon_offset_stipple:
        return L"polygon_offset_stipple";
    case srgbwriteenable:
        return L"srgbwriteenable";
    case stencilenable:
        return L"stencilenable";
    case stencilfail:
        return L"stencilfail";
    case stencilzfail:
        return L"stencilzfail";
    case stencilpass:
        return L"stencilpass";
    case stencilfunc:
        return L"stencilfunc";
    case stencilref:
        return L"stencilref";
    case stencilmask:
        return L"stencilmask";
    case stencilwritemask:
        return L"stencilwritemask";
    case scissortestenable:
        return L"scissortestenable";
    case zenable:
        return L"zenable";
    case zwriteenable:
        return L"zwriteenable";
    case zfunc:
        return L"zfunc";
    case znear:
        return L"znear";
    case zfar:
        return L"zfar";
    default:
        return L"unknown";
    }
}

StateAttribute::StateKeyword::operator StateAttribute::statecap() const noexcept
{
    return this->value();
}

StateAttribute::StateKeyword::operator const char*() const noexcept
{
    return this->c_str();
}

StateAttribute::StateKeyword::operator const wchar_t*() const noexcept
{
    return this->c_wcs();
}

StateAttribute::StateValue::StateValue(stateval type) noexcept
{
    this->assign(type);
}

StateAttribute::StateValue::StateValue(const std::string& line) noexcept
{
    this->assign(line);
}

StateAttribute::StateValue::StateValue(const std::wstring& line) noexcept
{
    this->assign(line);
}

void
StateAttribute::StateValue::assign(stateval value) noexcept
{
    assert(value >= 0);
	assert(value <= _nums_val);

    _value = value;
}

void
StateAttribute::StateValue::assign(const std::string& line) noexcept
{
    if ("off" == line)
        this->assign(off);
    else if ("on" == line)
        this->assign(on);
    if ("0" == line)
        this->assign(off);
    else if ("1" == line)
        this->assign(on);
	if ("false" == line)
		this->assign(off);
	else if ("true" == line)
		this->assign(on);
    else if ("protect" == line)
        this->assign(protect);
    else if ("inherit" == line)
        this->assign(inherit);
    else if ("never" == line)
        this->assign(never);
    else if ("less" == line)
        this->assign(less);
    else if ("equal" == line)
        this->assign(equal);
    else if ("lessequal" == line)
        this->assign(lessequal);
    else if ("greater" == line)
        this->assign(greater);
    else if ("notequal" == line)
        this->assign(notequal);
    else if ("greaterequal" == line)
        this->assign(greaterequal);
    else if ("always" == line)
        this->assign(always);
    else if ("point" == line)
        this->assign(point);
    else if ("line" == line)
        this->assign(line);
    else if ("solid" == line)
        this->assign(solid);
    else if ("none" == line)
        this->assign(none);
    else if ("front" == line)
        this->assign(front);
    else if ("back" == line)
        this->assign(back);
    else if ("frontback" == line)
        this->assign(frontback);
    else if ("all" == line)
        this->assign(all);
    else if ("zero" == line)
        this->assign(zero);
    else if ("one" == line)
        this->assign(one);
    else if ("srccolor" == line)
        this->assign(srccolor);
    else if ("srcalpha" == line)
        this->assign(srcalpha);
    else if ("destcolor" == line)
        this->assign(destcolor);
    else if ("destalpha" == line)
        this->assign(destalpha);
    else if ("invsrccolor" == line)
        this->assign(invsrccolor);
    else if ("invsrcalpha" == line)
        this->assign(invsrcalpha);
    else if ("invdestcolor" == line)
        this->assign(invdestcolor);
    else if ("invdestalpha" == line)
        this->assign(invdestalpha);
    else if ("additive" == line)
        this->assign(additive);
    else if ("subtract" == line)
        this->assign(subtract);
    else if ("revsubtract" == line)
        this->assign(revsubtract);
    else if ("min" == line)
        this->assign(min);
    else if ("max" == line)
        this->assign(max);
    else if ("min_factor" == line)
        this->assign(min_factor);
    else if ("max_factor" == line)
        this->assign(max_factor);
    else if ("min_alpha" == line)
        this->assign(min_alpha);
    else if ("max_alpha" == line)
        this->assign(max_alpha);
    else
		this->assign((stateval)parseInteger(line.c_str()));
}

void
StateAttribute::StateValue::assign(const std::wstring& line) noexcept
{
    if (L"off" == line)
        this->assign(off);
    else if (L"on" == line)
        this->assign(on);
    if (L"0" == line)
        this->assign(off);
    else if (L"1" == line)
        this->assign(on);
	if (L"false" == line)
		this->assign(off);
	else if (L"true" == line)
		this->assign(on);
    else if (L"protect" == line)
        this->assign(protect);
    else if (L"inherit" == line)
        this->assign(inherit);
    else if (L"never" == line)
        this->assign(never);
    else if (L"less" == line)
        this->assign(less);
    else if (L"equal" == line)
        this->assign(equal);
    else if (L"lessequal" == line)
        this->assign(lessequal);
    else if (L"greater" == line)
        this->assign(greater);
    else if (L"notequal" == line)
        this->assign(notequal);
    else if (L"greaterequal" == line)
        this->assign(greaterequal);
    else if (L"always" == line)
        this->assign(always);
    else if (L"point" == line)
        this->assign(point);
    else if (L"line" == line)
        this->assign(line);
    else if (L"solid" == line)
        this->assign(solid);
    else if (L"none" == line)
        this->assign(none);
    else if (L"front" == line)
        this->assign(front);
    else if (L"back" == line)
        this->assign(back);
    else if (L"frontback" == line)
        this->assign(frontback);
    else if (L"all" == line)
        this->assign(all);
    else if (L"zero" == line)
        this->assign(zero);
    else if (L"one" == line)
        this->assign(one);
    else if (L"srccolor" == line)
        this->assign(srccolor);
    else if (L"srcalpha" == line)
        this->assign(srcalpha);
    else if (L"destcolor" == line)
        this->assign(destcolor);
    else if (L"destalpha" == line)
        this->assign(destalpha);
    else if (L"invsrccolor" == line)
        this->assign(invsrccolor);
    else if (L"invsrcalpha" == line)
        this->assign(invsrcalpha);
    else if (L"invdestcolor" == line)
        this->assign(invdestcolor);
    else if (L"invdestalpha" == line)
        this->assign(invdestalpha);
    else if (L"additive" == line)
        this->assign(additive);
    else if (L"subtract" == line)
        this->assign(subtract);
    else if (L"revsubtract" == line)
        this->assign(revsubtract);
    else if (L"min" == line)
        this->assign(min);
    else if (L"max" == line)
        this->assign(max);
    else if (L"min_factor" == line)
        this->assign(min_factor);
    else if (L"max_factor" == line)
        this->assign(max_factor);
    else if (L"min_alpha" == line)
        this->assign(min_alpha);
    else if (L"max_alpha" == line)
        this->assign(max_alpha);
    else
		this->assign((stateval)parseInteger(line.c_str()));
}

StateAttribute::stateval
StateAttribute::StateValue::value() const noexcept
{
    return _value;
}

const char*
StateAttribute::StateValue::c_str() const noexcept
{
    switch (this->value())
    {
    case off:
        return "off";
    case on:
        return "on";
    case protect:
        return "protect";
    case inherit:
        return "inherit";
    case never:
        return "never";
    case less:
        return "less";
    case equal:
        return "equal";
    case lessequal:
        return "lessequal";
    case greater:
        return "greater";
    case notequal:
        return "notequal";
    case greaterequal:
        return "greaterequal";
    case always:
        return "always";
    case point:
        return "point";
    case line:
        return "line";
    case solid:
        return "solid";
    case none:
        return "none";
    case front:
        return "front";
    case back:
        return "back";
    case frontback:
        return "frontback";
    case all:
        return "all";
    case zero:
        return "zero";
    case one:
        return "one";
    case srccolor:
        return "srccolor";
    case srcalpha:
        return "srcalpha";
    case destcolor:
        return "destcolor";
    case destalpha:
        return "destalpha";
    case invsrccolor:
        return "invsrccolor";
    case invsrcalpha:
        return "invsrcalpha";
    case invdestcolor:
        return "invdestcolor";
    case invdestalpha:
        return "invdestalpha";
    case additive:
        return "additive";
    case subtract:
        return "subtract";
    case revsubtract:
        return "revsubtract";
    case min:
        return "min";
    case max:
        return "max";
    case min_factor:
        return "min_factor";
    case max_factor:
        return "max_factor";
    case min_alpha:
        return "min_alpha";
    case max_alpha:
        return "max_alpha";
    default:
        return "unknown";
    }
}

const wchar_t*
StateAttribute::StateValue::c_wcs() const noexcept
{
    switch (this->value())
    {
    case off:
        return L"off";
    case on:
        return L"on";
    case protect:
        return L"protect";
    case inherit:
        return L"inherit";
    case never:
        return L"never";
    case less:
        return L"less";
    case equal:
        return L"equal";
    case lessequal:
        return L"lessequal";
    case greater:
        return L"greater";
    case notequal:
        return L"notequal";
    case greaterequal:
        return L"greaterequal";
    case always:
        return L"always";
    case point:
        return L"point";
    case line:
        return L"line";
    case solid:
        return L"solid";
    case none:
        return L"none";
    case front:
        return L"front";
    case back:
        return L"back";
    case frontback:
        return L"frontback";
    case all:
        return L"all";
    case zero:
        return L"zero";
    case one:
        return L"one";
    case srccolor:
        return L"srccolor";
    case srcalpha:
        return L"srcalpha";
    case destcolor:
        return L"destcolor";
    case destalpha:
        return L"destalpha";
    case invsrccolor:
        return L"invsrccolor";
    case invsrcalpha:
        return L"invsrcalpha";
    case invdestcolor:
        return L"invdestcolor";
    case invdestalpha:
        return L"invdestalpha";
    case additive:
        return L"additive";
    case subtract:
        return L"subtract";
    case revsubtract:
        return L"revsubtract";
    case min:
        return L"min";
    case max:
        return L"max";
    case min_factor:
        return L"min_factor";
    case max_factor:
        return L"max_factor";
    case min_alpha:
        return L"min_alpha";
    case max_alpha:
        return L"max_alpha";
    default:
        return L"unknown";
    }
}

StateAttribute::StateValue::operator StateAttribute::stateval() const noexcept
{
    return this->value();
}

StateAttribute::StateValue::operator const char*() const noexcept
{
    return this->c_str();
}

StateAttribute::StateValue::operator const wchar_t*() const noexcept
{
    return this->c_wcs();
}

StateAttribute::StateAttribute(const std::string& name, const std::string& value)
{
	_cap = StateKeyword(name).value();
	_val = StateValue(value).value();
}

RenderState::RenderState() noexcept
{
}

_NAME_END