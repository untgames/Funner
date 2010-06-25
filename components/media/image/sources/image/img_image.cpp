#include "shared.h"

using namespace stl;
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
    throw xtl::make_null_argument_exception ("media::Image::Image", "file_name");

  try
  {
    static ComponentLoader loader ("media.image.loaders.*");

    ImageManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
    
    if (!impl.get ())
      throw xtl::format_operation_exception ("media::Image::Image", "Error at load image '%s' (format=%s)", file_name, get_format_name (format));

    Rename  (file_name);
    Convert (format);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Image::Image");
    throw;
  }
}

Image::Image (size_t layers_count, Image* layers, LayersCloneMode clone_mode)
{
  if (!layers_count)
    throw xtl::make_null_argument_exception ("media::Image::Image", "layers_count");
    
  if (!layers)
    throw xtl::make_null_argument_exception ("media::Image::Image", "layers");
    
  switch (clone_mode)
  {
    case LayersCloneMode_Copy:
    case LayersCloneMode_Capture:
      break;
    default:
      throw xtl::make_argument_exception ("media::Image::Image", "clone_mode", clone_mode);
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
    throw xtl::make_null_argument_exception ("media::Image::Rename", "new_name");

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
    throw xtl::make_argument_exception ("media::Image::Convert", "new_format", new_format);

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

  if (!width)  throw xtl::make_argument_exception ("media::Image::Resize", "width", width);
  if (!height) throw xtl::make_argument_exception ("media::Image::Resize", "height", height);
  if (!depth)  throw xtl::make_argument_exception ("media::Image::Resize", "depth", depth);

  impl->Resize (width, height, depth);
}

/*
    Работа с образом
*/

const void* Image::Bitmap (size_t z) const
{
  if (z >= impl->Depth ())
    throw xtl::make_range_exception ("media::Image::Bitmap", "z", z, impl->Depth ());

  return impl->Bitmap (z);
}

void* Image::Bitmap (size_t z)
{
  return const_cast<void*> (const_cast<const Image&> (*this).Bitmap (z));
}

void Image::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  if (!data)
    throw xtl::make_null_argument_exception ("media::Image::PutImage", "data");

  impl->PutImage (x, y, z, width, height, depth, format, data);
}

void Image::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data) const
{
  if (!data)
    throw xtl::make_null_argument_exception ("media::Image::GetImage", "data");

  impl->GetImage (x, y, z, width, height, depth, format, data);
}

/*
    Загрузка / сохранение
*/

void Image::Load (const char* file_name)
{
  try
  {
    Image (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Image::Load");
    throw;
  }
}

void Image::Save (const char* file_name, PixelFormat recommended_format, const char* format_specific_flags)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::Image::Save", "file_name");
    
  if (!format_specific_flags)
    throw xtl::make_null_argument_exception ("media::Image::Save", "format_specific_flags");

  if (recommended_format < 0 || recommended_format >= PixelFormat_Num)
    throw xtl::make_argument_exception ("media::Image::Save", "recommended_format", recommended_format);

  if (recommended_format != PixelFormat_Default && recommended_format != Format ())
  {  
    Image (*this, recommended_format).Save (file_name, PixelFormat_Default, format_specific_flags);
    return;
  }

  try
  {
    static ComponentLoader loader ("media.image.savers.*");

    ImageManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this, format_specific_flags);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Image::Save");
    throw;
  }
}

void Image::DefaultLoader (const char* file_name, Image& image)
{
  image.impl = create_bitmap_image (file_name);
}

void Image::CubemapLoader (const char* file_name, Image& image)
{
  image.impl = create_cubemap_image (file_name);
}

void Image::SkyBoxLoader (const char* file_name, Image& image)
{
  image.impl = create_skybox_image (file_name);
}

void Image::DefaultSaver (const char* file_name, const Image& image, const char*)
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
