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

#ifndef INCLUDED_TECHNIQUE_D3D11
#define INCLUDED_TECHNIQUE_D3D11

#include <CtrPlatform.h>
#include <CtrGpuTechnique.h>
#include <CtrEffectD3D11.h>

namespace Ctr
{
class Mesh;
class InputLayoutCacheD3D11;

//---------------------------------
// Manages a directx HLSL technique
//---------------------------------
class GpuTechniqueD3D11 : public Ctr::GpuTechnique
{
  public:
    GpuTechniqueD3D11(Ctr::IDevice * device);
    virtual ~GpuTechniqueD3D11();

    virtual ID3DX11EffectTechnique*       handle() const;
    //-------------------------------
    // Set the handle of the techniqe
    //-------------------------------
    virtual void                setHandle (ID3DX11EffectTechnique* handle);

    //-------------------
    // resource handlers
    //-------------------
    virtual bool                create(){ return true; };
    virtual bool                cache(){ return true; };
    virtual bool                free();
    
    //------------------------------
    // Get the name of the technique
    //------------------------------
    virtual const std::string&  name() const;

    //-----------------------------
    // Creates the technique object
    //-----------------------------
    virtual bool                initialize (Ctr::IEffect* effect, 
                                            ID3DX11EffectTechnique* techniquehandle);


    virtual Ctr::IEffect* effect () const;
    const D3DX11_TECHNIQUE_DESC& description() const;

    bool                        setupInputLayout (const Ctr::Mesh* mesh,
                                                  uint32_t passIndex) const;

    virtual bool                hasTessellationStage() const;

  protected:
    typedef std::vector<InputLayoutCacheD3D11*>       InputLayoutCacheVector;
    EffectD3D11*                _effect;
    InputLayoutCacheVector      _layoutCache;

    ID3DX11EffectTechnique*     _handle;
    D3DX11_TECHNIQUE_DESC       _desc;

    std::string                 _name;
    std::string                 _description;    
    bool                        _hasTessellationStage;

  protected:
    ID3D11Device*                _direct3d;
    ID3D11DeviceContext *      _immediateCtx;
};    
}

#endif