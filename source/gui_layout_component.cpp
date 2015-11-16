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
#include <ray/gui_layout_component.h>
#include <ray/render_buffer.h>

_NAME_BEGIN

__ImplementSubClass(GUILayoutComponent, GameComponent, "GUILayout")

GUILayoutComponent::GUILayoutComponent() noexcept
{
}

GUILayoutComponent::~GUILayoutComponent() noexcept
{
}

void
GUILayoutComponent::drawAABB(const Vector3& min, const Vector3& max, const Vector4& color) noexcept
{
	drawLineColor(Vector3(min[0], min[1], min[2]), Vector3(max[0], min[1], min[2]), color);
	drawLineColor(Vector3(max[0], min[1], min[2]), Vector3(max[0], max[1], min[2]), color);
	drawLineColor(Vector3(max[0], max[1], min[2]), Vector3(min[0], max[1], min[2]), color);
	drawLineColor(Vector3(min[0], max[1], min[2]), Vector3(min[0], min[1], min[2]), color);
	drawLineColor(Vector3(min[0], min[1], min[2]), Vector3(min[0], min[1], max[2]), color);
	drawLineColor(Vector3(max[0], min[1], min[2]), Vector3(max[0], min[1], max[2]), color);
	drawLineColor(Vector3(max[0], max[1], min[2]), Vector3(max[0], max[1], max[2]), color);
	drawLineColor(Vector3(min[0], max[1], min[2]), Vector3(min[0], max[1], max[2]), color);
	drawLineColor(Vector3(min[0], min[1], max[2]), Vector3(max[0], min[1], max[2]), color);
	drawLineColor(Vector3(max[0], min[1], max[2]), Vector3(max[0], max[1], max[2]), color);
	drawLineColor(Vector3(max[0], max[1], max[2]), Vector3(min[0], max[1], max[2]), color);
	drawLineColor(Vector3(min[0], max[1], max[2]), Vector3(min[0], min[1], max[2]), color);
}

void
GUILayoutComponent::drawAABB(const Vector3& min, const Vector3& max, const Matrix4x4& trans, const Vector4& color) noexcept
{
	drawLineColor(trans * Vector3(min[0], min[1], min[2]), trans * Vector3(max[0], min[1], min[2]), color);
	drawLineColor(trans * Vector3(max[0], min[1], min[2]), trans * Vector3(max[0], max[1], min[2]), color);
	drawLineColor(trans * Vector3(max[0], max[1], min[2]), trans * Vector3(min[0], max[1], min[2]), color);
	drawLineColor(trans * Vector3(min[0], max[1], min[2]), trans * Vector3(min[0], min[1], min[2]), color);
	drawLineColor(trans * Vector3(min[0], min[1], min[2]), trans * Vector3(min[0], min[1], max[2]), color);
	drawLineColor(trans * Vector3(max[0], min[1], min[2]), trans * Vector3(max[0], min[1], max[2]), color);
	drawLineColor(trans * Vector3(max[0], max[1], min[2]), trans * Vector3(max[0], max[1], max[2]), color);
	drawLineColor(trans * Vector3(min[0], max[1], min[2]), trans * Vector3(min[0], max[1], max[2]), color);
	drawLineColor(trans * Vector3(min[0], min[1], max[2]), trans * Vector3(max[0], min[1], max[2]), color);
	drawLineColor(trans * Vector3(max[0], min[1], max[2]), trans * Vector3(max[0], max[1], max[2]), color);
	drawLineColor(trans * Vector3(max[0], max[1], max[2]), trans * Vector3(min[0], max[1], max[2]), color);
	drawLineColor(trans * Vector3(min[0], max[1], max[2]), trans * Vector3(min[0], min[1], max[2]), color);
}

void
GUILayoutComponent::drawArc(float x, float y, float radius, float angle, float angle2, float segments) noexcept
{
}

void
GUILayoutComponent::drawArcSolid(float x, float y, float radius, float angle, float angle2, float segments) noexcept
{
}

void
GUILayoutComponent::drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radius, float minAngle, float maxAngle, float segments) noexcept
{
	const Vector3& vx = axis;
	Vector3 vy = normal.cross(axis);
	float step = degrees(segments);
	int nSteps = (int)((maxAngle - minAngle) / step);

	if (!nSteps) nSteps = 1;
	Vector3 prev = center + radius * vx * std::cos(minAngle) + radius * vy * std::sin(minAngle);

	for (int i = 1; i <= nSteps; i++)
	{
		float angle = minAngle + (maxAngle - minAngle) * i / nSteps;
		Vector3 next = center + radius * vx * std::cos(angle) + radius * vy * std::sin(angle);
		this->drawLine(prev, next);
		prev = next;
	}
}

void
GUILayoutComponent::drawCircle(float x, float  y, float radius, float segmenst) noexcept
{
}

void
GUILayoutComponent::drawCircleSolid(float x, float  y, float radius, float segmenst) noexcept
{
}

void
GUILayoutComponent::drawQuad(float x, float y, float w, float h, const Vector4& color) noexcept
{
	Vector3 v0(x, y, 0);
	Vector3 v1(x, y - h, 0);
	Vector3 v2(x + w, y - h, 0);
	Vector3 v3(x + w, y, 0);
	this->drawQuad(v0, v1, v2, v3, color);
}

void
GUILayoutComponent::drawQuad(float dx, float dy, float dz, float x, float y, float z) noexcept
{
}

void
GUILayoutComponent::drawQuad(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1) noexcept
{
}

void
GUILayoutComponent::drawQuad(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color) noexcept
{
	_vertices.push_back(v0);
	_vertices.push_back(v1);
	_vertices.push_back(v2);
	_vertices.push_back(v0);
	_vertices.push_back(v2);
	_vertices.push_back(v3);

	_coords.push_back(Vector2(1, 1));
	_coords.push_back(Vector2(0, 1));
	_coords.push_back(Vector2(0, 0));
	_coords.push_back(Vector2(1, 1));
	_coords.push_back(Vector2(0, 0));
	_coords.push_back(Vector2(1, 0));

	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);

	RenderIndirect renderable;
	renderable.startVertice = _vertices.size() - 6;
	renderable.numVertices = 6;
	_renderables.push_back(std::make_pair(_material, renderable));
}

void
GUILayoutComponent::drawQuadWithUV(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector4& color) noexcept
{
	_vertices.push_back(v0);
	_vertices.push_back(v1);
	_vertices.push_back(v2);
	_vertices.push_back(v0);
	_vertices.push_back(v2);
	_vertices.push_back(v3);

	_coords.push_back(uv0);
	_coords.push_back(uv1);
	_coords.push_back(uv2);
	_coords.push_back(uv0);
	_coords.push_back(uv2);
	_coords.push_back(uv3);

	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);
	_colors.push_back(color);

	RenderIndirect renderable;
	renderable.startVertice = _vertices.size() - 6;
	renderable.numVertices = 6;
	_renderables.push_back(std::make_pair(_material, renderable));
}

void 
GUILayoutComponent::drawQuadsWithUV(const std::vector<Vector4>& vertices, const std::vector<Vector4>& uvs, const std::vector<Vector4>& colors) noexcept
{
	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		auto& rect = vertices[i];
		auto& uv = uvs[i];
		auto& color = colors[i];

		auto v0 = float3(rect.x, rect.w, 0);
		auto v1 = float3(rect.x, rect.y, 0);
		auto v2 = float3(rect.z, rect.y, 0);
		auto v3 = float3(rect.z, rect.w, 0);

		auto uv0 = float2(uv.x, uv.w);
		auto uv1 = float2(uv.x, uv.y);
		auto uv2 = float2(uv.z, uv.y);
		auto uv3 = float2(uv.z, uv.w);

		_vertices.push_back(v0);
		_vertices.push_back(v1);
		_vertices.push_back(v2);
		_vertices.push_back(v0);
		_vertices.push_back(v2);
		_vertices.push_back(v3);

		_coords.push_back(uv0);
		_coords.push_back(uv1);
		_coords.push_back(uv2);
		_coords.push_back(uv0);
		_coords.push_back(uv2);
		_coords.push_back(uv3);

		_colors.push_back(color);
		_colors.push_back(color);
		_colors.push_back(color);
		_colors.push_back(color);
		_colors.push_back(color);
		_colors.push_back(color);
	}

	RenderIndirect renderable;
	renderable.startVertice = _vertices.size() - 6 * vertices.size();
	renderable.numVertices = 6 * vertices.size();
	_renderables.push_back(std::make_pair(_material, renderable));
}

void
GUILayoutComponent::drawImage(float xpos, float ypos, float w, float h, int textureid, float s0, float t0, float s1, float t1, float r, float g, float b, float a) noexcept
{
}

void
GUILayoutComponent::drawImage(float xpos, float ypos, float z, float w, float h, int textureid, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a) noexcept
{
}

void
GUILayoutComponent::drawImageWithUV(float xpos, float ypos, float z, float w, float h, int texture_id, float *s, float *t, float r, float g, float b, float a) noexcept
{
}

void
GUILayoutComponent::drawLine(const Vector3& pos1, const Vector3& pos2) noexcept
{
	this->drawLineColor(pos1, pos2, _globalColor);
}

void
GUILayoutComponent::drawLineColor(const Vector3& pos1, const Vector3 & pos2, const Vector4& color) noexcept
{
	this->drawLineColor(pos1, color, pos2, color);
}

void
GUILayoutComponent::drawLineColor(const Vector3& pos1, const Vector4& color1, const Vector3 & pos2, const Vector4& color2) noexcept
{
	_vertices.push_back(pos1);
	_vertices.push_back(pos2);

	_colors.push_back(color2);
	_colors.push_back(color2);
}

void
GUILayoutComponent::drawLines(const Vector3 v[], std::size_t num, const Vector4& col, int flags, float ground) noexcept
{
	std::size_t index = 0;
	for (std::size_t i = 0; i < num; i++)
	{
		this->drawLineColor(v[index], col, v[index + 1], col);
		index += 2;
	}
}

void
GUILayoutComponent::drawPoint(const Vector3& pt) noexcept
{
	this->drawPoint(pt, _globalColor);
}

void
GUILayoutComponent::drawPoint(const Vector3& pt, const Vector4& color) noexcept
{
}

void
GUILayoutComponent::drawPoints(const Vector3 pt[], std::size_t num) noexcept
{
	for (std::size_t i = 0; i < num; i++)
	{
		this->drawPoint(pt[i]);
	}
}

void
GUILayoutComponent::drawText(const Vector3& pt, const std::string& string) noexcept
{
}

GameComponentPtr 
GUILayoutComponent::clone() const except
{
	auto other = std::make_shared<GUILayoutComponent>();
	return other;
}

void 
GUILayoutComponent::load(iarchive& reader) noexcept
{
}

void 
GUILayoutComponent::save(oarchive& write) noexcept
{
}

void 
GUILayoutComponent::setMaterialPass(MaterialPassPtr material) noexcept
{
	_material = material;
}

MaterialPassPtr 
GUILayoutComponent::getMaterialPass() noexcept
{
	return _material;
}

void
GUILayoutComponent::clear() noexcept
{
	_vertices.clear();
	_colors.clear();
	_coords.clear();
	_renderables.clear();
}

const Vector3Array& 
GUILayoutComponent::getVertices() noexcept
{
	return _vertices;
}

const Vector2Array& 
GUILayoutComponent::getCoords() noexcept
{
	return _coords;
}

const Vector4Array& 
GUILayoutComponent::getColors() noexcept
{
	return _colors;
}

std::vector<std::pair<MaterialPassPtr, RenderIndirect>>
GUILayoutComponent::getRenderIndirect() const noexcept
{
	return _renderables;
}

_NAME_END