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

#include "CtrTimer.h"
#include <thread>
#include <cmath>
#include <cstdint>

namespace Ctr
{ 
Timer::Timer(unsigned int targetFps) noexcept
    : _startTime(clock::now()),
    _lastTime(_startTime),
    _frameDuration(std::chrono::nanoseconds::zero()),
    _lastDeltaSeconds(0.0),
    _apptime(0.0),
    _absTime(0.0),
    _normalizedTime(0.0),
    frameRateCount(0u)
{
    setTargetFPS(targetFps);
}

void Timer::reset() noexcept
{
    _startTime = clock::now();
    _lastTime = _startTime;
    _lastDeltaSeconds = 0.0;
    _apptime = 0.0;
    _absTime = 0.0;
    _normalizedTime = 0.0;
    frameRateCount = 0u;
}

double Timer::tick() noexcept
{
    auto now = clock::now();
    auto delta = now - _lastTime;
    _lastDeltaSeconds = std::chrono::duration<double>(delta).count();

    // Advance logical/accumulated times
    _apptime += _lastDeltaSeconds;
    _absTime = std::chrono::duration<double>(now - _startTime).count();
    _normalizedTime = std::fmod(_apptime, 1.0);
    ++frameRateCount;

    // Move last time to "now" so next tick measures from here.
    _lastTime = now;
    return _lastDeltaSeconds;
}

bool Timer::waitUntilNextFrame() noexcept
{
    if (_frameDuration == std::chrono::nanoseconds::zero())
        return false; // unlocked

    // Scheduled target based on the last measured frame boundary.
    auto target = _lastTime + _frameDuration;
    auto now = clock::now();

    if (now >= target)
    {
        // We're already late; don't sleep.
        return false;
    }

    std::this_thread::sleep_until(target);

    // Align lastTime to the scheduled target so next tick() measures from expected frame boundary.
    _lastTime = target;

    // Reflect that we slept a full frame
    _lastDeltaSeconds = std::chrono::duration<double>(_frameDuration).count();
    _apptime += _lastDeltaSeconds;
    _absTime = std::chrono::duration<double>(_lastTime - _startTime).count();
    _normalizedTime = std::fmod(_apptime, 1.0);
    ++frameRateCount;

    return true;
}

void Timer::setTargetFPS(unsigned int fps) noexcept
{
    if (fps == 0u)
    {
        _frameDuration = std::chrono::nanoseconds::zero();
    }
    else
    {
        // Compute nanoseconds per frame rounded to nearest integer
        double nanosPerFrame = 1e9 / static_cast<double>(fps);
        auto nanos = static_cast<long long>(std::llround(nanosPerFrame));
        _frameDuration = std::chrono::nanoseconds(nanos);
    }
}

unsigned int Timer::targetFPS() const noexcept
{
    if (_frameDuration == std::chrono::nanoseconds::zero()) return 0u;
    auto count = _frameDuration.count();
    if (count <= 0) return 0u;
    auto fps = static_cast<unsigned int>(1000000000LL / count);
    return fps == 0u ? 1u : fps;
}

double Timer::lastDeltaSeconds() const noexcept
{
    return _lastDeltaSeconds;
}
}