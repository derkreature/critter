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

#ifndef INCLUDED_TECHNIQUE_SAS_VARIABLE
#define INCLUDED_TECHNIQUE_SAS_VARIABLE

#include <CtrPlatform.h>
#include <CtrShaderParameterValue.h>
#include <CtrGpuVariable.h>

namespace Ctr
{
class Hash;

class IEffect;
class ITexture;
class ShaderParameterValue;
class GpuTechnique;
class TextureD3D11;

//---------------------------------
// Manages a directx HLSL Variable
//---------------------------------
class GpuVariableD3D11 : public Ctr::GpuVariable
{
  public:
    GpuVariableD3D11(Ctr::IDevice* device);
    virtual ~GpuVariableD3D11();

    void                        setParameterType (Ctr::ShaderParameter type);
    virtual bool                initialize (Ctr::IEffect* effect, 
                                            const int& index);

    virtual bool                create(){ return true; };
    virtual bool                cache(){ return true; };
    virtual bool                free();

    virtual void                set (const void*, uint32_t size) const;
    virtual void                setMatrix(const float*) const;
    virtual void                setMatrixArray (const float*, uint32_t size) const;
    virtual void                setVectorArray (const float*, uint32_t size) const;    
    virtual void                setVector (const float*) const;
    virtual void                setFloatArray(const float*, uint32_t count) const;
    virtual void                setTexture (const Ctr::ITexture*) const;
    virtual void                setDepthTexture (const Ctr::IDepthSurface*) const;
    virtual void                setResource (const Ctr::IGpuBuffer*) const;
    virtual void                setUnorderedResource(const Ctr::IGpuBuffer*) const;
    virtual void                setStream (const Ctr::IVertexBuffer* vertexBuffer) const;
    virtual void                setStream (const Ctr::IIndexBuffer* indexBuffer) const;

    //-----------------------
    // Gets the semantic name
    //-----------------------
    const std::string&          semantic() const;

    //-----------------------
    // Gets the variable name
    //-----------------------
    const std::string&          name() const;

    //---------------------------
    // Gets the annotation stream
    //---------------------------
    const std::string&          annotation (const std::string&);

    //---------------------------------
    // Gets the handle for the variable
    //---------------------------------
    ID3DX11EffectVariable*      handle() const;

    //--------------------------
    // Gets the shader parameter
    //--------------------------
    Ctr::ShaderParameter    parameterType() const;

    //-------------------------------------
    // Get the default texture if available
    //-------------------------------------
    virtual const Ctr::ITexture*    texture() const;


    Ctr::ShaderParameterValue* shaderParameterValue() const;

    Ctr::IEffect*         effect() const;

    virtual void                unbind() const;

  protected:
    typedef std::map<std::string, std::string>  StringMap;
      
    //---------------------------------------
    // Internal handle to the shader variable
    //---------------------------------------
    ID3DX11EffectVariable*      _handle;

    //---------------------------------------
    // Description structure of the parameter    
    //---------------------------------------
    D3DX11_EFFECT_VARIABLE_DESC _parameterDescription;

    //------------------
    // The variable name
    //------------------
    std::string                 _name;

    //------------------------------
    // Semantic Name of the variable
    //------------------------------
    std::string                 _semantic;

    //-------------------------------------
    // List of annotations for the variable
    //-------------------------------------
    StringMap                   _annotations;
    Ctr::ShaderParameter  _parameterType;
    const TextureD3D11*         _texture;
    Ctr::ShaderParameterValue*    _shaderValue;
    Ctr::IEffect*         _effect;
    
    D3DX11_EFFECT_TYPE_DESC      _variableDesc;
    ID3DX11EffectType*           _effectType;

};
}
#endif
