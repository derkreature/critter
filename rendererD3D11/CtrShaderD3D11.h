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

#ifndef INCLUDED_CRT_SHADER
#define INCLUDED_CRT_SHADER

#include <CtrPlatform.h>
#include <CtrIShader.h>
#include <CtrVector2.h>

namespace Ctr
{
class EffectD3D11;
class DeviceD3D11;

class Mesh;
class PostEffect;
class ITexture;
class GpuTechnique;
class GpuVariable;
class GpuConstantBuffer;
class IEffect;
class ShaderParameterValue;
class Camera;
class Light;
class DeviceD3D11;

class ShaderD3D11: public Ctr::IShader
{
  public:
    typedef std::list<Ctr::GpuTechnique*>            TechniqueList;
    typedef std::list<Ctr::GpuVariable*>             VariableList;
    typedef std::list<const Ctr::ShaderParameterValue*>   VariableValueList;
    typedef std::list<Ctr::GpuConstantBuffer*>            ConstantBufferList;

  public:
    ShaderD3D11(Ctr::DeviceD3D11* device);
    virtual ~ShaderD3D11();
    
    //-----------------------
    // Frees shader resources
    //-----------------------
    virtual bool                free();

    //----------------------------
    // re creates shader resources
    //----------------------------
    virtual bool                create();
    virtual bool                cache();

    //----------------------------------
    // Creates a shader in the shadermgr
    //----------------------------------
    virtual bool                initialize (const std::string& filename, 
                                            const std::string& includePathName,
                                            bool  verbose, 
                                            bool  allowDeprecated);

    virtual bool                initialize(const std::string& file,
                                           const std::string& include,
                                           bool verbose,
                                           bool allowDeprecated,
                                           const std::map<std::string, std::string>& defines);

    //-----------------------------
    // Gets the specified technique
    //-----------------------------
    virtual bool               getTechniqueByName (const std::string& name, 
                                                   const Ctr::GpuTechnique*& _technique) const;

    virtual bool               getParameterByName (const std::string& parameterName,
                                                   const Ctr::GpuVariable*& coreVariable) const;

    virtual bool               getConstantBufferByName(const std::string& constantBufferName,
                                                       const Ctr::GpuConstantBuffer*&) const;


    virtual bool               renderMesh (const Ctr::RenderRequest& request) const;


    bool                       renderMeshes (const Ctr::RenderRequest& request,
                                             const std::set<const Ctr::Mesh*>      & mesh) const;

    virtual bool               renderInstancedBuffer (const Ctr::RenderRequest& request,
                                                      const Ctr::IGpuBuffer* instanceBuffer)  const;

    virtual bool               renderMeshSubset (Ctr::PrimitiveType primitiveType,
                                                 size_t startIndex,
                                                 size_t numIndices,
                                                 const Ctr::RenderRequest& request) const;

    //-------------------------------------------
    // Passes variables to find shader parameters
    //-------------------------------------------
    bool                       passVariables();

    bool                       setTechniqueParameters (const Ctr::RenderRequest& request) const;

    bool                       setMeshParameters (const Ctr::RenderRequest& request) const;

    //--------------------------------------------
    // Sets up parameters for rendering for a mesh
    //--------------------------------------------
    bool                        setParameters (const Ctr::RenderRequest& request) const;

    //---------------------------------------------------
    // Gets the parameter type for the specified variable
    //---------------------------------------------------    
    void                        getParameterType (Ctr::GpuVariable* param);

    //-----------------------------------------------------
    // Sets up parameters for rendering for a render target
    //-----------------------------------------------------
    bool                        setParameters (const Ctr::PostEffect* target) const;

    //-------------------------------------------
    // Gets the count of techniques in the shader
    //-------------------------------------------
    uint32_t                    techniqueCount() const;

    //------------------------------------------
    // Gets the technique at the specified index
    //------------------------------------------
    virtual const Ctr::GpuTechnique*    getTechnique (uint32_t index) const;

    virtual const Ctr::IEffect* effect () const;


  protected:
    //-----------------------------------
    // Enumerate the available techniques
    //-----------------------------------
    bool                        enumerateTechniques (const D3DX11_EFFECT_DESC& desc, 
                                                     bool verbose = false);

    //----------------------------------
    // Enumerate the available variables
    //----------------------------------
    bool                        enumerateVariables (const D3DX11_EFFECT_DESC& desc, 
                                                    bool verbose = false);

    //-------------------
    // Creates the effect
    //-------------------
    bool                        createEffect ();

    void                        unbindShaderResources();

  private:
    //------------------------
    // The internal EffectD3D11
    //-------------------------
     Ctr::EffectD3D11*          _effect;

    //-----------------------
    // The list of techniques
    //-----------------------
    TechniqueList                _techniques;

    //-----------------------------
    // The list of stock parameters
    //-----------------------------
    VariableList                _parameters;

    ConstantBufferList          _constantBuffers;

    bool                        _createdFromFile;
    VariableValueList           _shaderParameterValues;
    VariableValueList           _meshParameters;
    VariableValueList           _techniqueParameters;

    //------------------------------------------------------------------
    // Maintain a cache of parameters that should be
    // set to nullptr after rendering all of the meshes assinged
    // to this shader. This is necessary to avoid binding rendertargets
    // that are bound as textures.
    //------------------------------------------------------------------
    VariableValueList           _resourceParameters;
    bool                        _verbose;
    bool                        _allowDeprecated;

    std::map<std::string, std::string> _defines;

    char *                      _compiledBuffer;
    size_t                      _compiledBufferSize;


  protected:
    ID3D11Device*               _direct3d;
    ID3D11DeviceContext *       _immediateCtx;
};
}

#endif