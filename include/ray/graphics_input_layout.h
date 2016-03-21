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
#ifndef _H_GRAPHICS_INPUT_LAYOUT_H_
#define _H_GRAPHICS_INPUT_LAYOUT_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT VertexComponent final
{
public:
	VertexComponent() noexcept;
	VertexComponent(const std::string& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint8_t slot = 0, std::uint8_t divisor = 0) noexcept;
	~VertexComponent() noexcept;

	void setSemantic(const std::string& semantic) noexcept;
	const std::string& getSemantic() const noexcept;

	void setSemanticIndex(std::uint8_t index) noexcept;
	std::uint8_t getSemanticIndex() const noexcept;

	void setVertexFormat(GraphicsFormat format) noexcept;
	GraphicsFormat getVertexFormat() const noexcept;

	void setVertexSlot(std::uint8_t slot) noexcept;
	std::uint8_t getVertexSlot() const noexcept;

	void setVertexDivisor(std::uint8_t divisor) noexcept;
	std::uint8_t getVertexDivisor() const noexcept;
	
	std::uint8_t getVertexCount() const noexcept;
	std::uint8_t getVertexSize() const noexcept;

	static std::uint8_t getVertexCount(GraphicsFormat) noexcept;
	static std::uint8_t getVertexSize(GraphicsFormat) noexcept;

private:
	std::string _semantic;
	std::uint8_t _index;
	std::uint8_t _slot;
	std::uint8_t _count;
	std::uint8_t _size;
	std::uint8_t _divisor;
	GraphicsFormat _format;
};

class EXPORT GraphicsInputLayoutDesc final
{
public:
	GraphicsInputLayoutDesc() noexcept;
	~GraphicsInputLayoutDesc() noexcept;

	void setVertexComponents(const VertexComponents& component) noexcept;
	const VertexComponents& getVertexComponents() const noexcept;

	void addComponent(const VertexComponent& compoent) noexcept;
	void removeComponent(const VertexComponent& compoent) noexcept;

	void setIndexType(GraphicsIndexType type) noexcept;
	GraphicsIndexType getIndexType() const noexcept;

	std::uint32_t getVertexSize() const noexcept;
	std::uint32_t getIndexSize() const noexcept;

private:
	GraphicsIndexType _indexType;
	VertexComponents _components;
};

class EXPORT GraphicsInputLayout : public GraphicsChild
{
	__DeclareSubInterface(GraphicsInputLayout, GraphicsChild)
public:
	GraphicsInputLayout() noexcept;
	virtual ~GraphicsInputLayout() noexcept;

	virtual const GraphicsInputLayoutDesc& getGraphicsInputLayoutDesc() const noexcept = 0;

private:
	GraphicsInputLayout(const GraphicsInputLayout&) = delete;
	GraphicsInputLayout& operator=(const GraphicsInputLayout&) = delete;
};

_NAME_END

#endif