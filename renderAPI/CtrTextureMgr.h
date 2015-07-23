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

#ifndef INCLUDED_CRT_TEXTURE_MANAGER
#define INCLUDED_CRT_TEXTURE_MANAGER

#include <CtrPlatform.h>
#include <CtrRenderEnums.h>
#include <CtrHash.h>
#include <CtrTextureImage.h>

namespace Ctr
{
class Application;
class IDevice;
class Texture2DProperty;
class ITexture;

class TextureMgr
{
  public:
    TextureMgr(const Ctr::Application* application,
               Ctr::IDevice* device);
    virtual ~TextureMgr();

    // Simple load texture
    ITexture*                    loadTexture (const std::string& filename,
                                              Ctr::PixelFormat format = Ctr::PF_A8R8G8B8);

    // Load texture from an array of images.
    ITexture*                    loadTextureSet (const std::string& key, 
                                                 const std::vector<std::string> & filenames);

    // Load texture from an array of images.
    ITexture*                    removeProperty ();


    // Usually for texture reads or building complex maps. These textures cannot be bound on the GPU.
    ITexture*                     loadStagingTexture(const std::string& filename);

    const ITexture*               createTexture (const std::string& name,
                                              Ctr::PixelFormat format = Ctr::PF_A8R8G8B8,
                                              unsigned int width = 0,
                                              unsigned int height = 0,
                                              unsigned int depth = 0);

    const ITexture*               loadCubeTexture (const std::string& name,
                                                Ctr::PixelFormat format = Ctr::PF_A8R8G8B8);

    const ITexture*               loadThreeD (const std::string& name,
                                                  Ctr::PixelFormat format = Ctr::PF_A8R8G8B8);

    void                          recycle(const ITexture* texture);

    void                          update (float delta);

    TextureImagePtr               loadImage(const std::string& filePathName,
                                            const Ctr::Hash& archiveHash);
    std::vector<TextureImagePtr>  loadImages(const std::vector<std::string>& filenames);

  protected:
    ITexture*                    findTexture (const std::string& name);

  private:
    typedef std::map<std::string, ITexture*> TextureMap;
    typedef std::map<Ctr::Hash, TextureImagePtr> ImageMap;
    TextureMap                   _textures;
    TextureMap                   _stagingTextures;
    ImageMap                     _images;
    Ctr::IDevice*                _deviceInterface;
};
}

#endif