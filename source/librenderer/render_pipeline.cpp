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
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/render_scene.h>
#include <ray/render_texture.h>
#include <ray/material_manager.h>

_NAME_BEGIN

const float ESM_FACTOR = 1000.0f;

DefaultRenderPipeline::DefaultRenderPipeline() except
{
	_leftCamera = std::make_shared<Camera>();
	_rightCamera = std::make_shared<Camera>();
	_frontCamera = std::make_shared<Camera>();
	_backCamera = std::make_shared<Camera>();
	_topCamera = std::make_shared<Camera>();
	_bottomCamera = std::make_shared<Camera>();
}

DefaultRenderPipeline::~DefaultRenderPipeline() noexcept
{
}

void
DefaultRenderPipeline::renderShadowMap(CameraPtr camera) noexcept
{
	this->setCamera(camera);

	this->setRenderTexture(camera->getRenderTexture());
	this->clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);

	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES, _deferredDepthOnly);
	this->drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT, _deferredDepthOnly);
}

void
DefaultRenderPipeline::renderCubeMapLayer(RenderScenePtr scene, CameraPtr camera, int layer) noexcept
{
	this->setCamera(camera);

	/*this->assignVisiable(scene, camera);
	this->assignLight(scene, camera);

	this->renderOpaques(_deferredGraphicCubeMaps);
	this->renderLights(_deferredLightCubeMap);
	this->renderOpaquesShading(_deferredShadingCubeMap, layer);
	this->renderOpaquesSpecificShading(_deferredShadingCubeMap, layer);*/

	this->setRenderTexture(_deferredShadingCubeMap);
	this->setRenderTextureLayer(_deferredShadingCubeMap, layer);
	this->clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0, 0);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_SPECIFIC);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_POSTPROCESS, nullptr, _deferredShadingCubeMap);

	/*auto renderable = this->getRenderData(RenderQueue::Transparent);
	if (!renderable.empty())
	{
		this->renderTransparent(_deferredGraphicCubeMaps);
		this->renderLights(_deferredLightCubeMap);
		this->renderTransparentShading(_deferredShadingCubeMap);
		this->renderTransparentSpecificShading(_deferredShadingCubeMap);
	}*/
}

void
DefaultRenderPipeline::renderCubeMap(CameraPtr camera) noexcept
{
	auto& semantic = this->getMaterialManager();
	semantic->setTexParam(MaterialSemantic::DeferredDepthMap, _deferredDepthCubeMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredNormalMap, _deferredNormalCubeMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredGraphicMap, _deferredGraphicCubeMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredLightMap, _deferredLightCubeMap->getResolveTexture());

	_texEnvironmentMap->assign(_deferredShadingCubeMap->getResolveTexture());

	_leftCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_leftCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() - Vector3::UnitX, Vector3::UnitY);

	_rightCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_rightCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() + Vector3::UnitX, Vector3::UnitY);

	_frontCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_frontCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() + Vector3::UnitZ, Vector3::UnitY);

	_backCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_backCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() - Vector3::UnitZ, Vector3::UnitY);

	_topCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_topCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() + Vector3::UnitY, -Vector3::UnitZ);

	_bottomCamera->makePerspective(90.0, 1.0, camera->getNear(), camera->getFar());
	_bottomCamera->makeLookAt(camera->getTranslate(), camera->getTranslate() - Vector3::UnitY, Vector3::UnitZ);

	this->renderCubeMapLayer(camera->getRenderScene(), _rightCamera, 0);
	this->renderCubeMapLayer(camera->getRenderScene(), _leftCamera, 1);
	this->renderCubeMapLayer(camera->getRenderScene(), _bottomCamera, 2);
	this->renderCubeMapLayer(camera->getRenderScene(), _topCamera, 3);
	this->renderCubeMapLayer(camera->getRenderScene(), _frontCamera, 4);
	this->renderCubeMapLayer(camera->getRenderScene(), _backCamera, 5);
}

void
DefaultRenderPipeline::renderIrradianceMap(CameraPtr camera) noexcept
{
	//_irradiance.renderParaboloidEnvMap(*this, _deferredShadingCubeMap->getResolveTexture());
}

void
DefaultRenderPipeline::render2DEnvMap(CameraPtr camera) noexcept
{
	this->setCamera(camera);

	this->setRenderTexture(_deferredShadingMap);
	this->clearRenderTexture(ClearFlags::CLEAR_ALL, this->getCamera()->getClearColor(), 1.0, 0);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DefaultRenderPipeline::render3DEnvMap(CameraPtr camera) noexcept
{
	//this->renderCubeMap(camera);
	//this->renderIrradianceMap(camera);

	auto semantic = this->getMaterialManager();
	semantic->setTexParam(MaterialSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredDepthLinearMap, _deferredDepthLinearMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());

	this->setCamera(camera);

	this->renderOpaques(_deferredGraphicMaps);
	this->renderOpaquesDepthLinear(_deferredDepthLinearMap);
	this->renderLights(_deferredLightMap);
	this->renderOpaquesShading(_deferredShadingMap);
	this->renderOpaquesSpecificShading(_deferredShadingMap);

	/*this->renderTransparent(_deferredGraphicMaps);
	this->renderLights(_deferredLightMap);
	this->renderTransparentShading(_deferredShadingMap);
	this->renderTransparentSpecificShading(_deferredShadingMap);*/

	this->renderPostProcess(_deferredShadingMap);

	auto renderTexture = camera->getRenderTexture();
	if (renderTexture)
		this->copyRenderTexture(_deferredShadingMap, renderTexture, camera->getViewport());
}

void
DefaultRenderPipeline::renderCamera(CameraPtr camera) noexcept
{
	assert(camera);

	auto cameraOrder = camera->getCameraOrder();
	switch (cameraOrder)
	{
	case CO_SHADOW:
		this->renderShadowMap(camera);
		break;
	case CO_LIGHT:
		this->renderLights(camera->getRenderTexture());
		break;
	case CO_CUBEMAP:
		this->renderCubeMap(camera);
		break;
	case CO_MAIN:
		{
			switch (camera->getCameraType())
			{
			case CameraType::CT_PERSPECTIVE:
				this->render3DEnvMap(camera);
				break;
			case CameraType::CT_ORTHO:
				this->render2DEnvMap(camera);
				break;
			default:
				break;
			}
		}
		break;
	}
}

void
DefaultRenderPipeline::renderOpaques(MultiRenderTexturePtr target) noexcept
{
	this->setMultiRenderTexture(target);
	this->clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DefaultRenderPipeline::renderOpaquesDepthLinear(RenderTexturePtr target) noexcept
{
	_clipInfo->assign(this->getCamera()->getClipConstant());
	_projInfo->assign(this->getCamera()->getProjConstant());

	this->setRenderTexture(target);
	this->clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);
	this->drawSceneQuad(_deferredDepthLinear);
}

void
DefaultRenderPipeline::renderOpaquesShading(RenderTexturePtr target, int layer) noexcept
{
	this->setRenderTexture(target);
	this->setRenderTextureLayer(target, layer);
	this->clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0.0);
	this->drawSceneQuad(_deferredShadingOpaques);
}

void
DefaultRenderPipeline::renderOpaquesSpecificShading(RenderTexturePtr target, int layer) noexcept
{
	this->setRenderTexture(target);
	this->setRenderTextureLayer(target, layer);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_SPECIFIC);
	this->drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_POSTPROCESS, nullptr, target);
}

void
DefaultRenderPipeline::renderTransparent(MultiRenderTexturePtr renderTexture) noexcept
{
	this->setMultiRenderTexture(renderTexture);
	this->clearRenderTexture(ClearFlags::CLEAR_COLOR_STENCIL, Vector4::Zero, 1.0, 0.0, 0);

	this->drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT);
}

void
DefaultRenderPipeline::renderTransparentDepthLinear(RenderTexturePtr target) noexcept
{
	this->setRenderTexture(target);
	this->clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0.0, 0);

	this->drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_DEPTH, _deferredDepthLinear);
}

void
DefaultRenderPipeline::renderTransparentShading(RenderTexturePtr target, int layer) noexcept
{
	this->setRenderTexture(target);
	this->setRenderTextureLayer(target, layer);
	this->drawSceneQuad(_deferredShadingTransparents);
}

void
DefaultRenderPipeline::renderTransparentSpecificShading(RenderTexturePtr target, int layer) noexcept
{
	this->setRenderTexture(target);
	this->setRenderTextureLayer(target, layer);
	this->drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_SPECIFIC);
	this->drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_POSTPROCESS);
}

void
DefaultRenderPipeline::renderLights(RenderTexturePtr target) noexcept
{
	this->setRenderTexture(target);
	this->clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0);

	auto& lights = this->getRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		switch (light->getLightType())
		{
		case ray::LT_SUN:
			this->renderSunLight(*light);
			break;
		case ray::LT_DIRECTIONAL:
			this->renderDirectionalLight(*light);
			break;
		case ray::LT_POINT:
			this->renderPointLight(*light);
			break;
		case ray::LT_AREA:
			this->renderAreaLight(*light);
			break;
		case ray::LT_SPOT:
			this->renderSpotLight(*light);
			break;
		case ray::LT_HEMI_SPHERE:
			this->renderHemiSphereLight(*light);
			break;
		case ray::LT_AMBIENT:
			this->renderAmbientLight(*light);
			break;
		default:
			break;
		}
	}
}

void
DefaultRenderPipeline::renderSunLight(const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(this->getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(float3x3(this->getCamera()->getView()) * this->getCamera()->getTranslate());

	if (light.getShadow())
	{
		auto& stencil = _deferredSunLightShadow->getRenderState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_shadowFactor->assign(ESM_FACTOR);
		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		this->drawSceneQuad(_deferredSunLightShadow);
	}
	else
	{
		auto& stencil = _deferredSunLight->getRenderState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		this->drawSceneQuad(_deferredSunLight);
	}
}

void
DefaultRenderPipeline::renderDirectionalLight(const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(this->getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(float3x3(this->getCamera()->getView()) * this->getCamera()->getTranslate());

	if (light.getShadow())
	{
		RenderStencilState stencil = _deferredDirectionalLightShadow->getRenderState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_deferredDirectionalLightShadow->getRenderState()->setStencilState(stencil);

		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowFactor->assign(ESM_FACTOR);
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		this->drawSceneQuad(_deferredDirectionalLightShadow);
	}
	else
	{
		RenderStencilState stencil = _deferredDirectionalLight->getRenderState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_deferredDirectionalLight->getRenderState()->setStencilState(stencil);

		this->drawSceneQuad(_deferredDirectionalLight);
	}
}

void
DefaultRenderPipeline::renderPointLight(const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(this->getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(float3x3(this->getCamera()->getView()) * this->getCamera()->getTranslate());

	auto semantic = this->getMaterialManager();
	semantic->setMatrixParam(matModel, light.getTransform());

	RenderStencilState stencil = _deferredPointLight->getRenderState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredPointLight->getRenderState()->setStencilState(stencil);

	this->drawSphere(_deferredPointLight);
}

void
DefaultRenderPipeline::renderSpotLight(const Light& light) noexcept
{
	auto semantic = this->getMaterialManager();
	semantic->setMatrixParam(matModel, light.getTransform());

	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(this->getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());
	_lightSpotInnerCone->assign(light.getSpotInnerCone());
	_lightSpotOuterCone->assign(light.getSpotOuterCone());

	_eyePosition->assign(float3x3(this->getCamera()->getView()) * this->getCamera()->getTranslate());

	RenderStencilState stencil = _deferredSpotLight->getRenderState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredSpotLight->getRenderState()->setStencilState(stencil);

	this->drawCone(_deferredSpotLight);
}

void
DefaultRenderPipeline::renderAmbientLight(const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(this->getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(this->getCamera()->getTranslate());

	RenderStencilState stencil = _deferredAmbientLight->getRenderState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredAmbientLight->getRenderState()->setStencilState(stencil);

	this->drawSceneQuad(_deferredAmbientLight);
}

void
DefaultRenderPipeline::renderHemiSphereLight(const Light& light) noexcept
{
}

void
DefaultRenderPipeline::renderAreaLight(const Light& light) noexcept
{
}

void
DefaultRenderPipeline::copyRenderTexture(RenderTexturePtr src, RenderTexturePtr dst, const Viewport& view) noexcept
{
	_texSource->assign(src->getResolveTexture());
	this->setRenderTexture(dst);
	this->drawSceneQuad(_deferredCopyOnly);
}

void
DefaultRenderPipeline::onActivate() except
{
	std::uint32_t width, height;
	this->getWindowResolution(width, height);

	_deferredLighting = this->createMaterial("sys:fx\\deferred_lighting.glsl");
	_deferredDepthOnly = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDepthOnly");
	_deferredDepthLinear = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDepthLinear");
	_deferredPointLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredPointLight");
	_deferredAmbientLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredAmbientLight");
	_deferredSunLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSunLight");
	_deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSunLightShadow");
	_deferredDirectionalLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DirectionalLight");
	_deferredDirectionalLightShadow = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DirectionalLightShadow");
	_deferredSpotLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSpotLight");
	_deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredShadingOpaques");
	_deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredShadingTransparents");
	_deferredDebugLayer = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDebugLayer");
	_deferredCopyOnly = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredCopyOnly");

	_texMRT0 = _deferredLighting->getParameter("texMRT0");
	_texMRT1 = _deferredLighting->getParameter("texMRT1");
	_texDepth = _deferredLighting->getParameter("texDepth");
	_texLight = _deferredLighting->getParameter("texLight");
	_texSource = _deferredLighting->getParameter("texSource");
	_texEnvironmentMap = _deferredLighting->getParameter("texEnvironmentMap");

	_eyePosition = _deferredLighting->getParameter("eyePosition");
	_clipInfo = _deferredLighting->getParameter("clipInfo");
	_projInfo = _deferredLighting->getParameter("projInfo");

	_lightColor = _deferredLighting->getParameter("lightColor");
	_lightPosition = _deferredLighting->getParameter("lightPosition");
	_lightDirection = _deferredLighting->getParameter("lightDirection");
	_lightRange = _deferredLighting->getParameter("lightRange");
	_lightIntensity = _deferredLighting->getParameter("lightIntensity");
	_lightAttenuation = _deferredLighting->getParameter("lightAttenuation");
	_lightSpotInnerCone = _deferredLighting->getParameter("lightSpotInnerCone");
	_lightSpotOuterCone = _deferredLighting->getParameter("lightSpotOuterCone");

	_shadowChannel = _deferredLighting->getParameter("shadowChannel");
	_shadowMap = _deferredLighting->getParameter("shadowMap");
	_shadowArrayMap = _deferredLighting->getParameter("shadowArrayMap");
	_shadowFactor = _deferredLighting->getParameter("shadowFactor");
	_shadowMatrix = _deferredLighting->getParameter("shadowMatrix");

	_deferredDepthMap = this->createRenderTexture();
	_deferredDepthMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::DEPTH32_STENCIL8);

#if defined(_BUILD_OPENGL_ES)
	_deferredDepthLinearMap = this->createRenderTexture();
	_deferredDepthLinearMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);
#else
	_deferredDepthLinearMap = this->createRenderTexture();
	_deferredDepthLinearMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R32F);
#endif

	_deferredGraphicMap = this->createRenderTexture();
	_deferredGraphicMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredNormalMap = this->createRenderTexture();
	_deferredNormalMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

#if defined(_BUILD_OPENGL_ES)
	_deferredLightMap = this->createRenderTexture();
	_deferredLightMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredLightMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);
#else
	_deferredLightMap = this->createRenderTexture();
	_deferredLightMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredLightMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R16G16B16A16F);
#endif

#if defined(_BUILD_OPENGL_ES)
	_deferredShadingMap = this->createRenderTexture();
	_deferredShadingMap->setSharedDepthTexture(_deferredDepthMap);
	_deferredShadingMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredShadingMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);
#else
	_deferredShadingMap = this->createRenderTexture();
	_deferredShadingMap->setSharedDepthTexture(_deferredDepthMap);
	_deferredShadingMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredShadingMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R16G16B16F);
#endif

	_deferredGraphicMaps = this->createMultiRenderTexture();
	_deferredGraphicMaps->setSharedDepthTexture(_deferredDepthMap);
	_deferredGraphicMaps->setSharedStencilTexture(_deferredDepthMap);
	_deferredGraphicMaps->attach(_deferredGraphicMap);
	_deferredGraphicMaps->attach(_deferredNormalMap);
	_deferredGraphicMaps->setup();

#if !defined(_BUILD_OPENGL_ES)
	_deferredDepthCubeMap = this->createRenderTexture();
	_deferredDepthCubeMap->setup(512, 512, TextureDim::DIM_2D, TextureFormat::DEPTH24_STENCIL8);

	_deferredGraphicCubeMap = this->createRenderTexture();
	_deferredGraphicCubeMap->setup(512, 512, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredNormalCubeMap = this->createRenderTexture();
	_deferredNormalCubeMap->setup(512, 512, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredLightCubeMap = this->createRenderTexture();
	_deferredLightCubeMap->setSharedStencilTexture(_deferredDepthCubeMap);
	_deferredLightCubeMap->setup(512, 512, TextureDim::DIM_2D, TextureFormat::R11G11B10F);

	_deferredShadingCubeMap = this->createRenderTexture();
	_deferredShadingCubeMap->setSharedDepthTexture(_deferredDepthCubeMap);
	_deferredShadingCubeMap->setSharedStencilTexture(_deferredDepthCubeMap);
	_deferredShadingCubeMap->setup(512, 512, TextureDim::DIM_CUBE, TextureFormat::R11G11B10F);

	_deferredGraphicCubeMaps = this->createMultiRenderTexture();
	_deferredGraphicCubeMaps->setSharedDepthTexture(_deferredDepthCubeMap);
	_deferredGraphicCubeMaps->setSharedStencilTexture(_deferredDepthCubeMap);
	_deferredGraphicCubeMaps->attach(_deferredGraphicCubeMap);
	_deferredGraphicCubeMaps->attach(_deferredNormalCubeMap);
	_deferredGraphicCubeMaps->setup();
#endif

	auto semantic = this->getMaterialManager();
	semantic->setTexParam(MaterialSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::ColorMap, _deferredShadingMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::NormalMap, _deferredNormalMap->getResolveTexture());
}

void
DefaultRenderPipeline::onDeactivate() noexcept
{
	if (_deferredDepthMap)
	{
		_deferredDepthMap.reset();
		_deferredDepthMap = nullptr;
	}

	if (_deferredGraphicMap)
	{
		_deferredGraphicMap.reset();
		_deferredGraphicMap = nullptr;
	}

	if (_deferredNormalMap)
	{
		_deferredNormalMap.reset();
		_deferredNormalMap = nullptr;
	}

	if (_deferredLightMap)
	{
		_deferredLightMap.reset();
		_deferredLightMap = nullptr;
	}

	if (_deferredGraphicMaps)
	{
		_deferredGraphicMaps.reset();
		_deferredGraphicMaps = nullptr;
	}
}

void
DefaultRenderPipeline::onRenderPre(CameraPtr camera) noexcept
{
}

void
DefaultRenderPipeline::onRenderPipeline(CameraPtr camera) noexcept
{
	this->renderCamera(camera);
}

void
DefaultRenderPipeline::onRenderPost(CameraPtr camera) noexcept
{
	assert(camera);

	if (camera->getCameraRender() == CR_RENDER_TO_SCREEN)
	{
		auto renderTexture = camera->getRenderTexture();
		if (!renderTexture)
			renderTexture = _deferredShadingMap;

		auto v1 = Viewport(0, 0, renderTexture->getWidth(), renderTexture->getHeight());
		auto v2 = camera->getViewport();
		if (v2.width == 0 || v2.height == 0)
		{
			std::uint32_t width, height;
			this->getWindowResolution(width, height);

			v2.left = 0;
			v2.top = 0;
			v2.width = width;
			v2.height = height;
		}

		this->copyRenderTexture(renderTexture, nullptr, v2);
	}

	/*if (_deferredGraphicMap)
	this->blitRenderTexture(_deferredGraphicMap, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 3, 768));
	if (_deferredNormalMap)
	this->blitRenderTexture(_deferredNormalMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));
	if (_deferredLightMap)
	this->blitRenderTexture(_deferredLightMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));
	if (_deferredShadingMap)
	this->blitRenderTexture(_deferredShadingMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 0, 1376, 768 / 2));*/

	/*this->setRenderTextureLayer(_deferredShadingCubeMap, 0);
	this->blitRenderTexture(_deferredShadingCubeMap, Viewport(0, 0, 512, 512), 0, Viewport(0, 768 / 2, 1376 / 3, 768));

	this->setRenderTextureLayer(_deferredShadingCubeMap, 1);
	this->blitRenderTexture(_deferredShadingCubeMap, Viewport(0, 0, 512, 512), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));

	this->setRenderTextureLayer(_deferredShadingCubeMap, 2);
	this->blitRenderTexture(_deferredShadingCubeMap, Viewport(0, 0, 512, 512), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));

	this->setRenderTextureLayer(_deferredShadingCubeMap, 3);
	this->blitRenderTexture(_deferredShadingCubeMap, Viewport(0, 0, 512, 512), 0, Viewport(0, 0, 1376 / 3, 768 / 2));

	this->setRenderTextureLayer(_deferredShadingCubeMap, 4);
	this->blitRenderTexture(_deferredShadingCubeMap, Viewport(0, 0, 512, 512), 0, Viewport(1376 / 3, 0, 1376 / 3 * 2, 768 / 2));*/
}

_NAME_END