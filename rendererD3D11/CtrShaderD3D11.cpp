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

#include <CtrShaderD3D11.h>
#include <CtrTextureD3D11.h>
#include <CtrMesh.h>
#include <CtrGpuTechniqueD3D11.h>
#include <CtrGpuVariableD3D11.h>
#include <CtrGpuConstantBufferD3D11.h>
#include <CtrPostEffect.h>
#include <CtrCamera.h>
#include <CtrEntity.h>
#include <CtrBufferD3D11.h>
#include <CtrShaderParameterValueFactory.h>
#include <CtrEffectD3D11.h>
#include <CtrAssetManager.h>
#include <CtrMaterial.h>
#include <d3dcompiler.h>

namespace Ctr
{
namespace
{

inline void generateDumpFile(const std::string& filename, const char* error, const std::string& shader)
{
    //LOG ("Generating FxoFileName for " << filename);

    std::string failureFilePath;

    size_t fxfilenamePos = filename.rfind("/");
    if (fxfilenamePos != std::string::npos)
    {
        std::string fxfilename = filename.substr (fxfilenamePos+1, ((filename.size()-(fxfilenamePos+1))-3)/*.fx*/); 
        std::string failurefilename = fxfilename + ".txt";
        failureFilePath = "data/shadersD3D11/shaderFailures/" + failurefilename;
        
        DeleteFileA(failureFilePath.c_str());

        FILE* file = 0;
        int result;
        if((result = fopen_s(&file, failureFilePath.c_str(), "ab+"))!=0)
            return;

        fwrite (shader.c_str(), sizeof (char), shader.length(), file);
        fwrite(error, sizeof(char), strlen(error), file);
        fclose (file);
     }

     return;
}

}

ShaderD3D11::ShaderD3D11(Ctr::DeviceD3D11* device)  :
    Ctr::IShader (device),
    _effect(nullptr),
    _createdFromFile (true),
    _compiledBuffer (nullptr),
    _compiledBufferSize(0),
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

ShaderD3D11::~ShaderD3D11()
{
    LOG ("Freeing " << includeFilePathName() << " " << filePathName());
    free();
}

bool
ShaderD3D11::cache()
{
    return true;
}

bool 
ShaderD3D11::free()
{
    for(auto it = _parameters.begin(); it != _parameters.end(); it++)
    {
        safedelete((*it));
    }

    _parameters.erase(_parameters.begin(), _parameters.end());
    for (auto it = _techniques.begin(); it != _techniques.end(); it++)
    {
        safedelete((*it));
    }

    for (auto it = _shaderParameterValues.begin(); it != _shaderParameterValues.end(); it++)
    {
        _deviceInterface->shaderValueFactory()->removeShaderParameterValue (*it);
    }

    for (auto it = _constantBuffers.begin(); it != _constantBuffers.end(); it++)
    {
        safedelete (*it);
    }

    _shaderParameterValues.clear();
    _meshParameters.clear();
    _techniqueParameters.clear();
    _parameters.clear();
    _techniques.clear();
    _constantBuffers.clear();

    safedelete (_effect);
    safedeletearray(_compiledBuffer);

    return true;
}


void
ShaderD3D11::unbindShaderResources()
{
    for(auto it = _resourceParameters.begin(); it != _resourceParameters.end(); it++)
    {
        (*it)->unbind();
    }
}

bool
ShaderD3D11::initialize (const std::string& filename, 
                         const std::string& includePathName,
                         bool  verbose, 
                         bool  allowDeprecated)
{
    setFilename(filename.c_str());
    setIncludeFilename(includePathName.c_str());

    _verbose = verbose;
    _allowDeprecated = allowDeprecated;

   return create();
}

bool
ShaderD3D11::initialize (const std::string& filePathName,
                         const std::string& includePathName,
                         bool verbose,
                         bool allowDeprecated,
                         const std::map<std::string, std::string>      & defines)
{
    _defines = defines;
    return initialize(filePathName, includePathName, verbose, allowDeprecated);
}

bool 
ShaderD3D11::create()
{
    free();


    std::string includeStream;
    if (IShader::includeFilePathName().length() > 0)
    if (std::unique_ptr<typename DataStream> fileStream =
        std::unique_ptr<typename DataStream>(AssetManager::assetManager()->openStream(IShader::includeFilePathName())))
    {
        size_t fileSize = fileStream->size();
        char* buffer = new char[fileSize + 1];
        memset(buffer, 0, fileSize + 1);
        fileStream->read(buffer, fileSize);
        includeStream = buffer;
        delete[] buffer;
    }

    std::string shaderStream;
    if (std::unique_ptr<typename DataStream> fileStream =
        std::unique_ptr<typename DataStream>(AssetManager::assetManager()->openStream(IShader::filePathName())))
    {
        size_t fileSize = fileStream->size();
        char* buffer = new char[fileSize + 1];
        memset(buffer, 0, fileSize + 1);
        fileStream->read(buffer, fileSize);
        shaderStream = buffer; 
        delete[] buffer;
    }

    setShaderStream((includeStream+shaderStream).c_str());

    if (createEffect ())
    {
        D3DX11_EFFECT_DESC desc;
        if (SUCCEEDED (_effect->effect()->GetDesc (&desc)))
        {
            enumerateTechniques (desc, _verbose);
            enumerateVariables (desc, _verbose);
            passVariables();
            return true;
        }
    }
    return false;
}

bool 
ShaderD3D11::passVariables()
{
    for (auto variable = _parameters.begin();
        variable != _parameters.end(); 
        variable++)
    {
        getParameterType ((*variable));
    }
    return true;
}

//---------------------------------------------------
// Gets the parameter type for the specified variable
//---------------------------------------------------
void 
ShaderD3D11::getParameterType(Ctr::GpuVariable* variable)
{
    if (const Ctr::ShaderParameterValue* value = 
        _deviceInterface->shaderValueFactory()->createShaderParameterValue(_deviceInterface,
                                                                         variable, _effect))
    {
        switch (value->parameterScope())
        {
            case Ctr::PerMesh:
                _meshParameters.insert (_meshParameters.begin(), value);
                break;
            case Ctr::PerTechnique:
                _techniqueParameters.insert (_techniqueParameters.begin(), value);
                break;
        }
        _shaderParameterValues.insert (_shaderParameterValues.begin(), value);
    }
}

bool 
ShaderD3D11::createEffect ()
{
    try
    {        
        uint32_t hlslFlags = 0;

        {
            hlslFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
        }
    
        ID3D10Blob * shaderCode = 0;
        ID3D10Blob * errors = 0;

        std::vector<D3D10_SHADER_MACRO>       vectorDefines;
        if (_defines.size() > 0)
        {
            for (auto it = _defines.begin(); it != _defines.end(); it++)
            {
                D3D10_SHADER_MACRO macro = {(*it).first.c_str(), (*it).second.c_str()};
                vectorDefines.push_back (macro);
            }
            D3D10_SHADER_MACRO nullMacro = {0, 0};
            vectorDefines.push_back (nullMacro);
            
            _hash.build(shaderStream());
        
            if(FAILED(D3DCompile(shaderStream().c_str(), shaderStream().size(), 0, (const D3D_SHADER_MACRO*)&*vectorDefines.begin(), nullptr, "", "fx_5_0", 
                                  hlslFlags, 0, &shaderCode, &errors)))
            { 
        
                // LOG ("Failed to compile ... " << shaderStream() << "\n");
                LOG ("Failed to compile " << IShader::filePathName());
        
                if(errors) 
                { 
        
                    char* buffer = new char[errors->GetBufferSize()+1];
                    memset(&buffer[0], 0, errors->GetBufferSize()+1);
                    memcpy(&buffer[0], errors->GetBufferPointer(), errors->GetBufferSize());
                    // LOG ("Error Compiling ShaderD3D11" << buffer);
        
                    generateDumpFile(IShader::filePathName(), buffer, shaderStream());
        
                    delete[] (buffer);
                } 
        
                return false;
            } 
        }
        else
        {
            _hash.build(shaderStream());
            if(FAILED(D3DCompile(shaderStream().c_str(), shaderStream().size(), 0, 0, nullptr, "", "fx_5_0", 
                                 D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_SKIP_OPTIMIZATION
                                 , 0,  &shaderCode, &errors)))
            { 
                LOG ("Failed to compile " << IShader::filePathName());
        
                // Open file and dump error information.
        
                if(errors) 
                { 
                    char* buffer = new char[errors->GetBufferSize()+1];
                    memset(&buffer[0], 0, errors->GetBufferSize()+1);
                    memcpy(&buffer[0], errors->GetBufferPointer(), errors->GetBufferSize());
                    generateDumpFile(IShader::filePathName(), buffer, shaderStream());
                    LOG ("Error Compiling ShaderD3D11" << buffer);
                    delete[] (buffer);
                } 
                return false;
            }
        }
        
        safedeletearray (_compiledBuffer);
        _compiledBufferSize = shaderCode->GetBufferSize();
        _compiledBuffer = new char[_compiledBufferSize];
        memcpy (_compiledBuffer, shaderCode->GetBufferPointer(), _compiledBufferSize * sizeof(char));
     
        // Create effect 
        ID3DX11Effect* effect = 0;
        if(FAILED(D3DX11CreateEffectFromMemory(_compiledBuffer, _compiledBufferSize, 0, _direct3d, &effect))) 
        { 
            LOG ("FAILED to create effect from memory ... \n");
        } 
        else
        {
            _effect = new EffectD3D11 (effect);
            return true;
        }
    }
    catch(...)
    {
        LOG ("Something bad and unknown happened while creating the shader");
    }
    return false;
}


bool 
ShaderD3D11::enumerateVariables(const D3DX11_EFFECT_DESC& desc, bool verbose)
{
    bool result = true;

    for (uint32_t i = 0; i < desc.GlobalVariables; i++)
    {
        GpuVariableD3D11 * variable = new GpuVariableD3D11(_deviceInterface);
        variable->initialize (_effect, i);
        _parameters.push_back(variable);
        
        if (variable->semantic() == "SHADERNAME")
        {
            setName(variable->annotation("ToString").c_str());                
        }
    }

    for (uint32_t i = 0; i < desc.ConstantBuffers; i++)
    {
        ConstantBufferD3D11 * constantBuffer = new ConstantBufferD3D11();
        constantBuffer->initialize (_effect, i);
        _constantBuffers.push_back(constantBuffer);
    }

    return result;        
}
bool 
ShaderD3D11::enumerateTechniques(const D3DX11_EFFECT_DESC& desc, bool verbose)
{
    bool result = true;

    for(uint32_t i = 0; i < desc.Techniques; i++)
    {
        if (ID3DX11EffectTechnique* techniquehandle = _effect->effect()->GetTechniqueByIndex(i))
        {
            GpuTechniqueD3D11 * technique = new GpuTechniqueD3D11(_deviceInterface);
            if (technique->initialize (_effect, techniquehandle))
            {
                _techniques.push_back (technique);
            }
        }
    }
    return result;
}

uint32_t 
ShaderD3D11::techniqueCount() const
{
    return (uint32_t)_techniques.size();
}

const Ctr::IEffect*
ShaderD3D11::effect() const
{
    return _effect;
}

const Ctr::GpuTechnique* 
ShaderD3D11::getTechnique(uint32_t index) const
{
    uint32_t currentindex = 0;
    for (auto technique =
        _techniques.begin(); 
        technique != _techniques.end(); 
        technique++)
    {
        if(currentindex == index)
        {
            return (*technique);
        }
        currentindex++;
    }
    return 0;
}

bool 
ShaderD3D11::getTechniqueByName(const std::string& name, 
                           const Ctr::GpuTechnique*& technique) const
{
    for (auto techniqueIt = _techniques.begin();
        techniqueIt != _techniques.end(); 
        techniqueIt++)
    {
        const std::string& techniqueName = (*techniqueIt)->name();
        if (techniqueName == name)
        {
            technique = (*techniqueIt);
            return true;
        }
    }

    LOG ("Failed to find technique by name = " << name << " for shader " << this->name()  << "\n");
    return false;
}

bool
ShaderD3D11::getConstantBufferByName(const std::string& constantBufferName,
                                const Ctr::GpuConstantBuffer*& constantBuffer) const
{
    constantBuffer = nullptr;

    for (auto it = _constantBuffers.begin();
         it != _constantBuffers.end();
         it++)
    {
        if ((*it)->name() == constantBufferName)
        {
            constantBuffer = (*it);
            return true;
        }
    }
    LOG ("Failed to find constantBuffer by name = " << constantBufferName << " for shader " << name()  << "\n");
    return false;

} 

bool
ShaderD3D11::getParameterByName (const std::string& parameterName, 
                            const Ctr::GpuVariable*& coreVariable) const
{
    coreVariable = nullptr;

    for (auto it = _parameters.begin();
         it != _parameters.end();
         it++)
    {
        if ((*it)->name() == parameterName)
        {
            coreVariable = (*it);
            return true;
        }
    }
    // LOG ("Failed to find parameter by name = " << parameterName << " for shader " << name()  << "\n");
    return false;
}

bool 
ShaderD3D11::setParameters (const Ctr::PostEffect* target) const
{
    return true;
}

bool 
ShaderD3D11::setParameters (const Ctr::RenderRequest& request) const
{
    for (auto parameter = _shaderParameterValues.begin();
             parameter != _shaderParameterValues.end(); 
             parameter++)
    {
        (*parameter)->setParam (request);
    }
    
    return true;
}

bool 
ShaderD3D11::setTechniqueParameters (const Ctr::RenderRequest& request) const
{
    for (auto it = _techniqueParameters.begin();
         it != _techniqueParameters.end(); 
         it++)
    {
        (*it)->setParam (request);
    }
    return true;
}

bool 
ShaderD3D11::setMeshParameters (const Ctr::RenderRequest& request) const
{
    for (auto it = _meshParameters.begin();
         it != _meshParameters.end(); 
         it++)
    {
        (*it)->setParam (request);
    }
    return true;
}

bool
ShaderD3D11::renderMeshSubset (Ctr::PrimitiveType primitiveType,
                               size_t startIndex,
                               size_t numIndices,
                               const Ctr::RenderRequest& request) const
{
    uint32_t cPasses = 0;
    uint32_t iPass = 0;

    Ctr::CullMode cachedCullMode =
            _deviceInterface->cullMode();
    bool hasMaterial = request.mesh->material() != nullptr;

    if (const Ctr::GpuTechniqueD3D11* technique = 
        dynamic_cast<const Ctr::GpuTechniqueD3D11*>(request.technique))
    {
        if (setParameters(request))
        {
            if (hasMaterial)
            {
                if (request.mesh->material()->twoSided())
                {
                    _deviceInterface->setCullMode (Ctr::CullNone);
                }
            }

            ID3DX11EffectTechnique* techniqueHandle = 
                technique->handle();

            const D3DX11_TECHNIQUE_DESC& description =
                technique->description();

            for (uint32_t passIndex = 0; passIndex < description.Passes; passIndex++)
            {
                // Need to set input handle here
                if (technique->setupInputLayout (request.mesh, passIndex))
                {
                    techniqueHandle->GetPassByIndex (passIndex)->Apply(0, _immediateCtx);

                    // This assumes that the vertex buffer is already bound
                    _immediateCtx->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)(primitiveType));
                    _immediateCtx->DrawIndexed((uint32_t)(numIndices), (uint32_t)(startIndex), 0);
                }
            }

            if (hasMaterial)
            {
                if (request.mesh->material()->twoSided())
                {
                    _deviceInterface->setCullMode (cachedCullMode);
                }
            }
        }
    }

    return true;
}

bool
ShaderD3D11::renderMesh (const Ctr::RenderRequest& request) const
{
    uint32_t cPasses = 0;
    uint32_t iPass = 0;

    if (!request.mesh->visible())
        return true;

    Ctr::CullMode cachedCullMode =
            _deviceInterface->cullMode();
    bool hasMaterial = request.mesh->material() != nullptr;

    if (const Ctr::GpuTechniqueD3D11* technique = 
        dynamic_cast<const Ctr::GpuTechniqueD3D11*>(request.technique))
    {
        if (setParameters(request))
        {
            if (hasMaterial)
            {
                if (request.mesh->material()->twoSided())
                {
                    _deviceInterface->setCullMode (Ctr::CullNone);
                }
            }

            ID3DX11EffectTechnique* techniqueHandle = 
                technique->handle();

            const D3DX11_TECHNIQUE_DESC& description =
                technique->description();

            for (uint32_t passIndex = 0; passIndex < description.Passes; passIndex++)
            {
                // Need to set input handle here
                if (technique->setupInputLayout (request.mesh, passIndex))
                {
                    techniqueHandle->GetPassByIndex (passIndex)->Apply(0, _immediateCtx);
                    request.mesh->render(&request, technique);
                }
            }

            if (hasMaterial)
            {
                if (request.mesh->material()->twoSided())
                {
                    _deviceInterface->setCullMode (cachedCullMode);
                }
            }
        }
    }

    return true;
}

bool 
ShaderD3D11::renderMeshes (const Ctr::RenderRequest& inputRequest,
                           const std::set<const Ctr::Mesh*>      & meshes) const
{

    Ctr::CullMode cachedCullMode =
            _deviceInterface->cullMode();

    // Form a new request around the meshes request.
    Ctr::RenderRequest request = inputRequest;

    if (meshes.size() > 0)
    {
        for (auto it =
             meshes.begin(); it != meshes.end(); it++)
        {
            const Ctr::Mesh* mesh = (*it);
            request.mesh = (mesh);
            request.material = mesh->material();

            if (!mesh->visible())
                continue;

            if (mesh->material()->twoSided())
            {
                _deviceInterface->setCullMode (Ctr::CullNone);
            }

            if (const Ctr::GpuTechniqueD3D11* technique = 
                    dynamic_cast<const Ctr::GpuTechniqueD3D11*>(request.technique))
            {
                ID3DX11EffectTechnique* techniqueHandle = 
                    technique->handle();

                const D3DX11_TECHNIQUE_DESC& description =
                    technique->description();

               setTechniqueParameters (request);
               setMeshParameters (request);
               
                for (uint32_t passIndex = 0; passIndex < description.Passes; passIndex++)
                {
                    // Need to set input handle here
                    if (technique->setupInputLayout (mesh, passIndex))
                    {
                        techniqueHandle->GetPassByIndex (passIndex)->Apply(0, _immediateCtx);
                        mesh->render(&request, technique);
                    }
                }
            }

            if (mesh->material()->twoSided())
            {
                _deviceInterface->setCullMode (cachedCullMode);
            }
        }
    }
    return true;
}

bool
ShaderD3D11::renderInstancedBuffer (const Ctr::RenderRequest& request,
                               const Ctr::IGpuBuffer* instanceBuffer)  const
{
    if (const Ctr::BufferD3D11* resource = 
        dynamic_cast<const Ctr::BufferD3D11*>(instanceBuffer))
    {
        if (const Ctr::GpuTechniqueD3D11* technique = 
            dynamic_cast<const Ctr::GpuTechniqueD3D11*>(request.technique))
        {
            const D3DX11_TECHNIQUE_DESC& description =
                    technique->description();

            ID3DX11EffectTechnique* techniqueHandle = 
                technique->handle();

            setTechniqueParameters (request);
            setMeshParameters (request);

            for (uint32_t passIndex = 0; passIndex < description.Passes; passIndex++)
            {
                techniqueHandle->GetPassByIndex (passIndex)->Apply(0, _immediateCtx);

                ID3D11Buffer* vertexBuffers[2] = { nullptr , nullptr};
                UINT strides[2] = { 0, 0 };
                UINT offsets[2] = { 0, 0 };
                _immediateCtx->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
                _immediateCtx->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
                _immediateCtx->IASetInputLayout(nullptr);
                // TODO: Topology as input
                _immediateCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); 

                _immediateCtx->DrawInstancedIndirect(resource->buffer(), 0);
            }
        }
    }

    return true;
}

}
