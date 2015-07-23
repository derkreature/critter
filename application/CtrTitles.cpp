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
#include <CtrTitles.h>
#include <CtrITexture.h>
#include <CtrIShader.h>
#include <CtrIEffect.h>
#include <CtrGpuVariable.h>
#include <CtrTextureMgr.h>
#include <CtrMaterial.h>
#include <CtrShaderMgr.h>
#include <CtrScreenOrientedQuad.h>
#include <CtrPostEffectsMgr.h>

namespace Ctr
{
Title::Title(Ctr::IDevice* device) :
    _device (device),
    _image (nullptr),
    _shader (nullptr),
    _quad(nullptr),
    _currentAlpha(0),
    _currentTime (0),
    _material(nullptr)
{
}

Title::~Title()
{
    safedelete(_quad);
    safedelete(_material);
    safedelete(_quad);
}

bool
Title::create(const std::string& texturePathName,
              const Ctr::Region2f& bounds,
              const Ctr::Vector4f& blendTerms)
{
    // Load the texture that this image widget displays
    _texturePath = texturePathName;
    _bounds = bounds;
    _blendTerms = blendTerms;

    if (_texturePath.size() > 0 && _image == nullptr)
    {
        _image = dynamic_cast <const Ctr::ITexture*>
            (_device->textureMgr()->loadTexture(_texturePath));
    }

    _material = new Material(_device);
    _material->setAlbedoMap(_image);

    // Load Shader
    if (Ctr::ShaderMgr* shaderMgr = _device->shaderMgr())
    {
        if (!shaderMgr->addShader ("IblImageBlit.fx", _shader, true))
        {
             return false;
        }
    }

    // create the quad used to render this image to the screen
    if (_quad = new Ctr::ScreenOrientedQuad(_device))
    {
        LOG ("Initializing image quad\n");
        if (_quad->initialize (_bounds))
        {
            _quad->setMaterial (_material);
        }
    }

    if (_quad && _shader)
    {
        if (_shader->getTechniqueByName ("postprocess", _technique))
        {
            _material->setShader (_shader);
            _material->setTechnique (_technique);
        } 
    }
    return true;
}

void
Title::update(float elapsed)
{
    _currentTime += elapsed;

    if (_currentTime <= _blendTerms.y)
    {
        _currentAlpha = (_currentTime - _blendTerms.x) / ((_blendTerms.y - _blendTerms.x) + 1e-12f);
    }
    else if (_currentTime >= _blendTerms.z)
    {
        _currentAlpha = 1.0f - ((_currentTime - _blendTerms.z) / ((_blendTerms.w - _blendTerms.z) + 1e-12f));
    }
    else if (_currentTime > _blendTerms.y && _currentTime < _blendTerms.z)
    {
        _currentAlpha = 1.0f;
    }

    _material->albedoColorProperty()->set(Ctr::Vector4f(1, 1, 1, std::max(0.0f, _currentAlpha)));
}

void
Title::render ()
{
    if (_image && _quad)
    {
        _shader->renderMesh (Ctr::RenderRequest(_technique, nullptr, nullptr, _quad));
    }
}


Titles::Titles(Ctr::IDevice* device) : 
    PostEffect (device),
    _titleLength (0),
    _titlesTime (0)
{
    // Offscreen target:
    Ctr::TextureParameters textureData  = 
        TextureParameters ("TitleRT",
                            Ctr::TextureImagePtr(),
                            Ctr::TwoD,
                            Ctr::RenderTarget,
                            Ctr::PF_A8R8G8B8,
                            Ctr::Vector3i(MIRROR_BACK_BUFFER, MIRROR_BACK_BUFFER, 1));

    _titlesTexture = _device->createTexture(&textureData);

}

Titles::~Titles()
{
    _device->destroyResource (_titlesTexture);
    for (auto it = _titles.begin(); it != _titles.end(); it++)
    {
        safedelete(*it);
    }
    _titles.clear();

}

bool 
Titles::free()
{
    return PostEffect::free();
}

bool
Titles::cache()
{
    _shader->getTechniqueByName(std::string ("postprocess"), _postProcessTechnique);
        
    //we have one offscrean texture already created.
    _shader->getParameterByName (std::string ("TitlesTexture"),      _titlesTextureVariable);
    _shader->getParameterByName (std::string ("BackbufferTexture"),  _backbufferTextureVariable);
    _shader->getParameterByName (std::string ("TitlesTime"),          _titlesTimeVariable);

    return PostEffect::cache();
}

bool
Titles::update (float elapsed)
{
    _titleLength -= elapsed;
    _titlesTime += elapsed;

    for (auto it = _titles.begin(); it != _titles.end(); it++)
    {
       (*it)->update(elapsed);
    }

    return true;
}

bool
Titles::addTitle(Ctr::Title* title)
{
    _titles.push_back(title);
    return true;
}

bool
Titles::isFinished() const
{
    return _titleLength < 0;
}

void
Titles::setTitleLength(float value)
{
    _titlesTime = 0;
    _titleLength = value;
}

float
Titles::titleLength() const
{
    return _titleLength;
}

//---------------------------
// Render the titles texture.
//---------------------------
void
Titles::render()
{
    Ctr::Viewport oldview;
    _device->getViewport(&oldview);

    Ctr::FrameBuffer framebuffer(_titlesTexture->surface(), nullptr);
    _device->bindFrameBuffer(framebuffer);
    _device->setupViewport(framebuffer);

    _device->disableZTest();
    _device->disableDepthWrite();
    _device->disableStencilTest();
    _device->setCullMode (Ctr::CullNone);

    _device->enableAlphaBlending();
    _device->setAlphaSrcFunction (Ctr::SourceAlpha);
    _device->setAlphaDestFunction (Ctr::InverseSourceAlpha);
    _device->setSrcFunction (Ctr::SourceAlpha);
    _device->setDestFunction (Ctr::InverseSourceAlpha);


    for (auto it = _titles.begin(); it != _titles.end(); it++)
    {
        (*it)->render();
    }

    _device->enableZTest();
    _device->enableDepthWrite();
    _device->setCullMode (Ctr::CullNone);
    _device->disableAlphaBlending();

    _device->bindFrameBuffer(_device->postEffectsMgr()->sceneFrameBuffer());
    _device->setViewport(&oldview);
}


// Blend the title system into the scene.
bool
Titles::render (const Ctr::ITexture* surface, 
                const Ctr::Camera* camera)
{
    {
        Ctr::FrameBuffer framebuffer(_postEffectTexture->surface(), nullptr);

        _device->bindFrameBuffer(framebuffer);
        _device->setupViewport(framebuffer);
        _textureIn = surface;

        _titlesTimeVariable->set(&_titlesTime, sizeof(float));
        _titlesTextureVariable->setTexture(_titlesTexture);

        _device->clearSurfaces(0, CLEAR_TARGET);
        _shader->renderMesh (Ctr::RenderRequest(_postProcessTechnique, nullptr, camera, this));
    }

    _device->setNullTarget (0);
    return true;
}

}