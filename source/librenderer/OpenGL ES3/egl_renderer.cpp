// +---------------------------------------------------------------------
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
#include "egl_renderer.h"
#include "egl_state.h"
#include "egl_shader.h"
#include "egl_texture.h"
#include "egl_buffer.h"

_NAME_BEGIN

EGL3Renderer::EGL3Renderer() noexcept
	: _initOpenGL(false)
	, _maxTextureUnits(32)
	, _maxViewports(4)
	, _clearColor(0.0, 0.0, 0.0)
	, _clearDepth(0.0)
	, _clearStencil(0xFFFFFFFF)
	, _state(GL_NONE)
	, _renderTexture(GL_NONE)
	, _defaultVAO(GL_NONE)
	, _enableWireframe(false)
{
	_textureUnits.resize(_maxTextureUnits);
	_viewport.resize(_maxViewports);

	_stateCaptured = std::make_shared<EGL3RenderState>();
}

EGL3Renderer::~EGL3Renderer() noexcept
{
	this->close();
}

bool
EGL3Renderer::open(WindHandle window) except
{
	if (!_initOpenGL)
	{
		_glcontext = this->createRenderWindow();
		_glcontext->open(window);

		this->initDebugControl();
		this->initStateSystem();
		this->initCommandList();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
EGL3Renderer::close() noexcept
{
	if (_renderBuffer)
	{
		_renderBuffer.reset();
		_renderBuffer = nullptr;
	}

	if (_shaderObject)
	{
		_shaderObject.reset();
		_shaderObject = nullptr;
	}

	if (_renderTexture)
	{
		_renderTexture.reset();
		_renderTexture = nullptr;
	}

	if (_multiRenderTexture)
	{
		_multiRenderTexture.reset();
		_multiRenderTexture = nullptr;
	}

	if (_state)
	{
		_state.reset();
		_state = nullptr;
	}

	if (_defaultVAO != GL_NONE)
	{
		glDeleteVertexArrays(1, &_defaultVAO);
		_defaultVAO = GL_NONE;
	}

	if (_glcontext)
	{
		_glcontext.reset();
		_glcontext = nullptr;
	}
}

void
EGL3Renderer::renderBegin() noexcept
{
	this->setShaderObject(nullptr);
	this->setRenderTexture(nullptr);
	this->setRenderBuffer(nullptr);
}

void
EGL3Renderer::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
EGL3Renderer::setViewport(const Viewport& view, std::size_t i) noexcept
{
	EGL3Check::checkError();

	if (_viewport[i] != view)
	{
#	if !defined(EGLAPI)
		if (EGL3Features::ARB_viewport_array)
			glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		else
			glViewport(view.left, view.top, view.width, view.height);
#	else
		glViewport(view.left, view.top, view.width, view.height);
#	endif
		_viewport[i] = view;
	}
	
	EGL3Check::checkError();
}

const Viewport&
EGL3Renderer::getViewport(std::size_t i) const noexcept
{
	return _viewport[i];
}

void
EGL3Renderer::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool 
EGL3Renderer::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

RenderWindowPtr 
EGL3Renderer::createRenderWindow() const noexcept
{
	return std::make_shared<EGLCanvas>();
}

void
EGL3Renderer::setRenderWindow(RenderWindowPtr glcontext) except
{
	assert(glcontext);

	if (_glcontext != glcontext)
	{
		if (_glcontext)
			_glcontext->setActive(false);

		_glcontext = glcontext;

		if (_glcontext)
			_glcontext->setActive(true);
	}
}

RenderWindowPtr
EGL3Renderer::getRenderWindow() const noexcept
{
	return _glcontext;
}

void
EGL3Renderer::setSwapInterval(SwapInterval interval) except
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
EGL3Renderer::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

RenderStatePtr 
EGL3Renderer::createRenderState() noexcept
{
	return std::make_shared<EGL3RenderState>();
}

void
EGL3Renderer::setRenderState(RenderStatePtr state) noexcept
{
	assert(state);

	state->apply(*_stateCaptured);

	_stateCaptured->setBlendState(state->getBlendState());
	_stateCaptured->setDepthState(state->getDepthState());
	_stateCaptured->setRasterState(state->getRasterState());
	_stateCaptured->setStencilState(state->getStencilState());

	_state = state;

	EGL3Check::checkError();
}

RenderStatePtr
EGL3Renderer::getRenderState() const noexcept
{
	return _state;
}

RenderBufferPtr
EGL3Renderer::createRenderBuffer() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3RenderBuffer>();
	EGL3Check::checkError();
	return result;
}

IndexBufferDataPtr 
EGL3Renderer::createIndexBufferData() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3IndexBuffer>();
	EGL3Check::checkError();
	return result;
}

VertexBufferDataPtr 
EGL3Renderer::createVertexBufferData() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3VertexBuffer>();
	EGL3Check::checkError();
	return result;
}

void
EGL3Renderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
	if (_renderBuffer != buffer)
	{
		if (buffer)
			buffer->apply();
		_renderBuffer = buffer;
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
	/*assert(renderBuffer);

	auto vb = renderBuffer->getVertexBuffer();
	auto ib = renderBuffer->getIndexBuffer();

	if (vb)
	{
		auto vertexUsage = EGL3Types::asEGL3VertexUsage(vb->getVertexUsage());
		auto vertexCount = vb->getVertexCount();
		auto vertexSize = vb->getVertexDataSize();

		glBindBuffer(GL_ARRAY_BUFFER, vb->getInstanceID());
		glBufferData(GL_ARRAY_BUFFER, vb->getVertexSize(), vb->data(), vertexUsage);
	}

	if (ib)
	{
		auto indexType = EGL3Types::asEGL3IndexType(ib->getIndexType());
		auto indexUsage = EGL3Types::asEGL3VertexUsage(ib->getIndexUsage());
		auto indexCount = ib->getIndexCount();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->getIndexDataSize(), ib->data(), indexUsage);
	}*/
}

void
EGL3Renderer::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	EGL3Check::checkError();

	assert(_renderBuffer && _stateCaptured);
	assert(_renderBuffer->getNumVertices() >= renderable.startVertice + renderable.numVertices);
	assert(_renderBuffer->getNumIndices() >= renderable.startIndice + renderable.numIndices);

	auto primitiveType = _stateCaptured->getRasterState().primitiveType;

	if (_enableWireframe)
	{
		if (primitiveType == GPU_POINT_OR_LINE ||
			primitiveType == GPU_TRIANGLE_OR_LINE ||
			primitiveType == GPU_FAN_OR_LINE)
		{
			primitiveType = GPU_LINE;
		}
	}
	
	GLenum drawType = EGL3Types::asEGL3VertexType(primitiveType);
	auto ib = _renderBuffer->getIndexBuffer();
	if (ib)
	{
		GLenum indexType = EGL3Types::asEGL3IndexType(ib->getIndexType());

#if !defined(EGLAPI)
		if (renderable.numInstances > 0)
		{
			auto offsetIndices = _renderBuffer->getIndexBuffer()->getIndexSize() * renderable.startIndice;
			glDrawElementsInstancedBaseVertexBaseInstance(drawType, renderable.numIndices, indexType, (char*)nullptr + offsetIndices, renderable.numInstances, renderable.startVertice, renderable.startInstances);
		}
		else
		{
			auto offsetIndices = _renderBuffer->getIndexBuffer()->getIndexSize() * renderable.startIndice;
			glDrawElementsBaseVertex(drawType, renderable.numIndices, indexType, (char*)nullptr + offsetIndices, renderable.startVertice);
		}
#else
		if (renderable.numInstances > 0)
			glDrawElementsInstanced(drawType, renderable.numIndices, indexType, (char*)(nullptr) + renderable.startIndice, renderable.numInstances);
		else
			glDrawElements(drawType, renderable.numIndices, indexType, (char*)nullptr + renderable.startIndice);
#endif
	}
	else
	{
		if (renderable.numInstances > 0)
			glDrawArraysInstanced(drawType, renderable.startVertice, renderable.numVertices, renderable.numIndices);
		else
			glDrawArrays(drawType, renderable.startVertice, renderable.numVertices);
	}

	EGL3Check::checkError();
}

void 
EGL3Renderer::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

RenderBufferPtr
EGL3Renderer::getRenderBuffer() const noexcept
{
	return _renderBuffer;
}

TexturePtr 
EGL3Renderer::createTexture() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3Texture>();
	EGL3Check::checkError();
	return result;
}

RenderTexturePtr 
EGL3Renderer::createRenderTexture() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3RenderTexture>();
	EGL3Check::checkError();
	return result;
}

MultiRenderTexturePtr 
EGL3Renderer::createMultiRenderTexture() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3MultiRenderTexture>();
	EGL3Check::checkError();
	return result;
}

void
EGL3Renderer::setRenderTexture(RenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			auto framebuffer = std::dynamic_pointer_cast<EGL3RenderTexture>(target)->getInstanceID();

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()));
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		}

		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(target);

	auto framebuffer = std::dynamic_pointer_cast<EGL3MultiRenderTexture>(target)->getInstanceID();
	if (_multiRenderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		auto& renderTextures = target->getRenderTextures();
		std::size_t size = renderTextures.size();
		for (std::size_t index = 0; index < size; index++)
		{
			auto renderTexture = renderTextures[index];
			this->setViewport(Viewport(0, 0, renderTexture->getWidth(), renderTexture->getHeight()), index);
		}

		_renderTexture = nullptr;
		_multiRenderTexture = target;
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::setRenderTextureLayer(RenderTexturePtr renderTexture, std::int32_t layer) noexcept
{
	assert(renderTexture);

	if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY ||
		renderTexture->getTexDim() == TextureDim::DIM_CUBE)
	{
		auto texture = std::dynamic_pointer_cast<EGL3Texture>(renderTexture->getResolveTexture());
		auto textureID = texture->getInstanceID();

		GLenum attachment = GL_COLOR_ATTACHMENT0;

		if (_multiRenderTexture)
		{
			for (auto& it : _multiRenderTexture->getRenderTextures())
			{
				if (it == renderTexture)
					break;
				attachment++;
			}
		}
		else if (_renderTexture != renderTexture)
		{
			this->setRenderTexture(renderTexture);
		}

		if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY)
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, 0, layer);
		else if (renderTexture->getTexDim() == TextureDim::DIM_CUBE)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0);
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = std::dynamic_pointer_cast<EGL3RenderTexture>(src)->getInstanceID();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

	if (dest)
	{
		auto destTarget = std::dynamic_pointer_cast<EGL3RenderTexture>(dest)->getInstanceID();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
	}
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_renderTexture = GL_NONE;
	_multiRenderTexture = GL_NONE;

	EGL3Check::checkError();
}

RenderTexturePtr
EGL3Renderer::getRenderTexture() const noexcept
{
	return _renderTexture;
}

MultiRenderTexturePtr
EGL3Renderer::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
EGL3Renderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	GLbitfield mode = 0;

	if (flags & ClearFlags::CLEAR_COLOR)
	{
		mode |= GL_COLOR_BUFFER_BIT;

		if (_clearColor != color)
		{
			glClearColor(color.x, color.y, color.z, color.w);
			_clearColor = color;
		}
	}

	if (flags & ClearFlags::CLEAR_DEPTH)
	{
		mode |= GL_DEPTH_BUFFER_BIT;

		if (_clearDepth != depth)
		{
			glClearDepthf(depth);
			_clearDepth = depth;
		}
	}

	if (flags & ClearFlags::CLEAR_STENCIL)
	{
		mode |= GL_STENCIL_BUFFER_BIT;

		if (_clearStencil != stencil)
		{
			glClearStencil(stencil);
			_clearStencil = stencil;
		}
	}

	if (mode != 0)
	{
		auto depthWriteMask = _stateCaptured->getDepthState().depthWriteMask;
		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_TRUE);
		}

		glClear(mode);

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	if (flags & ClearFlags::CLEAR_DEPTH)
	{
		auto depthWriteMask = _stateCaptured->getDepthState().depthWriteMask;
		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		glClearBufferfv(GL_DEPTH, 0, &f);

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & ClearFlags::CLEAR_STENCIL)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, 0, &s);
	}

	if (flags & ClearFlags::CLEAR_COLOR)
	{
		glClearBufferfv(GL_COLOR, i, color.ptr());
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::discardRenderTexture() noexcept
{
	assert(_renderTexture || _multiRenderTexture);

	if (_renderTexture)
	{
		GLenum attachment = GL_COLOR_ATTACHMENT0;
		glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachment);
	}
	else if (_multiRenderTexture)
	{
		GLenum attachments[24];
		GLenum attachment = GL_COLOR_ATTACHMENT0;

		auto& targets = _multiRenderTexture->getRenderTextures();
		auto size = targets.size();

		for (std::size_t i = 0; i < size; i++)
		{
			attachments[i] = attachment;
			attachment++;
		}

		glInvalidateFramebuffer(GL_FRAMEBUFFER, size, attachments);
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(target && w && h && data);

	auto framebuffer = std::dynamic_pointer_cast<EGL3RenderTexture>(target)->getInstanceID();
	if (_renderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}
	
	GLenum format = EGL3Types::asEGL3Format(pfd);
	GLenum type = EGL3Types::asEGL3Type(pfd);

	glReadPixels(0, 0, w, h, format, type, data);

	EGL3Check::checkError();
}

ShaderPtr 
EGL3Renderer::createShader() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3Shader>();
	EGL3Check::checkError();
	return result;
}

ShaderObjectPtr 
EGL3Renderer::createShaderObject() noexcept
{
	EGL3Check::checkError();
	auto result = std::make_shared<EGL3ShaderObject>();
	EGL3Check::checkError();
	return result;
}

void
EGL3Renderer::setShaderObject(ShaderObjectPtr shader) noexcept
{
	if (shader)
	{
		auto program = std::dynamic_pointer_cast<EGL3ShaderObject>(shader)->getInstanceID();
		if (_shaderObject != shader)
		{
			glUseProgram(program);
			_shaderObject = shader;
		}

		for (auto& it : shader->getActiveUniforms())
		{
			this->setShaderUniform(it, it->getValue());
		}
	}
	else
	{
		glUseProgram(GL_NONE);
		_shaderObject = nullptr;
	}

	EGL3Check::checkError();
}

ShaderObjectPtr
EGL3Renderer::getShaderObject() const noexcept
{
	return _shaderObject;
}

bool
EGL3Renderer::createShaderVariant(ShaderVariant& constant) noexcept
{
	if (constant.getType() != ShaderVariantType::SPT_BUFFER)
		return true;
	return true;
}

void
EGL3Renderer::destroyShaderVariant(ShaderVariant& constant) noexcept
{
}

void
EGL3Renderer::updateShaderVariant(ShaderVariantPtr constant) noexcept
{
	std::vector<char> _data;
	_data.resize(constant->getSize());

	std::size_t offset = 0;

	for (auto& it : constant->getParameters())
	{
		switch (it->getType())
		{
		case SPT_BOOL:
		{
			auto value = it->getBool();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_INT:
		{
			auto value = it->getInt();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_INT2:
		{
			auto value = it->getInt2();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT:
		{
			auto value = it->getFloat();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT2:
		{
			auto value = it->getFloat2();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT3:
		{
			auto value = it->getFloat3();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT4:
		{
			auto value = it->getFloat4();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT3X3:
		{
			auto value = it->getFloat3x3();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		case SPT_FLOAT4X4:
		{
			auto value = it->getFloat4x4();
			std::memcpy(&_data[offset], &value, it->getSize());
		}
		break;
		default:
			assert(false);
		}

		offset += it->getSize();
	}

	/*auto& buffer = _constantBuffers[constant->getInstanceID()];

#if !defined(EGLAPI)
	if (EGL3Features::ARB_direct_state_access)
	{
		glNamedBufferSubDataEXT(buffer.ubo, 0, _data.size(), _data.data());
	}
	else
#endif
	{
		glBindBuffer(GL_UNIFORM_BUFFER, buffer.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, _data.size(), _data.data());
	}*/
}

void
EGL3Renderer::setShaderUniform(ShaderUniformPtr uniform, TexturePtr texture, TextureSamplePtr sample) noexcept
{
	assert(uniform && texture);
	assert(uniform->getBindingPoint() < _maxTextureUnits);

	auto _texture = std::dynamic_pointer_cast<EGL3Texture>(texture);
	auto _sample = std::dynamic_pointer_cast<EGL3TextureSample>(texture);

	auto location = uniform->getLocation();
	auto program = uniform->getBindingProgram();
	auto unit = uniform->getBindingPoint();
	auto target = EGL3Types::asEGL3Target(texture->getTexDim());
	auto textureID = _texture->getInstanceID();

	glProgramUniform1i(program, location, unit);

	if (_textureUnits[unit] != textureID)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(target, textureID);

		_textureUnits[unit] = textureID;
	}

	if (sample)
	{
		glBindSampler(GL_TEXTURE0 + unit, _sample->getInstanceID());
	}
}

void
EGL3Renderer::setShaderUniform(ShaderUniformPtr uniform, ShaderVariantPtr constant) noexcept
{
	assert(constant && uniform);
	assert(uniform->getValue());

	auto type = uniform->getValue()->getType();
	if (type != ShaderVariantType::SPT_TEXTURE)
	{
		if (!uniform->needUpdate())
			return;
		uniform->needUpdate(false);
	}

	auto location = uniform->getLocation();
	auto program = uniform->getBindingProgram();

	switch (type)
	{
	case ShaderVariantType::SPT_BOOL:
	{
		glProgramUniform1i(program, location, uniform->getValue()->getBool());
		break;
	}
	case ShaderVariantType::SPT_INT:
	{
		glProgramUniform1i(program, location, uniform->getValue()->getInt());
		break;
	}
	case ShaderVariantType::SPT_INT2:
	{
		glProgramUniform2iv(program, location, 1, uniform->getValue()->getInt2().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT:
	{
		glProgramUniform1f(program, location, uniform->getValue()->getFloat());
		break;
	}
	case ShaderVariantType::SPT_FLOAT2:
	{
		glProgramUniform2fv(program, location, 1, uniform->getValue()->getFloat2().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT3:
	{
		glProgramUniform3fv(program, location, 1, uniform->getValue()->getFloat3().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT4:
	{
		glProgramUniform4fv(program, location, 1, uniform->getValue()->getFloat4().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT3X3:
	{
		glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat3x3().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT4X4:
	{
		glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat4x4().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT_ARRAY:
	{
		glProgramUniform1fv(program, location, uniform->getValue()->getFloatArray().size(), uniform->getValue()->getFloatArray().data());
		break;
	}
	case ShaderVariantType::SPT_FLOAT2_ARRAY:
	{
		glProgramUniform2fv(program, location, uniform->getValue()->getFloat2Array().size(), (GLfloat*)uniform->getValue()->getFloat2Array().data());
		break;
	}
	case ShaderVariantType::SPT_TEXTURE:
	{
		auto texture = uniform->getValue()->getTexture();
		auto sample = uniform->getValue()->getTextureSample();
		if (texture)
		{
			this->setShaderUniform(uniform, texture, sample);
		}
		break;
	}
	/*case ShaderVariantType::SPT_BUFFER:
	{
		if (uniform->needUpdate())
		{
			this->updateShaderVariant(constant);
			uniform->needUpdate(false);
		}

		auto index = constant->getInstanceID();
		if (index != 0)
		{
			auto& buffer = _constantBuffers[index];

#if !defined(EGLAPI)
			if (EGL3Features::NV_vertex_buffer_unified_memory)
			{
				glBindBufferRange(GL_UNIFORM_BUFFER, location, buffer.ubo, 0, constant->getSize());
			}
			else
#endif
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, location, buffer.ubo);
			}
		}
	}*/
	default:
		assert(false);
		break;
	}

	EGL3Check::checkError();
}

void
EGL3Renderer::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
	std::cerr << "source : ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		std::cerr << "GL_DEBUG_SOURCE_API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cerr << "GL_DEBUG_SOURCE_OTHER";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "type : ";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << "GL_DEBUG_TYPE_ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cerr << "GL_DEBUG_TYPE_OTHER";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cerr << "GL_DEBUG_TYPE_MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "id : " << id << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cerr << "notice";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "low";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "medium";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cerr << "high";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "message : " << message << std::endl;
}

void
EGL3Renderer::initDebugControl() noexcept
{
#if defined(_DEBUG) && !defined(__ANDROID__)
	// 131184 memory info
	// 131185 memory allocation info
	GLuint ids[] =
	{
		131076,
		131169,
		131184,
		131185,
		131218,
		131204
	};

	GL_CHECK(glEnable(GL_DEBUG_OUTPUT));
#if !defined(EGLAPI)
	GL_CHECK(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
	GL_CHECK(glDebugMessageCallback(debugCallBack, this));
	// enable all
	GL_CHECK(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE));
	// disable ids
	GL_CHECK(glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE));
#endif
}

void
EGL3Renderer::initCommandList() noexcept
{
#if defined(_BUILD_OPENGL)
	initCommandListNV();
#endif
}

void
EGL3Renderer::initStateSystem() noexcept
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &_defaultVAO);
	glBindVertexArray(_defaultVAO);

	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE);

	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0);

	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, (GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, (GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, (GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE, (GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE);
}

_NAME_END