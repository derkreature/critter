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


#include <CtrVertexDeclarationD3D11.h>
#include <CtrRenderDeviceD3D11.h>
#include <CtrVertexElement.h>
#include <CtrLog.h>

namespace Ctr
{
namespace 
{
static std::string positionSemantic = "POSITION";
static std::string normalSemantic = "NORMAL";
static std::string texCoordSemantic = "TEXCOORD";
static std::string tangentSemantic = "TANGENT";
static std::string binormalSemantic = "BINORMAL";
static std::string bonesSemantic = "BLENDINDICES";
static std::string weightsSemantic = "BLENDWEIGHT";
static std::string colorSemantic = "COLOR";
static std::string seedSemantic = "SEED";
static std::string speedSemantic = "SPEED";
static std::string randSemantic = "RAND";
static std::string typeSemantic = "TYPE";
static std::string unknownSemantic = "Unknown";
static std::string stateSemantic = "STATE";
static std::string oldStateSemantic = "OLDSTATE";

const std::string&
usageToString (BYTE usage)
{
    if (usage == Ctr::POSITION)
    {
        return positionSemantic;
    }
    if (usage == Ctr::NORMAL)
    {
        return normalSemantic;
    }
    if (usage == Ctr::TEXCOORD)
    {
        return texCoordSemantic;
    }
    if (usage == Ctr::TANGENT)
    {
        return tangentSemantic;
    }
    if (usage == Ctr::BINORMAL)
    {
        return binormalSemantic;
    }
    if (usage == Ctr::BLENDINDICES)
    {
        return bonesSemantic;
    }
    if (usage == Ctr::BLENDWEIGHT)
    {
        return weightsSemantic;
    }
    if (usage == Ctr::COLOR)
    {
        return colorSemantic;
    }
    if (usage == Ctr::SEED)
    {
        return seedSemantic;
    }
    if (usage == Ctr::SPEED)
    {
        return speedSemantic;
    }
    if (usage == Ctr::RAND)
    {
        return randSemantic;
    }
    if (usage == Ctr::TYPE)
    {
        return typeSemantic;
    }
    if (usage == Ctr::STATE)
    {
        return stateSemantic;
    }
    if (usage == Ctr::OLDSTATE)
    {
        return oldStateSemantic;
    }
    return unknownSemantic;
}
}

DXGI_FORMAT
typeToFormat (Ctr::DeclarationType type)
{
    switch (type)
    {
        case Ctr::FLOAT4: 
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Ctr::FLOAT3:  
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Ctr::FLOAT2: 
            return DXGI_FORMAT_R32G32_FLOAT;
        case Ctr::FLOAT1: 
            return DXGI_FORMAT_R32_FLOAT;
        case Ctr::UBYTE4:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Ctr::UINT8:
            return DXGI_FORMAT_R8_UINT;
        case Ctr::UINT32:
            return DXGI_FORMAT_R32_UINT;
        default:
            break;
    };
    return DXGI_FORMAT_UNKNOWN;
}

VertexDeclarationD3D11::VertexDeclarationD3D11 (Ctr::DeviceD3D11* device) :
    Ctr::IVertexDeclaration (device),
    _vertexAttributes (nullptr),
    _elementCount (0),
    _vertexStride (0),
    _direct3d(nullptr),
    _immediateCtx(nullptr)
{
    if (Ctr::DeviceD3D11* _device = 
        dynamic_cast <Ctr::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }
}

VertexDeclarationD3D11::~VertexDeclarationD3D11()
{
    _declaration = std::vector <Ctr::VertexElement>();
    safedeletearray(_vertexAttributes);
}

const std::vector <Ctr::VertexElement>      & 
VertexDeclarationD3D11::getDeclaration() const
{
    return _declaration;
}

bool
VertexDeclarationD3D11::free()
{
    return true;
}

bool
VertexDeclarationD3D11::create()
{
    return (_vertexStride > 0);
}

bool
VertexDeclarationD3D11::cache()
{
    return true;
}

bool
VertexDeclarationD3D11::initialize (const Ctr::VertexDeclarationParameters* resource)
{  
    safedelete(_vertexAttributes);
    const std::vector<Ctr::VertexElement> & elements = resource->elements();

    if (elements.size() == 0)
        return false;
    _vertexAttributes = new D3D11_INPUT_ELEMENT_DESC[elements.size()];

    uint32_t index = 0;
    for (uint32_t i = 0; i < elements.size() - 1; i++)
    {
        const Ctr::VertexElement& element = elements[i];
        _vertexAttributes[index] = 
            createVertexElement (element.stream(), element.offset(), 
                                 element.type(), element.method(), 
                                 element.usage(), element.usageIndex(),
                                 element.streamIndex());
        index++;
    }

    Ctr::VertexElement element = elements[elements.size()-2];
    _vertexStride = (element.offset() + VertexDeclarationD3D11::elementToSize(element.type()));    
    _elementCount = (uint32_t)(elements.size()) - 1;    


    index = 0;
    for (auto it = elements.begin();
         it != elements.end();
         it++)
    {
        _declaration.push_back (*it);
        index++;
    }

    return create();
}

bool
VertexDeclarationD3D11::bind() const
{
    return false;
}

D3D11_INPUT_ELEMENT_DESC
VertexDeclarationD3D11::createVertexElement (WORD stream, 
                                             WORD offset, 
                                             BYTE type, 
                                             BYTE method, 
                                             BYTE usage, 
                                             BYTE usageIndex,
                                             BYTE streamIndex)
{
    D3D11_INPUT_ELEMENT_DESC element = 
    { usageToString((Ctr::DeclarationType)usage).c_str(), 
      usageIndex, 
      typeToFormat((Ctr::DeclarationType)type), 
      streamIndex, 
      offset, 
      D3D11_INPUT_PER_VERTEX_DATA, 
      0 };

    return element;
}

bool 
VertexDeclarationD3D11::isTypeOf (const Ctr::DeclarationUsage& usage, const uint32_t& index)
{
    // mdavidson, casulatiy of integration
    LOG ("Is this really still used?");
    return false;
}

uint32_t
VertexDeclarationD3D11::vertexStride() const
{
    return _vertexStride;
}

uint32_t
VertexDeclarationD3D11::vertexStreamCount() const
{
    // Always float.
    return _vertexStride / sizeof(float);
}


}