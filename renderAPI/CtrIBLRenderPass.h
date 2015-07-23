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

#ifndef INCLUDED_CRT_IBL_RENDERPASS
#define INCLUDED_CRT_IBL_RENDERPASS

#include <CtrPlatform.h>
#include <CtrRenderPass.h>
#include <CtrCamera.h>
#include <CtrScene.h>
#include <CtrIDepthSurface.h>
#include <CtrIBLProbe.h>

namespace Ctr
{
class IShader;
class GPUTechnique;
class GPUVariable;
class IBLConvolutions;

// Render pass for volume generation.
class IBLRenderPass : public Ctr::RenderPass
{
  public:
    IBLRenderPass(Ctr::IDevice* device);
    virtual ~IBLRenderPass(void);

    virtual bool               cache();
    virtual bool               free ();
    virtual void               render(Ctr::Scene* scene);

    void                       colorConvert(Ctr::Scene* scene,
                                            Ctr::IBLProbe* probe);
    void                       colorConvert(bool applyMDR,
                                            bool useMips,
                                            Ctr::Scene* scene,
                                            Ctr::ITexture* dst,
                                            Ctr::ITexture* src,
                                            Ctr::IBLProbe* probe);

  protected:
    bool                       loadMesh();

  private:
    // Refine importance sampling for specular cube.
    void                       refineSpecular(Ctr::Scene* scene,
                                              const Ctr::IBLProbe* probe);

    void                       refineDiffuse(Ctr::Scene* scene,
                                             const Ctr::IBLProbe* probe);


    // the objects that are visible to the camera.
    Ctr::CameraTransformCachePtr _paraboloidTransformCache;
    Ctr::CameraTransformCachePtr _environmentTransformCache;
    Ctr::IBLConvolutions*        _convolve;

    // Procedural Splat Geometry
    Ctr::Entity*                _sphereEntity;
    Ctr::Mesh*                  _sphereMesh; 
    Ctr::Material*              _material;
    bool                        _cached;

    // Invalidation.
    Hash                       _diffuseHash;
    Hash                       _specularHash;
    Hash                       _colorHash;

    // Color Conversion shader to LDR and MDR.
    const Ctr::IShader*        _colorConversionShader;
    const Ctr::GpuTechnique*   _colorConversionTechnique;
    const Ctr::GpuVariable*    _colorConversionMipLevelVariable;
    const Ctr::GpuVariable*    _colorConversionIsMDRVariable;
    const Ctr::GpuVariable*    _colorConversionGammaVariable;
    const Ctr::GpuVariable*    _colorConversionLDRExposureVariable;
    const Ctr::GpuVariable*    _colorConversionMDRScaleVariable;

    float                       _colorConversionGamma;
    float                       _colorConversionLDRExposure;
    float                       _colorConversionMDRScale;
};

}

#endif
