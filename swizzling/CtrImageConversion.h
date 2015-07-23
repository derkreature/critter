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

#ifndef INCLUDED_IMAGE_CONVERSION
#define INCLUDED_IMAGE_CONVERSION

#include <CtrPlatform.h>
#include <CtrRenderNode.h>
#include <CtrTypedProperty.h>
#include <CtrIDevice.h>
#include <CtrBitwise.h>
#include <ppl.h>

namespace Ctr
{

    struct ConvertPixel
    {
        // No gamma conversion
        template <typename T, typename S>
        inline void operator()(T& dst, const S& src) const
        {
            dst = T(src);
        }
        
        template < >
        inline void operator()(uint8_t& dst, const uint8_t& src) const
        {
            dst = src;
        }

        template < >
        inline void operator()(uint8_t& dst, const uint16_t& src) const
        {
            dst = uint8_t(((float)(src) / USHRT_MAX) * 255.0f);
        }

        template < >
        inline void operator()(uint8_t& dst, const half& src) const
        {
            dst = (uint8_t)(Bitwise::halfToFloat(src()) * 255.0f);
        }

        template < >
        inline void operator()(uint8_t& dst, const float& src) const
        {
            dst = (uint8_t)(src * 255.0f);
        }
        template < >
        inline void operator()(uint16_t& dst, const uint8_t& src) const
        {
            dst = (uint16_t)((src / 255.0f) * USHRT_MAX);
        }

        template < >
        inline void operator()(uint16_t& dst, const float& src) const
        {
            dst = (uint16_t)(saturate(src) * USHRT_MAX);
        }

        template < >
        inline void operator()(uint16_t& dst, const uint16_t& src) const
        {
            dst = src;
        }
        template < >
        inline void operator()(uint16_t& dst, const half& src) const
        {
            dst = uint16_t(saturate(Bitwise::halfToFloat(src())) / USHRT_MAX);
        }

        template < >
        inline void operator()(half& dst, const uint8_t& src) const
        {
            dst = Bitwise::floatToHalf((src / 255.0f));
        }

        template < >
        inline void operator()(half& dst, const uint16_t& src) const
        {
            dst = Bitwise::floatToHalf(float(src / USHRT_MAX));
        }

        template < >
        inline void operator()(half& dst, const half& src) const
        {
            dst = src;
        }

        template < >
        inline void operator()(half& dst, const float& src) const
        {
            dst = Bitwise::floatToHalf(src);
        }
         
        template < >
        inline void operator()(float& dst, const uint8_t& src) const
        {
            dst = (float)(src) / 255.0f;
        }

        template < >
        inline void operator()(float& dst, const uint16_t& src) const
        {
            dst = (float)(src) / USHRT_MAX;
        }

        template < >
        inline void operator()(float& dst, const half& src) const
        {
            dst = Bitwise::halfToFloat(src());
        }

        template < >
        inline void operator()(float& dst, const float& src) const
        {
            dst = src;
        }

        // With gamma conversion.
        template <typename T, typename S>
        inline void operator()(T& dst, const S& src, float power) const
        {
            dst = T(src);
        }

        template < >
        inline void operator()(uint8_t& dst, const uint8_t& src, float power) const
        {
            float converted = saturate(pow((((float)src)/255.0f), power));
            dst = (uint8_t)(converted * 255.0f);
        }

        template < >
        inline void operator()(uint8_t& dst, const uint16_t& src, float power) const
        {
            float converted = saturate(pow((((float)src) / USHRT_MAX), power));
            dst = uint8_t(converted * 255.0f);
        }

        template < >
        inline void operator()(uint8_t& dst, const half& src, float power) const
        {
            float converted = saturate(pow(Bitwise::halfToFloat(src()), power));
            dst = (uint8_t)(converted * 255.0f);
        }

        template < >
        inline void operator()(uint8_t& dst, const float& src, float power) const
        {
            float converted = saturate(pow(src, power));
            dst = (uint8_t)(converted * 255.0f);
        }
        template < >
        inline void operator()(uint16_t& dst, const uint8_t& src, float power) const
        {
            float converted = saturate(pow(((src / 255.0f)), power));
            dst = (uint16_t)(converted * USHRT_MAX);
        }

        template < >
        inline void operator()(uint16_t& dst, const float& src, float power) const
        {
            float converted = saturate(pow(src, power));
            dst = (uint16_t)(converted * USHRT_MAX);
        }

        template < >
        inline void operator()(uint16_t& dst, const uint16_t& src, float power) const
        {
            float converted = saturate(float(pow((float)(src / USHRT_MAX), power)));
            dst = uint16_t(converted * USHRT_MAX);
        }
        template < >
        inline void operator()(uint16_t& dst, const half& src, float power) const
        {
            float converted = saturate(pow(saturate(Bitwise::halfToFloat(src())), power));
            dst = uint16_t(converted / USHRT_MAX);
        }

        template < >
        inline void operator()(half& dst, const uint8_t& src, float power) const
        {
            float converted = saturate(pow((src / 255.0f), power));
            dst = Bitwise::floatToHalf(converted);
        }

        template < >
        inline void operator()(half& dst, const uint16_t& src, float power) const
        {
            float converted = saturate(pow(float(src / USHRT_MAX), power));
            dst = Bitwise::floatToHalf(converted);
        }

        template < >
        inline void operator()(half& dst, const half& src, float power) const
        {
            float converted = saturate(pow(Bitwise::halfToFloat(src()), power));
            dst = Bitwise::floatToHalf(converted);
        }

        template < >
        inline void operator()(half& dst, const float& src, float power) const
        {
            float converted = saturate(pow(src, power));
            dst = Bitwise::floatToHalf(converted);
        }

        template < >
        inline void operator()(float& dst, const uint8_t& src, float power) const
        {
            float converted = saturate(pow(float(src / 255.0f), power));
            dst = (float)(converted);
        }

        template < >
        inline void operator()(float& dst, const uint16_t& src, float power) const
        {
            float converted = saturate(pow(float(src / USHRT_MAX), power));
            dst = (float)(converted);
        }

        template < >
        inline void operator()(float& dst, const half& src, float power) const
        {
            float converted = saturate(pow(Bitwise::halfToFloat(src()), power));
            dst = converted;
        }

        template < >
        inline void operator()(float& dst, const float& src, float power) const
        {
            float converted = saturate(pow(src, power));
            dst = converted;
        }
    };


    struct ConvertImage
    {
        template <typename T, typename S>
        void convert(size_t rowId,
                    T* dst,
                    S* src,
                    size_t width,
                    size_t height,
                    size_t dstChannels,
                    size_t srcChannels,
                    uint32_t * channelMapping,
                    float   dstGamma,
                    float   srcGamma)
        {
            ConvertPixel convertPixel;
            size_t dstOffset = (width * rowId) * dstChannels;
            size_t srcOffset = (width * rowId) * srcChannels;

            if (Ctr::Limits<float>::isEqual(dstGamma, srcGamma))
            {
                for (uint32_t i = 0; i < width; i++)
                {
                    size_t dstPixelId = dstOffset + (i * dstChannels);
                    size_t srcPixelId = srcOffset + (i * srcChannels);
                    for (uint32_t c = 0; c < dstChannels; c++)
                    {
                        uint32_t srcChannel = channelMapping[c];
                        convertPixel(dst[dstPixelId + c], src[srcPixelId + srcChannel]);
                    }
                }
            }
            else
            {
                float power = srcGamma/dstGamma;
                for (uint32_t i = 0; i < width; i++)
                {
                    size_t dstPixelId = dstOffset + (i * dstChannels);
                    size_t srcPixelId = srcOffset + (i * srcChannels);
                    for (uint32_t c = 0; c < dstChannels; c++)
                    {
                        uint32_t srcChannel = channelMapping[c];
                        convertPixel(dst[dstPixelId + c], src[srcPixelId + srcChannel], power);
                    }
                }
            }
        }

        template <typename T, typename S>
        void convert(size_t rowId,
            T* dst,
            S* src,
            size_t width,
            size_t height,
            size_t dstChannels,
            size_t srcChannels,
            float   dstGamma,
            float   srcGamma)
        {
            ConvertPixel convertPixel;

            if (Ctr::Limits<float>::isEqual(dstGamma, srcGamma))
            {
                size_t dstOffset = (width * rowId) * dstChannels;
                size_t srcOffset = (width * rowId) * srcChannels;
                for (uint32_t i = 0; i < width; i++)
                {
                    size_t dstPixelId = dstOffset + (i * dstChannels);
                    size_t srcPixelId = srcOffset + (i * srcChannels);
                    for (uint32_t c = 0; c < dstChannels; c++)
                        convertPixel(dst[dstPixelId+c], src[srcPixelId + c]);
                }
            }
            else
            {
                float power = srcGamma / dstGamma;
                size_t dstOffset = (width * rowId) * dstChannels;
                size_t srcOffset = (width * rowId) * srcChannels;
                for (uint32_t i = 0; i < width; i++)
                {
                    size_t dstPixelId = dstOffset + (i * dstChannels);
                    size_t srcPixelId = srcOffset + (i * srcChannels);
                    for (uint32_t c = 0; c < dstChannels; c++)
                        convertPixel(dst[dstPixelId + c], src[srcPixelId + c], power);
                }
            }
        }

        template <typename T, typename S>
        void convert(T* dst, 
                     S* src,
                     size_t width,
                     size_t height,
                     size_t dstChannels,
                     size_t srcChannels,
                     uint32_t * channelMapping,
                     float   dstGamma,
                     float   srcGamma)
        {
            if (channelMapping)
            {
                concurrency::parallel_for(size_t(0), size_t(height), [&](size_t rowId)
                {
                    convert(rowId, dst, src, width, height, dstChannels, srcChannels, channelMapping, dstGamma, srcGamma);
                });
            }
            else
            {
                concurrency::parallel_for(size_t(0), size_t(height), [&](size_t rowId)
                {
                    convert(rowId, dst, src, width, height, dstChannels, srcChannels, dstGamma, srcGamma);
                });
            }
        }

        uint32_t* defaultChannelMapping(size_t dstComponents, size_t srcComponents)
        {
            // Setup default channel mapping.
            switch (dstComponents)
            {
                case 1:
                {
                    static uint32_t mapping[] = { 0, 0 };
                    return mapping;
                }
                case 2:
                {
                    switch (srcComponents)
                    {
                        case 1:
                        {
                            static uint32_t mapping[] = { 0, 0 };
                            return mapping;
                        }
                        case 3:
                        {
                            static uint32_t mapping[] = { 0, 1 };
                            return mapping;
                        }
                        case 4:
                        {
                            static uint32_t mapping[] = { 0, 1 };
                            return mapping;
                        }
                        default:
                            break;
                    }
                }
                case 3:
                {
                    switch (srcComponents)
                    {
                        case 1:
                        {
                            static uint32_t mapping[] = { 0, 0, 0 };
                            return mapping;
                        }
                        case 2:
                        {
                            static uint32_t mapping[] = { 0, 1, 1 };
                            return mapping;
                        }
                        case 4:
                        {
                            static uint32_t mapping[] = { 0, 1, 2 };
                            return mapping;
                        }
                        default:
                            break;
                    }
                }
                case 4:
                {
                    switch (srcComponents)
                    {
                        case 1:
                        {
                            static uint32_t mapping[] = { 0, 0, 0, 0 };
                            return mapping;
                        }
                        case 2:
                        {
                            static uint32_t mapping[] = { 0, 1, 0, 1 };
                            return mapping;
                        }
                        case 3:
                        {
                            static uint32_t mapping[] = { 0, 1, 2, 2 };
                            return mapping;
                        }
                        default:
                            break;
                    }
                }
            }
            return nullptr;
        }

        void convert(Ctr::TextureImagePtr& dstImage, float dstGamma, 
                     Ctr::TextureImagePtr& srcImage, float srcGamma,
                     uint32_t* channelMapping = nullptr)
        {
            Ctr::PixelFormat dstFormat = dstImage->getFormat();
            Ctr::PixelFormat srcFormat = srcImage->getFormat();

            Ctr::PixelBox dstPixelBox = dstImage->getPixelBox(0, 0);
            Ctr::PixelBox srcPixelBox = srcImage->getPixelBox(0, 0);

            Ctr::PixelComponentType dstType = PixelUtil::getComponentType(dstFormat);
            Ctr::PixelComponentType srcType = PixelUtil::getComponentType(srcFormat);

            size_t dstComponents = PixelUtil::getComponentCount(dstFormat);
            size_t srcComponents = PixelUtil::getComponentCount(srcFormat);

            size_t width = dstImage->getWidth();
            size_t height = dstImage->getHeight();

            if (channelMapping == nullptr && srcComponents != dstComponents)
            {
                channelMapping = defaultChannelMapping(dstComponents, srcComponents);
            }

            switch (dstType)
            {
                case PCT_BYTE:
                {
                    uint8_t* dst = (uint8_t*)(dstPixelBox.data);
                    switch (srcType)
                    {
                        case PCT_BYTE:
                        {
                            return convert(dst, (uint8_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_SHORT:
                        {
                            return convert(dst, (uint16_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT16:
                        {
                            return convert(dst, (half*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT32:
                        {
                            return convert(dst, (float*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                    }
                }
                case PCT_SHORT:
                {
                    uint16_t* dst = (uint16_t*)(dstPixelBox.data);
                    switch (srcType)
                    {
                       case PCT_BYTE:
                        {
                            return convert(dst, (uint8_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_SHORT:
                        {
                            return convert(dst, (uint16_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT16:
                        {
                            return convert(dst, (half*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT32:
                        {
                            return convert(dst, (float*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                    }
                }
                case PCT_FLOAT16:
                {
                    uint16_t* dst = (uint16_t*)(dstPixelBox.data);
                    switch (srcType)
                    {
                        case PCT_BYTE:
                        {
                            return convert(dst, (uint8_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_SHORT:
                        {
                            return convert(dst, (uint16_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT16:
                        {
                            return convert(dst, (half*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT32:
                        {
                            return convert(dst, (float*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                    }
                }
                case PCT_FLOAT32:
                {
                    float* dst = (float*)(dstPixelBox.data);
                    switch (srcType)
                    {
                        case PCT_BYTE:
                        {
                            return convert(dst, (uint8_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_SHORT:
                        {
                            return convert(dst, (uint16_t*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT16:
                        {
                            return convert(dst, (half*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                        case PCT_FLOAT32:
                        {
                            return convert(dst, (float*)(srcPixelBox.data), width, height, dstComponents, srcComponents,
                                           channelMapping, dstGamma, srcGamma);
                        }
                    }
                }
            }
        }
    };
}

#endif