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

#ifndef INCLUDED_CRT_COMPUTESHADER_D3D11
#define INCLUDED_CRT_COMPUTESHADER_D3D11

#include <CtrPlatform.h>
#include <CtrIGpuBuffer.h>
#include <CtrIComputeShader.h>

namespace Ctr
{
class ComputeShaderD3D11 : public Ctr::IComputeShader
{
  public:
    ComputeShaderD3D11 (Ctr::IDevice* device);
    virtual ~ComputeShaderD3D11();

    virtual bool                initializeFromFile(const std::string& shaderFilePathName,
                                                    const std::string& includeFilePathName, 
                                                    const std::string& functionName,
                                                    const std::map<std::string, std::string>      & defines);

    virtual bool                initializeFromStream (const std::string& stream,
                                                      const std::string& functionName,
                                                      const std::map<std::string, std::string>      & defines);

    virtual bool                createConstantBuffer(size_t byteCount);
    virtual bool                updateConstantBuffer(void* ptr, size_t byteCount);

    virtual bool                bind() const;
    virtual bool                unbind() const;

    virtual bool                dispatch(const Ctr::Vector3i& groupBounds) const;

    virtual bool                setResources(const std::vector<const Ctr::IRenderResource*> & texture) const;
    virtual bool                setViews(const std::vector<const Ctr::IRenderResource*> & texture) const;

    virtual bool                create();
    virtual bool                free();
    virtual bool                cache();

    virtual const std::string&  filePathName() const;
    virtual const std::string&  includePathName() const;

  private:
    D3D10_SHADER_MACRO*         setupDefines(const std::map<std::string, std::string> & defines);

  protected:
    ID3D11ComputeShader*        _computeShader;
    ID3D11SamplerState*         _pointSampler;
    Ctr::IGpuBuffer*             _constantBuffer;
    std::vector<D3D10_SHADER_MACRO> _defines;

    std::string                 _filePathName;
    std::string                 _stream;
    std::string                 _includeFilePathName;
    std::string                 _functionName;

  private:
    ID3D11Device*                _direct3d;
    ID3D11DeviceContext *        _immediateCtx;

};
}

#endif
