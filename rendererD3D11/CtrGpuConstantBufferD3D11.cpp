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

#include <CtrGpuConstantBufferD3D11.h>
#include <CtrShaderParameterValue.h>
#include <CtrTextureMgr.h>
#include <CtrTextureD3D11.h>
#include <CtrEffectD3D11.h>
#include <CtrLog.h>
#include <CtrIEffect.h>
#include <CtrGpuTechnique.h>
#include <CtrBufferD3D11.h>

namespace Ctr
{
ConstantBufferD3D11::ConstantBufferD3D11() :
GpuConstantBuffer (0),
_handle(nullptr),
_name(""),
_semantic(""),
_parameterType(Ctr::UnknownParameter),
_effectType (nullptr)
{
}

ConstantBufferD3D11::~ConstantBufferD3D11() 
{
    free();
}


Ctr::ShaderParameter 
ConstantBufferD3D11::parameterType() const
{
    return _parameterType;
}

Ctr::IEffect*
ConstantBufferD3D11::effect() const 
{ 
    return _effect;
}

ID3DX11EffectConstantBuffer*
ConstantBufferD3D11::handle() const
{
    return _handle;
}

void ConstantBufferD3D11::setParameterType (Ctr::ShaderParameter type)
{
    _parameterType = type;
}

const std::string& 
ConstantBufferD3D11::annotation (const std::string& name)
{
    for(auto annotations = _annotations.begin(); annotations != _annotations.end(); annotations++)
    {
        if((annotations)->first == name)
        {
            return (annotations)->second;
        }
    }
    static const std::string noAnnotation = "";
    return noAnnotation;
}

bool 
ConstantBufferD3D11::initialize (Ctr::IEffect* effectInterface, const int& index)
{
    bool result = false;
    free();
    
    ID3DX11Effect* effect;
    if (EffectD3D11* effectPtr = dynamic_cast<Ctr::EffectD3D11*>(effectInterface))
    {
        effect = effectPtr->effect();
    }
    else
    {
        return false;
    }

    _handle = effect->GetConstantBufferByIndex (index);
    _handle->GetDesc (&_parameterDescription);


    _effectType = _handle->GetType();
    _effectType->GetDesc (&_variableDesc);

    _name = std::string (_parameterDescription.Name);
    _effect = effectInterface;

    return true;
}

bool
ConstantBufferD3D11::free()
{
    memset ((void*)&_parameterDescription, 
            0, sizeof(D3DX11_EFFECT_VARIABLE_DESC));

    // Effect and handles are owned by the d3dx effects framework.
    _effectType = nullptr;
    _handle = nullptr;
    return true;
}

const std::string& 
ConstantBufferD3D11::semantic() const
{
    return _semantic;
} 

const std::string& 
ConstantBufferD3D11::name() const
{
    return _name;
}

void
ConstantBufferD3D11::setConstantBuffer (const Ctr::IGpuBuffer* buffer) const
{
    if (buffer)
    {
       if (ID3DX11EffectConstantBuffer* resourceVariable = _handle)
       {
           ID3D11Buffer * constantBuffer = ((const Ctr::BufferD3D11*)buffer)->buffer();
    
           if (SUCCEEDED(resourceVariable->SetConstantBuffer(constantBuffer)))
           {
               return;
           }
           else
           {
               LOG ("Failed set buffer resource...");
           }
       }
    }
}


}