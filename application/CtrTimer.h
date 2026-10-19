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

#pragma once

#include <chrono>

namespace Ctr
{ 
class Timer
{
public:
    // Construct with optional target FPS (0 = unlocked)
    explicit Timer(unsigned int targetFps = 0u) noexcept;

    // Reset reference clock to now and zero accumulated state
    void reset() noexcept;

    // Call once per loop; returns elapsed delta in seconds since last tick
    double tick() noexcept;

    // Block until the next frame time if target FPS is set.
    // Returns true if the thread slept; false if already behind schedule or unlocked.
    bool waitUntilNextFrame() noexcept;

    // Set or change the target FPS (0 => unlocked)
    void setTargetFPS(unsigned int fps) noexcept;

    // Get the current target FPS (0 means unlocked)
    unsigned int targetFPS() const noexcept;

    // Get last tick delta in seconds
    double lastDeltaSeconds() const noexcept;

    // Preserved public fields (kept public to preserve existing call-sites)
    double _apptime;         // accumulated application time (seconds)
    double _absTime;         // absolute time since timer creation (seconds)
    double _normalizedTime;  // normalized time in [0,1) (apptime mod 1.0)
    unsigned int frameRateCount; // increments each tick

private:
    using clock = std::chrono::steady_clock;
    clock::time_point _startTime;
    clock::time_point _lastTime;
    std::chrono::nanoseconds _frameDuration;
    double _lastDeltaSeconds;
};
}