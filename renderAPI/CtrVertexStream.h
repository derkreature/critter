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

#ifndef INCLUDED_CRT_VERTEX_STREAM
#define INCLUDED_CRT_VERTEX_STREAM

#include <CtrPlatform.h>
#include <CtrVertexElement.h>
#include <CtrRenderEnums.h>

namespace Ctr
{
class VertexStream  
{
  public:
    VertexStream(Ctr::DeclarationUsage usage,
                 uint32_t usageIndex,
                 uint32_t stride, 
                 uint32_t count, 
                 const float*);

    virtual ~VertexStream();

    const float*               stream () const;    
    float*                       stream ();

    const uint32_t               stride () const;
    const uint32_t               count () const;
    const uint32_t               size () const;
    const uint32_t               bufferSizeInBytes() const;
    const Ctr::DeclarationUsage           usage () const;
    const uint32_t               usageIndex() const;

    void                       setStream (uint32_t stride,
                                          uint32_t count, const float*);

    static uint32_t            id (const VertexStream&); 
    static uint32_t            id (const VertexElement& element);

    void                       optimize();

protected:
    void                       setStream(const float*);

  private:
    uint32_t                  _stride;
    uint32_t                  _count;
    float*                    _stream;
    Ctr::DeclarationUsage      _usage;
    uint32_t                  _usageIndex;
};
}
#endif