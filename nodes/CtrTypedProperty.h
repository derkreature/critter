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

#ifndef INCLUDED_TYPED_PROPERTY
#define INCLUDED_TYPED_PROPERTY

#include <CtrProperty.h>
#include <CtrPlatform.h>
#include <CtrTypedProperty.h>
#include <CtrLog.h>
#include <CtrVector3.h>
#include <CtrQuaternion.h>
#include <CtrMatrix44.h>
#include <CtrRegion.h>
#include <CtrVertexStream.h>
#include <CtrViewport.h>
#include <CtrIDevice.h>
#include <CtrTextureImage.h>
#include <memory>

namespace Ctr
{
class ITexture;
class IDepthSurface;
class ISurface;
class IRenderResource;

template <typename T>
class TypedProperty : public Property
{
  public:
    TypedProperty(Node* node, const std::string& name, Node* group = nullptr);
    TypedProperty(Node* node, const std::string& name, TweakFlags* flags);
    virtual ~TypedProperty();

    void                       removeDependency(Property* p);
    void                       addDependency(Property* p);

    virtual T&                  get();
    virtual const T&            get() const;
    virtual void                set (const T& value);

  protected:
    // Not at all happy about this.
    mutable T                   _value;

  private:
    TypedProperty<T> *          _dependency;
};

template <typename T>
TypedProperty<T>::TypedProperty (Node* node, 
                                 const std::string& name,
                                 Node* group) :
    Property (node, name, group),
    _dependency(nullptr)
{
}

template <typename T>
TypedProperty<T>::TypedProperty(Node* node,
                                const std::string& name, 
                                TweakFlags* flags) :
                                Property(node, name, flags),
    _dependency(nullptr)
{
}

template <typename T>
TypedProperty<T>::~TypedProperty()
{
}

template <typename T>
T&
TypedProperty<T>::get()
{
    if (_dependency)
    {
        _value = _dependency->get();
        _cached = true;
        return _value;
    }
    if (_group && !_cached)
    {
        _group->cache(this);
    }

    return _value;
}

template <typename T>
const T&
TypedProperty<T>::get() const
{
    if (_dependency)
    {
        _value = _dependency->get();
        _cached = true;
        return _value;
    }
    else if (_group && !_cached)
    {
        _group->cache(this);
    }
    return _value;
}

template <typename T>
void
TypedProperty<T>::set (const T& value)
{
    if (value == _value)
    { 
        _cached = true;
        return;
    }
    else
    { 
        _value = value;
        {
            uncache();
        }
        _cached = true;
    }
}

template <typename T>
void
TypedProperty<T>::removeDependency(Property* p)
{
    _dependency = nullptr;
    Property::removeDependency(p, 0);
    _value = T(0);
    uncache();
}

template <typename T>
void
TypedProperty<T>::addDependency(Property* p)
{
    _dependency = dynamic_cast<TypedProperty<T>*>(p);
    Property::addDependency(p, 0);
    uncache();
}

typedef TypedProperty <std::string>                  StringProperty;
typedef TypedProperty <bool>                         BoolProperty;
typedef TypedProperty <float>                        FloatProperty;
typedef TypedProperty <int32_t>                      IntProperty;
typedef TypedProperty <uint32_t>                     UIntProperty;
typedef TypedProperty <ITexture*>                    TextureProperty;
typedef TypedProperty <Ctr::Matrix44f>               MatrixProperty;
typedef TypedProperty <Ctr::Quaternionf>             QuaternionProperty;
typedef TypedProperty <Ctr::Vector2f>                Vector2fProperty;
typedef TypedProperty <Ctr::Vector2i>                Vector2iProperty;
typedef TypedProperty <Ctr::Vector3f>                VectorProperty;
typedef TypedProperty <Ctr::Vector4f>                Vector4fProperty;
typedef TypedProperty <Ctr::PixelFormat>             PixelFormatProperty;
typedef TypedProperty <Ctr::IDevice*>                DeviceProperty;
typedef TypedProperty <float*>                       FloatPtrProperty;
typedef TypedProperty <uint32_t*>                    UIntPtrProperty;
typedef TypedProperty <Region3f>                     BoundingBoxTypedProperty;
typedef TypedProperty <Ctr::VertexStream*>           VertexStreamProperty;
typedef TypedProperty <std::vector<float> >         FloatArrayProperty;
typedef TypedProperty <Ctr::IDepthSurface*>          DepthSurfaceTypedProperty;
typedef TypedProperty <std::shared_ptr<ITexture> >   RenderTextureTypedProperty;
typedef TypedProperty <std::shared_ptr<ITexture> >   Texture2DTextureTypedProperty;
typedef TypedProperty <std::shared_ptr<ITexture> >   CubeTextureTypedProperty;
typedef TypedProperty <Ctr::Viewport>                ViewportTypedProperty;
typedef TypedProperty <std::vector<Ctr::Matrix44f> > MatrixArrayProperty;
typedef TypedProperty <std::vector<float> >          FloatArrayProperty;
typedef TypedProperty <std::vector<Ctr::Vector4f>>   Vector4fArrayProperty;
typedef TypedProperty <std::vector<std::string>>     StringArrayProperty;
typedef TypedProperty <Ctr::TextureImagePtr>	     TextureImageProperty;
typedef TypedProperty <Ctr::Hash>                    HashProperty;

}

#endif