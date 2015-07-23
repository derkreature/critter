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

#ifndef INCLUDED_IBL_UI_RENDERER
#define INCLUDED_IBL_UI_RENDERER

#include <CtrPlatform.h>
#include <CtrMesh.h>
#include <CtrGpuTechnique.h>
#include <CtrColor.h>

namespace Ctr
{
class IShader;
class RenderPass;
class IIndexBuffer;
class IVertexBuffer;

class UIRenderer :
    public Ctr::Mesh
{
  public:
    UIRenderer(Ctr::IDevice* device);
    virtual ~UIRenderer();

    static void                create(Ctr::IDevice* device);
    static UIRenderer*         renderer();
    Ctr::IDevice*              device();

    void                       setDrawIndexed(bool drawIndexed);
    void                       setVertexBuffer(IVertexBuffer* vertexBuffer);
    void                       setShader(const Ctr::IShader* vertexBuffer);

    void                       render(uint32_t count, uint32_t offset);

    virtual bool               render(const Ctr::RenderRequest* request,
                                      const Ctr::GpuTechnique* technique) const;

    // Get a vertex buffer to satisfy a given declaration.
    IVertexBuffer*             vertexBuffer(IVertexDeclaration* declaration);
    IIndexBuffer*              indexBuffer();

    void                       setViewProj(const Ctr::Matrix44f& ortho);

  protected:
    // Pipeline State.
    Ctr::IShader*              _currentShader;
    Ctr::IVertexBuffer*        _currentVertexBuffer;

    // Buffer offset state.
    uint32_t                   _vertexOffset;
    bool                       _drawIndexed;
    uint32_t                   _primitiveCount;

    // Buffers
    Ctr::IIndexBuffer*         _indexBuffer;
    std::map<IVertexDeclaration*, IVertexBuffer*> _vertexBuffers;

    Ctr::Matrix44f             _viewProj;

    static UIRenderer*         _uiRenderer;
};
}

#endif