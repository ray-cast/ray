// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_GRAPHICS_SWAPCHAIN_H_
#define _H_GRAPHICS_SWAPCHINA_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsSwapchainDesc final
{
public:
	GraphicsSwapchainDesc() noexcept;
	~GraphicsSwapchainDesc() noexcept;

	void setWindHandle(WindHandle hwnd) noexcept;
	WindHandle getWindHandle() const noexcept;

	void setWidth(std::uint32_t width) noexcept;
	std::uint32_t getWidth() const noexcept;

	void setHeight(std::uint32_t height) noexcept;
	std::uint32_t getHeight() const noexcept;

	void setSwapInterval(GraphicsSwapInterval interval) noexcept;
	GraphicsSwapInterval getSwapInterval() const noexcept;

	void setColorFormat(GraphicsFormat format) noexcept;
	GraphicsFormat getColorFormat() const noexcept;

	void setDepthStencilFormat(GraphicsFormat format) noexcept;
	GraphicsFormat getDepthStencilFormat() const noexcept;

	void setImageNums(std::uint32_t imageNums) noexcept;
	std::uint32_t getImageNums() const noexcept;

private:
	WindHandle _window;
	std::uint32_t _width;
	std::uint32_t _height;

	std::uint32_t _imageNums;

	GraphicsSwapInterval _interval;

	GraphicsFormat _format;
	GraphicsFormat _depthStencilFormat;
};

class EXPORT GraphicsSwapchain : public GraphicsChild
{
	__DeclareSubInterface(GraphicsSwapchain, GraphicsChild)
public:
	GraphicsSwapchain() noexcept;
	virtual ~GraphicsSwapchain() noexcept;

	virtual void setSwapInterval(GraphicsSwapInterval interval) noexcept = 0;
	virtual GraphicsSwapInterval getSwapInterval() const noexcept = 0;

	virtual const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept = 0;

private:
	GraphicsSwapchain(const GraphicsSwapchain&) = delete;
	GraphicsSwapchain& operator=(const GraphicsSwapchain&) = delete;
};

_NAME_END

#endif