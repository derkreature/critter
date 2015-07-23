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

#ifndef INCLUDED_CRT_BRDF_LUT
#define INCLUDED_CRT_BRDF_LUT

#include <CtrPlatform.h>
#include <CtrTransformNode.h>
#include <CtrIDevice.h>
#include <CtrTextureImage.h>
#include <CtrHash.h>

namespace Ctr
{
class Entity;
class IDevice;
class IVertexDeclaration;
class IVertexBuffer;
class GpuTechnique;
class VertexStream;
class Mesh;
class Camera;
class IGpuBuffer;
class IShader;
class Scene;
class RenderRequest;
class Material;

class Brdf : public Ctr::RenderNode
{
  public:
    Brdf (Ctr::IDevice* device);
    virtual ~Brdf();

    bool                       load(const std::string& brdfInclude);

    void                       compute();
    const Ctr::ITexture*       brdfLut() const;

    const Ctr::IShader*        specularImportanceSamplingShader() const;
    const Ctr::IShader*        diffuseImportanceSamplingShader() const;

  private:
    Ctr::ITexture*             _brdfLut;

    const Ctr::IComputeShader* _brdfLutShader;
    Ctr::Hash                  _hash;

    // Cubemap importance sampling variables and shader.
    const Ctr::IShader*        _importanceSamplingShaderSpecular;
    const Ctr::IShader*        _importanceSamplingShaderDiffuse;

};
}

#endif
