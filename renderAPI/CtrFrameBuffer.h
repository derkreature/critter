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
#ifndef INCLUDED_CRT_FRAMEBUFFER
#define INCLUDED_CRT_FRAMEBUFFER

#include <CtrPlatform.h>

namespace Ctr
{
class ISurface;
class IDepthSurface;
class IGpuBuffer;
class Surface;
class IDepthSurface;
class IRenderResource;
//-----------------------------------------------
// Caches frame buffer attacments for direct3d.
// Allows device to track invalid attachements
// and correct / warn against invalid selections.
//-----------------------------------------------
class FrameBuffer 
{
  public:
    FrameBuffer();
    FrameBuffer(const FrameBuffer&);
    FrameBuffer(const Ctr::ISurface* colorSurface,
                const Ctr::IDepthSurface* depthSurface);
    ~FrameBuffer();

    void                       setColorSurface (size_t index, 
                                                const Ctr::ISurface* surface);
    void                       setDepthSurface(const Ctr::IDepthSurface* surface);
    void                       setUnorderedSurface (size_t index, 
                                                    const Ctr::IRenderResource* unorderedSurface);

    const Ctr::IDepthSurface*   depthSurface () const;
    const Ctr::ISurface *       colorSurface (size_t index) const;
    const Ctr::IRenderResource* unorderedSurface(size_t index) const;

  protected:
    const Ctr::ISurface*            _colorSurfaces[4];
    const Ctr::IDepthSurface*       _depthSurface;
    const Ctr::IRenderResource *    _unorderedSurfaces[4];
};
}

#endif