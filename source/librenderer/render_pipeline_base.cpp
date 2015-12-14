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
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_manager.h>
#include <ray/render_post_process.h>
#include <ray/render_device.h>
#include <ray/render_scene.h>
#include <ray/render_window.h>
#include <ray/render_texture.h>
#include <ray/render_object.h>
#include <ray/material.h>
#include <ray/material_manager.h>
#include <ray/model.h>
#include <ray/image.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>
#include <ray/resource.h>

#if defined(_BUILD_OPENGL) || defined(_BUILD_OPENGL_CORE)
#	include "OpenGL Core/ogl_renderer.h"
#	define RenderDevice OGLRenderer
#elif defined(_BUILD_OPENGL) || defined(_BUILD_OPENGL_ES3)
#	include "OpenGL ES3/egl3_renderer.h"
#	define RenderDevice EGL3Renderer
#elif defined(_BUILD_OPENGL) || defined(_BUILD_OPENGL_ES2)
#	include "OpenGL ES2/egl2_renderer.h"
#	define RenderDevice EGL2Renderer
#endif

_NAME_BEGIN

RenderPipeline::RenderPipeline() noexcept
{
}

RenderPipeline::~RenderPipeline() noexcept
{
	this->close();
}

void
RenderPipeline::open(WindHandle window, std::size_t w, std::size_t h) except
{
	_renderDevice = std::make_shared<RenderDevice>();
	_renderDevice->open(window);

	_renderWindow = _renderDevice->getRenderWindow();

	_materialManager = std::make_shared<MaterialManager>();
	_pipelineManager = std::make_shared<DefaultRenderPipelineManager>();

	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_renderSceneQuad = this->createRenderBuffer(mesh);

	mesh.makeSphere(1, 16, 12);

	_renderSphere = this->createRenderBuffer(mesh);

	mesh.makeCone(1, 1, 16);

	_renderCone = this->createRenderBuffer(mesh);
	
	this->setWindowResolution(w, h);

	this->onActivate();
}

void
RenderPipeline::close() noexcept
{
	this->onDeactivate();

	if (_renderSceneQuad)
	{
		_renderSceneQuad.reset();
		_renderSceneQuad = nullptr;
	}

	if (_renderSphere)
	{
		_renderSphere.reset();
		_renderSphere = nullptr;
	}

	if (_renderCone)
	{
		_renderCone.reset();
		_renderCone = nullptr;
	}

	for (auto& postprocess : _postprocessors)
	{
		for (auto& it : postprocess)
			it->setActive(false);

		postprocess.clear();
	}

	if (_pipelineManager)
	{
		_pipelineManager.reset();
		_pipelineManager = nullptr;
	}

	if (_materialManager)
	{
		_materialManager->close();
		_materialManager = nullptr;
	}

	if (_renderDevice)
	{
		_renderDevice.reset();
		_renderDevice = nullptr;
	}
}

void 
RenderPipeline::setRenderPipelineManager(RenderPipelineManagerPtr manager) noexcept
{
	_pipelineManager = manager;
}

RenderPipelineManagerPtr 
RenderPipeline::getRenderPipelineManagerPtr() const noexcept
{
	return _pipelineManager;
}

void
RenderPipeline::setWireframeMode(bool enable) noexcept
{
	assert(_renderDevice);
	_renderDevice->setWireframeMode(enable);
}

bool
RenderPipeline::getWireframeMode() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getWireframeMode();
}

void
RenderPipeline::setSwapInterval(SwapInterval interval) except
{
	assert(_renderDevice);
	_renderDevice->setSwapInterval(interval);
}

SwapInterval
RenderPipeline::getSwapInterval() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getSwapInterval();
}

void
RenderPipeline::setCamera(CameraPtr camera) noexcept
{
	if (camera->getCameraOrder() != CameraOrder::CO_SHADOW)
	{
		if (camera->getCameraType() == CameraType::CT_PERSPECTIVE)
		{
			float ratio;
			float aperture;
			float znear;
			float zfar;
			camera->getPerspective(aperture, ratio, znear, zfar);

			std::uint32_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makePerspective(aperture, znear, zfar, ratio);
			}
		}
		else
		{
			float left;
			float right;
			float top;
			float bottom;
			float ratio;
			float znear;
			float zfar;
			camera->getOrtho(left, right, top, bottom, ratio, znear, zfar);

			std::uint32_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makeOrtho(left, right, top, bottom, znear, zfar, ratio);
			}
		}
	}

	float ratio;
	float aperture;
	float znear;
	float zfar;
	camera->getPerspective(aperture, ratio, znear, zfar);

	_materialManager->setFloatParam(MaterialSemantic::CameraNear, znear);
	_materialManager->setFloatParam(MaterialSemantic::CameraFar, zfar);
	_materialManager->setFloatParam(MaterialSemantic::CameraAperture, aperture);
	_materialManager->setFloat3Param(MaterialSemantic::CameraView, camera->getLookAt());
	_materialManager->setFloat3Param(MaterialSemantic::CameraPosition, camera->getTranslate());
	_materialManager->setFloat3Param(MaterialSemantic::CameraDirection, camera->getLookAt() - camera->getTranslate());
	_materialManager->setMatrixParam(MaterialSemantic::matView, camera->getView());
	_materialManager->setMatrixParam(MaterialSemantic::matViewInverse, camera->getViewInverse());
	_materialManager->setMatrixParam(MaterialSemantic::matViewInverseTranspose, camera->getViewInverseTranspose());
	_materialManager->setMatrixParam(MaterialSemantic::matProject, camera->getProject());
	_materialManager->setMatrixParam(MaterialSemantic::matProjectInverse, camera->getProjectInverse());
	_materialManager->setMatrixParam(MaterialSemantic::matViewProject, camera->getViewProject());
	_materialManager->setMatrixParam(MaterialSemantic::matViewProjectInverse, camera->getViewProjectInverse());

	_pipelineManager->assginVisiable(camera);

	_camera = camera;
}

CameraPtr
RenderPipeline::getCamera() const noexcept
{
	return _camera;
}

void
RenderPipeline::setViewport(const Viewport& view) noexcept
{
	_renderDevice->setViewport(view);
}

const Viewport& 
RenderPipeline::getViewport() const noexcept
{
	return _renderDevice->getViewport();
}

void
RenderPipeline::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	_pipelineManager->addRenderData(queue, pass, object);
}

RenderObjects&
RenderPipeline::getRenderData(RenderQueue queue, RenderPass pass) noexcept
{
	return _pipelineManager->getRenderData(queue, pass);
}

void
RenderPipeline::drawMesh(MaterialPassPtr pass, RenderBufferPtr buffer, const RenderIndirect& renderable) noexcept
{
	this->setMaterialPass(pass);
	this->setRenderBuffer(buffer);
	this->drawRenderBuffer(renderable);
}

RenderTexturePtr 
RenderPipeline::createRenderTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderTexture();
}

MultiRenderTexturePtr 
RenderPipeline::createMultiRenderTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createMultiRenderTexture();
}

void
RenderPipeline::setRenderTexture(RenderTexturePtr target) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderTexture(target);
}

void
RenderPipeline::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(_renderDevice);
	_renderDevice->setMultiRenderTexture(target);
}

void
RenderPipeline::setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderTextureLayer(target, layer);
}

void
RenderPipeline::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_renderDevice);
	_renderDevice->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipeline::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_renderDevice);
	_renderDevice->clearRenderTexture(flags, color, depth, stencil, i);
}

void
RenderPipeline::discradRenderTexture() noexcept
{
	assert(_renderDevice);
	_renderDevice->discardRenderTexture();
}

void
RenderPipeline::readRenderTexture(RenderTexturePtr texture, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_renderDevice);
	_renderDevice->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipeline::blitRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
	assert(_renderDevice);
	_renderDevice->blitRenderTexture(srcTarget, src, destTarget, dest);
}

RenderStatePtr
RenderPipeline::createRenderState() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderState();
}

void
RenderPipeline::setRenderState(RenderStatePtr state) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderState(state);
}

RenderStatePtr
RenderPipeline::getRenderState() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getRenderState();
}

ShaderPtr
RenderPipeline::createShader() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createShader();
}

ShaderObjectPtr 
RenderPipeline::createShaderObject() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createShaderObject();
}

void 
RenderPipeline::setShaderObject(ShaderObjectPtr progaram) noexcept
{
	assert(_renderDevice);
	return _renderDevice->setShaderObject(progaram);
}

ShaderObjectPtr 
RenderPipeline::getShaderObject() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getShaderObject();
}

TexturePtr 
RenderPipeline::createTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createTexture();
}

TexturePtr 
RenderPipeline::createTexture(const std::string& name) except
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, name))
	{
		Image image;
		if (image.load(*stream))
		{
			TextureFormat format = TextureFormat::R8G8B8A8;

			if (image.getImageType() == ImageType::dds1)
				format = TextureFormat::RGBA_DXT1;
			else if (image.getImageType() == ImageType::dds3)
				format = TextureFormat::RGBA_DXT3;
			else if (image.getImageType() == ImageType::dds5)
				format = TextureFormat::RGBA_DXT5;
			else if (image.getImageType() == ImageType::ati2)
				format = TextureFormat::RG_ATI2;
			else
			{
				if (image.bpp() == 24)
					format = TextureFormat::R8G8B8;
				else if (image.bpp() == 32)
					format = TextureFormat::R8G8B8A8;
				else
				{
					assert(false);
				}
			}

			auto texture = this->createTexture();
			texture->setMipLevel(image.getMipLevel());
			texture->setMipSize(image.size());
			texture->setSize(image.width(), image.height());
			texture->setTexDim(TextureDim::DIM_2D);
			texture->setTexFormat(format);
			texture->setStream(image.data());
			texture->setup();
			return texture;
		}
	}

	return nullptr;
}

MaterialPtr 
RenderPipeline::createMaterial(const std::string& name) except
{
	return _materialManager->createMaterial(name);
}

void
RenderPipeline::setMaterialPass(MaterialPassPtr pass) noexcept
{
	_materialManager->setMaterialPass(pass);
	
	auto& textures = pass->getTextures();
	for (int i = 0; i < textures.size(); i++)
	{
		auto& uniforms = textures[i]->getShaderUniform();
		auto texture = textures[i]->getTexture();

		for (auto& it : uniforms)
			it->assign(i);
		_renderDevice->setTexture(texture, i);
	}

	this->setRenderState(pass->getRenderState());
	this->setShaderObject(pass->getShaderObject());
}

void
RenderPipeline::setMaterialManager(MaterialManagerPtr manager) noexcept
{
	_materialManager = manager;
}

MaterialPassPtr&
RenderPipeline::getMaterialPass() noexcept
{
	return _materialManager->getMaterialPass();
}

MaterialManagerPtr& 
RenderPipeline::getMaterialManager() noexcept
{
	return _materialManager;
}

const MaterialPassPtr&
RenderPipeline::getMaterialPass() const noexcept
{
	return _materialManager->getMaterialPass();
}

const MaterialManagerPtr& 
RenderPipeline::getMaterialManager() const noexcept
{
	return _materialManager;
}

RenderBufferPtr
RenderPipeline::createRenderBuffer() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderBuffer();
}

RenderBufferPtr
RenderPipeline::createRenderBuffer(const MeshProperty& mesh) except
{
	auto numVertex = mesh.getNumVertices();
	auto numIndices = mesh.getNumIndices();

	VertexComponents components;

	auto& vertices = mesh.getVertexArray();
	if (!vertices.empty())
	{
		if (vertices.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	auto& colors = mesh.getColorArray();
	if (!colors.empty())
	{
		if (colors.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));
	}

	auto& normals = mesh.getNormalArray();
	if (!normals.empty())
	{
		if (normals.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	auto& texcoords = mesh.getTexcoordArray();
	if (!texcoords.empty())
	{
		if (texcoords.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));
	}

	auto& tangent = mesh.getTangentArray();
	if (!tangent.empty())
	{
		if (tangent.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TANGENT, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	VertexLayout layout;
	layout.setVertexComponents(components);

	VertexBufferDataPtr vb;

	if (numVertex)
	{
		std::size_t offset = 0;
		std::size_t stride = layout.getVertexSize();
		std::vector<char> _data(numVertex * stride);

		if (vertices.size() == numVertex)
		{
			char* data = (char*)_data.data();
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (colors.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += stride;
			}

			offset += sizeof(float4);
		}

		if (normals.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (texcoords.size() == numVertex)
		{
			char* data = (char*)_data.data() + offset;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += stride;
			}

			offset += sizeof(float2);
		}

		vb = this->createVertexBufferData();
		vb->open(layout, VertexUsage::MAP_READ_BIT, _data.data(), _data.size());
	}

	IndexBufferDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndices > 0)
	{
		ib = this->createIndexBufferData();
		ib->open(IndexType::GPU_UINT32, VertexUsage::MAP_READ_BIT, (char*)faces.data(), faces.size() * sizeof(std::uint32_t));
	}

	auto buffer = this->createRenderBuffer();
	buffer->setup(vb, ib);

	return buffer;
}

RenderBufferPtr 
RenderPipeline::createRenderBuffer(const MeshPropertys& meshes) except
{
	auto buffer = this->createRenderBuffer();

	auto numVertex = 0;
	auto numIndices = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndices += it->getNumIndices();
	}

	VertexLayout layout;

	if (!meshes.front()->getVertexArray().empty())
		layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getColorArray().empty())
		layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));

	if (!meshes.front()->getNormalArray().empty())
		layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getTexcoordArray().empty())
		layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));

	if (!meshes.front()->getTangentArray().empty())
		layout.addComponent(VertexComponent(VertexAttrib::GPU_ATTRIB_TANGENT, VertexFormat::GPU_VERTEX_FLOAT3));

	VertexBufferDataPtr vb;

	if (numVertex)
	{
		std::size_t offsetVertices = 0;
		std::size_t stride = layout.getVertexSize();
		std::vector<char> _data(numVertex * stride);

		const char* mapBuffer = _data.data();

		for (auto& mesh : meshes)
		{
			auto& vertices = mesh->getVertexArray();
			auto& colors = mesh->getColorArray();
			auto& normals = mesh->getNormalArray();
			auto& tangents = mesh->getTangentArray();
			auto& texcoords = mesh->getTexcoordArray();

			std::size_t offset1 = 0;

			if (!vertices.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : vertices)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!colors.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : colors)
				{
					*(float4*)data = it;
					data += stride;
				}

				offset1 += sizeof(float4);
			}

			if (!normals.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : normals)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!texcoords.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : texcoords)
				{
					*(float2*)data = it;
					data += stride;
				}

				offset1 += sizeof(float2);
			}

			if (!tangents.empty())
			{
				char* data = (char*)mapBuffer + offset1 + offsetVertices;
				for (auto& it : tangents)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			offsetVertices += mesh->getNumVertices() * layout.getVertexSize();
		}

		vb = this->createVertexBufferData();
		vb->open(layout, VertexUsage::MAP_READ_BIT, _data.data(), layout.getVertexSize() * numVertex);
	}

	IndexBufferDataPtr ib;

	if (numIndices > 0)
	{
		std::vector<std::uint32_t> faces(numIndices);
		std::size_t offsetIndices = 0;

		for (auto& it : meshes)
		{
			auto& array = it->getFaceArray();
			if (!array.empty())
			{
				std::uint32_t* indices = (std::uint32_t*)faces.data() + offsetIndices;
				for (auto& face : array)
				{
#if !defined(EGLAPI)
					*indices++ = face;
#else
					*indices++ = offsetIndices + face;
#endif
				}

				offsetIndices += array.size();
			}
		}

		ib = this->createIndexBufferData();
		ib->open(IndexType::GPU_UINT32, VertexUsage::MAP_READ_BIT, (char*)faces.data(), faces.size() * sizeof(std::uint32_t));
	}

	buffer->setup(vb, ib);

	return buffer;
}

IndexBufferDataPtr 
RenderPipeline::createIndexBufferData() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createIndexBufferData();
}

VertexBufferDataPtr
RenderPipeline::createVertexBufferData() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createVertexBufferData();
}

void 
RenderPipeline::setRenderBuffer(RenderBufferPtr buffer) except
{
	assert(_renderDevice);
	_renderDevice->setRenderBuffer(buffer);
}

void 
RenderPipeline::drawRenderBuffer(const RenderIndirect& renderable) except
{
	assert(_renderDevice);
	_renderDevice->drawRenderBuffer(renderable);
}

void 
RenderPipeline::drawRenderBuffer(const RenderIndirects& renderable) except
{
	assert(_renderDevice);
	_renderDevice->drawRenderBuffer(renderable);
}

RenderBufferPtr 
RenderPipeline::getRenderBuffer() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getRenderBuffer();
}

void
RenderPipeline::drawSceneQuad(MaterialPassPtr pass) noexcept
{
	assert(pass);

	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderSceneQuad->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderSceneQuad->getNumIndices();
	renderable.numInstances = 0;

	this->drawMesh(pass, _renderSceneQuad, renderable);
}

void
RenderPipeline::drawSphere(MaterialPassPtr pass) noexcept
{
	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderSphere->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderSphere->getNumIndices();
	renderable.numInstances = 0;

	this->drawMesh(pass, _renderSphere, renderable);
}

void
RenderPipeline::drawCone(MaterialPassPtr pass) noexcept
{
	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderCone->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderCone->getNumIndices();
	renderable.numInstances = 0;

	this->drawMesh(pass, _renderCone, renderable);
}

void
RenderPipeline::drawRenderQueue(RenderQueue queue, RenderPass pass, MaterialPassPtr material, RenderTexturePtr target) noexcept
{
	auto& renderable = this->getRenderData(queue, pass);
	for (auto& it : renderable)
	{
		this->onRenderObjectPre(*it, queue, pass, material);
		this->onRenderObject(*it, queue, pass, material);
		this->onRenderObjectPost(*it, queue, pass, material);
	}

	if (pass == RenderPass::RP_POSTPROCESS)
	{
		auto postprocess = _postprocessors[queue];
		for (auto& it : postprocess)
		{
			it->onPreRender(*this);
			it->onRender(*this, target);
			it->onPostRender(*this);
		}
	}
}

void
RenderPipeline::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_width != width || _height != height)
	{
		auto& renderPostProcess = _postprocessors[RenderQueue::RQ_POSTPROCESS];
		for (auto& it : renderPostProcess)
		{
			if (it->getActive())
				it->onResolutionChangeBefore(*this);
		}

		_width = width;
		_height = height;

		for (auto& it : renderPostProcess)
		{
			if (it->getActive())
				it->onResolutionChangeAfter(*this);
		}
	}
}

void
RenderPipeline::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _width;
	h = _height;
}

void
RenderPipeline::addPostProcess(RenderPostProcessPtr postprocess) except
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& renderPostProcess = _postprocessors[renderQueue];

	if (std::find(renderPostProcess.begin(), renderPostProcess.end(), postprocess) == renderPostProcess.end())
	{
		postprocess->_setRenderPipeline(this);
		postprocess->setActive(true);
		renderPostProcess.push_back(postprocess);
	}
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
	auto renderQueue = postprocess->getRenderQueue();
	auto& renderPostProcess = _postprocessors[renderQueue];

	auto it = std::find(renderPostProcess.begin(), renderPostProcess.end(), postprocess);
	if (it != renderPostProcess.end())
	{
		postprocess->setActive(false);
		renderPostProcess.erase(it);
	}
}

void
RenderPipeline::renderPostProcess(RenderTexturePtr renderTexture) except
{
	auto& renderPostProcess = _postprocessors[RenderQueue::RQ_POSTPROCESS];

	for (auto& it : renderPostProcess)
	{
		if (it->getActive())
		{
			it->onPreRender(*this);
			it->onRender(*this, renderTexture);
			it->onPostRender(*this);
		}
	}
}

void 
RenderPipeline::renderBegin() noexcept
{
	assert(_renderDevice);
	_renderDevice->renderBegin();
}

void
RenderPipeline::render(const RenderScene& scene) noexcept
{
	assert(_renderDevice);

	auto& cameras = scene.getCameraList();
	for (auto& camera : cameras)
	{
		auto window = camera->getRenderWindow();
		if (!window)
			window = _renderWindow;

		_renderDevice->setRenderWindow(window);

		this->onRenderPre(camera);

		RenderListener* renderListener = camera->getOwnerListener();
		if (renderListener)
			renderListener->onWillRenderObject(*camera);

		this->onRenderPipeline(camera);

		if (renderListener)
			renderListener->onRenderObject(*camera);

		this->onRenderPost(camera);
	}
}

void
RenderPipeline::renderEnd() noexcept
{
	assert(_renderDevice);
	_renderDevice->renderEnd();
}

void
RenderPipeline::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipeline::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) except
{
	auto pass = _pass ? _pass : object.getMaterial()->getTech(queue)->getPass(passType);

	if (pass)
	{
		_materialManager->setMatrixParam(MaterialSemantic::matModel, object.getTransform());
		_materialManager->setMatrixParam(MaterialSemantic::matModelInverse, object.getTransformInverse());
		_materialManager->setMatrixParam(MaterialSemantic::matModelInverseTranspose, object.getTransformInverseTranspose());

		if (!_pass)
		{
			RenderStencilState stencil = pass->getRenderState()->getStencilState();
			stencil.stencilEnable = true;
			stencil.stencilPass = StencilOperation::STENCILOP_REPLACE;
			stencil.stencilFunc = CompareFunction::GPU_ALWAYS;
			stencil.stencilRef = 1 << object.getLayer();
			stencil.stencilReadMask = 0xFFFFFFFF;

			pass->getRenderState()->setStencilState(stencil);
		}

		this->drawMesh(pass, object.getRenderBuffer(), *object.getRenderIndirect());

		auto listener = object.getOwnerListener();
		if (listener)
			listener->onRenderObject(*this->getCamera());
	}
}

void
RenderPipeline::onActivate() except
{
}

void
RenderPipeline::onDeactivate() except
{
}

void
RenderPipeline::onRenderPre(CameraPtr) except
{
}

void
RenderPipeline::onRenderPost(CameraPtr) except
{
}

void
RenderPipeline::onRenderPipeline(CameraPtr) except
{
}

_NAME_END