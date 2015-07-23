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
#include <CtrMaterial.h>
#include <CtrAssetManager.h>
#include <CtrTextureMgr.h>
#include <pugixml.hpp>
#include <Ctrimgui.h>

namespace Ctr
{
namespace
{


bool stringToBool(const std::string& value)
{
    if (value == "True")
        return true;
    else
        return false;
}
}

Material::Material (IDevice* device,    
                    const std::string& filePathName) :
    RenderNode(device),
    _shader (nullptr),
    _technique(nullptr),
    // Flags
    _twoSidedProperty(new Ctr::BoolProperty(this, "TwoSided", new TweakFlags(0, 1, 1, "Material"))),
    _textureGammaProperty(new Ctr::FloatProperty(this, "Input Gamma", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _debugTermProperty(new IntProperty(this, "Debug Visualization")),
    _albedoColorProperty(new Ctr::Vector4fProperty(this, "Albedo Color", new TweakFlags(0, 6, 1e-2f, "Material"))),
    _specularWorkflowProperty(new IntProperty(this, "Specular workflow")),
    _roughnessScaleProperty(new Ctr::FloatProperty(this, "Roughness/Gloss Scale", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _specularIntensityProperty(new Ctr::FloatProperty(this, "Specular Intensity", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _userAlbedoProperty(new Ctr::Vector4fProperty(this, "userAlbedo", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _userRMProperty(new Ctr::Vector4fProperty(this, "userRMC", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _iblOcclProperty(new Ctr::Vector4fProperty(this, "iblOccl", new TweakFlags(1e-2f, 6, 1e-2f, "Material"))),
    _specularRMCMap(new Ctr::TextureProperty(this, "rmc")),
    _normalMap(new Ctr::TextureProperty(this, "normal")),
    _environmentMap(new Ctr::TextureProperty(this, "env")),
    _albedoMap(new Ctr::TextureProperty(this, "albedo")),
    _detailMap(new Ctr::TextureProperty(this, "detail")),
    _textureScaleOffsetProperty(new Ctr::Vector4fProperty(this, "Texture Scale Offset"))
{
    _textureScaleOffsetProperty->set(Ctr::Vector4f(1, 1, 0, 0));
    _userAlbedoProperty->set(Ctr::Vector4f(1, 1, 1, 0));
    _userRMProperty->set(Ctr::Vector4f(1, 0, 1, 0));
    _iblOcclProperty->set(Ctr::Vector4f(0,0,0,0));

    _twoSidedProperty->set(false);
    _textureGammaProperty->set(2.2f);
    _albedoColorProperty->set(Ctr::Vector4f(1,1,1,1));
    _debugTermProperty->set(NoDebugTerm);
    _specularWorkflowProperty->set(RoughnessMetal);
    _roughnessScaleProperty->set(1.0f);
    _specularIntensityProperty->set(1.0);

    if (filePathName.length() > 0)
    {
        assert(load(filePathName));
    }
}

Material::~Material()
{

// TODO: Check texture against texture manager.
//    _device->textureMgr()->recycle(_albedoMap);
//    _device->textureMgr()->recycle(_detailMap);
//    _device->textureMgr()->recycle(_normalMap);
//    _device->textureMgr()->recycle(_environmentMap);
//    _device->textureMgr()->recycle(_specularRMCMap);
}
Ctr::Vector4fProperty*
Material::textureScaleOffsetProperty()
{
    return _textureScaleOffsetProperty;
}

const Ctr::Vector4f&
Material::textureScaleOffset() const
{
    return _textureScaleOffsetProperty->get();
}

TextureProperty*
Material::specularRMCMapProperty()
{
    return _specularRMCMap;
}

TextureProperty*
Material::normalMapProperty()
{
    return _normalMap;
}

TextureProperty*
Material::environmentMapProperty()
{   
    return _environmentMap;
}

TextureProperty*
Material::albedoMapProperty()
{
    return _albedoMap;
}

TextureProperty*
Material::detailMapProperty()
{
    return _detailMap;
}

const Ctr::Vector4f&
Material::userAlbedo() const
{
    return _userAlbedoProperty->get();
}

Ctr::Vector4fProperty*
Material::userRMProperty()
{
    return _userRMProperty;
}

const Ctr::Vector4f&
Material::userRM() const
{
    return _userRMProperty->get();
}

const Ctr::Vector4f& 
Material::iblOccl() const
{
    return _iblOcclProperty->get();
}

Ctr::Vector4fProperty*
Material::userAlbedoProperty()
{
    return _userAlbedoProperty;
}

Ctr::Vector4fProperty*
Material::iblOcclProperty()
{
    return _iblOcclProperty;
}

Ctr::BoolProperty*
Material::twoSidedProperty()
{
    return _twoSidedProperty;
}

bool
Material::twoSided() const
{
    return _twoSidedProperty->get();
}

Ctr::FloatProperty*
Material::textureGammaProperty()
{
    return _textureGammaProperty;
}

float
Material::textureGamma() const
{
    return _textureGammaProperty->get();
}

Ctr::Vector4fProperty*
Material::albedoColorProperty()
{
    return _albedoColorProperty;
}

const Ctr::Vector4f&
Material::albedoColor() const
{
    return _albedoColorProperty->get();
}

Ctr::IntProperty*
Material::debugTermProperty()
{
    return _debugTermProperty;
}

int32_t
Material::debugTerm() const
{
    return _debugTermProperty->get();
}


void
Material::setShaderName(const std::string & value)
{
    _shaderName = value;
}

void
Material::setTechniqueName(const std::string & value)
{
    _techniqueName = value;
}
const Ctr::ITexture*
Material::specularRMCMap() const
{
    return _specularRMCMap->get();
}

const Ctr::ITexture*
Material::normalMap() const
{
    return _normalMap->get();
}

const Ctr::ITexture*
Material::environmentMap() const
{
    return _environmentMap->get();
}

const Ctr::ITexture*
Material::albedoMap() const
{
    return _albedoMap->get();
}

const Ctr::ITexture*
Material::detailMap() const
{
    return _detailMap->get();
}

bool
Material::load(const std::string& filename)
{
    if (std::unique_ptr<typename pugi::xml_document> doc =
        std::unique_ptr<typename pugi::xml_document>(Ctr::AssetManager::assetManager()->openXmlDocument(filename)))
    {
        LOG ("Loaded manifest file " << filename);

        // Selet node.
        pugi::xpath_node_set shaderSet = doc->select_nodes("/Materials/Material");
        for (auto nodeIt = shaderSet.begin(); nodeIt != shaderSet.end(); ++nodeIt)
        {
            auto xpathNode = *nodeIt;
            if (xpathNode)
            {
                // Load default user shaders.
                if (const char* xpathValue = xpathNode.node().attribute("ShaderName").value())
                {
                    _shaderName = std::string(xpathValue);
                }
                if (const char* xpathValue = xpathNode.node().attribute("TechniqueName").value())
                {
                    _techniqueName= std::string(xpathValue);
                }

                // Load textures.
                if (const char* xpathValue = xpathNode.node().attribute("AlbedoMap").value())
                {
                    std::string albedoMapPathName = xpathValue;
                    setAlbedoMap(albedoMapPathName);
                }

                if (const char* xpathValue = xpathNode.node().attribute("NormalMap").value())
                {
                    setNormalMap(xpathValue);
                }

                if (const char* xpathValue = xpathNode.node().attribute("SpecularRMCMap").value())
                {
                    setSpecularRMCMap(xpathValue);
                }

                // Load flags
                if (const char* xpathValue = xpathNode.node().attribute("TwoSided").value())
                {
                    _twoSidedProperty->set(stringToBool(xpathValue));
                }

                if (const char* xpathValue = xpathNode.node().attribute("TextureGamma").value())
                {
                    _textureGammaProperty->set((float)(atof(xpathValue)));
                }

               pugi::xpath_node_set passes = xpathNode.node().select_nodes("Pass");
               for (auto passIt = passes.begin(); passIt != passes.end(); passIt++)
               {
                   LOG("Contributes to pass: " << (*passIt).node().attribute("name").value());
                    _passes.push_back((*passIt).node().attribute("name").value());
               }

                break;
            }
        }

        return true;
    }
    else
    {
        THROW ("Failed to load material " << filename);
        return false;
    }
    return false;
}

const std::string & 
Material::shaderName() const
{
    return _shaderName;
}

const std::string & 
Material::techniqueName() const
{
    return _techniqueName;
}

void
Material::setTechnique(const GpuTechnique* technique)
{
    _technique = technique;
}

void
Material::setShader(const IShader* shader)
{
    _shader = shader;
}


void
Material::addPass(const std::string& passName)
{
    auto it = std::find(_passes.begin(), _passes.end(), passName);
    if (it == _passes.end())
        _passes.push_back(passName);
}

const GpuTechnique*
Material::technique() const
{
    return _technique;
}

const IShader*
Material::shader() const
{
    return _shader;
}

const std::vector<std::string>& 
Material::passes() const
{
    return _passes;
}

void
Material::setAlbedoMap(Ctr::ITexture* texture)
{
    _albedoMap->set(texture);
}

void
Material::setDetailMap(Ctr::ITexture* texture)
{
    _detailMap->set(texture);
}

void
Material::setNormalMap(Ctr::ITexture* texture)
{
    _normalMap->set(texture);
}

void
Material::setEnvironmentMap(Ctr::ITexture* texture)
{
    _environmentMap->set(texture);
}

void
Material::setSpecularRMCMap(Ctr::ITexture* texture)
{
    _specularRMCMap->set(texture);
}

void
Material::setAlbedoMap(const std::string& filePathName)
{
    setAlbedoMap(_device->textureMgr()->loadTexture(filePathName));
}

void
Material::setDetailMap(const std::string& filePathName)
{
    setDetailMap(_device->textureMgr()->loadTexture(filePathName));
}

void
Material::setNormalMap(const std::string& filePathName)
{
    
    setNormalMap(_device->textureMgr()->loadTexture(filePathName));
}

void
Material::setEnvironmentMap(const std::string& filePathName)
{
    setEnvironmentMap(_device->textureMgr()->loadTexture(filePathName));
}

void
Material::setSpecularRMCMap(const std::string& filePathName)
{
    setSpecularRMCMap(_device->textureMgr()->loadTexture(filePathName));
}

Ctr::IntProperty*
Material::specularWorkflowProperty()
{
    return _specularWorkflowProperty;
}

int32_t 
Material::specularWorkflow() const
{
    return _specularWorkflowProperty->get();
}

Ctr::FloatProperty*
Material::roughnessScaleProperty()
{
    return _roughnessScaleProperty;
}

float
Material::roughnessScale() const
{
    return _roughnessScaleProperty->get();
}

Ctr::FloatProperty*
Material::specularIntensityProperty()
{
    return _specularIntensityProperty;
}

float
Material::specularIntensity() const
{
    return _specularIntensityProperty->get();
}


}

