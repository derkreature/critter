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

#include <CtrFocusedInput.h>

namespace Ctr
{
FocusedInput::FocusedInput(Ctr::InputState* state) :
    _inputState(state)
{
}

FocusedInput::~FocusedInput(void)
{
}

bool 
FocusedInput::update (float elapsedTime, 
                      Ctr::Vector3f &translation, 
                      Ctr::Vector3f &rotate, 
                      bool &updatecamerainput)
{
    Ctr::Vector3f ttrans = translation;
    Ctr::Vector3f trotate = rotate;

    if (_inputState->leftMouseDown() && !_inputState->hasGUIFocus() && !_inputState->getKeyState(DIK_LCONTROL))
    {    
        rotate += Ctr::Vector3f ( 0, ((float)-_inputState->_y), ((float)-_inputState->_x));
        if (_inputState->getKeyState(DIK_UP))
            translation.y += -255 * elapsedTime;
            
        if (_inputState->getKeyState(DIK_DOWN))
            translation.y += 255 * elapsedTime;

        translation.y += 0.2f * -_inputState->_z;
    }

    return !(ttrans == translation) || !(trotate == rotate);
}

}