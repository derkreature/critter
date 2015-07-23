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

#include <CtrHash.h>
#include <MurmurHash.h>

namespace Ctr
{
Hash::Hash(const std::string& source)
{
    build(source);
}

Hash::Hash()
{
    memset(&_hash[0], 0, Hash::HashSize);
}
Hash::Hash(const Hash& other)
{
    memcpy(&_hash[0], &other._hash[0], Hash::HashSize);
}

Hash::~Hash()
{
}

bool
Hash::valid() const
{
    uint64_t hash[2];
    memset(&hash[0], 0, Hash::HashSize);
    if (memcmp(&_hash[0], &hash[0], Hash::HashSize) == 0)
        return false;
    else
        return true;
}

bool
Hash::operator==(const Hash& other) const
{
    return memcmp(&_hash[0], &other._hash[0], Hash::HashSize) == 0;
}

bool
Hash::operator!=(const Hash& other) const
{
    return memcmp(&_hash[0], &other._hash[0], Hash::HashSize) != 0;
}

bool
Hash::operator<(const Hash& other) const
{
    return memcmp(&_hash[0], &other._hash[0], Hash::HashSize) < 0;
}

const Hash&
Hash::operator=(const Hash& other)
{
    if (&other != this)
    {
        memcpy(&_hash[0], &other._hash[0], Hash::HashSize);
    }
    return *this;
}

void
Hash::build(const std::string& string)
{
    MurmurHash3_x64_128(string.c_str(), (int32_t)(string.length()), 0, &_hash[0]);
}

void
Hash::build(const std::wstring& string)
{
    MurmurHash3_x64_128(string.c_str(), (int32_t)(string.length() * sizeof(wchar_t)), 0, &_hash[0]);
}

void
Hash::append(const Hash& other)
{
    std::ostringstream stream;
    stream << _hash[0] << "_" << _hash[1] << other._hash[0] << "_" << other._hash[1];
    MurmurHash3_x64_128(stream.str().c_str(), (int32_t)(stream.str().length() * sizeof(uint8_t)), 0, &_hash[0]);
}

}