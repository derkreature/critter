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
#ifndef INCLUDED_POST_COLORRESOLVE
#define INCLUDED_POST_COLORRESOLVE

#include <CtrPostEffect.h>

namespace Ctr
{
class ColorResolve : public Ctr::PostEffect
{
  public:
    ColorResolve (Ctr::IDevice* device);
    virtual ~ColorResolve();

    virtual bool free();

    virtual bool cache();

    virtual bool                update (float elapsed){return true;};
    bool                        render (const Ctr::ISurface* dest, 
                                        const Ctr::ITexture* source,
                                        const Ctr::Camera* camera);

    bool                        render (const Ctr::ISurface* dest, 
                                        const Ctr::ITexture* source,
                                        const Ctr::Camera* camera,
                                        size_t arrayOffset);

  protected:
    const GpuTechnique*        _arrayColorResolve;        
    const GpuTechnique*        _colorResolve;        
    const GpuTechnique*        _colorResolve2;        
    const GpuTechnique*        _colorResolve4;        
    const GpuTechnique*        _colorResolve8;    


    const GpuVariable*         _arrayOffset;
    const GpuVariable*         _sourceWidthVariable;
    const GpuVariable*         _sourceHeightVariable;
    const GpuVariable*         _colorSourceArray;
    const GpuVariable*         _colorSource;
    const GpuVariable*         _colorSource2;
    const GpuVariable*         _colorSource4;
    const GpuVariable*         _colorSource8;

    const IEffect*             _effect;
};

}

#endif