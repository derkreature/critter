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

#ifndef INCLUDED_CRT_DEVICE_INTERFACE
#define INCLUDED_CRT_DEVICE_INTERFACE

#include <CtrIRenderResource.h>
#include <CtrRenderWindow.h>
#include <CtrRenderEnums.h>
#include <CtrIDepthSurface.h>
#include <CtrISurface.h>
#include <CtrViewport.h>
#include <CtrFrameBuffer.h>

namespace Ctr
{
class Window;
class DeviceParameters;
class IIndexBuffer;
class GpuTechnique;
class IVertexBuffer;
class IVertexDeclaration;
class ISurface;
class Mesh;
class RenderAPIFactory;
class IGpuBuffer;
class IDepthSurface;
class ITexture;
class IShader;
class IComputeShader;
class IUberShaderGraph;
class RenderResourceParameters;
class ShaderMgr;
class VertexDeclarationMgr;
class TextureMgr;
class PostEffectsMgr;
class ShaderParameterValueFactory;
class DepthResolve;
class ColorResolve;


class ApplicationRenderParameters 
{
  public:
    ApplicationRenderParameters ();
    ApplicationRenderParameters (const Ctr::Application* application,
                                 const std::string& applicationName,
                                 const Ctr::Vector2i& windowSize,
                                 bool windowed,
                                 bool headless);

    ApplicationRenderParameters (const Ctr::Application* application);
    ~ApplicationRenderParameters ();

    // Load parameters from xml.
    bool                       load(const std::string& filePathName);

    const Ctr::Application*    application() const;
    WindowHandle               windowHandle() const;
    bool                       multithreaded() const;

    const Ctr::Vector2i&       size() const;    
    bool                       windowed() const;
    const std::string&         applicationName() const;
    bool                       headless() const;

  private:  
    const Ctr::Application*     _application;
    WindowHandle               _windowHandle;
    bool                       _useConfig;
    bool                       _windowed;
    Ctr::Vector2i               _requestedSize;
    bool                       _nvidiaDebugSession;
    bool                       _useSLIIfAvailable;
    bool                       _multithreaded;
    std::string                _applicationName;
    bool                       _headless;
};

class IDevice 
{
  public:
    IDevice ();
    virtual ~IDevice();

    void                        setSceneDrawMode(Ctr::DrawMode);
    Ctr::DrawMode                sceneDrawMode() const;

    void                        update();

    virtual void                printState() = 0;
    virtual void                syncState() = 0;

    virtual IGpuBuffer *        createBufferResource (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IVertexBuffer *     createVertexBuffer (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IIndexBuffer *      createIndexBuffer (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IVertexDeclaration * createVertexDeclaration (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IDepthSurface *     createDepthSurface(const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual ITexture *          createTexture (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IComputeShader *    createComputeShader (const Ctr::RenderResourceParameters* data = 0) = 0;
    virtual IShader *           createShader (const Ctr::RenderResourceParameters* data = 0) = 0;

    virtual void                destroyResource(Ctr::IRenderResource* resource) = 0;
    virtual void                setupBlendPipeline(BlendPipelineType blendPipelineType) = 0;

    virtual bool                create() { return true; };
    virtual bool                cache() { return true; };
    virtual bool                free();

    virtual bool                initialize (const Ctr::ApplicationRenderParameters& deviceParameters);
    virtual bool                postInitialize(const Ctr::ApplicationRenderParameters& deviceParameters);

    virtual bool                reset() = 0;
    virtual bool                beginRender() = 0;
    virtual bool                present() = 0;
    virtual bool                supportsHardwareTessellationStage() const = 0;

    virtual void                bindSurface (int level, const ISurface* surface) {};
    virtual void                bindDepthSurface (const IDepthSurface* surface) {};

    // State Management Functions
    virtual void                enableAlphaBlending() = 0;
    virtual void                disableAlphaBlending() = 0;

    virtual void                fogEnable() = 0;
    virtual void                fogDisable() = 0;
    virtual void                disableDepthWrite() = 0;
    virtual void                enableDepthWrite() = 0;

    virtual void                disableZTest() = 0;
    virtual void                enableZTest() = 0;

    virtual void                setZFunction (CompareFunction) = 0;

    virtual void                setDrawMode (DrawMode) = 0;
    virtual DrawMode            getDrawMode () const = 0;

    virtual void                resetShaderPipeline() = 0;

    virtual void                copyStructureCount(const Ctr::IGpuBuffer* dst, const Ctr::IGpuBuffer* src) = 0;

    virtual void                setFrontFaceStencilFunction(Ctr::CompareFunction) = 0;
    virtual void                setFrontFaceStencilPass(Ctr::StencilOp compareFunc) = 0;
    virtual void                enableStencilTest() = 0;
    virtual void                disableStencilTest() = 0;


    
    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ctr::CompareFunction frontCompare,
                                             Ctr::StencilOp frontStencilFailOp,
                                             Ctr::StencilOp frontStencilPassOp,
                                             Ctr::StencilOp frontZFailOp,
                                             Ctr::CompareFunction backCompare,
                                             Ctr::StencilOp backStencilFailOp,
                                             Ctr::StencilOp backStencilPassOp,
                                             Ctr::StencilOp backZFailOp) = 0;

    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ctr::CompareFunction frontCompare,
                                             Ctr::StencilOp frontStencilFailOp,
                                             Ctr::StencilOp frontStencilPassOp,
                                             Ctr::StencilOp frontZFailOp) = 0;
    
    virtual Ctr::CullMode cullMode() const = 0;
    virtual void                setCullMode (CullMode) = 0;
    virtual void                setNullPixelShader() = 0;
    virtual void                setNullVertexShader() = 0;
    virtual void                setNullStreamOut() {};

    virtual bool                drawPrimitive (const IVertexDeclaration*, 
                                               const IVertexBuffer*, 
                                               const GpuTechnique *,
                                               PrimitiveType, 
                                               uint32_t faceCount,
                                               uint32_t vertexOffset) const = 0;

    virtual bool                drawIndexedPrimitive (const IVertexDeclaration*, 
                                                      const IIndexBuffer*, 
                                                      const IVertexBuffer*, 
                                                      const GpuTechnique *,
                                                      PrimitiveType, 
                                                      uint32_t faceCount, 
                                                      uint32_t indexOffset,
                                                      uint32_t vertexOffset) const = 0;
    
    virtual bool                blitSurfaces (const ISurface* destination, 
                                              const ISurface* src, 
                                              TextureFilter filterType = TEXFILTER_POINT,
                                              size_t arrayOffset = 0) const = 0;

    virtual bool                blitSurfaces (const IDepthSurface* destination, 
                                              const IDepthSurface* src, 
                                              TextureFilter filterType = TEXFILTER_POINT) const = 0;

    virtual bool                clearSurfaces (uint32_t index, unsigned long clearType, 
                                               float redClear = 0.0f, 
                                               float greenClear = 0.0f, 
                                               float blueClear = 0.0f, 
                                               float alphaClear = 0.0f) const = 0;

    virtual bool                setNullTarget (uint32_t index) = 0;

    virtual bool                setColorWriteState (bool r, bool g, bool b, bool a) = 0;

    virtual void*               rawDevice() = 0;
    virtual Window*             renderWindow() = 0;

    virtual bool                isRenderTextureFormatSupported (const Ctr::PixelFormat& format) = 0;

    virtual void                setViewport(const Viewport*) = 0;
    virtual void                getViewport(Viewport*) const = 0;

    virtual const ISurface*     backbuffer() const = 0;
    virtual const IDepthSurface* depthbuffer() const = 0;

    virtual void                setAlphaToCoverageEnable (bool value) = 0;
 
    virtual void                setBlendProperty (const Ctr::BlendOp&) = 0;
    virtual void                setSrcFunction (const Ctr::AlphaFunction&) = 0;
    virtual void                setDestFunction (const Ctr::AlphaFunction&) = 0;
    virtual Ctr::BlendPipelineType blendPipeline() const = 0;

    virtual bool                 scissorEnabled() const = 0;
    virtual void                 setScissorEnabled(bool scissorEnabled) = 0;
    virtual void                 setScissorRect(int x, int y, int width, int height) = 0;

    virtual void                setAlphaBlendProperty (const Ctr::BlendOp&) = 0;
    virtual void                setAlphaDestFunction (const Ctr::AlphaFunction&) = 0;
    virtual void                setAlphaSrcFunction (const Ctr::AlphaFunction&) = 0;

    virtual bool                writeFrontBufferToFile (const std::string& filename) const = 0;

    virtual uint32_t            multiSampleCount() const {return _multiSampleCount; };
    virtual uint32_t            multiSampleQuality() const { return _multiSampleQuality; }
    virtual bool                useMultiSampleAntiAliasing() const { return _useMultiSampleAntiAliasing; }

    virtual bool                resizeDevice (const Ctr::Vector2i& newSize) = 0;
    virtual bool                texelIsCenter() = 0;

    virtual const Ctr::FrameBuffer& deviceFrameBuffer() const = 0;
    virtual bool                bindFrameBuffer (const Ctr::FrameBuffer& framebuffer) = 0;
    virtual void                setupViewport (const Ctr::FrameBuffer& frameBuffer) = 0;

    // Get a disposable render target from the core pool
    virtual Ctr::ITexture* sharedRenderTarget(int width, int height, Ctr::PixelFormat, bool useUAV = false);
    virtual void                resetViewsAndShaders() const = 0;
    virtual void                clearShaderResources() const = 0;

    bool                        usePrecompiledShaders() const;
    void                        setUsePrecompiledShaders(bool);

    ShaderMgr*                   shaderMgr();
    VertexDeclarationMgr*        vertexDeclarationMgr();
    TextureMgr*                  textureMgr();
    ShaderParameterValueFactory* shaderValueFactory();
    PostEffectsMgr *             postEffectsMgr();

  protected:
    bool                         _useMultiSampleAntiAliasing;
    uint32_t                     _multiSampleCount;
    uint32_t                     _multiSampleQuality;
    Ctr::FrameBuffer              _currentFrameBuffer;
    Ctr::DrawMode                 _sceneDrawMode;
    bool                         _usePrecompiledShaders;
    std::vector <Ctr::ITexture*>  _temporaryTexturePool;
    const Ctr::Application*       _application;

    Ctr::ShaderMgr*               _shaderMgr;
    Ctr::VertexDeclarationMgr*    _vertexDeclarationMgr;
    TextureMgr*                  _textureMgr;
    ShaderParameterValueFactory* _shaderValueFactory;
    PostEffectsMgr *             _postEffectsMgr;
    DepthResolve*                _depthResolveEffect;
    ColorResolve*                _colorResolveEffect;
};
}

#endif