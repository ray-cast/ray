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
#ifndef _H_GRAPHICS_DEVICE_H_
#define _H_GRAPHICS_DEVICE_H_

#include <ray/graphics_types.h>

_NAME_BEGIN

class EXPORT GraphicsPixelFormatDesc final
{
public:
	GraphicsPixelFormatDesc() noexcept;
	~GraphicsPixelFormatDesc() noexcept;

	void setFormatType(GraphicsFormatType type) noexcept;
	GraphicsFormatType getFormatType() const noexcept;

	void setMinColorBits(std::uint8_t min) noexcept;
	void setMinRedBits(std::uint8_t min) noexcept;
	void setMinGreenBits(std::uint8_t min) noexcept;
	void setMinBlueBits(std::uint8_t min) noexcept;
	void setMinAlphaBits(std::uint8_t min) noexcept;
	void setMinDepthBits(std::uint8_t min) noexcept;
	void setMinStencilBits(std::uint8_t min) noexcept;

	void setMaxColorBits(std::uint8_t max) noexcept;
	void setMaxRedBits(std::uint8_t max) noexcept;
	void setMaxGreenBits(std::uint8_t max) noexcept;
	void setMaxBlueBits(std::uint8_t max) noexcept;
	void setMaxAlphaBits(std::uint8_t max) noexcept;
	void setMaxDepthBits(std::uint8_t max) noexcept;
	void setMaxStencilBits(std::uint8_t max) noexcept;

	std::uint8_t getMinColorBits() const noexcept;
	std::uint8_t getMinRedBits() const noexcept;
	std::uint8_t getMinGreenBits() const noexcept;
	std::uint8_t getMinBlueBits() const noexcept;
	std::uint8_t getMinAlphaBits() const noexcept;
	std::uint8_t getMinDepthBits() const noexcept;
	std::uint8_t getMinStencilBits() const noexcept;

	std::uint8_t getMaxColorBits() const noexcept;
	std::uint8_t getMaxRedBits() const noexcept;
	std::uint8_t getMaxGreenBits() const noexcept;
	std::uint8_t getMaxBlueBits() const noexcept;
	std::uint8_t getMaxAlphaBits() const noexcept;
	std::uint8_t getMaxDepthBits() const noexcept;
	std::uint8_t getMaxStencilBits() const noexcept;

private:
	std::uint8_t _minColorBits;
	std::uint8_t _minRedBits;
	std::uint8_t _minGreenBits;
	std::uint8_t _minBlueBits;
	std::uint8_t _minAlphaBits;
	std::uint8_t _minDepthBits;
	std::uint8_t _minStencilBits;

	std::uint8_t _maxColorBits;
	std::uint8_t _maxRedBits;
	std::uint8_t _maxGreenBits;
	std::uint8_t _maxBlueBits;
	std::uint8_t _maxAlphaBits;
	std::uint8_t _maxDepthBits;
	std::uint8_t _maxStencilBits;

	GraphicsFormatType _pixelType;
};

class EXPORT GraphicsDeviceDesc final
{
public:
	GraphicsDeviceDesc() noexcept;
	~GraphicsDeviceDesc() noexcept;

	void setDeviceType(GraphicsDeviceType type) noexcept;
	GraphicsDeviceType getDeviceType() const noexcept;

private:
	GraphicsDeviceType _deviceType;
};

class EXPORT GraphicsDevice : public rtti::Interface
{
	__DeclareSubInterface(GraphicsDevice, rtti::Interface)
public:
	GraphicsDevice() noexcept;
	virtual ~GraphicsDevice() noexcept;

	virtual GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept = 0;
	virtual GraphicsContextPtr createGraphicsContext(const GraphicsContextDesc& desc) noexcept = 0;
	virtual GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept = 0;
	virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept = 0;
	virtual GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept = 0;
	virtual GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept = 0;
	virtual GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept = 0;
	virtual GraphicsStatePtr createRenderState(const GraphicsStateDesc& desc) noexcept = 0;
	virtual GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept = 0;
	virtual GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept = 0;
	virtual GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept = 0;

	virtual GraphicsFormat findCompatibleFormat(GraphicsPixelFormatDesc& desc) noexcept = 0;

	virtual const GraphicsDeviceDesc& getGraphicsDeviceDesc() const noexcept = 0;

private:
	GraphicsDevice(const GraphicsDevice&) noexcept = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;
};

_NAME_END

#endif