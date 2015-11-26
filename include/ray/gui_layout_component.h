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
#ifndef _H_GUI_LAYOUT_COMPONENT_H_
#define _H_GUI_LAYOUT_COMPONENT_H_

#include <ray/gui_listener_component.h>

_NAME_BEGIN

struct GUIStyleState
{
	TexturePtr background;

	Vector4 textColor;
};

class GUILayoutComponent : public GameComponent
{
	__DeclareSubClass(GUILayoutComponent, ray::GameComponent)
public:
	GUILayoutComponent() noexcept;
	~GUILayoutComponent() noexcept;

	void drawAABB(const Vector3& min, const Vector3& max, const Vector4& color) noexcept;
	void drawAABB(const Vector3& min, const Vector3& max, const Matrix4x4& trans, const Vector4& color) noexcept;

	void drawArc(float x, float y, float radius, float angle, float angle2, float segments) noexcept;
	void drawArcSolid(float x, float y, float radius, float angle, float angle2, float segments) noexcept;
	void drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radius, float minAngle, float maxAngle, float segments) noexcept;

	void drawCircle(float x, float  y, float radius, float segmenst) noexcept;
	void drawCircleSolid(float x, float  y, float radius, float segmenst) noexcept;

	void drawQuad(float x, float y, float w, float h, const Vector4& color) noexcept;
	void drawQuad(float dx, float dy, float dz, float x, float y, float z) noexcept;
	void drawQuad(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1 = 1, float t1 = 1) noexcept;
	void drawQuad(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color) noexcept;
	void drawQuadWithUV(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector4& color) noexcept;
	void drawQuadsWithUV(const std::vector<Vector4>& rect, const std::vector<Vector4>& uv, const std::vector<Vector4>& color) noexcept;

	void drawImage(float xpos, float ypos, float w, float h, int textureid, float s0, float t0, float s1, float t1, float r, float g, float b, float a) noexcept;
	void drawImage(float xpos, float ypos, float z, float w, float h, int textureid, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a) noexcept;
	void drawImageWithUV(float xpos, float ypos, float z, float w, float h, int texture_id, float *s, float *t, float r, float g, float b, float a) noexcept;

	void drawLine(const Vector3& pos1, const Vector3& pos2) noexcept;
	void drawLineColor(const Vector3& pos1, const Vector3& pos2, const Vector4& color) noexcept;
	void drawLineColor(const Vector3& pos1, const Vector4& color1, const Vector3 & pos2, const Vector4& color2) noexcept;
	void drawLines(const Vector3 v[], std::size_t num, const Vector4& col, int flags, float fGround) noexcept;

	void drawPoint(const Vector3& pt) noexcept;
	void drawPoint(const Vector3& pt, const Vector4& color) noexcept;
	void drawPoints(const Vector3 pt[], std::size_t num) noexcept;

	void drawText(const Vector3& pt, const std::string& string) noexcept;

	void clear() noexcept;

	void setMaterialPass(MaterialPassPtr material) noexcept;
	MaterialPassPtr getMaterialPass() noexcept;

	const Vector3Array& getVertices() noexcept;
	const Vector2Array& getCoords() noexcept;
	const Vector4Array& getColors() noexcept;

	std::vector<std::pair<MaterialPassPtr, RenderIndirect>> getRenderIndirect() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	virtual GameComponentPtr clone() const except;

private:

	MaterialPassPtr _material;

	Vector4 _globalColor;

	Vector3Array _vertices;
	Vector2Array _coords;
	Vector4Array _colors;

	std::vector<std::pair<MaterialPassPtr, RenderIndirect>> _renderables;
};

_NAME_END

#endif