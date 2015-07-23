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
#ifndef INCLUDED_IBL_FUNCTION_NODE
#define INCLUDED_IBL_FUNCTION_NODE

#include <CtrPlatform.h>
#include <CtrRenderNode.h>
#include <CtrTypedProperty.h>
#include <CtrIDevice.h>
#include <CtrImageConversion.h>
#include <CtrITexture.h>
#include <CtrTextureMgr.h>
#include <ppl.h>
#include <CtrVector3.h>

namespace Ctr
{
class ImageNode;
class TextureImageNode;


// Later:
// Bump amount.
// HorizonOcclusion
// SMAA
// Generate Toksvig.
// Save height.

enum InterpretPixelsAsType
{
    UnknownImage,
    AlbedoImage,
    TangentNormalImage,
    SpecularImage,
    GlossImage,
    RoughnessImage,
    MetalMaskImage,
    MetalImage,
    HeightImage,
    AOImage,
    CavietyImage,
    ToksvigImage
};

class ImageFunction : public Property
{
public:
    ImageFunction(RenderNode* n) :
        Property(n, "ImageFunction"),
        _node(n),
        _device(n->device()),
        _imageResultProperty(nullptr),
        _textureResultProperty(nullptr)
    {
        _imageResultProperty = new TextureImageProperty(this, std::string("imageResult"), this);
        _textureResultProperty = new TextureProperty(this, std::string("textureResult"), this);
        _convertedRGBAImageProperty = new TextureImageProperty(this, std::string("imageResultRGBA"), this);

        memset(&_imageDependencies[0], 0, sizeof(TextureImageProperty*)*5);
    }

    virtual ~ImageFunction()
    {
    }

    virtual void refilterMip(Ctr::TextureImagePtr& mipImage) const
    {
    }

    const TextureImageProperty*     imageResultProperty() const
    {
        return _imageResultProperty;
    }

    TextureImageProperty*           imageResultProperty()
    {
        return _imageResultProperty;
    }

    const TextureImageProperty* imageDependency(uint32_t id) const
    {
        return _imageDependencies[id];
    } 

    void                       setImageDependency(uint32_t id,
                                                  TextureImageProperty* textureImageProperty)
    {
        std::ostringstream textureImagePropertyName;
        textureImagePropertyName << "image" << id;
        if (Property* property = dependency(textureImagePropertyName.str()))
        {
            removeDependency(property, textureImagePropertyName.str());
            _imageDependencies[id] = nullptr;
        }
        if (textureImageProperty)
        {
            addDependency(textureImageProperty, textureImagePropertyName.str());
            _imageDependencies[id] = textureImageProperty;
        }
    }

    TextureProperty*           textureResultProperty()
    {
        return _textureResultProperty;
    }

    const TextureProperty*     textureResultProperty() const
    {
        return _textureResultProperty;
    }

    void                       computeRGBAImage(const Property* property) const
    {
        Ctr::TextureImagePtr sourceImage = _imageResultProperty->get();
        if (PixelUtil::getComponentCount(sourceImage->getFormat()) != 4)
        {
            Ctr::TextureImagePtr convertedImage(new Ctr::TextureImage());
            convertedImage->create(Ctr::Vector2i(int32_t(sourceImage->getWidth()), int32_t(sourceImage->getHeight())),
                PF_FLOAT32_RGBA,
                (uint32_t)(0) /* no mips*/,
                IF_DEFAULT);
            ConvertImage converter;
            // Implicit channel remapping to debug output.
            converter.convert(convertedImage, 1.0f, sourceImage, 1.0f);
            _convertedRGBAImageProperty->set(convertedImage);
        }
        else
        {
            _convertedRGBAImageProperty->set(sourceImage);
        }
    }

    void                       computeTexture(const Property* property) const
    {
        Ctr::TextureImagePtr sourceImage = _convertedRGBAImageProperty->get();

        Ctr::TextureImagePtr convertedImage(new Ctr::TextureImage());
        convertedImage->create(Ctr::Vector2i(int32_t(sourceImage->getWidth()), int32_t(sourceImage->getHeight())),
            PF_A8R8G8B8,
            (uint32_t)(0),
            IF_DEFAULT);

        FloatProperty* gammaDisplayProperty =
            dynamic_cast<FloatProperty*>(_node->property("gammaDisplay"));
        float srcGamma = 1.0f;
        float dstGamma = gammaDisplayProperty->get();

        ConvertImage converter;
        uint32_t channelMapping[] = { 0, 1, 2, 3 };
        converter.convert(convertedImage, dstGamma, sourceImage, srcGamma, channelMapping);

        // Check mip generation.
        BoolProperty* generateMipMapsProperty =
            dynamic_cast<BoolProperty*>(_node->property("generateMipMaps"));
        uint32_t mipLevels = 1;
        if (generateMipMapsProperty->get())
            mipLevels = Ctr::numberOfMipsInChain(uint32_t(minValue(convertedImage->getWidth(), convertedImage->getHeight())));

        Ctr::ITexture* texture = nullptr;
        if (mipLevels == 1)
        {
            Ctr::TextureParameters textureData =
            Ctr::TextureParameters("ImageFunctionOutput",
                                    convertedImage,
                                    Ctr::TwoD,
                                    Ctr::FromFile);
            texture = _device->createTexture(&textureData);
        }
        else
        {
            Ctr::TextureImagePtr mipChainImage(new Ctr::TextureImage());
            mipChainImage->create(
                Ctr::Vector2i(int32_t(sourceImage->getWidth()), int32_t(sourceImage->getHeight())),
                PF_A8R8G8B8,
                mipLevels,
                IF_DEFAULT);
            int32_t mipWidth = int32_t(convertedImage->getWidth());
            int32_t mipHeight = int32_t(convertedImage->getHeight());

            bool resampleLevel0 = false;

            // TODO: Push this to a compute shader.
            for (int32_t mipId = 0; mipId < int32_t(mipLevels); mipId++)
            {
                PixelBox mipLevelPixels = mipChainImage->getPixelBox(0, mipId);

                if (mipId > 0)
                {
                    PixelBox convertedPixels = convertedImage->getPixelBox();

                    if (resampleLevel0)
                    {
                        // Create temporary, copy and resize.
                        Ctr::TextureImagePtr resizedConvertedImage(new Ctr::TextureImage());
                        resizedConvertedImage->create(Ctr::Vector2i(int32_t(convertedImage->getWidth()), int32_t(convertedImage->getHeight())),
                            PF_A8R8G8B8,
                            (uint32_t)(0),
                            IF_DEFAULT);

                        // Resize to mip size.
                        {
                            PixelBox resizedPixels = resizedConvertedImage->getPixelBox();
                            memcpy(resizedPixels.data, convertedPixels.data, convertedPixels.getConsecutiveSize());
                        }
                
                        resizedConvertedImage->resize(mipWidth, mipHeight);

                        // Give the node a chance to fix up any problems as a result of 
                        // scaling down.
                        refilterMip(resizedConvertedImage);

                        // Copy to mip chain.
                        {
                            PixelBox resizedPixels = resizedConvertedImage->getPixelBox();
                            memcpy(mipLevelPixels.data, resizedPixels.data, resizedPixels.getConsecutiveSize());
                        }
                    }
                    else
                    {
                        convertedImage->resize(mipWidth, mipHeight);
                        refilterMip(convertedImage);
                        PixelBox convertedPixels = convertedImage->getPixelBox();
                        memcpy(mipLevelPixels.data, convertedPixels.data, convertedPixels.getConsecutiveSize());
                    }
                }
                else
                {
                    PixelBox convertedPixels = convertedImage->getPixelBox();
                    memcpy(mipLevelPixels.data, convertedPixels.data, convertedPixels.getConsecutiveSize() );
                }

                mipWidth = mipWidth / 2;
                mipHeight = mipHeight / 2;
            }

            Ctr::TextureParameters textureData =
                Ctr::TextureParameters("ImageFunctionOutput",
                                        mipChainImage,
                                        Ctr::TwoD,
                                        Ctr::FromFile);
            texture = _device->createTexture(&textureData);
        }

        _textureResultProperty->set(texture);
    }

  protected:
    RenderNode*                _node;
    TextureImageProperty*      _imageResultProperty;
    TextureImageProperty*      _convertedRGBAImageProperty;
    TextureProperty*           _textureResultProperty;
    IDevice*                   _device;
    TextureImageProperty*      _imageDependencies[5];
};

class ImageFileSourceFunction : public ImageFunction
{
  public:
    ImageFileSourceFunction(RenderNode* n) : ImageFunction(n)
    {
        addDependency(new HashProperty(_node, "archiveHandle"),0);
        addDependency(new StringProperty(_node, "filename"),0);
        addDependency(new BoolProperty(_node, "linearize"),0);

        using std::placeholders::_1;
        addTask(std::make_pair(_imageResultProperty,
            std::bind(&ImageFileSourceFunction::computeImage, this, _1)));
        addTask(std::make_pair(_textureResultProperty,
            std::bind(&ImageFunction::computeTexture, this, _1)));
        addTask(std::make_pair(_convertedRGBAImageProperty,
            std::bind(&ImageFunction::computeRGBAImage, this, _1)));
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          Ctr::PixelBox& destination,
                                          uint8_t colorValue,
                                          uint8_t alphaValue) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        uint8_t* destinationPtr = (uint8_t*)destination.data;
        for (size_t columnId = 0; columnId < imageWidth; columnId++)
        {
            size_t dstPixelId = (startId + columnId) * dstComponents;
            destinationPtr[dstPixelId + 0] = colorValue;
            destinationPtr[dstPixelId + 1] = colorValue;
            destinationPtr[dstPixelId + 2] = colorValue;
            destinationPtr[dstPixelId + 3] = alphaValue;
        }
    }

    void computeImage(const Property* property) const
    {
        const std::string& filename = dynamic_cast<const StringProperty*>
            (dependency("filename"))->get();
        const Hash& hash = dynamic_cast<const HashProperty*>
            (dependency("archiveHandle"))->get();

        IntProperty* interpretPixelsAsProperty =
            dynamic_cast<IntProperty*>(_node->property("interpretAs"));
        const Ctr::Vector2i& commonSize =
            dynamic_cast<Ctr::Vector2iProperty*>(_node->property("commonSize"))->get();
        TextureImagePtr sourceImage = _device->textureMgr()->loadImage(filename, hash);
        if (!sourceImage->valid())
        {
            uint8_t fillColor = 0;
            uint8_t fillAlpha = 255;
            switch ((InterpretPixelsAsType)(interpretPixelsAsProperty->get()))
            {
                case AOImage:
                    fillColor = 255;
                default:
                    break;
            }
            sourceImage->create(Ctr::Vector2i(commonSize.x, commonSize.y), PF_A8R8G8B8, 1, 0);

            // Fill the missing image.
            PixelBox sourcePixelBox = sourceImage->getPixelBox();
            size_t sourceWidth = sourceImage->getWidth();
            size_t sourceHeight = sourceImage->getHeight();
            concurrency::parallel_for(size_t(0), size_t(sourceWidth), [&](size_t rowId)
            {
                (*this)(rowId, sourceWidth, sourceHeight, sourcePixelBox, fillColor, fillAlpha);
            });
        }

        if (commonSize != Ctr::Vector2i(int32_t(sourceImage->getWidth()), int32_t(sourceImage->getHeight())))
        {
            sourceImage->resize(commonSize.x, commonSize.y);
        }

        Ctr::TextureImagePtr convertedImage(new Ctr::TextureImage());
        convertedImage->create(Ctr::Vector2i(int32_t(sourceImage->getWidth()), int32_t(sourceImage->getHeight())),
            PF_FLOAT32_RGBA,
            (uint32_t)(0) /* no mips*/,
            IF_DEFAULT);

        FloatProperty* gammaInProperty = 
            dynamic_cast<FloatProperty*>(_node->property("gammaIn"));

        float dstGamma = 1.0f;
        float srcGamma = gammaInProperty->get();

        ConvertImage converter;
        if (sourceImage->getFormat() == Ctr::PF_A8B8G8R8)
        {
            uint32_t channelMapping[] = { 2, 1, 0, 3 };
            converter.convert(convertedImage, dstGamma, sourceImage, srcGamma, channelMapping);
        }
        else
        {
            uint32_t channelMapping[] = { 0, 1, 2, 3 };
            converter.convert(convertedImage, dstGamma, sourceImage, srcGamma, channelMapping);
        }
        _imageResultProperty->set(convertedImage);
    }
};

enum ImageProcessorMips
{
    MatchMips,
    NoMips
};

class ImageFunctionProcessor : public ImageFunction
{
  public:
      ImageFunctionProcessor(RenderNode* n, Property* p) : 
        ImageFunction(n),
        _processingNode(n),
        _processingProperty(p),
        _imageWidth(0),
        _imageHeight(0)
    {
    }

    ImageProcessorMips         mipType() const
    {
        return MatchMips;
    }

    size_t                     imageWidth() const
    {
        return _imageWidth;
    }

    size_t                     imageHeight() const
    {
        return _imageHeight;
    }

    uint32_t                   componentNameToId(const std::string& name) const
    {
        if (name == "R" || name == "Y")
            return 0;
        else if (name == "G")
            return 1;
        else if (name == "B")
            return 2;
        else if (name == "A")
            return 3;

        IBLASSERT(0, "Cannot find component named: " << name)
        return -1;
    }

  protected:
    Node*                      _processingNode;
    Property*                  _processingProperty;
    mutable size_t             _imageWidth;
    mutable size_t             _imageHeight;
};

// An operator with 3 inputs,
// Ouput is D = lerp(A, B, C);
class ImageSolidColor : public ImageFunctionProcessor
{
public:
    ImageSolidColor(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p),
        _componentCount(4)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        _imageWidth = _widthProperty->get();
        _imageHeight = _heightProperty->get();
        _color = _colorProperty->get();
    }

    void                       setup()
    {
        _colorProperty = new Vector4fProperty(_processingNode, "LerpSourceComponent");
        _widthProperty = new IntProperty(_processingNode, "ImageWidth");
        _heightProperty = new IntProperty(_processingNode, "ImageHeight");

        _colorProperty->set(Vector4f(1, 1, 1, 1));
        _widthProperty->set(0);
        _heightProperty->set(0);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        float* destinationPtr = (float*)destination.data;
        for (size_t columnId = 0; columnId < imageWidth; columnId++)
        {
            size_t dstPixelId = (startId + columnId) * dstComponents;

            for (uint32_t componentId = 0; componentId < _componentCount; componentId++)
            {
                destinationPtr[dstPixelId + componentId] = _color[componentId];
            }
        }
    }

    Vector4fProperty*          colorProperty() { return  _colorProperty; }
    IntProperty*               widthProperty() { return  _widthProperty; }
    IntProperty *              heightProperty() { return _heightProperty; }

  private:
    Vector4fProperty*          _colorProperty;
    IntProperty*               _widthProperty;
    IntProperty *              _heightProperty;
    uint32_t                   _componentCount;
    mutable Vector4f           _color;
};

// Assemble Voltron.
class MergeComponents : public ImageFunctionProcessor
{
public:
    MergeComponents(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;


        const std::vector<std::string>& srcComponentNames = _srcComponentsProperty->get();
        _componentCount = uint32_t(srcComponentNames.size());
        for (uint32_t componentId = 0; componentId < _componentCount; componentId++)
            _srcComponentIds[componentId] = componentNameToId(srcComponentNames[componentId]);
    }

    void                       setup()
    {
        _srcComponentsProperty = new StringArrayProperty(_processingNode, "SrcComponentsProperty");
        _processingProperty->addDependency(_srcComponentsProperty, 0);

        std::vector<std::string> defaultSrcDstComponentNames;
        defaultSrcDstComponentNames.push_back("R");
        defaultSrcDstComponentNames.push_back("R");
        defaultSrcDstComponentNames.push_back("R");
        defaultSrcDstComponentNames.push_back("R");
        _srcComponentsProperty->set(defaultSrcDstComponentNames);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);

        float* destinationPtr = (float*)destination.data;
        for (size_t columnId = 0; columnId < imageWidth; columnId++)
        {
            size_t dstPixelId = (startId + columnId) * dstComponents;

            for (uint32_t componentId = 0; componentId < _componentCount; componentId++)
            {
                size_t srcComponents = PixelUtil::getComponentCount(sources[componentId].format);
                size_t srcComponentOffset = _srcComponentIds[componentId];
                size_t srcAPixelId = (startId + columnId) * srcComponents;

                float sourcePixel = 
                    ((float*)sources[componentId].data)[srcAPixelId + srcComponentOffset];
                destinationPtr[dstPixelId + componentId] = sourcePixel;

            }
        }
    }

    StringArrayProperty*       srcComponentsProperty() { return _srcComponentsProperty; }

  private:
    // 1 image per channel.
    StringArrayProperty*       _srcComponentsProperty;
    mutable uint32_t           _srcComponentIds[4];
    mutable uint32_t           _componentCount;
};

// An operator with 3 inputs,
// Ouput is D = lerp(A, B, C);
class ImageLerp : public ImageFunctionProcessor
{
  public:
    ImageLerp (RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;

        const std::vector<std::string>& srcComponentNames = _srcComponentsProperty->get();

        _componentCount = srcComponentNames.size();
        for (uint32_t componentId = 0; componentId < _componentCount; componentId++)
            _srcComponentIds[componentId] = componentNameToId(srcComponentNames[componentId]);

        _lerpComponentId = componentNameToId(_lerpSourceComponentProperty->get());
        _lerpAlpha = _lerpAlphaProperty->get();

        _useConstantLerp = _useConstantLerpProperty->get();
        _constantLerp = _constantLerpProperty->get();
    }

    void                       setup()
    {
        _lerpSourceComponentProperty = new StringProperty(_processingNode, "LerpSourceComponent");
        _srcComponentsProperty = new StringArrayProperty(_processingNode, "SrcComponentsProperty");
        _lerpAlphaProperty = new BoolProperty(_processingNode, "LerpAlpha");
        _useConstantLerpProperty = new BoolProperty(_processingNode, "UseLerp");
        _constantLerpProperty = new FloatProperty(_processingNode, "ConstantLerp");

        _processingProperty->addDependency(_lerpSourceComponentProperty, 0);
        _processingProperty->addDependency(_srcComponentsProperty, 0);
        _processingProperty->addDependency(_lerpAlphaProperty, 0);
        _processingProperty->addDependency(_useConstantLerpProperty, 0);
        _processingProperty->addDependency(_constantLerpProperty, 0);

        std::vector<std::string> defaultSrcDstComponentNames;
        defaultSrcDstComponentNames.push_back("R");
        defaultSrcDstComponentNames.push_back("G");
        defaultSrcDstComponentNames.push_back("B");
        defaultSrcDstComponentNames.push_back("A");
        _srcComponentsProperty->set(defaultSrcDstComponentNames);
        _lerpAlphaProperty->set(false);
        _constantLerpProperty->set(1.0f);
        _useConstantLerpProperty->set(false);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);

        size_t srcComponentsA = PixelUtil::getComponentCount(sources[0].format);
        size_t srcComponentsB = PixelUtil::getComponentCount(sources[1].format);
        size_t srcComponentsL = 0;
        if (!_useConstantLerp)
            srcComponentsL = PixelUtil::getComponentCount(sources[2].format);

        size_t dstComponents = PixelUtil::getComponentCount(destination.format);

        float* destinationPtr = (float*)destination.data;
        float alpha = _constantLerp;
        for (size_t columnId = 0; columnId < imageWidth; columnId++)
        {
            size_t srcAPixelId = (startId + columnId) * srcComponentsA;
            size_t srcBPixelId = (startId + columnId) * srcComponentsB;
            size_t srcLPixelId = (startId + columnId) * srcComponentsL;
            size_t dstPixelId = (startId + columnId) * dstComponents;

            if (!_useConstantLerp)
                alpha = ((float*)sources[2].data)[srcLPixelId + _lerpComponentId];

            for (uint32_t componentId = 0; componentId < _componentCount; componentId++)
            {
                size_t srcComponentOffset = _srcComponentIds[componentId];

                float sourceAPixel = ((float*)sources[0].data)[srcAPixelId + srcComponentOffset];
                float sourceBPixel = ((float*)sources[1].data)[srcBPixelId + srcComponentOffset];

                if (srcComponentOffset == _lerpComponentId && !_lerpAlpha)
                    destinationPtr[dstPixelId + componentId] = sourceAPixel;
                else
                    destinationPtr[dstPixelId + componentId] =
                        Ctr::lerp(sourceAPixel, sourceBPixel, alpha);
            }
        }
    }

  private:
    StringProperty*            _lerpSourceComponentProperty;
    StringArrayProperty*       _srcComponentsProperty;
    BoolProperty *             _lerpAlphaProperty;
    FloatProperty *            _constantLerpProperty;
    BoolProperty *             _useConstantLerpProperty;

    // Src to Dst Index.
    mutable uint32_t           _lerpComponentId;
    mutable uint32_t           _srcComponentIds[4];
    mutable size_t             _componentCount;
    mutable bool               _lerpAlpha;
    mutable bool               _useConstantLerp;
    mutable float              _constantLerp;
};

class ConvertRoughnessImage : public ImageFunctionProcessor
{
  public:
      ConvertRoughnessImage(RenderNode* n, Property* p) :
          ImageFunctionProcessor(n, p),
        _srcIsGloss(false),
        _componentCount(1)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;

        const std::string& glossComponentName = _glossComponentProperty->get();
        _componentCount = 1;
        _srcIsGloss = _glossToRoughnessProperty->get();
        _glossComponentId = componentNameToId(glossComponentName);
    }

    void                       setup()
    {
        _glossComponentProperty = new StringProperty(_processingNode, "SrcComponentsProperty");
        _glossToRoughnessProperty = new BoolProperty(_processingNode, "SrcIsGloss");
        _glossToRoughnessProperty->set(true);
        _processingProperty->addDependency(_glossToRoughnessProperty, 0);
        _processingProperty->addDependency(_glossComponentProperty, 0);

        std::string defaultGlossComponentName = "R";
        _glossComponentProperty->set(defaultGlossComponentName);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        float* destinationPtr = (float*)destination.data;

        if (_srcIsGloss)
        {
            for (size_t columnId = 0; columnId < imageWidth; columnId++)
            {
                size_t dstPixelId = (startId + columnId) * dstComponents;
                uint32_t glossComponentId = 0;
                {
                    size_t srcComponents = PixelUtil::getComponentCount(sources[0].format);
                    size_t srcComponentOffset = glossComponentId;
                    size_t srcAPixelId = (startId + columnId) * srcComponents;

                    float sourcePixel =
                        ((float*)sources[0].data)[srcAPixelId + srcComponentOffset];
                    destinationPtr[dstPixelId] = 1.0f-sourcePixel;
                }
            }
        }
        else
        {
            for (size_t columnId = 0; columnId < imageWidth; columnId++)
            {
                size_t dstPixelId = (startId + columnId) * dstComponents;
                uint32_t glossComponentId = 0;
                {
                    size_t srcComponents = PixelUtil::getComponentCount(sources[0].format);
                    size_t srcComponentOffset = glossComponentId;
                    size_t srcAPixelId = (startId + columnId) * srcComponents;

                    float sourcePixel =
                        ((float*)sources[0].data)[srcAPixelId + srcComponentOffset];
                    destinationPtr[dstPixelId] = sourcePixel;
                }
            }
        }
    }

    BoolProperty*              glossToRoughnessProperty() { return _glossToRoughnessProperty; }

  private:
    // 1 image per channel.
    StringProperty*            _glossComponentProperty;
    BoolProperty*              _glossToRoughnessProperty;
    mutable uint32_t           _glossComponentId;
    mutable uint32_t           _componentCount;
    mutable bool               _srcIsGloss;

};

class ScaleImage : public ImageFunctionProcessor
{
public:
    ScaleImage(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p),
        _componentCount(4)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;
        
        IBLASSERT((bool)(sources.size() > 0), "No sources for opertation!");
        PixelUtil::getComponentCount(sources[0].format);
        _rescaleRanges = _rescaleRangesProperty->get();
    }

    void                       setup()
    {
        _rescaleRangesProperty = new Vector4fProperty(_processingNode, "MetalnessMask");
        _rescaleRangesProperty->set(Ctr::Vector4f(0, 1, 0.5, 1));
        _processingProperty->addDependency(_rescaleRangesProperty, 0);
    }

    inline float               rescaleComponent(float src, float min, float max, float center, float multiplier) const
    {
        //return src;
        return saturate(((src - min) / (max - min)) * multiplier);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        float* destinationPtr = (float*)destination.data;
        float* sourcePtr = (float*)sources[0].data;
        size_t srcComponents = PixelUtil::getComponentCount(sources[0].format);
        Vector4f rescaleRanges = _rescaleRanges;

        {
            for (size_t columnId = 0; columnId < imageWidth; columnId++)
            {
                size_t dstPixelId = (startId + columnId) * dstComponents;

                for (size_t componentId = 0; componentId < srcComponents; componentId++)
                {
                    size_t srcPixelId = (startId + columnId) * srcComponents;
                    float sourcePixel = sourcePtr[srcPixelId + componentId];

                    destinationPtr[dstPixelId+componentId] =
                        rescaleComponent(sourcePixel, rescaleRanges.x, rescaleRanges.y, rescaleRanges.z, rescaleRanges.w);
                }
            }
        }
    }

    Vector4fProperty*          rescaleRangesProperty() { return _rescaleRangesProperty; }

  private:
    // 1 image per channel.
    Vector4fProperty*          _rescaleRangesProperty;
    mutable Vector4f           _rescaleRanges;
    mutable uint32_t           _componentCount;
};

class ExtractMetalnessImage : public ImageFunctionProcessor
{
public:
    ExtractMetalnessImage(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p),
        _componentCount(1)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;

        _metalnessMask = _metalnessMaskProperty->get();
    }

    void                       setup()
    {
        _metalnessMaskProperty = new Vector4fProperty(_processingNode, "MetalnessMask");
        _metalnessMaskProperty->set(Ctr::Vector4f(1,1,1,0));
        _processingProperty->addDependency(_metalnessMaskProperty, 0);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        float* destinationPtr = (float*)destination.data;

        size_t srcComponents = PixelUtil::getComponentCount(sources[0].format);

        {
            for (size_t columnId = 0; columnId < imageWidth; columnId++)
            {
                size_t dstPixelId = (startId + columnId) * dstComponents;

                float metalness = 0;
                for (size_t srcComponentId = 0; srcComponentId < srcComponents; srcComponentId++)
                {
                    size_t srcPixelId = (startId + columnId) * srcComponents;

                    metalness += _metalnessMask[srcComponentId] *
                        ((float*)sources[0].data)[srcPixelId + srcComponentId];

                }
                destinationPtr[dstPixelId] = Ctr::saturate(metalness);
            }
        }
    }

    Vector4fProperty*          metalnessMaskProperty() { return _metalnessMaskProperty; }

  private:
    // 1 image per channel.
    Vector4fProperty*          _metalnessMaskProperty;
    mutable Vector4f           _metalnessMask;
    uint32_t                   _componentCount;
};

class ConvertNormalImage : public ImageFunctionProcessor
{
public:
    ConvertNormalImage(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p),
        _componentCount(4),
        _normalizeMips(true),
        _generateToksvig(false)
    {
    }

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;

        _inversionMask = _inversionMaskProperty->get();
        _swizzleRG = _swizzleRGProperty->get();
        _generateToksvig = _generateToksvigProperty->get();
        _normalizeMips = _normalizeMipsProperty->get();
    }

    void                       setup()
    {
        _swizzleRGProperty = new BoolProperty(_processingNode, "swizzleRG");
        _swizzleRGProperty->set(false);

        _inversionMaskProperty = new Vector4fProperty(_processingNode, "InversionMask");
        _inversionMaskProperty->set(Ctr::Vector4f(0, 0, 0, 0));

        _generateToksvigProperty = new BoolProperty(_processingNode, "generateToksvig");
        _generateToksvigProperty->set(_generateToksvig);

        _normalizeMipsProperty = new BoolProperty(_processingNode, "normalizeMips");
        _normalizeMipsProperty->set(_normalizeMips);

        _processingProperty->addDependency(_inversionMaskProperty, 0);
        _processingProperty->addDependency(_swizzleRGProperty, 0);
        _processingProperty->addDependency(_normalizeMipsProperty, 0);
        _processingProperty->addDependency(_generateToksvigProperty, 0);
    }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);
        float* destinationPtr = (float*)destination.data;
        float* sourcePtr = (float*)sources[0].data;
        size_t srcComponents = PixelUtil::getComponentCount(sources[0].format);

        {
            for (size_t columnId = 0; columnId < imageWidth; columnId++)
            {
                size_t dstPixelId = (startId + columnId) * dstComponents;
                size_t srcPixelId = (startId + columnId) * srcComponents;

                Ctr::Vector4f normal;
                memcpy(&normal.x, &sourcePtr[srcPixelId], sizeof(float) * 4);
                
                if (_swizzleRG)
                {
                    float tmp = normal.y;
                    normal.y = normal.x;
                    normal.x = tmp;
                }
                for (uint32_t channelId = 0; channelId < 4; channelId++)
                    normal[channelId] = lerp(normal[channelId], 1.0f - normal[channelId], _inversionMask[channelId]);

                memcpy(&destinationPtr[dstPixelId], &normal, sizeof(float) * 4);
            }
        }
    }

    virtual void refilterMip(Ctr::TextureImagePtr& mipImage) const
    {
        if (mipImage->getFormat() == PF_A8R8G8B8)
        {
            if (_normalizeMips)
            {
                Ctr::PixelBox mipPixelBox = mipImage->getPixelBox(0, 0);
                size_t mipWidth = mipImage->getWidth();
                size_t mipHeight = mipImage->getHeight();
                uint8_t* mipPixels = (uint8_t*)mipPixelBox.data;
                concurrency::parallel_for(size_t(0), size_t(mipHeight), [&](size_t rowId)
                {
                    for (size_t columnId = 0; columnId < mipWidth; columnId++)
                    {
                        size_t startId = (rowId * mipWidth);
                        size_t pixelId = (startId + columnId) * 4;
                        Vector3f normal(mipPixels[pixelId] / 255.0f, mipPixels[pixelId+1] / 255.0f, mipPixels[pixelId+2] / 255.0f);
                        normal.expandUnit();
                        normal.normalize();
                        normal.compressUnit();

                        for (uint32_t componentId = 0; componentId < 3; componentId++)
                            mipPixels[pixelId + componentId] = uint8_t(normal[componentId] * 255.0f);
                    }
                });
            }
        }
    }

    Vector4fProperty*          inversionMaskProperty() { return _inversionMaskProperty; }
    BoolProperty*              swizzleRGProperty() { return _swizzleRGProperty; }
    BoolProperty*              normalizeMipsProperty() { return _normalizeMipsProperty; }
    BoolProperty*              generateToksvigProperty() { return _generateToksvigProperty; }

  private:
    // 1 image per channel.
    BoolProperty*              _swizzleRGProperty;
    mutable bool               _swizzleRG;

    Vector4fProperty*          _inversionMaskProperty;
    mutable Vector4f           _inversionMask;
    uint32_t                   _componentCount;

    BoolProperty*              _normalizeMipsProperty;
    mutable bool               _normalizeMips;

    BoolProperty*              _generateToksvigProperty;
    mutable bool               _generateToksvig;
};

class ComputeAlbedoImage : public ImageFunctionProcessor
{
public:
    ComputeAlbedoImage(RenderNode* n, Property* p) :
        ImageFunctionProcessor(n, p),
        _componentCount(4)
    {
        // Inputs are Specular, Metalness, Albedo.
    }

    enum ImageInputs
    {
        AlbedoSource = 0,
        SpecularSource = 1,
        MetalnessSource = 2
    };

    size_t                     componentCount() const
    {
        return _componentCount;
    }

    void                       cacheProcessingOptions(const std::vector<Ctr::PixelBox>& sources) const
    {
        // TODO: Sanitize inputs.
        _imageWidth = sources[0].size().x;
        _imageHeight = sources[0].size().y;
    }

    void                       setup() { }

    void                       operator()(size_t rowId,
                                          size_t imageWidth,
                                          size_t imageHieght,
                                          const std::vector<Ctr::PixelBox>& sources,
                                          Ctr::PixelBox& destination) const
    {
        // Processing always assumed to be floats (cuts down on type combination explosion).
        size_t startId = (rowId * imageWidth);
        size_t dstComponents = PixelUtil::getComponentCount(destination.format);

        size_t albedoComponents = PixelUtil::getComponentCount(sources[AlbedoSource].format);
        size_t specularComponents = PixelUtil::getComponentCount(sources[SpecularSource].format);
        size_t metalComponents = PixelUtil::getComponentCount(sources[MetalnessSource].format);

        float* destinationPtr = (float*)destination.data;
        float* albedoPtr = (float*)sources[AlbedoSource].data;
        float* metalPtr = (float*)sources[MetalnessSource].data;
        float* specularPtr = (float*)sources[SpecularSource].data;

        Ctr::Vector4f albedo;
        for (size_t columnId = 0; columnId < imageWidth; columnId++)
        {
            size_t dstPixelId = (startId + columnId) * dstComponents;
            size_t srcAlbedoPixelId = (startId + columnId) * albedoComponents;
            size_t srcMetalPixelId = (startId + columnId) * metalComponents;
            size_t srcSpecularPixelId = (startId + columnId) * specularComponents;
            
            const float& metalness = metalPtr[srcMetalPixelId];
            const Ctr::Vector3f& specular = *((Ctr::Vector3f*)&specularPtr[srcSpecularPixelId]);

            memcpy(&albedo.x, &albedoPtr[srcAlbedoPixelId], sizeof(Ctr::Vector4f));
        
            albedo.x = (albedo.x * (1.0f - metalness)) + (specular.x * metalness);
            albedo.y = (albedo.y * (1.0f - metalness)) + (specular.y * metalness);
            albedo.z = (albedo.z * (1.0f - metalness)) + (specular.z * metalness);
        
            memcpy(&destinationPtr[dstPixelId], &albedo.x, sizeof(Ctr::Vector4f));
        }
    }

  private:
    uint32_t                   _componentCount;
};

template <typename ImageFunctionT>
class ImageProcessorFunction : public ImageFunctionT
{
  public:
    ImageProcessorFunction(RenderNode* n) : 
        ImageFunctionT(n, this)
    {
        using std::placeholders::_1;
        addTask(std::make_pair(_imageResultProperty,
            std::bind(&ImageProcessorFunction<typename ImageFunctionT>::computeImage, this, _1)));
        addTask(std::make_pair(_textureResultProperty,
            std::bind(&ImageFunction::computeTexture, this, _1)));
        addTask(std::make_pair(_convertedRGBAImageProperty,
            std::bind(&ImageFunction::computeRGBAImage, this, _1)));

        ImageFunctionT::setup();
    }


    void computeImage(const Property* property) const
    {

        std::vector<Ctr::TextureImagePtr> sourceImages;
        std::vector<Ctr::PixelBox> sources;
        // Processor nodes are always assumed to be float.
        std::vector<const float*> sourcePtrs;
        for (uint32_t sourceId = 0; sourceId < 5; sourceId++)
        {
            const TextureImageProperty* sourceProperty = 
                imageDependency(sourceId);

            if (!sourceProperty)
            {
                // IBLASSERT(0, "Invalid source for input image " << sourceId);
                // Todo ... make less shit.
                sources.push_back(Ctr::PixelBox());
            }
            else
            {
                Ctr::TextureImagePtr sourceImage = sourceProperty->get();
                // TODO: Mips and faces later.
                size_t faceId = 0;
                size_t mipId = 0;
                Ctr::PixelBox sourcePixelBox = sourceImage->getPixelBox(faceId, mipId);
                sources.push_back(sourcePixelBox);
            }
        }

        cacheProcessingOptions(sources);
        size_t _imageWidth = imageWidth();
        size_t _imageHeight = imageHeight();

        PixelFormat format;
        switch (componentCount())
        {
            case 1:
                format = PF_FLOAT32_R;
                break;
            case 2:
                format = PF_FLOAT32_GR;
                break;
            case 3:
                format = PF_FLOAT32_RGB;
                break;
            case 4:
                format = PF_FLOAT32_RGBA;
                break;
            default:
                IBLASSERT(0, "Unknown channel count");
        }

        // Create the result image.
        Ctr::TextureImagePtr destinationImage(new Ctr::TextureImage());
        destinationImage->create(Ctr::Vector2i(int32_t(_imageWidth), int32_t(_imageHeight)),
                             format,
                             (uint32_t)(0) /* no mips*/,
                             IF_DEFAULT);
        Ctr::PixelBox destinationPixelBox = destinationImage->getPixelBox(0,0);

        concurrency::parallel_for(size_t(0), size_t(_imageHeight), [&](size_t rowId)
        {
            (*this)(rowId, _imageWidth, _imageHeight, sources, destinationPixelBox);
        });

        _imageResultProperty->set(destinationImage);
    }

  protected:

};

template <typename Function>
class ImageFunctionPropertyT : public Function
{
  public:
    ImageFunctionPropertyT(RenderNode* node, const std::string& name) :
        Function(node)
    {
    }

    virtual ~ImageFunctionPropertyT()
    {
    }
};

template <typename Function>
class ImageFunctionNode : public Ctr::RenderNode
{
  public:
    typedef ImageFunctionPropertyT<typename Function> ImageFunctionProperty;

    ImageFunctionNode(Ctr::IDevice* device) : 
        Ctr::RenderNode(device),
        _imageFunctionProperty(nullptr)
    {
        _imageFunctionProperty = new ImageFunctionProperty(this, std::string("imageFunction"));
        _interpretPixelsAsProperty = new IntProperty(this, std::string("interpretAs"));
        _gammaDisplayProperty = new FloatProperty(this, std::string("gammaDisplay"));
        _gammaInProperty = new FloatProperty(this, std::string("gammaIn"));
        _sizeProperty = new Vector2iProperty(this, std::string("commonSize"));
        _generateMipMapsProperty = new BoolProperty(this, std::string("generateMipMaps"));
        _generateMipMapsProperty->set(false);

        _imageFunctionProperty->addDependency(_sizeProperty, 0);
        _imageFunctionProperty->addDependency(_generateMipMapsProperty, 0);
        _imageFunctionProperty->addDependency(_gammaInProperty, 0);
        _imageFunctionProperty->addDependency(_gammaDisplayProperty, 0);

        _gammaInProperty->set(1.0f);
        _gammaDisplayProperty->set(1.0f);
        _interpretPixelsAsProperty->set(0);
    }

    virtual ~ImageFunctionNode() {}

    void      setImageDependency(uint32_t id,
                                 TextureImageProperty* imageResult)
    {
        if (imageResult)
            _imageFunctionProperty->setImageDependency(id, imageResult);
        else
            _imageFunctionProperty->setImageDependency(id, nullptr);

        _imageDependencyNodes[id] = imageResult;

    }

    IntProperty*               interpretPixelsAsProperty()
    {
        return _interpretPixelsAsProperty;
    }

    TextureImageProperty*      imageResultProperty() const 
    { 
        return _imageFunctionProperty->imageResultProperty();
    }

    TextureProperty*           textureResultProperty() const
    {
        return _imageFunctionProperty->textureResultProperty();
    }
    
    ImageFunctionProperty *    imageFunctionProperty() { return _imageFunctionProperty; }

  protected:
    std::map<uint32_t, Ctr::TextureImageProperty*> _imageDependencyNodes;
    ImageFunctionProperty *    _imageFunctionProperty;
    IntProperty*               _interpretPixelsAsProperty;
    FloatProperty*             _gammaInProperty;
    FloatProperty*             _gammaDisplayProperty;
    Vector2iProperty*          _sizeProperty;
    BoolProperty*              _generateMipMapsProperty;
};

typedef ImageFunctionNode<ImageFileSourceFunction> ImageFileSourceNode;
typedef ImageFunctionNode<ImageProcessorFunction<ImageSolidColor> > ImageSolidColorNode;
typedef ImageFunctionNode<ImageProcessorFunction<ImageLerp> > ImageLerpNode;
typedef ImageFunctionNode<ImageProcessorFunction<ExtractMetalnessImage> > ExtractMetalnessImageNode;
typedef ImageFunctionNode<ImageProcessorFunction<ConvertRoughnessImage> > RoughnessImageNode;
typedef ImageFunctionNode<ImageProcessorFunction<MergeComponents> > ImageMergeNode;
typedef ImageFunctionNode<ImageProcessorFunction<ComputeAlbedoImage> > ComputeAlbedoImageNode;
typedef ImageFunctionNode<ImageProcessorFunction<ConvertNormalImage> > ConvertTangentNormalNode;
typedef ImageFunctionNode<ImageProcessorFunction<ScaleImage> > ScaleImageNode;

}

#endif