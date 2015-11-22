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
#include <ray/ogl_renderer.h>
#include <ray/ogl_state.h>
#include <ray/ogl_shader.h>
#include <ray/ogl_texture.h>
#include <ray/ogl_buffer.h>
#include <ray/ogl_commandlist.h>

_NAME_BEGIN

OGLRenderer::OGLRenderer() noexcept
	: _initOpenGL(false)
	, _constantBuffers(512)
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

	_stateCaptured = std::make_shared<OGLRenderState>();
}

OGLRenderer::~OGLRenderer() noexcept
{
	this->close();
}

bool
OGLRenderer::open(RenderWindowPtr window) except
{
	if (!_initOpenGL)
	{
		this->setRenderWindow(window);

		this->initDebugControl();
		this->initStateSystem();
		this->initCommandList();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
OGLRenderer::close() noexcept
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
OGLRenderer::renderBegin() noexcept
{
	this->setShaderObject(nullptr);
	this->setRenderTexture(nullptr);
}

void
OGLRenderer::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
OGLRenderer::setViewport(const Viewport& view, std::size_t i) noexcept
{
	if (_viewport[i] != view)
	{
#if _USE_RENDER_COMMAND
		ViewportCommandNV command;
		command.header = s_nvcmdlist_header[GL_VIEWPORT_COMMAND_NV];
		command.x = view.left;
		command.y = view.top;
		command.width = view.width;
		command.height = view.height;

		_renderCommands.write(&command, sizeof(command));
#else
#	if !defined(EGLAPI)
		if (OGLFeatures::ARB_viewport_array)
			glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		else
			glViewport(view.left, view.top, view.width, view.height);
#	else
		glViewport(view.left, view.top, view.width, view.height);
#	endif
#endif
		_viewport[i] = view;
	}
}

const Viewport&
OGLRenderer::getViewport(std::size_t i) const noexcept
{
	return _viewport[i];
}

void
OGLRenderer::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool 
OGLRenderer::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

void
OGLRenderer::setRenderWindow(RenderWindowPtr glcontext) except
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
OGLRenderer::getRenderWindow() const noexcept
{
	return _glcontext;
}

void
OGLRenderer::setSwapInterval(SwapInterval interval) except
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
OGLRenderer::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

void
OGLRenderer::setRenderState(RenderStatePtr state) noexcept
{
	assert(state);

	state->apply(*_stateCaptured);

	_stateCaptured->setBlendState(state->getBlendState());
	_stateCaptured->setDepthState(state->getDepthState());
	_stateCaptured->setRasterState(state->getRasterState());
	_stateCaptured->setStencilState(state->getStencilState());

	_state = state;
}

RenderStatePtr
OGLRenderer::getRenderState() const noexcept
{
	return _state;
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
	assert(buffer);

	if (_renderBuffer != buffer)
	{
		_renderBuffer = buffer;
		_renderBuffer->apply();
	}
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
	/*assert(renderBuffer);

	auto vb = renderBuffer->getVertexBuffer();
	auto ib = renderBuffer->getIndexBuffer();

	if (vb)
	{
		auto vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
		auto vertexCount = vb->getVertexCount();
		auto vertexSize = vb->getVertexDataSize();

		glBindBuffer(GL_ARRAY_BUFFER, vb->getInstanceID());
		glBufferData(GL_ARRAY_BUFFER, vb->getVertexSize(), vb->data(), vertexUsage);
	}

	if (ib)
	{
		auto indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
		auto indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
		auto indexCount = ib->getIndexCount();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->getIndexDataSize(), ib->data(), indexUsage);
	}*/
}

void
OGLRenderer::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
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
	
	GLenum drawType = OGLTypes::asOGLVertexType(primitiveType);
	auto ib = _renderBuffer->getIndexBuffer();
	if (ib)
	{
		GLenum indexType = OGLTypes::asOGLIndexType(ib->getIndexType());

#if !defined(EGLAPI)
		if (renderable.numInstances > 0)
		{
			auto offsetIndices = _renderBuffer->getIndexBuffer()->getIndexSize() * renderable.startIndice;
			glDrawElementsInstancedBaseVertex(drawType, renderable.numIndices, indexType, (char*)nullptr + offsetIndices, renderable.numInstances, renderable.startVertice);
		}
		else
		{
			auto offsetIndices = _renderBuffer->getIndexBuffer()->getIndexSize() * renderable.startIndice;
			glDrawElementsBaseVertex(drawType, renderable.numIndices, indexType, (char*)nullptr + offsetIndices, renderable.startVertice);
		}
#else
		if (renderable.numInstances > 0)
			glDrawElements(drawType, renderable.numIndices, indexType, (char*)nullptr + renderable.startIndice);
		else
			glDrawElementsInstanced(drawType, renderable.numIndices, indexType, (char*)(nullptr) + renderable.startIndice, renderable.numInstances);
#endif
	}
	else
	{
		if (renderable.numInstances > 0)
			glDrawArraysInstanced(drawType, renderable.startVertice, renderable.numVertices, renderable.numIndices);
		else
			glDrawArrays(drawType, renderable.startVertice, renderable.numVertices);
	}
}

void 
OGLRenderer::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

RenderBufferPtr
OGLRenderer::getRenderBuffer() const noexcept
{
	return _renderBuffer;
}

void
OGLRenderer::setRenderTexture(RenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			auto framebuffer = std::dynamic_pointer_cast<OGLRenderTexture>(target)->getInstanceID();

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
}

void
OGLRenderer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(target);

	auto framebuffer = std::dynamic_pointer_cast<OGLMultiRenderTexture>(target)->getInstanceID();
	if (_multiRenderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		auto& targets = target->getRenderTextures();
		for (std::size_t index = 0; index < targets.size(); index++)
		{
			auto target = targets[index];
			this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()), index);
		}

		_renderTexture = nullptr;
		_multiRenderTexture = target;
	}
}

void
OGLRenderer::setRenderTextureLayer(RenderTexturePtr renderTexture, std::int32_t layer) noexcept
{
	assert(renderTexture);

	if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY ||
		renderTexture->getTexDim() == TextureDim::DIM_CUBE)
	{
		auto texture = std::dynamic_pointer_cast<OGLTexture>(renderTexture->getResolveTexture());
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
}

void
OGLRenderer::copyRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = std::dynamic_pointer_cast<OGLRenderTexture>(src)->getInstanceID();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

	if (dest)
	{
		auto destTarget = std::dynamic_pointer_cast<OGLRenderTexture>(dest)->getInstanceID();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
	}
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_renderTexture = GL_NONE;
	_multiRenderTexture = GL_NONE;
}

RenderTexturePtr
OGLRenderer::getRenderTexture() const noexcept
{
	return _renderTexture;
}

MultiRenderTexturePtr
OGLRenderer::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
OGLRenderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
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
}

void
OGLRenderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
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
}

void
OGLRenderer::discardRenderTexture() noexcept
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
}

void
OGLRenderer::readRenderTexture(RenderTexturePtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(target && w && h && data);

	auto framebuffer = std::dynamic_pointer_cast<OGLRenderTexture>(target)->getInstanceID();
	if (_renderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}
	
	GLenum format = OGLTypes::asOGLFormat(pfd);
	GLenum type = OGLTypes::asOGLType(pfd);

	glReadPixels(0, 0, w, h, format, type, data);
}

void
OGLRenderer::setShaderObject(ShaderObjectPtr shader) noexcept
{
	if (shader)
	{
		auto program = std::dynamic_pointer_cast<OGLShaderObject>(shader)->getInstanceID();
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
}

ShaderObjectPtr
OGLRenderer::getShaderObject() const noexcept
{
	return _shaderObject;
}

bool
OGLRenderer::createShaderVariant(ShaderVariant& constant) noexcept
{
	if (constant.getType() != ShaderVariantType::SPT_BUFFER)
		return true;

	OGLShaderVariant buffer;

	glGenBuffers(1, &buffer.ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer.ubo);
	glBufferData(GL_UNIFORM_BUFFER, constant.getSize(), 0, GL_DYNAMIC_DRAW);

#if !defined(EGLAPI)
	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		glGetNamedBufferParameterui64vNV(buffer.ubo, GL_BUFFER_GPU_ADDRESS_NV, &buffer.bindlessUbo);
		glMakeNamedBufferResidentNV(buffer.ubo, GL_READ_ONLY);
	}
#endif

	return true;
}

void
OGLRenderer::destroyShaderVariant(ShaderVariant& constant) noexcept
{
}

void
OGLRenderer::updateShaderVariant(ShaderVariantPtr constant) noexcept
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
	if (OGLFeatures::ARB_direct_state_access)
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
OGLRenderer::setShaderUniform(ShaderUniformPtr uniform, TexturePtr texture, TextureSamplePtr sample) noexcept
{
	assert(uniform && texture);
	assert(uniform->getBindingPoint() < _maxTextureUnits);

	auto _texture = std::dynamic_pointer_cast<OGLTexture>(texture);
	auto _sample = std::dynamic_pointer_cast<OGLTextureSample>(texture);

	auto location = uniform->getLocation();
	auto program = uniform->getBindingProgram();
	auto unit = uniform->getBindingPoint();
	auto target = OGLTypes::asOGLTarget(texture->getTexDim());
	auto textureAddr = _texture->getInstanceAddr();
	auto textureID = _texture->getInstanceID();

#if !defined(EGLAPI)
	if (OGLFeatures::ARB_bindless_texture)
	{
		glProgramUniformHandleui64ARB(program, location, textureAddr);
	}
	else if (OGLFeatures::ARB_direct_state_access)
	{
		glProgramUniform1i(program, location, unit);
		glBindTextureUnit(GL_TEXTURE0 + unit, textureID);
	}
	else
#endif
	{
		glProgramUniform1i(program, location, unit);

		if (_textureUnits[unit] != textureID)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(target, textureID);

			_textureUnits[unit] = textureID;
		}
	}

	if (sample)
	{
		glBindSampler(GL_TEXTURE0 + unit, _sample->getInstanceID());
	}
}

void
OGLRenderer::setShaderUniform(ShaderUniformPtr uniform, ShaderVariantPtr constant) noexcept
{
	assert(constant && uniform);
	assert(uniform->getValue());

	auto type = uniform->getValue()->getType();
	auto location = uniform->getLocation();
	auto program = uniform->getBindingProgram();

	if (constant->isSemantic())
	{
		uniform->needUpdate(true);
	}

	switch (type)
	{
	case ShaderVariantType::SPT_BOOL:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform1i(program, location, uniform->getValue()->getBool());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_INT:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform1i(program, location, uniform->getValue()->getInt());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_INT2:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform2iv(program, location, 1, uniform->getValue()->getInt2().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform1f(program, location, uniform->getValue()->getFloat());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT2:
	{
		//if (uniform->needUpdate())
		{
			glProgramUniform2fv(program, location, 1, uniform->getValue()->getFloat2().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT3:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform3fv(program, location, 1, uniform->getValue()->getFloat3().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT4:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform4fv(program, location, 1, uniform->getValue()->getFloat4().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT3X3:
	{
		if (uniform->needUpdate())
		{
			glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat3x3().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT4X4:
	{
		if (uniform->needUpdate())
		{
			glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, uniform->getValue()->getFloat4x4().ptr());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT_ARRAY:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform1fv(program, location, uniform->getValue()->getFloatArray().size(), uniform->getValue()->getFloatArray().data());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_FLOAT2_ARRAY:
	{
		if (uniform->needUpdate())
		{
			glProgramUniform2fv(program, location, uniform->getValue()->getFloat2Array().size(), (GLfloat*)uniform->getValue()->getFloat2Array().data());
			uniform->needUpdate(false);
		}
		break;
	}
	case ShaderVariantType::SPT_TEXTURE:
	{
		if (uniform->needUpdate())
		{
			auto texture = uniform->getValue()->getTexture();
			auto sample = uniform->getValue()->getTextureSample();
			if (texture)
			{
				this->setShaderUniform(uniform, texture, sample);
#if !defined(EGLAPI)
				if (OGLFeatures::ARB_bindless_texture)
				{
					uniform->needUpdate(false);
				}
#endif
			}
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
			if (OGLFeatures::NV_vertex_buffer_unified_memory)
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
}

void
OGLRenderer::checkError() noexcept
{
#ifdef _DEBUG
	switch (glGetError())
	{
	case GL_NO_ERROR:
		break;
	case GL_INVALID_ENUM:
		std::cerr << "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		std::cerr << "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		std::cerr << "GL_INVALID_OPERATION";
		break;
#if !defined(EGLAPI)
	case GL_STACK_OVERFLOW:
		std::cerr << "GL_STACK_OVERFLOW";
		break;
#endif
	case GL_OUT_OF_MEMORY:
		std::cerr << "GL_OUT_OF_MEMORY";
		break;
	};
#endif
}

void
OGLRenderer::checkFramebuffer() noexcept
{
#if defined(_DEBUG)
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != result)
	{
		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cerr << "FBO:Incomplete attachment";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cerr << "FBO:Incomplete missing attachment";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cerr << "FBO:Unsupported";
			break;
#if !defined(EGLAPI)
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cerr << "FBO:Incomplete draw buffer";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cerr << "FBO:Incomplete read buffer";
			break;
#endif
		}
		assert(false);
	}
#endif
}

void
OGLRenderer::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
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
OGLRenderer::initDebugControl() noexcept
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

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugCallBack, this);
	
	// enable all
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	// disable ids
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);
#endif
}

void
OGLRenderer::initCommandList() noexcept
{
#if defined(_BUILD_OPENGL)
	initCommandListNV();
#endif
}

void
OGLRenderer::initStateSystem() noexcept
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

#if !defined(EGLAPI)
	if (OGLFeatures::ARB_provoking_vertex)
	{
		glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
	}

	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		glGenVertexArrays(1, &_defaultVAO);
		glBindVertexArray(_defaultVAO);

		glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
		glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
		glEnableClientState(GL_UNIFORM_BUFFER_UNIFIED_NV);

		glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
		glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
		glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);

		glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0);

		glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, (GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
		glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, (GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
		glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, (GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
	}
#endif	
}

_NAME_END