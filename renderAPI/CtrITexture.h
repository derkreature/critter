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
#ifndef INCLUDED_CRT_TEXTURE_INTERFACE
#define INCLUDED_CRT_TEXTURE_INTERFACE

#include <CtrIRenderResource.h>
#include <CtrIRenderResourceParameters.h>
#include <CtrTextureImage.h>

namespace Ctr
{

class IEffect;
class ISurface;
class GpuVariable;

class 
ITexture : public IRenderResource
{
  public:
    ITexture (Ctr::IDevice* device);
    virtual ~ITexture();

    virtual bool               initialize (const Ctr::TextureParameters* resource) = 0;
    virtual bool               bindSurface(int renderTargetIndex) const = 0;
    virtual bool               clearSurface (uint32_t layerId, float r  = 1.0f, float g  = 1.0f, float b  = 1.0f, float a = 1.0f)  = 0;


    virtual bool               isCubeMap() const = 0;
    // Get the surface for the texture.
    virtual const ISurface*    surface(int32_t arrayId = -1, int32_t mipId = -1) const;
    virtual const Ctr::Vector4f& maxValue() const = 0;

    virtual unsigned int       width() const;
    virtual unsigned int       height() const;
    virtual unsigned int       depth() const;
    virtual unsigned int       channels() const;
    virtual PixelFormat        format() const;
    virtual unsigned int       textureCount() const;
    virtual void               generateMipMaps() const = 0;
    virtual void               setSize (const Ctr::Vector2i& size);
    
    virtual bool               map(uint32_t imageLevel = 0, uint32_t mipLevel = 0) const = 0;
    virtual bool               unmap() const = 0;

    virtual bool               mapForRead() const = 0;
    virtual bool               unmapFromRead() const = 0;

    virtual bool               mapForWrite() = 0;

    // Clear UAV, texture must be created with UAV support.
    virtual void               clearUnorderedAccessViewUint(uint32_t value) const;

    // Write a pixel
    virtual bool               write (const Ctr::Vector4f&, const Ctr::Vector2f& pos) = 0;

    // Write all pixels. Pixels is assumed to be the correct type and
    // allocated to byteSize()
    virtual bool               write(const Ctr::PixelBox& pixelBox,
                                     uint32_t mipLevel) = 0;
    virtual bool               write(uint8_t* pixels) = 0;

    virtual bool               writeSubRegion(const Ctr::byte* srcPtr, uint32_t offsetX, uint32_t offsetY, uint32_t w, uint32_t h, uint32_t bytesPerPixel) = 0;
    // Read a texel
    virtual Ctr::Vector4f      read (const Ctr::Vector2f& pos) const  = 0;
    // read a pixel
    virtual Ctr::Vector4f      read (const Ctr::Vector2i& pos) const = 0;

    // Read all pixels. Pixels should be preallocated to byteSize().
    virtual Ctr::Vector4f      read (Ctr::byte* pos) const = 0;
    
    virtual bool               save(const std::string& filePathName,
                                    bool fixSeams = false,
                                    bool splitChannels = false,
                                    bool rgbOnly = false,
                                    int32_t mipLevel = -1,
                                    const Ctr::ITexture* mergeMap = nullptr) const = 0;

    // Returns the total size in bytes of the texture.
    virtual size_t             byteSize() const;
    unsigned int               area() const;
    unsigned int               bytesPerPixel() const;
    unsigned int               pixelChannelPitch() const;

    virtual int                multiSampleCount() const;
    virtual int                multiSampleQuality() const;

    // The slice that will be bound when setTexture is called
    // to bind a resource to the shader. The surface handles targets.
    // -1 means that all slices are bound to the shader.
    int32_t                    activeSlice() const;
    void                       setActiveSlice(int32_t activeSlice);

    // -1 means that the entire mip chain is bound.
    void                       setActiveMipId(int32_t activeMipId);
    int32_t                    activeMipId () const;

    bool                       inUse() const;
    void                       setInUse(bool);

    const Ctr::TextureParameters* resource() const;

    TextureImagePtr            textureImage() const;
    TextureImagePtr            uiImage() const;

    virtual uint32_t           mipCount() const;

  protected:
    unsigned int               _width;
    unsigned int               _height;
    unsigned int               _depth;
    unsigned int               _channels;
    unsigned int               _pixelChannelPitch;
    unsigned int               _pixelType;
    unsigned int               _mipCount;

    PixelFormat                _format;
    unsigned int               _textureCount;

    int                        _multiSampleQuality;
    int                        _multiSampleCount;
    int                        _activeSlice;
    int                        _activeMipId;
    Ctr::TextureParameters*    _resource;
    bool                       _inUse;
    std::string                _filename;
};

typedef std::vector<ITexture*> TextureArray;

}

#endif