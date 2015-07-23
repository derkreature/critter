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

#include <CtrUIRenderer.h>
#include <CtrIVertexDeclaration.h>
#include <CtrVertexElement.h>
#include <CtrVertexDeclarationMgr.h>
#include <CtrMaterial.h>
#include <CtrIVertexBuffer.h>
#include <CtrIDevice.h>
#include <CtrIIndexBuffer.h>
#include <CtrGpuTechnique.h>
#include <CtrGpuVariable.h>
#include <CtrIShader.h>
#include <CtrScene.h>

namespace Ctr
{

UIRenderer*         UIRenderer::_uiRenderer = nullptr;

void
UIRenderer::create(Ctr::IDevice* device)
{
    _uiRenderer = new UIRenderer(device);
}

UIRenderer*
UIRenderer::renderer()
{
    return _uiRenderer;
}

UIRenderer::UIRenderer(Ctr::IDevice* device) :
    Ctr::Mesh (device),
    _vertexOffset(0),
    _indexBuffer(nullptr),
    _currentShader(nullptr),
    _currentVertexBuffer(nullptr)
{
    setDynamic (true);
    setPrimitiveType(Ctr::TriangleList);

    // Setup a Default Material.
    setMaterial(new Material(device));

    // Setup Ringbuffered index buffer.
    IndexBufferParameters ibResource = IndexBufferParameters(sizeof(uint32_t)*15000, true, true);
    _indexBuffer = _device->createIndexBuffer(&ibResource);
    if (!_indexBuffer)
    {
        LOG("Failed to create ringed IndexBuffer for UIRenderer")
        assert(0);
    }
}

UIRenderer::~UIRenderer()
{
    if (_indexBuffer)
        delete _indexBuffer;
    for (auto it = _vertexBuffers.begin(); it != _vertexBuffers.end(); it++)
    {
        delete it->second;
    }
}

Ctr::IDevice*
UIRenderer::device()
{
    return _device;
}

void
UIRenderer::setDrawIndexed(bool drawIndexed)
{
    _drawIndexed = drawIndexed;
}

void
UIRenderer::setVertexBuffer(IVertexBuffer* vertexBuffer)
{
    _currentVertexBuffer = vertexBuffer;
    _vertexDeclaration = vertexBuffer->vertexDeclaration();
}

void
UIRenderer::setShader(const Ctr::IShader* shader)
{
    _material->setShader(shader);
    _material->setTechnique(shader->getTechnique(0));
}

void
UIRenderer::setViewProj(const Ctr::Matrix44f& viewProj)
{
    _viewProj = viewProj;
}

IVertexBuffer*
UIRenderer::vertexBuffer(IVertexDeclaration* declaration)
{
    // This is a bit of a waste. I really prefer the bgfx "Transient Buffer" idea.

    // Find the matching vertex declaration for this buffer.
    // If it doesn't exist, create it.
    auto vit = _vertexBuffers.find(declaration);
    if (vit == _vertexBuffers.end())
    {
        // Create one.
        VertexBufferParameters vertexBufferParameters;
        vertexBufferParameters =
            VertexBufferParameters((uint32_t)(10000 * (declaration->vertexStride())), true, false,
            declaration->vertexStride(),
            nullptr, false, true);
        if (IVertexBuffer* vertexBuffer = _device->createVertexBuffer(&vertexBufferParameters))
        {
            vertexBuffer->setVertexDeclaration(declaration);
            _vertexBuffers[declaration] = vertexBuffer;
            vit = _vertexBuffers.find(declaration);
            assert(vit != _vertexBuffers.end());
        }
        else
        {
            LOG("Failed to create ringbuffered vertex buffer for UIRenderer");
            assert(0);
        }
    }

    return vit->second;
}

IIndexBuffer*
UIRenderer::indexBuffer()
{
    return _indexBuffer;
}

bool
UIRenderer::render(const Ctr::RenderRequest* request,
                   const Ctr::GpuTechnique* technique) const
{
    if (_drawIndexed)
    {
        return _device->drawIndexedPrimitive(_currentVertexBuffer->vertexDeclaration(), 
                                             _indexBuffer,
                                             _currentVertexBuffer, technique, (PrimitiveType)primitiveType(),
                                             _primitiveCount, 0, _vertexOffset);

    }
    else
    {
        return _device->drawPrimitive(_currentVertexBuffer->vertexDeclaration(), _currentVertexBuffer, technique,
            (PrimitiveType)primitiveType(), _primitiveCount, _vertexOffset);
    }

    return true;
}

void
UIRenderer::render(uint32_t count, uint32_t vertexOffset)
{
    _vertexOffset = vertexOffset;
    // Count is index or vertex count depending on indexed or not. Convert.
    const Ctr::IShader* shader = material()->shader();

    if (primitiveType() == Ctr::TriangleList)
    {
        _primitiveCount = count / 3;
    }
    else if (primitiveType() == Ctr::TriangleStrip)
    {
        _primitiveCount = (count-2);
    }

    const Ctr::GpuVariable* viewProjVariable = nullptr;
    if (shader->getParameterByName("u_viewProj", viewProjVariable))
        viewProjVariable->setMatrix(&_viewProj._m[0][0]);

    const Ctr::GpuVariable* viewTexelVariable = nullptr;
    if (shader->getParameterByName("u_viewTexel", viewTexelVariable))
    {
        Ctr::Vector4f viewTexel = Ctr::Vector4f(1.0f / _device->backbuffer()->width(), 
                                                1.0f / _device->backbuffer()->height(), 0,0);
        viewTexelVariable->setVector(&viewTexel.x);
    }


    // Try to setup ortho if it is available.
    shader->renderMesh (Ctr::RenderRequest(material()->technique(), nullptr, nullptr, this));
}

}
