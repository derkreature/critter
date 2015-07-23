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
#include <CtrPostEffect.h>
#include <CtrIShader.h>
#include <CtrShaderMgr.h>
#include <CtrGpuTechnique.h>
#include <CtrISurface.h>
#include <CtrLog.h>
#include <CtrRegion.h>
#include <CtrIGpuBuffer.h>

namespace Ctr
{
PostEffect::PostEffect (Ctr::IDevice* device, 
                        PixelFormat format,
                        LensDistortionType distortionType) :
Ctr::RenderTargetQuad (device),
_technique(0),
_shader(0),
_textureIn(0),
_postEffectTexture(nullptr),
_format (format),
_verticalPass (false)
{
    _name = "";
    
    RenderTargetQuad::initialize (Region2f());
    // This still needs work, and needs to be recalced when
    // when the backbuffer size changes
    if (_format != Ctr::PF_UNKNOWN)
    {
        Ctr::TextureParameters textureData  = 
            TextureParameters ("PostEffectTexture",
                             Ctr::TextureImagePtr(),
                             Ctr::TwoD,
                             Ctr::RenderTarget,
                             _format,
                             Ctr::Vector3i(MIRROR_BACK_BUFFER, MIRROR_BACK_BUFFER, 1));

        _postEffectTexture = _device->createTexture(&textureData);
    }

    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(_device->backbuffer()->width(), _device->backbuffer()->height()));
}

PostEffect::~PostEffect()
{
    _device->destroyResource(_postEffectTexture);
}


bool
PostEffect::verticalPass () const
{
    return _verticalPass;
}

void
PostEffect::setVerticalPass (bool verticalPassValue)
{
    _verticalPass = verticalPassValue;
}

bool PostEffect::update(float felapsed)
{
    return true;
}

//--------------------------------------------------
// Gets the name of the target
//--------------------------------------------------
const std::string&  PostEffect::name() const
{
    return _name;
}

//--------------------------------------------------
//sets the name of the target
//--------------------------------------------------
void PostEffect::name(const std::string& name)
{
    _name = name;
}

bool PostEffect::free()
{
    //reset render surface
    return RenderTargetQuad::free();
}

bool
PostEffect::create()
{
    return RenderTargetQuad::create();
}

bool
PostEffect::cache()
{
    if (_shader)
    {
        if (!_shader->getTechniqueByName ("postprocess", _technique))
        {
            LOG ("couldn't find default postprocess technique for file" << _filename);
        }
    }

    return RenderTargetQuad::cache();
}


const Ctr::ITexture*
PostEffect::currentSource() const
{
    return _textureIn;
}

const Ctr::Region2i& 
PostEffect::postEffectBounds() const
{
    return _postEffectBounds;
}

bool 
PostEffect::render (const Ctr::ITexture* textureIn, const Ctr::Camera* camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(_device->backbuffer()->width(), _device->backbuffer()->height()));

    Ctr::FrameBuffer frameBuffer(_postEffectTexture->surface(), nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _textureIn = textureIn;
    _shader->renderMesh (Ctr::RenderRequest(_technique, nullptr, camera, this));

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool 
PostEffect::render (const Ctr::ITexture* textureIn, 
                    const Ctr::ITexture* textureOut,
                    const Ctr::Camera* camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(textureOut->width(), textureOut->height()));

    // Todo, setup viewport for framebuffer...
    Ctr::FrameBuffer frameBuffer(textureOut->surface(), nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);
    _inputs.push_back(_textureIn);

    _textureIn = textureIn;

    _shader->renderMesh (Ctr::RenderRequest(_technique, nullptr, camera, this));

    _inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool
PostEffect::render (const Ctr::ITexture* texture, 
                    const Ctr::ISurface* surface,
                    const Ctr::Camera* camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(surface->width(), surface->height()));

    Ctr::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _textureIn = texture;
    _inputs.push_back(_textureIn);
    _shader->renderMesh (Ctr::RenderRequest(_technique, nullptr, camera, this));

    _inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool
PostEffect::composite (const Ctr::GpuTechnique* technique,
                       const Ctr::ISurface* surface,
                       std::vector<const Ctr::IGpuBuffer*> & uavs,
                       std::vector<const Ctr::ITexture*>  & inputs,
                       const Ctr::Camera* camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(surface->width(), surface->height()));
    Ctr::FrameBuffer frameBuffer(surface, nullptr);

    for (size_t i = 0; i < uavs.size(); i++)
    {
        frameBuffer.setUnorderedSurface(i, uavs[i]);
    }

    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);
    _inputs = inputs;

    _shader->renderMesh (Ctr::RenderRequest(technique, nullptr, camera, this));

    inputs.clear();
    uavs.clear();

    // Needless stall. Needs optimize

    Ctr::FrameBuffer frameBuffer0(_device->backbuffer(), nullptr);
    _device->bindFrameBuffer(frameBuffer0);
    return true;
}


bool
PostEffect::composite (const Ctr::GpuTechnique* technique,
                       const Ctr::ISurface* surface,
                       std::vector<const Ctr::ITexture*>      & inputs,
                       const Ctr::Camera* camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(surface->width(), surface->height()));
    _inputs = inputs;
    Ctr::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);


    _shader->renderMesh (Ctr::RenderRequest(technique, nullptr, camera, this));


    inputs.clear();
    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);;
}

bool
PostEffect::composite (const Ctr::ISurface* surface,
                       std::vector<const Ctr::ITexture*> &  inputs,
                       const Ctr::Camera*   camera)
{
    _postEffectBounds = Ctr::Region2i(Ctr::Vector2i(0,0), Ctr::Vector2i(surface->width(), surface->height()));
    _inputs = inputs;
    Ctr::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _shader->renderMesh (Ctr::RenderRequest(_technique, nullptr, camera, this));

    inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

const Ctr::ITexture*  
PostEffect::input (uint32_t index) const 
{
    if (index >= _inputs.size())
    {
        // LOG ("Requesting invalid input index " << index << " in post effect");
        return nullptr; 
    }
    return _inputs[index];
}

bool 
PostEffect::initialize (const std::string& filename)
{
    _filename   = filename;
    _shader        = 0;
    if (Ctr::ShaderMgr* shaderMgr = _device->shaderMgr())
    {
        if (!shaderMgr->addShader (_filename, _shader, true))
        {
            LOG ("could not create shader from file " << filename);            
            return false;
        }
    }

    create();
    cache();

    return true;
}

}
