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
#include <ray/material_manager.h>
#include <ray/material_maker.h>
#include <ray/material_param.h>
#include <ray/material_pass.h>

#include <ray/graphics_sampler.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_device.h>

#include <ray/image.h>
#include <ray/ioserver.h>

_NAME_BEGIN

MaterialManager::MaterialManager() noexcept
{
}

MaterialManager::~MaterialManager() noexcept
{
	this->close();
}

bool
MaterialManager::setup(GraphicsDevicePtr device) noexcept
{
	_graphicsDevice = device;
	return true;
}

GraphicsDeviceType
MaterialManager::getDeviceType() const noexcept
{
	return _graphicsDevice->getGraphicsDeviceDesc().getDeviceType();
}

void
MaterialManager::close() noexcept
{
	_shaders.clear();
	_samplers.clear();
	_textures.clear();
	_inputLayouts.clear();
	_materials.clear();
}

GraphicsStatePtr
MaterialManager::createRenderState(const GraphicsStateDesc& shaderDesc) noexcept
{
	return _graphicsDevice->createRenderState(shaderDesc);
}

GraphicsShaderPtr
MaterialManager::createShader(const GraphicsShaderDesc& shaderDesc) noexcept
{
	return _graphicsDevice->createShader(shaderDesc);
}

GraphicsProgramPtr
MaterialManager::createProgram(const GraphicsProgramDesc& programDesc) noexcept
{
	return _graphicsDevice->createProgram(programDesc);
}

GraphicsSamplerPtr
MaterialManager::createSampler(const std::string& name, const GraphicsSamplerDesc& samplerDesc) noexcept
{
	if (name.empty())
		return nullptr;

	auto sampler = _samplers[name];
	if (sampler)
		return nullptr;

	sampler = _graphicsDevice->createSampler(samplerDesc);
	if (!sampler)
		return nullptr;

	_samplers[name] = sampler;
	return sampler;
}

void
MaterialManager::destroySampler(GraphicsSamplerPtr sampler) noexcept
{
	assert(sampler);
	for (auto& it : _samplers)
	{
		if (it.second == sampler)
		{
			it.second = nullptr;
		}
	}
}

void
MaterialManager::destroySampler(const std::string& name) noexcept
{
	assert(name.empty());
	_samplers[name] = nullptr;
}

GraphicsSamplerPtr
MaterialManager::getSampler(const std::string& name) noexcept
{
	assert(!name.empty());
	return _samplers[name];
}

GraphicsTexturePtr
MaterialManager::createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter, GraphicsSamplerWrap warp) noexcept
{
	if (name.empty())
		return nullptr;

	auto texture = _textures[name];
	if (texture)
		return texture;

	StreamReaderPtr stream;
	if (!IoServer::instance()->openFile(stream, name))
		return nullptr;

	image::Image image;
	if (!image.load(*stream))
		return nullptr;

	auto imageFormat = image.format();
	GraphicsFormat format = GraphicsFormat::GraphicsFormatUndefined;
	if (imageFormat == image::format_t::BC1RGBUNormBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBUNormBlock;
	else if (imageFormat == image::format_t::BC1RGBAUNormBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock;
	else if (imageFormat == image::format_t::BC1RGBSRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock;
	else if (imageFormat == image::format_t::BC1RGBASRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock;
	else if (imageFormat == image::format_t::BC3UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC3UNormBlock;
	else if (imageFormat == image::format_t::BC3SRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC3SRGBBlock;
	else if (imageFormat == image::format_t::BC4UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC4UNormBlock;
	else if (imageFormat == image::format_t::BC4SNormBlock)
		format = GraphicsFormat::GraphicsFormatBC4SNormBlock;
	else if (imageFormat == image::format_t::BC5UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC5UNormBlock;
	else if (imageFormat == image::format_t::BC5SNormBlock)
		format = GraphicsFormat::GraphicsFormatBC5SNormBlock;
	else if (imageFormat == image::format_t::BC6HUFloatBlock)
		format = GraphicsFormat::GraphicsFormatBC6HUFloatBlock;
	else if (imageFormat == image::format_t::BC6HSFloatBlock)
		format = GraphicsFormat::GraphicsFormatBC6HSFloatBlock;
	else if (imageFormat == image::format_t::BC7UNormBlock)
		format = GraphicsFormat::GraphicsFormatBC7UNormBlock;
	else if (imageFormat == image::format_t::BC7SRGBBlock)
		format = GraphicsFormat::GraphicsFormatBC7SRGBBlock;
	else if (imageFormat == image::format_t::R8G8B8UNorm || imageFormat == image::format_t::R8G8B8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	else if (imageFormat == image::format_t::R8G8B8A8UNorm || imageFormat == image::format_t::R8G8B8A8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else if (imageFormat == image::format_t::B8G8R8UNorm || imageFormat == image::format_t::B8G8R8SRGB)
		format = GraphicsFormat::GraphicsFormatB8G8R8UNorm;
	else if (imageFormat == image::format_t::B8G8R8A8UNorm || imageFormat == image::format_t::B8G8R8A8SRGB)
		format = GraphicsFormat::GraphicsFormatB8G8R8A8UNorm;
	else if (imageFormat == image::format_t::R8UNorm || imageFormat == image::format_t::R8SRGB)
		format = GraphicsFormat::GraphicsFormatR8UNorm;
	else if (imageFormat == image::format_t::R8G8UNorm || imageFormat == image::format_t::R8G8SRGB)
		format = GraphicsFormat::GraphicsFormatR8G8UNorm;
	else if (imageFormat == image::format_t::R16SFloat)
		format = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (imageFormat == image::format_t::R16G16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16SFloat;
	else if (imageFormat == image::format_t::R16G16B16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16B16SFloat;
	else if (imageFormat == image::format_t::R16G16B16A16SFloat)
		format = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (imageFormat == image::format_t::R32SFloat)
		format = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (imageFormat == image::format_t::R32G32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32SFloat;
	else if (imageFormat == image::format_t::R32G32B32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32B32SFloat;
	else if (imageFormat == image::format_t::R32G32B32A32SFloat)
		format = GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;

	if (format == GraphicsFormat::GraphicsFormatUndefined)
		return nullptr;

	GraphicsTextureDesc textureDesc;
	textureDesc.setSize(image.width(), image.height(), image.depth());
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	textureDesc.setStream(image.data());
	textureDesc.setStreamSize(image.size());
	textureDesc.setMipBase(image.mipBase());
	textureDesc.setMipLevel(image.mipLevel());
	textureDesc.setLayerBase(image.layerBase());
	textureDesc.setLayerNums(image.layerLevel());
	textureDesc.setSamplerFilter(filter);
	textureDesc.setSamplerWrap(warp);

	texture = _graphicsDevice->createTexture(textureDesc);
	if (!texture)
		return nullptr;

	_textures[name] = texture;
	return texture;
}

void
MaterialManager::destroyTexture(GraphicsTexturePtr texture) noexcept
{
	assert(texture);

	for (auto& it : _textures)
	{
		if (it.second == texture)
		{
			it.second = nullptr;
		}
	}
}

void
MaterialManager::destroyTexture(const std::string& name) noexcept
{
	assert(name.empty());
	_textures[name] = nullptr;
}

GraphicsTexturePtr
MaterialManager::getTexture(const std::string& name) noexcept
{
	assert(!name.empty());
	return _textures[name];
}

GraphicsInputLayoutPtr
MaterialManager::createInputLayout(const std::string& name, GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
{
	if (name.empty())
		return nullptr;

	auto inputLayout = _inputLayouts[name];
	if (inputLayout)
		return nullptr;

	inputLayout = _graphicsDevice->createInputLayout(inputLayoutDesc);
	if (!inputLayout)
		return nullptr;

	_inputLayouts[name] = inputLayout;
	return inputLayout;
}

void
MaterialManager::destroyInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	assert(inputLayout);

	for (auto& it : _inputLayouts)
	{
		if (it.second == inputLayout)
		{
			it.second = nullptr;
		}
	}
}

void
MaterialManager::destroyInputLayout(const std::string& name) noexcept
{
	assert(name.empty());
	_inputLayouts[name] = nullptr;
}

GraphicsInputLayoutPtr
MaterialManager::getInputLayout(const std::string& name) noexcept
{
	assert(!name.empty());
	return _inputLayouts[name];
}

MaterialPtr
MaterialManager::createMaterial(const std::string& name) noexcept
{
	auto& material = _materials[name];
	if (material)
		return material->clone();

	if (!material)
	{
		MaterialMaker materialLoader;
		auto newMaterial = std::make_shared<Material>();
		if (!materialLoader.load(*this, *newMaterial, name))
			return nullptr;

		_materials[name] = newMaterial;
	}

	return material;
}

MaterialPtr
MaterialManager::getMaterial(const std::string& name) noexcept
{
	return _materials[name];
}

void
MaterialManager::destroyMaterial(MaterialPtr& material) noexcept
{
	auto it = std::find_if(_materials.begin(), _materials.end(), [material](const std::pair<std::string, MaterialPtr>& pair) { return pair.second == material; });
	if (it != _materials.end())
		_materials.erase(it);
}

void
MaterialManager::destroyMaterial(MaterialPtr&& material) noexcept
{
	auto it = std::find_if(_materials.begin(), _materials.end(), [material](const std::pair<std::string, MaterialPtr>& pair) { return pair.second == material; });
	if (it != _materials.end())
		_materials.erase(it);
}

_NAME_END