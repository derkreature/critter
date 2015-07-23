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
#include <CtrRenderPass.h>
#include <CtrIDevice.h>
#include <strstream>
#include <CtrLog.h>
#include <CtrShaderMgr.h>
#include <CtrMesh.h>
#include <CtrEntity.h>
#include <CtrScene.h>
#include <CtrMaterial.h>

#include <CtrIDevice.h>
#include <CtrIShader.h>
#include <CtrITexture.h>
#include <CtrISurface.h>
#include <CtrIDepthSurface.h>
#include <CtrCamera.h>
#include <CtrMesh.h>
#include <CtrLog.h>
#include <CtrIShader.h>
#include <CtrGpuTechnique.h>
#include <CtrIDepthSurface.h>
#include <CtrScreenOrientedQuad.h>
#include <CtrMaterial.h>
#include <CtrRenderTargetQuad.h>
#include <CtrViewport.h>
#include <CtrPostEffectsMgr.h>

namespace Ctr
{
RenderPass::RenderPass(Ctr::IDevice* device) :
    Ctr::Node (std::string("RenderPass")),
    IRenderResource (device),
    _cullMode (Ctr::CCW),
    _enabled (true)
{
}

RenderPass::~RenderPass()
{
}

const std::string&
RenderPass::passName() const
{
    return _passName;
}

void 
RenderPass::renderMeshes(const std::string& passName, const Ctr::Scene* scene)
{
    const std::vector<Ctr::Mesh*>& meshes = scene->meshesForPass(passName);
    for (auto it = meshes.begin(); it != meshes.end(); it++)
    {
        const Ctr::Mesh* mesh = (*it);
        const Ctr::Material* material = mesh->material();
        const Ctr::IShader* shader = material->shader();
        const Ctr::GpuTechnique* technique = material->technique();
    
        RenderRequest renderRequest (technique, scene, scene->camera(), mesh);
        if (mesh->visible())
        {
            shader->renderMesh(renderRequest);
        }
    }
}

}