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

#ifndef INCLUDED_TITLES
#define INCLUDED_TITLES

#include <CtrPostEffect.h>

namespace Ctr
{
class ScreenOrientedQuad;

class Title
{
  public:
    Title(Ctr::IDevice* device);
    virtual ~Title();

    bool                       create(const std::string& texturePathName,
                                      const Ctr::Region2f& bounds,
                                      const Ctr::Vector4f& blendTerms);

    void                       update (float);
    void                       render();

  private:
    Ctr::IDevice*                _device;
    Ctr::ITexture*               _image;
    const Ctr::GpuTechnique*     _technique;
    const Ctr::IShader*          _shader;

    float                       _currentAlpha;
    Ctr::Vector4f                _blendTerms;
    std::string                 _texturePath;
    Ctr::ScreenOrientedQuad*     _quad;
    Ctr::Material*               _material;
    Region2f                    _bounds;
    float                       _currentTime;
};

//----------------
// Titles manager.
//----------------
class Titles : public Ctr::PostEffect
{
  public:
    Titles  (Ctr::IDevice* device);
    virtual ~Titles ();

    //----------------------------------------
    // Release device resources on lost device
    //----------------------------------------
    virtual bool                free();
    virtual bool                cache();

    //--------------------------------------------
    // Create the shader and all related resources
    //--------------------------------------------
    virtual bool                update (float elapsed);
    bool                        addTitle(Ctr::Title* title);
    bool                        isFinished() const;

    void                        setTitleLength(float);
    float                       titleLength() const;

    //---------------------------
    // Render the titles texture.
    //---------------------------
    void                         render();

    //------------------------
    // Render the post effect.
    //------------------------
    virtual bool                render (const Ctr::ITexture* surface, 
                                        const Ctr::Camera* camera);

  protected:
    std::vector<Ctr::Title*>    _titles;

    //-----------------------
    // Target render texture.
    //-----------------------
    Ctr::ITexture*              _titlesTexture;
    //---------------------------------
    // Total required length of titles.
    //---------------------------------
    float                      _titleLength;
    float                      _titlesTime;

    const Ctr::GpuTechnique *   _postProcessTechnique;
    const Ctr::GpuVariable *    _titlesTextureVariable;
    const Ctr::GpuVariable *    _backbufferTextureVariable;
    const Ctr::GpuVariable *    _titlesTimeVariable;
};

}

#endif