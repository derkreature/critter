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

#include <CtrRenderTargetQuad.h>
#include <CtrIVertexDeclaration.h>
#include <CtrVertexStream.h>
#include <CtrVector4.h>
#include <CtrLog.h>
#include <CtrVertexElement.h>
#include <CtrVertexDeclarationMgr.h>
#include <CtrISurface.h>

namespace Ctr
{
RenderTargetQuad::RenderTargetQuad(Ctr::IDevice* device) : IndexedMesh  (device)
{
}

bool
RenderTargetQuad::initialize (const Ctr::Region2f& screenLocation)
{    
    setPrimitiveType (Ctr::TriangleList);
    setPrimitiveCount (2);
    setVertexCount (4);
    _screenLocation = screenLocation;

    uint32_t width  = _device->backbuffer()->width();
    uint32_t height = _device->backbuffer()->height();

    uint32_t indices[] = {0, 1, 2, 1, 2, 3};
    IndexedMesh::setIndices (indices, 6, 2);

    //create vertex streams and vertex declaration information
    std::vector<VertexElement> vertexElements;
    vertexElements.push_back (VertexElement( 0, 0,  FLOAT4, METHOD_DEFAULT, POSITION, 0));
    vertexElements.push_back (VertexElement( 0, 16, FLOAT2, METHOD_DEFAULT, TEXCOORD, 0));
    vertexElements.push_back (VertexElement(0xFF,0, UNUSED,0,0,0));

    if (IVertexDeclaration* vertexDeclaration = 
        VertexDeclarationMgr::vertexDeclarationMgr()
        ->createVertexDeclaration (&VertexDeclarationParameters (vertexElements)))
    {
        float lhx = _screenLocation.minExtent.x * width;
        float lhy = _screenLocation.minExtent.y * height;
        float screenWidth = _screenLocation.size().x * width;
        float screenHeight = _screenLocation.size().y * height;

        float dx = _device->texelIsCenter() ? (1.0f / width) : 0;
        float dy = _device->texelIsCenter() ? (1.0f / height) : 0;
        
        Vector4f vpos[] = {Vector4f (-1, -1, 1, 1), 
                           Vector4f (-1, 1.0, 1, 1), 
                           Vector4f (1.0, -1, 1, 1), 
                           Vector4f (1.0, 1.0, 1, 1) };    
        Vector2f tpos[] = {Vector2f(dx, 1.0f + dy), Vector2f(dx, dy), 
                           Vector2f(1.0f + dy , 1.0f +dy), Vector2f(1.0f + dx, dy) };    

        setVertexDeclaration (vertexDeclaration);
        _positionStream = new Ctr::VertexStream
            (Ctr::POSITION, 0, 4, 4, (float*)vpos);
        _texCoordStream = new Ctr::VertexStream
            (Ctr::TEXCOORD, 0, 2, 4, (float*)tpos);
        addStream (_positionStream);
        addStream (_texCoordStream);

        return true;
    }
    return false;
}

RenderTargetQuad::~RenderTargetQuad()
{
}

}