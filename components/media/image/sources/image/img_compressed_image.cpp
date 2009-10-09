#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPRESSED_IMAGE_COMPONENT_MASK = "media.compressed_image.*"; //маска имён компонентов загрузки сжатых изображений

/*
    Сжатое изображение по умолчанию
*/

class DefaultCompressedImage: public ICustomCompressedImage
{
  public:
    size_t      Width     () { return 0; }
    size_t      Height    () { return 0; }
    size_t      Depth     () { return 0; }
    size_t      MipsCount () { return 0; }
    const char* Format    () { return ""; }

    size_t BitmapSize (size_t layer, size_t mip_level)
    {
      throw xtl::make_range_exception ("media::DefaultCompressedImage::BitmapSize", "layer", layer, 0u);
    }

    const void* Bitmap (size_t layer, size_t mip_level)
    {
      throw xtl::make_range_exception ("media::DefaultCompressedImage::Bitmap", "layer", layer, 0u);
    }
};

}

/*
    Описание реализации CompressedImage
*/

typedef stl::auto_ptr<ICustomCompressedImage> CompressedImagePtr;

struct CompressedImage::Impl: public xtl::reference_counter
{
  CompressedImagePtr image; //реализация сжатого изображения
};

/*
    Конструкторы / деструктор / присваивание
*/

CompressedImage::CompressedImage ()
  : impl (new Impl)
{
  try
  {
    impl->image = CompressedImagePtr (new DefaultCompressedImage);    
  }
  catch (xtl::exception& e)
  {
    delete impl;

    e.touch ("media::CompressedImage::CompressedImage()");

    throw;
  }
}

CompressedImage::CompressedImage (const char* file_name)
  : impl (new Impl)
{
  try
  {
    static ComponentLoader loader (COMPRESSED_IMAGE_COMPONENT_MASK);
    
    impl->image = CompressedImagePtr (CompressedImageManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name));

    if (!impl->image.get ())
      throw xtl::format_operation_exception ("", "Error at load image '%s'. Internal loader error" , file_name);
  }
  catch (xtl::exception& exception)
  {
    delete impl;

    exception.touch ("media::CompressedImage::CompressedImage(const char*)");

    throw;
  }
}

CompressedImage::CompressedImage (const CompressedImage& image)
  : impl (image.impl)
{
  addref (impl);
}

CompressedImage::~CompressedImage ()
{
  release (impl);
}

CompressedImage& CompressedImage::operator = (const CompressedImage& image)
{
  CompressedImage (image).Swap (*this);

  return *this;
}

/*
    Размеры картинки в пикселях
*/

size_t CompressedImage::Width () const
{
  try
  {
    return impl->image->Width ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Width");
    throw;
  }
}

size_t CompressedImage::Height () const
{
  try
  {
    return impl->image->Height ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Height");
    throw;
  }
}

/*
    Количество слоёв / mip-уровней
*/

size_t CompressedImage::Depth () const
{
  try
  {
    return impl->image->Depth ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Depth");
    throw;
  }
}

size_t CompressedImage::MipsCount () const
{
  try
  {
    return impl->image->MipsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::MipsCount");
    throw;
  }
}

/*
    Формат
*/

const char* CompressedImage::Format () const
{
  try
  {
    return impl->image->Format ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Format");
    throw;
  }
}

/*
    Размер образа / получение битовой карты
*/

size_t CompressedImage::BitmapSize (size_t layer, size_t mip_level) const
{
  try
  {
    return impl->image->BitmapSize (layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::BitmapSize");
    throw;
  }
}

const void* CompressedImage::Bitmap (size_t layer, size_t mip_level) const
{
  try
  {
    return impl->image->Bitmap (layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Bitmap");
    throw;
  }
}

/*
    Загрузка
*/

void CompressedImage::Load (const char* file_name)
{
  try
  {
    CompressedImage (file_name).Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Load");
    throw;
  }
}

/*
    Обмен
*/

void CompressedImage::Swap (CompressedImage& image)
{
  stl::swap (impl, image.impl);
}

namespace media
{

void swap (CompressedImage& image1, CompressedImage& image2)
{
  image1.Swap (image2);
}

}
