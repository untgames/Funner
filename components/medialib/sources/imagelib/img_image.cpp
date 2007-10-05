#include "shared.h"

using namespace common;
using namespace media;

/*
    Конструкторы / деструктор
*/

Image::Image ()
  : impl (create_bitmap_image ())
{
}

Image::Image (const Image& source, PixelFormat format)
  : impl (source.impl->Clone ())
{
  Convert (format);
}

Image::Image (const char* file_name, PixelFormat format)
{  
  if (!file_name)
    RaiseNullArgument ("media::Image::Image", "file_name");

  try
  {
    ImageSystemSingleton::Instance ().Load (file_name, *this);    

    Rename  (file_name);
    Convert (format);
  }
  catch (Exception& exception)
  {
    exception.Touch ("media::Image::Image");
    throw;
  }
}

Image::Image (size_t layers_count, Image* layers, LayersCloneMode clone_mode)
{
  if (!layers_count)
    RaiseNullArgument ("media::Image::Image", "layers_count");
    
  if (!layers)
    RaiseNullArgument ("media::Image::Image", "layers");
    
  switch (clone_mode)
  {
    case LayersCloneMode_Copy:
    case LayersCloneMode_Capture:
      break;
    default:
      RaiseInvalidArgument ("media::Image::Image", "clone_mode", clone_mode);
      break;
  }

  impl = create_multilayer_image (layers_count, layers, clone_mode);
}

Image::Image (size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
  : impl (create_bitmap_image (width, height, depth, format, data))
{
}

Image::~Image ()
{
}

/*
    Присваивание
*/

Image& Image::operator = (const Image& source)
{
  if (this != &source)
    Image (source).Swap (*this);

  return *this; 
}

/*
    Имя картинки
*/

const char* Image::Name () const
{
  return impl->Name ();
}

void Image::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("media::Image::Rename", "new_name");

  impl->Rename (new_name);
}

/*
    Формат картинки
*/

PixelFormat Image::Format () const
{
  return impl->Format ();
}

void Image::Convert (PixelFormat new_format)
{
  if (new_format == impl->Format () || new_format == PixelFormat_Default)
    return;

  if (new_format < 0 || new_format >= PixelFormat_Num)
    RaiseInvalidArgument ("media::Image::Convert", "new_format", new_format);

  impl->Convert (new_format);
}

size_t Image::Width () const
{
  return impl->Width ();
}

size_t Image::Height () const
{
  return impl->Height ();
}

size_t Image::Depth () const
{
  return impl->Depth ();
}

void Image::Resize (size_t width, size_t height, size_t depth)
{
  if (width == impl->Width () && height == impl->Height () && depth == impl->Depth ())
    return;

  if (!width)  RaiseInvalidArgument ("media::Image::Resize", "width", width);
  if (!height) RaiseInvalidArgument ("media::Image::Resize", "height", height);
  if (!depth)  RaiseInvalidArgument ("media::Image::Resize", "depth", depth);

  impl->Resize (width, height, depth);
}

/*
    Работа с образом
*/

const void* Image::Bitmap (size_t z) const
{
  if (z >= impl->Depth ())
    RaiseOutOfRange ("media::Image::Bitmap", "z", z, impl->Depth ());

  return impl->Bitmap (z);
}

void* Image::Bitmap (size_t z)
{
  return const_cast<void*> (const_cast<const Image&> (*this).Bitmap (z));
}

void Image::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  if (!data)
    RaiseNullArgument ("media::Image::PutImage", "data");

  impl->PutImage (x, y, z, width, height, depth, format, data);
}

void Image::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data) const
{
  if (!data)
    RaiseNullArgument ("media::Image::GetImage", "data");

  impl->GetImage (x, y, z, width, height, depth, format, data);
}

/*
    Загрузка / сохранение
*/

void Image::Load (const char* file_name)
{
  Image (file_name).Swap (*this);
}

void Image::Save (const char* file_name, PixelFormat recommended_format)
{
  if (!file_name)
    RaiseNullArgument ("media::Image::Save", "file_name");
    
  if (recommended_format < 0 || recommended_format >= PixelFormat_Num)
    RaiseInvalidArgument ("media::Image::Save", "recommended_format", recommended_format);

  try
  {
    if (recommended_format != PixelFormat_Default && recommended_format != Format ())
    {
      ImageSystemSingleton::Instance ().Save (file_name, Image (*this, recommended_format));
    }
    else
    {
      ImageSystemSingleton::Instance ().Save (file_name, *this);
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::Image::Save");
    throw;
  }
}

void Image::DefaultLoader (const char* file_name, Image& image)
{
  image.impl = create_bitmap_image (file_name);
}

void Image::DefaultSaver (const char* file_name, const Image& image)
{
  image.impl->Save (file_name);
}

/*
    Обмен
*/

void Image::Swap (Image& image)
{
  swap (impl, image.impl);
}

namespace media
{

void swap (Image& image1,Image& image2)
{
  image1.Swap (image2);
}

}

/*
    ImageImpl
*/

ImageImpl::ImageImpl ()
{
}

ImageImpl::ImageImpl (const ImageImpl& impl)
  : name (impl.name)
{
}

const char* ImageImpl::Name ()
{
  return name.c_str ();
}

void ImageImpl::Rename (const char* new_name)
{
  name = new_name;
}
