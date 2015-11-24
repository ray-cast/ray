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
#include <ray/render_factory.h>
#include <ray/material_maker.h>
#include <ray/render_texture.h>
#include <ray/render_object.h>
#include <ray/material.h>
#include <ray/model.h>

_NAME_BEGIN

RenderPipelineLayer::RenderPipelineLayer() noexcept
{
}

RenderPipelineLayer::~RenderPipelineLayer() noexcept
{
	this->close();
}

void
RenderPipelineLayer::open(RenderWindowPtr window, std::size_t w, std::size_t h) except
{
	_renderWindow = window;

	_renderDevice = RenderFactory::createRenderDevice();
	_renderDevice->open(_renderWindow);

	MeshProperty mesh;
	mesh.makePlane(2, 2, 1, 1);

	_renderSceneQuad = RenderFactory::createRenderBuffer(mesh);

	mesh.makeSphere(1, 16, 12);

	_renderSphere = RenderFactory::createRenderBuffer(mesh);

	mesh.makeCone(1, 1, 16);

	_renderCone = RenderFactory::createRenderBuffer(mesh);
	
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
	_renderDevice->setWireframeMode(enable);
}

bool
RenderPipelineLayer::getWireframeMode() const noexcept
{
	return _renderDevice->getWireframeMode();
}

void
RenderPipelineLayer::setSwapInterval(SwapInterval interval) except
{
	_renderDevice->setSwapInterval(interval);
}

SwapInterval
RenderPipelineLayer::getSwapInterval() const noexcept
{
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

void
RenderPipelineLayer::setRenderTexture(RenderTexturePtr target) noexcept
{
	_renderDevice->setRenderTexture(target);
}

void
RenderPipelineLayer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	_renderDevice->setMultiRenderTexture(target);
}

void
RenderPipelineLayer::setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept
{
	_renderDevice->setRenderTextureLayer(target, layer);
}

void
RenderPipelineLayer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	_renderDevice->clearRenderTexture(flags, color, depth, stencil);
}

void
RenderPipelineLayer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	_renderDevice->clearRenderTexture(flags, color, depth, stencil, i);
}

void
RenderPipelineLayer::discradRenderTexture() noexcept
{
	_renderDevice->discardRenderTexture();
}

void
RenderPipelineLayer::readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	_renderDevice->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipelineLayer::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
	_renderDevice->copyRenderTexture(srcTarget, src, destTarget, dest);
}

void
RenderPipelineLayer::setRenderState(RenderStatePtr state) noexcept
{
	_renderDevice->setRenderState(state);
}

RenderStatePtr
RenderPipelineLayer::getRenderState() const noexcept
{
	return _renderDevice->getRenderState();
}

void
RenderPipelineLayer::setShaderObject(ShaderObjectPtr shader) noexcept
{
	_renderDevice->setShaderObject(shader);
}

ShaderObjectPtr
RenderPipelineLayer::getShaderObject() const noexcept
{
	return _renderDevice->getShaderObject();
}

void
RenderPipelineLayer::setMaterialPass(MaterialPassPtr pass) noexcept
{
	this->setRenderState(pass->getRenderState());
	this->setShaderObject(pass->getShaderObject());

	_currentMaterial = pass;
}

MaterialPassPtr
RenderPipelineLayer::getMaterialPass() const noexcept
{
	return _currentMaterial;
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
	_renderDevice->renderBegin();
}

void
RenderPipelineLayer::render(const RenderScene& scene) noexcept
{
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

		this->setShaderObject(pass->getShaderObject());
		this->setRenderState(pass->getRenderState());

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