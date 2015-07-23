//------------------------------------------------------------------------------------//
//                                                                                    //
//               _________        .__  __    __                                       //
//               \_   ___ \_______|__|/  |__/  |_  ___________                        //
//               /    \  \/\_  __ \  \   __\   __\/ __ \_  __ \                       //
//               \     \____|  | \/  ||  |  |  | \  ___/|  | \/                       //
//                \______  /|__|  |__||__|  |__|  \___  >__|                          //
//                       \/                           \/                              //
//                                                                                    //
//    Critter is provided under the MIT License(MIT)                                  //
//    Critter uses portions of other open source software.                            //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2015 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//

#include <CtrIBLRenderPass.h>
#include <CtrBrdf.h>
#include <CtrShaderMgr.h>
#include <CtrEntity.h>
#include <CtrCamera.h>
#include <CtrIBLProbe.h>
#include <CtrScene.h>
#include <CtrMaterial.h>
#include <CtrMesh.h>
#include <CtrIShader.h>
#include <CtrGpuVariable.h>
#include <CtrPostEffectsMgr.h>
#include <CtrLog.h>
#include <CtrEntity.h>
#include <CtrIndexedMesh.h>
#include <CtrShaderMgr.h>
#include <CtrIEffect.h>
#include <CtrMatrixAlgo.h>

namespace Ctr
{
IBLRenderPass::IBLRenderPass(Ctr::IDevice* device) :
    Ctr::RenderPass (device),
    _convolve (nullptr),
    _cached (false),
    _material(nullptr),
    _colorConversionShader(nullptr),
    _colorConversionTechnique(nullptr),
    _colorConversionMipLevelVariable(nullptr),
    _colorConversionIsMDRVariable(nullptr),
    _colorConversionGammaVariable(nullptr),
    _colorConversionLDRExposureVariable(nullptr),
    _colorConversionMDRScaleVariable(nullptr),
    _colorConversionGamma(2.2f),
    _colorConversionLDRExposure(1.0f),
    _colorConversionMDRScale(6)
{
    _passName = "ibl";

    _paraboloidTransformCache.reset(new CameraTransformCache());
    _environmentTransformCache.reset(new CameraTransformCache());

    // Load the MDR conversion shader.
    // Load the importance sampling shader and variables.
    if (!_deviceInterface->shaderMgr()->addShader("IblColorConvertEnvironment.fx", _colorConversionShader, true))
    {
        LOG("ERROR: Could not add the environment color conversion shader.");
        throw (std::exception("No color conversion shader available for probes"));
    }
    else
    {
        _colorConversionShader->getTechniqueByName(std::string("Default"), _colorConversionTechnique);
        _colorConversionShader->getParameterByName("CurrentMipLevel", _colorConversionMipLevelVariable);
        _colorConversionShader->getParameterByName("IsMDR", _colorConversionIsMDRVariable);
        _colorConversionShader->getParameterByName("Gamma", _colorConversionGammaVariable);
        _colorConversionShader->getParameterByName("LDRExposure", _colorConversionLDRExposureVariable);
        _colorConversionShader->getParameterByName("MDRScale", _colorConversionMDRScaleVariable);
    }



    cache();
}

IBLRenderPass::~IBLRenderPass()
{
    safedelete(_sphereEntity);
    safedelete(_material);
}

bool
IBLRenderPass::free ()
{
    return true;
}

bool
IBLRenderPass::cache()
{
    if (!_cached)
    {
        // TODO: Load sphere projection resource.
        if (!loadMesh())
        {
            LOG ("Failed to load sphere projection mesh for IBLRenderPass");
            return false;
        }
    }
    return true;
}

bool
IBLRenderPass::loadMesh()
{
    // Static scene load does not resolve materials.
    // This sphere is for reprojection for dual projection to cube map.
    if (_sphereEntity = Scene::load(_deviceInterface, "data/meshes/Sphere/sphere.obj"))
    {
        _sphereMesh = _sphereEntity->meshes()[0];
        _sphereMesh->scaleProperty()->set(Ctr::Vector3f(5,5,5));
        _material = new Material(_deviceInterface);
        _material->textureGammaProperty()->set(1.0f);
        _sphereMesh->setMaterial(_material);
    }
    else
    {
        LOG_CRITICAL("Could not load sphere");
    }

    return true;
}

void
IBLRenderPass::refineDiffuse(Ctr::Scene* scene,
                             const Ctr::IBLProbe* probe)
{
    // TODO: Optimize.
	Ctr::Camera* camera = scene->camera();

	float projNear = camera->zNear();
	float projFar = camera->zFar();
    Ctr::Matrix44f proj;
    Ctr::projectionPerspectiveMatrixLH (Ctr::BB_PI * 0.5f,
                                       1.0, 
                                       projNear, 
                                       projFar,
                                       &proj);

	Ctr::Vector3f origin(0, 0, 0);
	// Setup view matrix
	_environmentTransformCache->set(probe->basis(), proj, probe->basis(), origin, projNear, projFar, -1);
	camera->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = probe->diffuseCubeMap()->resource()->mipLevels();
    const Ctr::ITexture* sourceTexture = probe->environmentCubeMap();

    float roughness = 0;
    float roughnessDelta = 1.0f / (float)(mipLevels);
    float samplesOffset = (float)(probe->sampleOffset());
    float samplesPerFrame = (float)(probe->samplesPerFrame());
    float sampleCount = (float)(probe->sampleCount());

    roughness = 1.0;

    const Ctr::Brdf* brdf = scene->activeBrdf();
    const Ctr::IShader* importanceSamplingShaderDiffuse = brdf->diffuseImportanceSamplingShader();
    // Convolve for diffuse.
    {
        float currentMip = 0;

        const Ctr::ISurface* targetSurface = probe->diffuseCubeMap()->surface();
        Ctr::FrameBuffer framebuffer(targetSurface, nullptr);

        Ctr::Viewport mipViewport (0,0, probe->diffuseResolution(), probe->diffuseResolution(), 0, 1);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);
        _deviceInterface->clearSurfaces (0, Ctr::CLEAR_TARGET, 0, 0, 0, 1);

        const Ctr::GpuTechnique* importanceSamplingDiffuseTechnique = nullptr;
        const Ctr::GpuVariable*  convolutionSrcDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionMipDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionRoughnessDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionSamplesOffsetDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionViewsDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionSampleCountDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionMaxSamplesDiffuseVariable = nullptr;
        const Ctr::GpuVariable*  convolutionSrcLastResultDiffuseVariable = nullptr;

        importanceSamplingShaderDiffuse->getTechniqueByName(std::string("basic"), importanceSamplingDiffuseTechnique);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSrc", convolutionSrcDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("LastResult", convolutionSrcLastResultDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionMip", convolutionMipDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionRoughness", convolutionRoughnessDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSamplesOffset", convolutionSamplesOffsetDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSampleCount", convolutionSampleCountDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionMaxSamples", convolutionMaxSamplesDiffuseVariable);

        // Set parameters
        convolutionSrcDiffuseVariable->setTexture(sourceTexture);
        convolutionSrcLastResultDiffuseVariable->setTexture(probe->lastDiffuseCubeMap());
        
        convolutionMipDiffuseVariable->set ((const float*)&currentMip, sizeof (float));
        convolutionRoughnessDiffuseVariable->set((const float*)&roughness, sizeof (float));
        convolutionSamplesOffsetDiffuseVariable->set((const float*)&samplesOffset, sizeof (float));        
        convolutionSampleCountDiffuseVariable->set(&samplesPerFrame , sizeof(float));
        convolutionMaxSamplesDiffuseVariable->set(&sampleCount, sizeof(float));

        importanceSamplingShaderDiffuse->renderMesh (Ctr::RenderRequest(importanceSamplingDiffuseTechnique, scene, camera, _sphereMesh));
    }
}

void
IBLRenderPass::refineSpecular(Ctr::Scene* scene,
                              const Ctr::IBLProbe* probe)
{
	Ctr::Camera* camera = scene->camera();

    float projNear = camera->zNear();
    float projFar = camera->zFar();
    Ctr::Matrix44f proj;
    Ctr::projectionPerspectiveMatrixLH (Ctr::BB_PI * 0.5f,
                                                        1.0, 
                                                        projNear, 
                                                        projFar,
                                                        &proj);

	Ctr::Vector3f origin(0, 0, 0);
    // Setup view matrix
	_environmentTransformCache->set(probe->basis(), proj, probe->basis(), origin, projNear, projFar, -1);
    camera->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = probe->specularCubeMap()->resource()->mipLevels() - probe->mipDrop();
    const Ctr::ITexture* sourceTexture = probe->environmentCubeMap();

    float roughness = 0;
    float roughnessDelta = 1.0f / (float)(mipLevels-1);
    float samplesOffset = (float)(probe->sampleOffset());
    float samplesPerFrame = (float)(probe->samplesPerFrame());
    float sampleCount = (float)(probe->sampleCount());

    const Ctr::Brdf* brdf = scene->activeBrdf();
    const Ctr::IShader* importanceSamplingShaderSpecular = brdf->specularImportanceSamplingShader();

    // Convolve specular.
    uint32_t mipSize = probe->specularCubeMap()->resource()->width();

    for (uint32_t mipId = 0; mipId < mipLevels; mipId++)
    {
        float currentMip = (float)(mipId);

        const Ctr::ISurface* targetSurface = probe->specularCubeMap()->surface(-1, mipId);
        Ctr::FrameBuffer framebuffer(targetSurface, nullptr);

        Ctr::Viewport mipViewport (0.0f,0.0f, (float)(mipSize), (float)(mipSize), 0.0f, 1.0f);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);


        const Ctr::GpuTechnique*     importanceSamplingSpecularTechnique = nullptr;
        const Ctr::GpuVariable*      convolutionSrcSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionMipSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionRoughnessSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionSamplesOffsetSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionSampleCountSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionMaxSamplesSpecularVariable = nullptr;
        const Ctr::GpuVariable*      convolutionSrcLastResultSpecularVariable = nullptr;

        importanceSamplingShaderSpecular->getTechniqueByName(std::string("basic"), importanceSamplingSpecularTechnique);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSrc",     convolutionSrcSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("LastResult",         convolutionSrcLastResultSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionMip",     convolutionMipSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionRoughness", convolutionRoughnessSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSamplesOffset", convolutionSamplesOffsetSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSampleCount", convolutionSampleCountSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionMaxSamples", convolutionMaxSamplesSpecularVariable);

        // Set parameters
        convolutionSrcSpecularVariable->setTexture(sourceTexture);
        convolutionSrcLastResultSpecularVariable->setTexture(probe->lastSpecularCubeMap());
        convolutionMipSpecularVariable->set ((const float*)&currentMip, sizeof (float));
        convolutionRoughnessSpecularVariable->set((const float*)&roughness, sizeof (float));
        convolutionSamplesOffsetSpecularVariable->set((const float*)&samplesOffset, sizeof (float));
        convolutionSampleCountSpecularVariable->set(&samplesPerFrame , sizeof(float));
        convolutionMaxSamplesSpecularVariable->set(&sampleCount, sizeof(float));

        // Render the paraboloid out.
        importanceSamplingShaderSpecular->renderMesh (Ctr::RenderRequest(importanceSamplingSpecularTechnique, scene, camera, _sphereMesh));
        roughness += roughnessDelta;

        mipSize = mipSize >> 1;
    }
}

void
IBLRenderPass::render (Ctr::Scene* scene)
{
    Ctr::Camera* camera           = scene->camera();

    _deviceInterface->enableDepthWrite();
    _deviceInterface->enableZTest();

    // For each ibl set on the scene, render and process the dynamic ibl.

    _deviceInterface->disableAlphaBlending ();
    _deviceInterface->setCullMode (Ctr::CullNone);
    
    Ctr::Viewport oldview;
    _deviceInterface->getViewport(&oldview);
    
    // Render all meshes that have an environment marker.
    const std::vector<Ctr::IBLProbe*>& probes = scene->probes();
    
    const Ctr::Brdf* brdf = scene->activeBrdf();
    if (!brdf)
    {
        THROW("Cannot find brdf to create IBL " << __FILE__ << " " << __LINE__);
    }

    // Detect uncache condition based on importance sampling shaders.
    bool forceUncache = false;
    if (_specularHash != brdf->specularImportanceSamplingShader()->hash() ||
        _diffuseHash != brdf->diffuseImportanceSamplingShader()->hash())
    {
        _specularHash = brdf->specularImportanceSamplingShader()->hash();
        _diffuseHash = brdf->diffuseImportanceSamplingShader()->hash();
        forceUncache = true;
    }

    Ctr::CameraTransformCachePtr cachedTransforms = scene->camera()->cameraTransformCache();
    for (auto it = probes.begin(); it != probes.end(); it++)
    {
        // Todo, cull probe by location and range.
        // if (probe->hasInfluence(scene->activeCamera()))
        IBLProbe * probe = *it;
        if (forceUncache)
            probe->uncache();

        bool cached = probe->isCached();
        if (cached)
        {
            continue;
        }
        else if (probe->sampleOffset() == 0)
        {
            // If sample offset is 0, we need to create the environment
            // map and perform a first set of samples.
            _deviceInterface->disableZTest();
            _deviceInterface->disableDepthWrite();
            _deviceInterface->disableStencilTest();
            _deviceInterface->setCullMode (Ctr::CullNone);

             {
				// The ibl probe could also have a znear and zfar.
				// In this example it is more expedient just to use the camera znear - zfar.
				float projNear = camera->zNear();
				float projFar = camera->zFar();
                Ctr::Matrix44f proj;
                Ctr::projectionPerspectiveMatrixLH (Ctr::BB_PI * 0.5f,
                                                    1.0, 
                                                    projNear, 
                                                    projFar,
                                                    &proj);
    
                // Setup view matrix for the environment source render.
                _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);
    
                // Setup camera cache.
                camera->setCameraTransformCache(_environmentTransformCache);
    
                // Set framebuffer to cubemap.
                // Render to environment top level mip (highest resolution).
                size_t mipLevels = probe->environmentCubeMap()->resource()->mipLevels();

                Ctr::Vector2f mipSize = Ctr::Vector2f(float(probe->environmentCubeMap()->resource()->width()), 
                                                    float(probe->environmentCubeMap()->resource()->height()));

                for (size_t mipId = 0; mipId < mipLevels; mipId++)
                {
                    Ctr::Viewport mipViewport (0.0f, 0.0f, (float)(mipSize.x), (float)(mipSize.y), 0.0f, 1.0f);

                    // Render to top level mip for both cubemaps. A better strategy would be to blit after the first render...
                    Ctr::FrameBuffer framebuffer(probe->environmentCubeMap()->surface(-1, (int32_t)(mipId)), nullptr);
                    _deviceInterface->bindFrameBuffer(framebuffer);
                    _deviceInterface->setViewport(&mipViewport);
                    _deviceInterface->clearSurfaces (0, Ctr::CLEAR_TARGET, 0, 0, 0, 1);
    
                    // Render the scene to cubemap (single pass).
                    //renderMeshes (_passName, scene);
                    const std::vector<Ctr::Mesh*>& meshes = scene->meshesForPass(_passName);
                    for (auto it = meshes.begin(); it != meshes.end(); it++)
                    {
                        const Ctr::Mesh* mesh = (*it);
                        const Ctr::Material* material = mesh->material();
                        const Ctr::IShader* shader = material->shader();
                        const Ctr::GpuTechnique* technique = material->technique();
    
                        RenderRequest renderRequest (technique, scene, scene->camera(), mesh);
                        shader->renderMesh(renderRequest);
                    }

                    mipSize.x /= 2.0f;
                    mipSize.y /= 2.0f;
                }

                // Generate mip maps post rendering.
                probe->environmentCubeMap()->generateMipMaps();    
                refineSpecular(scene, probe);
                refineDiffuse(scene, probe);

                colorConvert(scene, probe);
                // Update the sample count
                probe->updateSamples();
            }
            _deviceInterface->enableZTest();
            _deviceInterface->enableDepthWrite();
            _deviceInterface->setCullMode (Ctr::CullNone);
        }
        else
        {
            // Refine samples.
            // If sample offset is 0, we need to create the environment
            // map and perform a first set of samples.
            _deviceInterface->disableZTest();
            _deviceInterface->disableDepthWrite();
            _deviceInterface->disableStencilTest();
            _deviceInterface->setCullMode (Ctr::CullNone);
    
            float projNear = camera->zNear();
            float projFar = camera->zFar();
            Ctr::Matrix44f proj;
            Ctr::projectionPerspectiveMatrixLH (Ctr::BB_PI * 0.5f,
                                                1.0, 
                                                projNear, 
                                                projFar,
                                                &proj);
    
    
            // Setup view matrix
            _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);
    
            // Setup camera cache.
            scene->camera()->setCameraTransformCache(_environmentTransformCache);
    
            refineSpecular(scene, probe);
            refineDiffuse(scene, probe);

            // Update the sample count
            probe->updateSamples();
            colorConvert(scene, probe);

            _deviceInterface->enableZTest();
            _deviceInterface->enableDepthWrite();
            _deviceInterface->setCullMode (Ctr::CullNone);
        }
    }
    
    // Restore original camera transforms.
    scene->camera()->setCameraTransformCache(cachedTransforms);
    
    _deviceInterface->disableAlphaBlending ();
    _deviceInterface->bindFrameBuffer(_deviceInterface->postEffectsMgr()->sceneFrameBuffer());
    _deviceInterface->setViewport(&oldview);

    _deviceInterface->setCullMode (Ctr::CCW);
}

void
IBLRenderPass::colorConvert(bool applyMDR,
                            bool useMips,
                            Ctr::Scene* scene,
                            Ctr::ITexture* dst,
                            Ctr::ITexture* src,
                            Ctr::IBLProbe* probe)
{
    // TODO: Optimize.
    float projNear = 0.5;
    float projFar = 1000.0f;
    Ctr::Matrix44f proj;
    Ctr::projectionPerspectiveMatrixLH(Ctr::BB_PI * 0.5f,
        1.0,
        projNear,
        projFar,
        &proj);

    // Setup view matrix
    _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);

    // Setup camera cache.
    scene->camera()->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = Ctr::minValue(src->resource()->mipLevels(), dst->resource()->mipLevels());
    Ctr::ITexture* sourceTexture = src;

    float isMDR = applyMDR ? 1.0f : 0.0f;
    //                                             bool useMips,

    _sphereMesh->material()->setAlbedoMap(src);
    Ctr::Vector2i mipSize = Ctr::Vector2i(dst->width(), dst->height());

    // Convolve specular.
    for (size_t mipId = 0; mipId < mipLevels; mipId++)
    {
        float currentMip = (float)(mipId);
        // Render to target mip.
        const Ctr::ISurface* targetSurface = dst->surface(-1, (int32_t)(mipId));
        Ctr::FrameBuffer framebuffer(targetSurface, nullptr);

        Ctr::Viewport mipViewport(0.0f, 0.0f, (float)(mipSize.x), (float)(mipSize.y), 0.0f, 1.0f);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);

        _colorConversionMipLevelVariable->set((const float*)&currentMip, sizeof(float));
        _colorConversionIsMDRVariable->set((const float*)&isMDR, sizeof(float));
        _colorConversionMDRScaleVariable->set((const float*)&_colorConversionMDRScale, sizeof(float));
        _colorConversionGammaVariable->set((const float*)&_colorConversionGamma, sizeof(float));
        _colorConversionLDRExposureVariable->set((const float*)&_colorConversionLDRExposure, sizeof(float));
        _colorConversionMDRScaleVariable->set((const float*)&_colorConversionMDRScale, sizeof(float));

        // Render the paraboloid out.
        _colorConversionShader->renderMesh(Ctr::RenderRequest(_colorConversionTechnique, scene, scene->camera(), _sphereMesh));
        mipSize.x /= 2;
        mipSize.y /= 2;
    }
}

void
IBLRenderPass::colorConvert(Ctr::Scene* scene, Ctr::IBLProbe* probe)
{
    {
        // Convert specular src to MDR, save. (small memory optimization).
        colorConvert(true, true, scene, probe->environmentCubeMapMDR(), probe->environmentCubeMap(), probe);

        // Convert specular to MDR
        colorConvert(true, true, scene, probe->specularCubeMapMDR(), probe->specularCubeMap(), probe);

        // Convert diffuse to MDR
        colorConvert(true, false, scene, probe->diffuseCubeMapMDR(), probe->diffuseCubeMap(), probe);
    }
}

}