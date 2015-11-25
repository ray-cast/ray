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
#include <ray/render_pipeline_layer.h>
#include <ray/render_device.h>
#include <ray/render_post_process.h>
#include <ray/render_scene.h>
#include <ray/render_window.h>
#include <ray/material_maker.h>
#include <ray/render_texture.h>
#include <ray/render_object.h>
#include <ray/material.h>
#include <ray/model.h>
#include <ray/image.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/material_maker.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>
#include <ray/resource.h>
#if defined(_BUILD_OPENGL) || defined(_BUILD_OPENGL_ES)
#	include <ray/ogl_renderer.h>
#	define RenderDevice OGLRenderer
#endif

_NAME_BEGIN

RenderPipelineLayer::RenderPipelineLayer() noexcept
{
}

RenderPipelineLayer::~RenderPipelineLayer() noexcept
{
	this->close();
}

void
RenderPipelineLayer::open(WindHandle window, std::size_t w, std::size_t h) except
{
	_renderDevice = std::make_shared<RenderDevice>();
	_renderDevice->open(window);

	_renderWindow = _renderDevice->getRenderWindow();

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
RenderPipelineLayer::close() noexcept
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

	auto semantic = Material::getMaterialSemantic();
	if (semantic)
	{
		semantic->close();
		Material::setMaterialSemantic(nullptr);
	}

	if (_renderDevice)
	{
		_renderDevice.reset();
		_renderDevice = nullptr;
	}
}

void
RenderPipelineLayer::setWireframeMode(bool enable) noexcept
{
	assert(_renderDevice);
	_renderDevice->setWireframeMode(enable);
}

bool
RenderPipelineLayer::getWireframeMode() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getWireframeMode();
}

void
RenderPipelineLayer::setSwapInterval(SwapInterval interval) except
{
	assert(_renderDevice);
	_renderDevice->setSwapInterval(interval);
}

SwapInterval
RenderPipelineLayer::getSwapInterval() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getSwapInterval();
}

void
RenderPipelineLayer::setCamera(CameraPtr camera) noexcept
{
	auto semantic = Material::getMaterialSemantic();

	if (camera->getCameraOrder() != CameraOrder::CO_SHADOW)
	{
		if (camera->getCameraType() == CameraType::CT_PERSPECTIVE)
		{
			float ratio;
			float aperture;
			float znear;
			float zfar;
			camera->getPerspective(aperture, ratio, znear, zfar);

			std::size_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makePerspective(aperture, znear, zfar, ratio);
			}

			semantic->setFloatParam(GlobalFloatSemantic::CameraAperture, aperture);
			semantic->setFloatParam(GlobalFloatSemantic::CameraNear, znear);
			semantic->setFloatParam(GlobalFloatSemantic::CameraFar, zfar);
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

			std::size_t width, height;
			this->getWindowResolution(width, height);

			float windowRatio = (float)width / (float)height;
			if (ratio != windowRatio)
			{
				ratio = windowRatio;
				camera->makeOrtho(left, right, top, bottom, znear, zfar, ratio);
			}

			semantic->setFloatParam(GlobalFloatSemantic::CameraNear, znear);
			semantic->setFloatParam(GlobalFloatSemantic::CameraFar, zfar);
		}
	}

	semantic->setFloat3Param(GlobalFloat3Semantic::CameraView, camera->getLookAt());
	semantic->setFloat3Param(GlobalFloat3Semantic::CameraPosition, camera->getTranslate());
	semantic->setFloat3Param(GlobalFloat3Semantic::CameraDirection, camera->getLookAt() - camera->getTranslate());

	semantic->setMatrixParam(GlobalMatrixSemantic::matView, camera->getView());
	semantic->setMatrixParam(GlobalMatrixSemantic::matViewInverse, camera->getViewInverse());
	semantic->setMatrixParam(GlobalMatrixSemantic::matViewInverseTranspose, camera->getViewInverseTranspose());
	semantic->setMatrixParam(GlobalMatrixSemantic::matProject, camera->getProject());
	semantic->setMatrixParam(GlobalMatrixSemantic::matProjectInverse, camera->getProjectInverse());
	semantic->setMatrixParam(GlobalMatrixSemantic::matViewProject, camera->getViewProject());
	semantic->setMatrixParam(GlobalMatrixSemantic::matViewProjectInverse, camera->getViewProjectInverse());

	_camera = camera;
}

CameraPtr
RenderPipelineLayer::getCamera() const noexcept
{
	return _camera.lock();
}

void
RenderPipelineLayer::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	assert(object);
	assert(queue == RenderQueue::RQ_OPAQUE || queue == RenderQueue::RQ_TRANSPARENT || queue == RenderQueue::RQ_LIGHTING);

	_renderQueue[queue][pass].push_back(object);
}

RenderObjects&
RenderPipelineLayer::getRenderData(RenderQueue queue, RenderPass pass) noexcept
{
	assert(queue == RenderQueue::RQ_OPAQUE || queue == RenderQueue::RQ_TRANSPARENT || queue == RenderQueue::RQ_LIGHTING);
	return _renderQueue[queue][pass];
}

void
RenderPipelineLayer::drawMesh(RenderBufferPtr buffer, const RenderIndirect& renderable) noexcept
{
	_renderDevice->setRenderBuffer(buffer);
	_renderDevice->drawRenderBuffer(renderable);
}

void
RenderPipelineLayer::updateMesh(RenderBufferPtr buffer, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
	_renderDevice->updateRenderBuffer(buffer);
}

RenderTexturePtr 
RenderPipelineLayer::createRenderTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderTexture();
}

MultiRenderTexturePtr 
RenderPipelineLayer::createMultiRenderTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createMultiRenderTexture();
}

void
RenderPipelineLayer::setRenderTexture(RenderTexturePtr target) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderTexture(target);
}

void
RenderPipelineLayer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(_renderDevice);
	_renderDevice->setMultiRenderTexture(target);
}

void
RenderPipelineLayer::setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderTextureLayer(target, layer);
}

void
RenderPipelineLayer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_renderDevice);
	_renderDevice->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipelineLayer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	assert(_renderDevice);
	_renderDevice->clearRenderTexture(flags, color, depth, stencil, i);
}

void
RenderPipelineLayer::discradRenderTexture() noexcept
{
	assert(_renderDevice);
	_renderDevice->discardRenderTexture();
}

void
RenderPipelineLayer::readRenderTexture(RenderTexturePtr texture, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_renderDevice);
	_renderDevice->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipelineLayer::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
	assert(_renderDevice);
	_renderDevice->copyRenderTexture(srcTarget, src, destTarget, dest);
}

RenderStatePtr
RenderPipelineLayer::createRenderState() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderState();
}

void
RenderPipelineLayer::setRenderState(RenderStatePtr state) noexcept
{
	assert(_renderDevice);
	_renderDevice->setRenderState(state);
}

RenderStatePtr
RenderPipelineLayer::getRenderState() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getRenderState();
}

ShaderPtr
RenderPipelineLayer::createShader() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createShader();
}

ShaderObjectPtr 
RenderPipelineLayer::createShaderObject() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createShaderObject();
}

TexturePtr 
RenderPipelineLayer::createTexture() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createTexture();
}

TexturePtr 
RenderPipelineLayer::createTexture(const std::string& name) except
{
	MemoryReader stream;
	IoServer::instance()->openFile(name, stream);
	Image image;
	if (image.load(stream))
	{
		TextureFormat format = TextureFormat::R8G8B8A8;

		if (image.getImageType() == Image::dds1)
			format = TextureFormat::RGBA_DXT1;
		else if (image.getImageType() == Image::dds3)
			format = TextureFormat::RGBA_DXT3;
		else if (image.getImageType() == Image::dds5)
			format = TextureFormat::RGBA_DXT5;
		else if (image.getImageType() == Image::ati2)
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

	return nullptr;
}

MaterialPtr 
RenderPipelineLayer::createMaterial(const std::string& name) except
{
	MaterialMaker maker;
	return maker.load(name);
}

void
RenderPipelineLayer::setMaterialPass(MaterialPassPtr pass) noexcept
{
	this->setRenderState(pass->getRenderState());
	_renderDevice->setShaderObject(pass->getShaderObject());
	_currentMaterial = pass;
}

MaterialPassPtr
RenderPipelineLayer::getMaterialPass() const noexcept
{
	return _currentMaterial;
}

RenderBufferPtr
RenderPipelineLayer::createRenderBuffer() noexcept
{
	assert(_renderDevice);
	return _renderDevice->createRenderBuffer();
}

RenderBufferPtr
RenderPipelineLayer::createRenderBuffer(const MeshProperty& mesh) except
{
	auto numVertex = mesh.getNumVertices();
	auto numIndex = mesh.getNumIndices();

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

	VertexBufferDataPtr vb;

	if (numVertex)
	{
		vb = std::make_shared<VertexBufferData>();
		vb->setVertexComponents(components);
		vb->setup(numVertex, VertexUsage::GPU_MAP_READ_BIT | VertexUsage::GPU_IMMUTABLE_STORAGE);

		std::size_t offset = 0;
		std::size_t stride = vb->getVertexSize();
		if (vertices.size() == numVertex)
		{
			char* data = (char*)vb->data();
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (colors.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += stride;
			}

			offset += sizeof(float4);
		}

		if (normals.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (texcoords.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += stride;
			}

			offset += sizeof(float2);
		}
	}

	IndexBufferDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndex > 0)
	{
		ib = std::make_shared<IndexBufferData>();
		ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_MAP_READ_BIT | VertexUsage::GPU_IMMUTABLE_STORAGE);

		if (!faces.empty())
		{
			std::uint32_t* indices = (std::uint32_t*)ib->data();
			for (auto& face : faces)
			{
				*indices++ = face;
			}
		}
	}

	auto buffer = this->createRenderBuffer();
	buffer->setup(vb, ib);

	return buffer;
}

RenderBufferPtr 
RenderPipelineLayer::createRenderBuffer(const MeshPropertys& meshes) except
{
	auto buffer = this->createRenderBuffer();

	auto numVertex = 0;
	auto numIndex = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndex += it->getNumIndices();
	}

	std::vector<VertexComponent> components;

	if (!meshes.front()->getVertexArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getColorArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));

	if (!meshes.front()->getNormalArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getTexcoordArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));

	if (!meshes.front()->getTangentArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TANGENT, VertexFormat::GPU_VERTEX_FLOAT3));

	VertexBufferDataPtr vb;

	if (numVertex)
	{
		vb = std::make_shared<VertexBufferData>();
		vb->setVertexComponents(components);
		vb->setup(numVertex, VertexUsage::GPU_MAP_READ_BIT | VertexUsage::GPU_IMMUTABLE_STORAGE);

		std::size_t offsetVertices = 0;
		std::size_t stride = vb->getVertexSize();

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
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : vertices)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!colors.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : colors)
				{
					*(float4*)data = it;
					data += stride;
				}

				offset1 += sizeof(float4);
			}

			if (!normals.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : normals)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!texcoords.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : texcoords)
				{
					*(float2*)data = it;
					data += stride;
				}

				offset1 += sizeof(float2);
			}

			if (!tangents.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : tangents)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			offsetVertices += mesh->getNumVertices() * vb->getVertexSize();
		}
	}

	IndexBufferDataPtr ib;

	if (numIndex > 0)
	{
		ib = std::make_shared<IndexBufferData>();
		ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_MAP_READ_BIT | VertexUsage::GPU_IMMUTABLE_STORAGE);

		std::size_t offsetIndices = 0;

		for (auto& it : meshes)
		{
			auto& faces = it->getFaceArray();
			if (!faces.empty())
			{
				std::uint32_t* indices = (std::uint32_t*)ib->data() + offsetIndices;
				for (auto& face : faces)
				{
#if !defined(EGLAPI)
					*indices++ = face;
#else
					*indices++ = offsetIndices + face;
#endif
				}

				offsetIndices += faces.size();
			}
		}
	}

	buffer->setup(vb, ib);

	return buffer;
}

void 
RenderPipelineLayer::setRenderBuffer(RenderBufferPtr buffer) except
{
	assert(_renderDevice);
	_renderDevice->setRenderBuffer(buffer);
}

void 
RenderPipelineLayer::drawRenderBuffer(const RenderIndirect& renderable) except
{
	assert(_renderDevice);
	_renderDevice->drawRenderBuffer(renderable);
}

void 
RenderPipelineLayer::drawRenderBuffer(const RenderIndirects& renderable) except
{
	assert(_renderDevice);
	_renderDevice->drawRenderBuffer(renderable);
}

RenderBufferPtr 
RenderPipelineLayer::getRenderBuffer() const noexcept
{
	assert(_renderDevice);
	return _renderDevice->getRenderBuffer();
}

void
RenderPipelineLayer::drawSceneQuad(MaterialPassPtr pass) noexcept
{
	assert(pass);

	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderSceneQuad->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderSceneQuad->getNumIndices();
	renderable.numInstances = 0;

	this->setMaterialPass(pass);
	this->drawMesh(_renderSceneQuad, renderable);
}

void
RenderPipelineLayer::drawSphere() noexcept
{
	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderSphere->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderSphere->getNumIndices();
	renderable.numInstances = 0;

	this->drawMesh(_renderSphere, renderable);
}

void
RenderPipelineLayer::drawCone() noexcept
{
	RenderIndirect renderable;
	renderable.startVertice = 0;
	renderable.numVertices = _renderCone->getNumVertices();
	renderable.startIndice = 0;
	renderable.numIndices = _renderCone->getNumIndices();
	renderable.numInstances = 0;

	this->drawMesh(_renderCone, renderable);
}

void
RenderPipelineLayer::drawRenderIndirect(RenderQueue queue, RenderPass pass, MaterialPassPtr material, RenderTexturePtr target) noexcept
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
RenderPipelineLayer::setWindowResolution(std::size_t w, std::size_t h) noexcept
{
	auto renderPostProcess = _postprocessors[RenderQueue::RQ_POSTPROCESS];
	for (auto& it : renderPostProcess)
	{
		if (it->getActive())
			it->onResolutionChangeBefore(*this);
	}

	_width = w;
	_height = h;

	for (auto& it : renderPostProcess)
	{
		if (it->getActive())
			it->onResolutionChangeAfter(*this);
	}
}

void
RenderPipelineLayer::getWindowResolution(std::size_t& w, std::size_t& h) const noexcept
{
	w = _width;
	h = _height;
}

void
RenderPipelineLayer::addPostProcess(RenderPostProcessPtr postprocess) except
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
RenderPipelineLayer::removePostProcess(RenderPostProcessPtr postprocess) noexcept
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
RenderPipelineLayer::renderPostProcess(RenderTexturePtr renderTexture) except
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
RenderPipelineLayer::renderBegin() noexcept
{
	assert(_renderDevice);
	_renderDevice->renderBegin();
}

void
RenderPipelineLayer::render(const RenderScene& scene) noexcept
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

		RenderListener* renderListener = camera->getRenderListener();
		if (renderListener)
			renderListener->onWillRenderObject(*camera);

		this->onRenderPipeline(camera);

		if (renderListener)
			renderListener->onRenderObject(*camera);

		this->onRenderPost(camera);
	}
}

void
RenderPipelineLayer::renderEnd() noexcept
{
	assert(_renderDevice);

	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_OPAQUES].clear();
	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_SPECIFIC].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_TRANSPARENT].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_SPECIFIC].clear();

	_renderDevice->renderEnd();
}

void
RenderPipelineLayer::assignVisiable(RenderScenePtr scene, CameraPtr camera) noexcept
{
	assert(camera && scene);

	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_OPAQUES].clear();
	_renderQueue[RenderQueue::RQ_OPAQUE][RenderPass::RP_SPECIFIC].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_TRANSPARENT].clear();
	_renderQueue[RenderQueue::RQ_TRANSPARENT][RenderPass::RP_SPECIFIC].clear();

	_visiable.clear();

	scene->computVisiable(camera->getViewProject(), _visiable);

	for (auto& it : _visiable.iter())
	{
		if (CameraOrder::CO_SHADOW == camera->getCameraOrder())
		{
			if (!it.getOcclusionCullNode()->getCastShadow())
				return;
		}

		auto material = it.getOcclusionCullNode()->getMaterial();
		if (material)
		{
			auto& techiniques = material->getTechs();
			for (auto& technique : techiniques)
			{
				auto queue = technique->getRenderQueue();
				for (auto& pass : technique->getPassList())
				{
					auto listener = it.getOcclusionCullNode()->getRenderListener();
					if (listener)
						listener->onWillRenderObject(*this->getCamera());

					this->addRenderData(queue, pass->getRenderPass(), it.getOcclusionCullNode());
				}
			}
		}
	}
}

void
RenderPipelineLayer::assignLight(RenderScenePtr scene, CameraPtr camera) noexcept
{
	assert(camera && scene);

	_renderQueue[RenderQueue::RQ_LIGHTING][RenderPass::RP_LIGHTS].clear();

	_visiable.clear();

	scene->computVisiableLight(camera->getViewProject(), _visiable);

	for (auto& it : _visiable.iter())
	{
		auto listener = it.getOcclusionCullNode()->getRenderListener();
		if (listener)
			listener->onWillRenderObject(*this->getCamera());

		this->addRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS, it.getOcclusionCullNode());
	}
}

void
RenderPipelineLayer::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipelineLayer::onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

void
RenderPipelineLayer::onRenderObject(RenderObject& object, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) except
{
	auto semantic = Material::getMaterialSemantic();
	semantic->setMatrixParam(GlobalMatrixSemantic::matModel, object.getTransform());
	semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverse, object.getTransformInverse());
	semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverseTranspose, object.getTransformInverseTranspose());

	auto pass = _pass ? _pass : object.getMaterial()->getTech(queue)->getPass(passType);

	if (pass)
	{
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

		this->setMaterialPass(pass);
		this->drawMesh(object.getRenderBuffer(), *object.getRenderIndirect());

		auto listener = object.getRenderListener();
		if (listener)
			listener->onRenderObject(*this->getCamera());
	}
}

void
RenderPipelineLayer::onActivate() except
{
}

void
RenderPipelineLayer::onDeactivate() except
{
}

void
RenderPipelineLayer::onRenderPre(CameraPtr) except
{
}

void
RenderPipelineLayer::onRenderPost(CameraPtr) except
{
}

void
RenderPipelineLayer::onRenderPipeline(CameraPtr) except
{
}

_NAME_END