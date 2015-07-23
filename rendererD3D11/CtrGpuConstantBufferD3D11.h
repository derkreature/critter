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

#ifndef INCLUDED_CONSTANT_BUFFER_SAS_VARIABLE
#define INCLUDED_CONSTANT_BUFFER_SAS_VARIABLE

#include <CtrPlatform.h>
#include <CtrShaderParameterValue.h>
#include <CtrGpuConstantBuffer.h>

namespace Ctr
{
class IEffect;
class ITexture;
class ShaderParameterValue;
class GpuTechnique;
class TextureD3D11;

class ConstantBufferD3D11 : public Ctr::GpuConstantBuffer
{
  public:
    ConstantBufferD3D11();
    virtual ~ConstantBufferD3D11();

    void                        setParameterType (Ctr::ShaderParameter type);

    virtual bool                initialize (Ctr::IEffect* effect, 
                                            const int& index);

    virtual bool                create(){ return true; };
    virtual bool                cache(){ return true; };
    virtual bool                free();

    virtual void                setConstantBuffer (const Ctr::IGpuBuffer* buffer) const;

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
    ID3DX11EffectConstantBuffer*    handle() const;


    //--------------------------
    // Gets the shader parameter
    //--------------------------
    Ctr::ShaderParameter  parameterType() const;


    Ctr::ShaderParameterValue* shaderParameterValue() const;

    Ctr::IEffect*         effect() const;


  protected:
    typedef std::map<std::string, std::string>       StringMap;
    typedef StringMap::iterator         AnnotationIt;
    typedef    StringMap::const_iterator   ConstAnnotationIt;
      
    //---------------------------------------
    // Internal handle to the shader variable
    //---------------------------------------

    ID3DX11EffectConstantBuffer*    _handle;

    //---------------------------------------
    // Description structure of the parameter    
    //---------------------------------------
    D3DX11_EFFECT_VARIABLE_DESC     _parameterDescription;

    //------------------
    // The variable name
    //------------------
    std::string                     _name;

    //------------------------------
    // Semantic Name of the variable
    //------------------------------
    std::string                 _semantic;

    //-------------------------------------
    // List of annotations for the variable
    //-------------------------------------
    StringMap                   _annotations;
    Ctr::ShaderParameter         _parameterType;
    Ctr::ShaderParameterValue*    _shaderValue;
    Ctr::IEffect*         _effect;
    
    D3DX11_EFFECT_TYPE_DESC     _variableDesc;
    ID3DX11EffectType*          _effectType;
};
}
#endif
