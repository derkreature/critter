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
#ifndef INCLUDED_MATERIAL
#define INCLUDED_MATERIAL

#include <CtrPlatform.h>
#include <CtrRenderNode.h>
#include <CtrVector4.h>

namespace Ctr
{
class GpuTechnique;
class IShader;

enum SpecularWorkflow
{
    RoughnessMetal,
    GlossMetal,
    RoughnessInverseMetal,
    GlossInverseMetal
};

enum RenderDebugTerm
{
    NoDebugTerm = 0,
    NormalTerm = 1,
    AmbientOcclusionTerm = 2,
    AlbedoTerm = 3,
    IBLDiffuseTerm = 4,
    IBLSpecularTerm = 5,
    MetalTerm = 6,
    RoughnessTerm = 7,
    BrdfTerm = 8
};

class Material : public RenderNode
{
  public:
    Material(Ctr::IDevice* device, 
             const std::string& filePathName = std::string());
    virtual ~Material();

    // Load from xml.
    bool                       load (const std::string& filename);

    const std::string &        shaderName() const;
    const std::string &        techniqueName() const;
    void                       setShaderName(const std::string &);
    void                       setTechniqueName(const std::string &);

    void                       setTechnique(const GpuTechnique*);
    void                       setShader(const IShader* shader);

    void                       addPass(const std::string& passName);

    const GpuTechnique*        technique() const;
    const IShader*             shader() const;

    const Ctr::ITexture*       specularRMCMap() const;
    const Ctr::ITexture*       normalMap() const;
    const Ctr::ITexture*       environmentMap() const;
    const Ctr::ITexture*       albedoMap() const;
    const Ctr::ITexture*       detailMap() const;

    void                       setAlbedoMap(Ctr::ITexture* texture);
    void                       setNormalMap(Ctr::ITexture* texture);
    void                       setEnvironmentMap(Ctr::ITexture* texture);
    void                       setSpecularRMCMap(Ctr::ITexture* texture);
    void                       setDetailMap(Ctr::ITexture* texture);

    void                       setAlbedoMap(const std::string& filePathName);
    void                       setNormalMap(const std::string& filePathName);
    void                       setEnvironmentMap(const std::string& filePathName);
    void                       setSpecularRMCMap(const std::string& filePathName);
    void                       setDetailMap(const std::string& filePathName);

    const std::vector<std::string>& passes() const;

    Ctr::BoolProperty*          twoSidedProperty();
    bool                        twoSided() const;

    Ctr::FloatProperty*         textureGammaProperty();
    float                       textureGamma() const;

    Ctr::Vector4fProperty*      albedoColorProperty();
    const Ctr::Vector4f&        albedoColor() const;

    Ctr::IntProperty*           debugTermProperty();
    int32_t                     debugTerm() const;

    Ctr::IntProperty*           specularWorkflowProperty();
    int32_t                     specularWorkflow() const;
    Ctr::FloatProperty*         roughnessScaleProperty();
    float                       roughnessScale() const;
    Ctr::FloatProperty*         specularIntensityProperty();
    float                       specularIntensity() const;

    Ctr::Vector4fProperty*      userAlbedoProperty();
    const Ctr::Vector4f&        userAlbedo() const;

    Ctr::Vector4fProperty*      userRMProperty();
    const Ctr::Vector4f&        userRM() const;

    Ctr::Vector4fProperty*      iblOcclProperty();
    const Ctr::Vector4f&        iblOccl() const;

    Ctr::Vector4fProperty*      textureScaleOffsetProperty();
    const Ctr::Vector4f&        textureScaleOffset() const;

    TextureProperty*            specularRMCMapProperty();
    TextureProperty*            normalMapProperty();
    TextureProperty*            environmentMapProperty();
    TextureProperty*            albedoMapProperty();
    TextureProperty*            detailMapProperty();

  private:
    // Shader and pass management
    std::vector<std::string>   _passes;
    std::string                _shaderName;
    std::string                _techniqueName;
    const GpuTechnique*        _technique;
    const IShader*             _shader;
    RenderDebugTerm            _debugTerm;

    // Maps
    TextureProperty*            _specularRMCMap;
    TextureProperty*            _normalMap;
    TextureProperty*            _environmentMap;
    TextureProperty*            _albedoMap;
    TextureProperty*            _detailMap;

    // Flags
    Ctr::BoolProperty *         _twoSidedProperty;
    Ctr::FloatProperty *        _textureGammaProperty;
    Ctr::IntProperty*           _debugTermProperty;
    Ctr::IntProperty*           _specularWorkflowProperty;
    Ctr::FloatProperty*         _roughnessScaleProperty;
    Ctr::FloatProperty*         _specularIntensityProperty;
    Ctr::Vector4fProperty*      _albedoColorProperty;


    Ctr::Vector4fProperty*      _userAlbedoProperty;
    Ctr::Vector4fProperty*      _userRMProperty;
    Ctr::Vector4fProperty*      _iblOcclProperty;
    Ctr::Vector4fProperty*      _textureScaleOffsetProperty;
};
}

#endif