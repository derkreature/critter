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

#ifndef INCLUDED_DIRECT3D11_DEVICE
#define INCLUDED_DIRECT3D11_DEVICE

#include <CtrRenderWindowD3D11.h>
#include <CtrIDevice.h>
#include <DxErr.h>

namespace Ctr
{
class Application;
class DXGIFactory;
class DepthSurfaceD3D11;
class SurfaceD3D11;
class RenderWindow;
class ISurface;
class IDepthSurface;
class DepthResolve;
class ColorResolve;

#define MAX_RENDER_TARGETS 8
#define MAX_ERROR_LENGTH 2048 

inline void LOG_D3D_ERROR (HRESULT hr) 
{
    std::wstring werror(DXGetErrorString(hr)); 
    wchar_t werrorDesc[MAX_ERROR_LENGTH]; 
    DXGetErrorDescriptionW(hr, (WCHAR*)werrorDesc, MAX_ERROR_LENGTH); 
    std::wstring errorDesc(werrorDesc); 
    LOG("Failure " << std::string(werror.begin(), werror.end())); 
    LOG("Error: " << std::string(errorDesc.begin(), errorDesc.end())); 
}

inline bool NO_D3D_ERROR(HRESULT hr)
{
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else
    {
        LOG_D3D_ERROR(hr);
        return false;
    }
}

class DXGIFactory
{
  public:
    DXGIFactory();
    ~DXGIFactory();

    bool                       D3D11Available() const;
    IDXGIFactory1*             factory();

  protected:
    void                       initialize();
    void                       setD3D11Available (bool available);
    void                       setFactory (IDXGIFactory1* factory);

  private:
    bool                       _D3D11Available;
    IDXGIFactory1*             _factory;
};
class DeviceSettingsD3D11
{
  public:
    DeviceSettingsD3D11 ();
    DeviceSettingsD3D11 (const DeviceSettingsD3D11& settings);

    uint32_t                   _adapterOrdinal;
    D3D_DRIVER_TYPE            _driverType;
    uint32_t                   _output;
    DXGI_SWAP_CHAIN_DESC       _sd;
    uint32_t                   _createFlags;
    uint32_t                   _syncInterval;
    uint32_t                   _presentFlags;
    bool                       _autoCreateDepthStencil; 
    DXGI_FORMAT                _autoDepthStencilFormat;
    DXGI_SAMPLE_DESC           _sampleDesc;
};

class DeviceD3D11 : public IDevice
{
  public:
    DeviceD3D11();
    virtual ~DeviceD3D11();

    Ctr::Window*                renderWindow();
    bool                       initialize (const Ctr::ApplicationRenderParameters& deviceResource);

    ID3D11DeviceContext*       immediateCtx() { return _immediateCtx;}
    bool                       beginRender();
    bool                       present();


    virtual void               printState();
    virtual void               syncState();

    operator                   ID3D11Device* const&() { return _direct3d; }

    bool                       initialized() const;
    
    virtual bool               reset();


    operator ID3D11Device* const&() const { return _direct3d; }
    ID3D11Device* const& operator->() const { return _direct3d; }

    virtual const Ctr::ISurface*    backbuffer() const;
    virtual const Ctr::IDepthSurface* depthbuffer() const;
    virtual const Ctr::FrameBuffer& deviceFrameBuffer() const;
    // Resource management functions
    virtual IGpuBuffer *        createBufferResource (const Ctr::RenderResourceParameters* data = 0);
    virtual IVertexBuffer *     createVertexBuffer (const Ctr::RenderResourceParameters* data = 0);
    virtual IIndexBuffer *      createIndexBuffer (const Ctr::RenderResourceParameters* data = 0);
    virtual IVertexDeclaration * createVertexDeclaration (const Ctr::RenderResourceParameters* data = 0);
    virtual IDepthSurface *     createDepthSurface(const Ctr::RenderResourceParameters* data = 0);
    virtual ITexture *          createTexture (const Ctr::RenderResourceParameters* data = 0);
    virtual IComputeShader *    createComputeShader (const Ctr::RenderResourceParameters* data = 0);
    virtual IShader *           createShader (const Ctr::RenderResourceParameters* data = 0);

    virtual void                destroyResource(Ctr::IRenderResource* resource);

    virtual void               resetShaderPipeline();

    bool                       setColorWriteState (bool r  = true, bool g  = true, 
                                                   bool b  = true, bool a = true);

    virtual bool               drawPrimitive (const IVertexDeclaration*, 
                                              const IVertexBuffer*, 
                                              const GpuTechnique* technique,
                                              PrimitiveType, 
                                              uint32_t primitiveCount,
                                              uint32_t vertexOffset) const;

    virtual bool               drawIndexedPrimitive (const IVertexDeclaration*, 
                                                     const IIndexBuffer*, 
                                                     const IVertexBuffer*, 
                                                     const GpuTechnique* technique,
                                                     PrimitiveType, 
                                                     uint32_t faceCount,
                                                     uint32_t indexOffset,
                                                     uint32_t vertexOffset) const;

    virtual bool               blitSurfaces (const ISurface* destination, 
                                             const ISurface* src, 
                                             TextureFilter filterType = Ctr::TEXFILTER_POINT,
                                             size_t arrayOffset = 0) const;

    virtual bool                blitSurfaces (const IDepthSurface* destination, 
                                              const IDepthSurface* src, 
                                              TextureFilter filterType =Ctr::TEXFILTER_POINT) const;

    virtual bool               clearSurfaces (uint32_t index, unsigned long clearType, 
                                              float redClear = 0.0f, 
                                              float greenClear = 0.0f, 
                                              float blueClear = 0.0f, 
                                              float alphaClear = 0.0f) const;

    virtual bool               scissorEnabled() const;
    virtual void               setScissorEnabled(bool scissorEnabled);
    virtual void               setScissorRect(int x, int y, int width, int height);

    virtual bool               setNullTarget (uint32_t index);

    virtual void               setNullStreamOut();

    virtual void               setViewport (const Viewport*);
    virtual void               getViewport(Viewport*) const;

    virtual void*              rawDevice();

    virtual bool               writeFrontBufferToFile (const std::string& ) const;

    // State Management Functions
    virtual void                enableAlphaBlending();
    virtual void                disableAlphaBlending();

    virtual void                setAlphaToCoverageEnable (bool value);
 
    virtual void                setBlendProperty (const Ctr::BlendOp&);
    virtual void                setSrcFunction (const Ctr::AlphaFunction&);
    virtual void                setDestFunction (const Ctr::AlphaFunction&);
 
    virtual void                setAlphaBlendProperty (const Ctr::BlendOp&);
    virtual void                setAlphaDestFunction (const Ctr::AlphaFunction&);
    virtual void                setAlphaSrcFunction (const Ctr::AlphaFunction&);

    virtual void                fogEnable();
    virtual void                fogDisable();
    
    virtual void                enableZTest();
    virtual void                disableZTest();

    virtual void                disableDepthWrite();
    virtual void                enableDepthWrite();

    virtual void                setZFunction (Ctr::CompareFunction);
    virtual void                setupBlendPipeline(Ctr::BlendPipelineType blendPipelineType);
    virtual Ctr::BlendPipelineType blendPipeline() const;

    virtual void                setFrontFaceStencilFunction(Ctr::CompareFunction);
    virtual void                setFrontFaceStencilPass(Ctr::StencilOp compareFunc);

    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ctr::CompareFunction frontCompare,
                                             Ctr::StencilOp frontStencilFailOp,
                                             Ctr::StencilOp frontStencilPassOp,
                                             Ctr::StencilOp frontZFailOp,
                                             Ctr::CompareFunction backCompare,
                                             Ctr::StencilOp backStencilFailOp,
                                             Ctr::StencilOp backStencilPassOp,
                                             Ctr::StencilOp backZFailOp);

    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ctr::CompareFunction frontCompare,
                                             Ctr::StencilOp frontStencilFailOp,
                                             Ctr::StencilOp frontStencilPassOp,
                                             Ctr::StencilOp frontZFailOp);


    virtual void                enableStencilTest();
    virtual void                disableStencilTest();
    virtual void                setCullMode (Ctr::CullMode);
    virtual void                setNullPixelShader();
    virtual void                setNullVertexShader();

    virtual bool                isRenderTextureFormatSupported (const Ctr::PixelFormat& format);

    virtual void                setDrawMode (Ctr::DrawMode);
    virtual Ctr::DrawMode getDrawMode () const;

    virtual Ctr::CullMode cullMode() const;

    virtual void                copyStructureCount(const Ctr::IGpuBuffer* dst, const Ctr::IGpuBuffer* src);

    virtual bool                supportsHardwareTessellationStage() const;
    ID3D11DepthStencilView*     primaryDepthStencilView();
    ID3D11RenderTargetView*     primaryBackBufferView();
    
    virtual void                bindNullFrameBuffer();
    virtual void                bindSurface (int level, const Ctr::ISurface* surface);
    virtual void                bindDepthSurface (const Ctr::IDepthSurface* surface);
    void                        bindSurfaceAndTargets();

    virtual bool                resizeDevice (const Ctr::Vector2i& newSize);

    virtual bool                texelIsCenter() { return false; }

    virtual bool                bindFrameBuffer (const Ctr::FrameBuffer& framebuffer);

    virtual void                setupViewport (const Ctr::FrameBuffer& frameBuffer);

    virtual void                resetViewsAndShaders() const;
    virtual void                clearShaderResources() const;


  protected:

    void                        initializeDeviceStates();
    void                        bindBlendState();
    void                        bindDepthState();
    void                        bindRasterState();

  private:
    D3D_FEATURE_LEVEL          _level;
    DXGIFactory *              _factory;
    ID3D11Device*              _direct3d;
    IDXGISwapChain*            _swapChain;
    IDXGIAdapter1*             _adapter;
    DeviceSettingsD3D11        _creationParameters;
    Ctr::RenderWindow*          _window;
    bool                       _initialized;
    D3D11_VIEWPORT             _currentViewport;
    ID3D11DeviceContext*       _immediateCtx;

    SurfaceD3D11*              _backbuffer;
    DepthSurfaceD3D11*         _depthbuffer;
    Ctr::FrameBuffer           _deviceFrameBuffer;

    const ISurface*            _currentSurfaces [MAX_RENDER_TARGETS];
    uint32_t                   _currentSurfaceCount;
    uint32_t                   _currentUAVCount;
    const IRenderResource*     _currentUnorderedSurfaces[MAX_RENDER_TARGETS];
    const IDepthSurface*       _currentDepthSurface;

    Ctr::DrawMode              _drawMode;
    Ctr::CullMode              _cullMode;
    bool                       _scissorEnabled;
    ID3D11RasterizerState *    _currentRasterState;
    D3D11_RASTERIZER_DESC      _currentRasterStateDesc;

    ID3D11DepthStencilState*   _currentDepthState;
    D3D11_DEPTH_STENCIL_DESC   _currentDepthStateDesc;

    ID3D11BlendState*          _currentBlendState;

    
    D3D11_BLEND_DESC           _currentBlendStateDesc;

    Ctr::Vector4f              _blendFactor;
    Ctr::BlendPipelineType     _blendPipelineType;
};

}

#endif