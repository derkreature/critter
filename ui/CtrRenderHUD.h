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


#ifndef INCLUDED_CRT_RENDERHUD
#define INCLUDED_CRT_RENDERHUD

#include <CtrPlatform.h>
#include <CtrIRenderResource.h>
//#include <CtrFrameCounter.h>
#include <CtrInputState.h>

namespace Ctr
{

class Widget;
class Dialog;
class EditBox;
class ListBox;
class ImageWidget;
class Button;

class Application;
class RenderWindow;

class ITexture;
class Font;
class IDevice;
class InputState;
class Camera;

class RenderHUD 
{
  public:
    RenderHUD(Ctr::Application* application,
              Ctr::IDevice* device,
              Ctr::InputState* inputState,
              const std::string& logoPath = std::string("data/Textures/BakerLogo.dds"));
    
    virtual ~RenderHUD();

    virtual bool               create();
    virtual bool               update(double elapsedTime);
    virtual void               render(const Ctr::Camera* camera);
    virtual void               showApplicationUI()=0;
    void                       toggleScriptControlVisibility();

    bool                       logoVisible() const;
    void                       setLogoVisible(bool state);

    void                       setUIVisible(bool);
    bool                       uiVisible() const;


    Ctr::ImageWidget*           logo();

  protected:

    Ctr::Application*           _application;
    Ctr::Font*                  _fpsFont;
    Ctr::RenderWindow*          _renderWindow;
    bool                       _scriptControlsVisible;
    bool                       _logoVisible;
    bool                       _uiVisible;

    Ctr::IDevice*               _deviceInterface;
    Ctr::ImageWidget *          _logo;
    Ctr::InputState*            _inputState;

    bool                       _drawFps;
    std::string                _logoPath;
    float                      _elapsedTime;
};
}
#endif