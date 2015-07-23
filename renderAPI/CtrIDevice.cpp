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

#include <CtrIDevice.h>
#include <CtrITexture.h>
#include <CtrLog.h>
#include <CtrShaderMgr.h>
#include <CtrTextureMgr.h>
#include <CtrShaderParameterValueFactory.h>
#include <CtrVertexDeclarationMgr.h>
#include <CtrInputManager.h>
#include <CtrColorResolve.h>
#include <CtrDepthResolve.h>
#include <CtrPostEffectsMgr.h>

namespace Ctr
{
ApplicationRenderParameters::ApplicationRenderParameters(const Ctr::Application* application) :
    _application(application),
    _windowed(false)
{
}

ApplicationRenderParameters::ApplicationRenderParameters(const Ctr::Application* application,
    const std::string& applicationName,
    const Ctr::Vector2i& windowSize,
    bool windowed,
    bool headless) :
    _application(application),
    _applicationName(applicationName),
    _requestedSize(windowSize),
    _windowed(windowed),
    _headless(headless)
{

}

ApplicationRenderParameters::~ApplicationRenderParameters()
{
}

bool
ApplicationRenderParameters::load(const std::string& filePathName)
{
    return false;
}

const Ctr::Application*
ApplicationRenderParameters::application() const
{
    return _application;
}

WindowHandle
ApplicationRenderParameters::windowHandle() const
{
    return _windowHandle;
}

bool
ApplicationRenderParameters::multithreaded() const
{
    return _multithreaded;
}

const Ctr::Vector2i&
ApplicationRenderParameters::size() const
{
    return _requestedSize;
}

bool
ApplicationRenderParameters::windowed() const
{
    return _windowed;
}

const std::string&
ApplicationRenderParameters::applicationName() const
{
    return _applicationName;
}

bool
ApplicationRenderParameters::headless() const
{
    return _headless;
}

IDevice::IDevice () : 
    _useMultiSampleAntiAliasing (false),
    _multiSampleCount(1),
    _multiSampleQuality(0),
    _sceneDrawMode(Ctr::Filled),
    _usePrecompiledShaders(true),
    _depthResolveEffect(nullptr),
    _colorResolveEffect(nullptr),
    _vertexDeclarationMgr(nullptr),
    _textureMgr(nullptr),
    _shaderValueFactory(nullptr),
    _shaderMgr(nullptr),
    _postEffectsMgr(nullptr)
{
}

void
IDevice::update()
{
    _shaderMgr->update();
}

bool
IDevice::initialize(const Ctr::ApplicationRenderParameters& deviceParameters)
{
    _application = deviceParameters.application();
    return true;
}

bool
IDevice::postInitialize(const Ctr::ApplicationRenderParameters& deviceParameters)
{
    _vertexDeclarationMgr = new Ctr::VertexDeclarationMgr(_application, this);
    _textureMgr = new TextureMgr(_application, this);
    _shaderValueFactory = new ShaderParameterValueFactory(this);
    _shaderMgr = new Ctr::ShaderMgr(this);
    _postEffectsMgr = new PostEffectsMgr(_application, this);

    {
        _colorResolveEffect = new Ctr::ColorResolve(this);
        _colorResolveEffect->initialize("IblColorResolve.fx");
    }

    {
        _depthResolveEffect = new Ctr::DepthResolve(this);
        _depthResolveEffect->initialize("IblDepthResolve.fx");
    }

    return true;
}

bool
IDevice::usePrecompiledShaders() const
{
    return _usePrecompiledShaders;
}

void
IDevice::setUsePrecompiledShaders(bool usePrecompiledShaders)
{
    _usePrecompiledShaders = usePrecompiledShaders;
}

IDevice::~IDevice()
{
}

bool
IDevice::free()
{
    safedelete(_colorResolveEffect);
    safedelete(_depthResolveEffect);

    safedelete(_vertexDeclarationMgr);
    safedelete(_shaderMgr);
    safedelete(_shaderValueFactory);
    safedelete(_postEffectsMgr);
    safedelete(_textureMgr);

    _application = nullptr;

    return true;
}

Ctr::ITexture* 
IDevice::sharedRenderTarget(int width, int height, Ctr::PixelFormat format, bool useUAV)
{
    for (auto it = _temporaryTexturePool.begin(); it != _temporaryTexturePool.end(); it++)
    {
        if (!(*it)->inUse() &&
            (*it)->resource()->format() == format &&
            (*it)->resource()->width() == width  &&
            (*it)->resource()->height() == height &&
            (*it)->resource()->useUAV() == useUAV)
        {
            (*it)->setInUse(true);
            return *it;
        }
    }

    Ctr::ITexture* texture = 
        createTexture(&TextureParameters ("tempTex",
                                          TextureImagePtr(),
                                          Ctr::TwoD,
                                          Ctr::RenderTarget,
                                          format,
                                          Ctr::Vector3i(width, height, 1),
                                          false,
                                          1, 1, 0, 1, useUAV));
    if (texture)
    {
        _temporaryTexturePool.push_back(texture);
        texture->setInUse(true);
        return texture;
    }
    else
    {
        return nullptr;
    }
}

void
IDevice::setSceneDrawMode(Ctr::DrawMode drawModeVal)
{
    _sceneDrawMode = drawModeVal;
}

Ctr::DrawMode
IDevice::sceneDrawMode() const
{
    return _sceneDrawMode;
}

ShaderMgr*
IDevice::shaderMgr()
{
    return _shaderMgr;
}

VertexDeclarationMgr*
IDevice::vertexDeclarationMgr()
{
    return _vertexDeclarationMgr;
}

TextureMgr*
IDevice::textureMgr()
{
    return _textureMgr;
}

ShaderParameterValueFactory* 
IDevice::shaderValueFactory()
{
    return _shaderValueFactory;
}

PostEffectsMgr *
IDevice::postEffectsMgr()
{
    return _postEffectsMgr;
}

}