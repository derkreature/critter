//------------------------------------------------------------------------------------//
//                                                                                    //
//            _____         _________       .__               .__                     //
//           /     \_______/   _____/_  _  _|__|______________|  |   ____             //
//          /  \ /  \_  __ \_____  \\ \/ \/ /  \___   /\___   /  | _/ __ \            //
//         /    Y    \  | \/        \\     /|  |/    /  /    /|  |_\  ___/            //
//         \____|__  /__| /_______  / \/\_/ |__/_____ \/_____ \____/\___  >           //
//                 \/             \/                 \/      \/         \/            //
//                                                                                    //
//    MrSwizzle is provided under the MIT License(MIT)                                //
//    MrSwizzle uses portions of other open source software.                          //
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

#include <CtrApplication.h>
#include <CtrRenderDeviceD3D11.h>

namespace Ctr
{
Application::Application(ApplicationHandle instance) : 
    Node(),
    _instance (instance),
    _mainWindow(nullptr),
    _device(nullptr)
{
#ifdef _DEBUG
    assert (_instance);
#endif
}

Application::~Application()
{
    _instance = nullptr;
    safedelete(_device);
}

ApplicationHandle
Application::instance() const
{
    return _instance;
}

Window*
Application::window()
{
    return _mainWindow; 
}

const Window*
Application::window() const
{
    return _mainWindow;
}

}