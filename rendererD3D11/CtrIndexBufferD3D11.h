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

#ifndef INCLUDED_CRT_INDEX_BUFFER_D3D11
#define INCLUDED_CRT_INDEX_BUFFER_D3D11

#include <CtrPlatform.h>
#include <CtrRenderDeviceD3D11.h>
#include <CtrIIndexBuffer.h>

namespace Ctr
{
class IndexBufferD3D11 : public Ctr::IIndexBuffer
{
public:
    IndexBufferD3D11(Ctr::DeviceD3D11* device);
    virtual ~IndexBufferD3D11();

    virtual bool                initialize(const Ctr::IndexBufferParameters* data);
    virtual bool                create();
    virtual bool                free();
    virtual bool                cache();

    virtual void*               lock(size_t byteSize);
    virtual bool                unlock();
    virtual bool                bind(uint32_t bufferOffset) const;
    ID3D11ShaderResourceView *  resourceView() const;

private:
    bool                        _locked;
    ID3D11Buffer*               _indexBuffer;
    ID3D11Buffer*               _stagingBuffer;

    mutable ID3D11ShaderResourceView *  _resourceView;
    Ctr::IndexBufferParameters  _resource;

    size_t                       _sizeInBytes;
    size_t                       _bufferCursor;
    size_t                       _lastCopySize;
    bool                         _needsDiscard;
    bool                         _isRingBuffer;
    bool                        _dynamic;

protected:
    ID3D11Device*               _direct3d;
    ID3D11DeviceContext *      _immediateCtx;
};
}

#endif