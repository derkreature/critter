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

#include <CtrEffectD3D11.h>
#include <CtrITexture.h>
#include <CtrVector4.h>
#include <CtrVector2.h>
#include <CtrGpuVariableD3D11.h>
#include <CtrGpuConstantBufferD3D11.h>
#include <CtrLog.h>
#include <CtrTextureD3D11.h>
#include <CtrDepthSurfaceD3D11.h>
#include <CtrVertexBufferD3D11.h>
#include <CtrIndexBufferD3D11.h>
#include <CtrBufferD3D11.h>

namespace Ctr
{
EffectD3D11::EffectD3D11(ID3DX11Effect* effect) : 
IEffect (0),
_effect (effect)
{
}

EffectD3D11::~EffectD3D11() 
{
    free();
}

bool
EffectD3D11::free()
{
    saferelease(_effect);
    return true; 
}

bool
EffectD3D11::create()
{ 
    return true; 
}

bool
EffectD3D11::cache()
{ 
    return true; 
}

const ID3DX11Effect*
EffectD3D11::effect() const
{
    return _effect;
}

ID3DX11Effect*
EffectD3D11::effect()
{
    return _effect;
}

}
