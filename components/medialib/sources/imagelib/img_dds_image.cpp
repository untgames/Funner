#include "shared.h"

using namespace media;
using namespace common;

/*
    Описание реализации DdsImage
*/

struct DdsImage::Impl
{
  //реализация здесь
};

/*
    Конструкторы / деструктор / присваивание
*/

DdsImage::DdsImage ()
{
  RaiseNotImplemented ("media::DdsImage::DdsImage");
}

DdsImage::DdsImage (const char* file_name)
{
  RaiseNotImplemented ("media::DdsImage::DdsImage");
}

DdsImage::DdsImage (const DdsImage& image)
  : impl (new Impl (*impl))
{
}

DdsImage::~DdsImage ()
{
}

DdsImage& DdsImage::operator = (const DdsImage& image)
{
  DdsImage (image).Swap (*this);

  return *this;
}

/*
    Размеры картинки в пикселях
*/

size_t DdsImage::Width () const
{
  RaiseNotImplemented ("media::DdsImage::Width");
  return 0;
}

size_t DdsImage::Height () const
{
  RaiseNotImplemented ("media::DdsImage::Height");
  return 0;
}

/*
    Количество слоёв / mip-уровней
*/

size_t DdsImage::Depth () const
{
  RaiseNotImplemented ("media::DdsImage::Depth");
  return 0;
}

size_t DdsImage::MipsCount () const
{
  RaiseNotImplemented ("media::DdsImage::MipsCount");
  return 0;
}

/*
    Длина блока пикселей в байтах / размер слоя
*/

size_t DdsImage::BlockSize () const
{
  RaiseNotImplemented ("media::DdsImage::BlockSize");
  return 0;
}

size_t DdsImage::LayerSize (size_t mip_level) const
{
  RaiseNotImplemented ("media::DdsImage::LayerSize");
  return 0;
}

/*
    Формат
*/

DdsPixelFormat DdsImage::Format () const
{
  RaiseNotImplemented ("media::DdsImage::Format");
  return (DdsPixelFormat)-1;
}

/*
    Получение битовой карты
*/

const void* DdsImage::Bitmap (size_t layer, size_t mip_level) const
{
  RaiseNotImplemented ("media::DdsImage::Bitmap");
  return 0;
}

/*
    Загрузка
*/

void DdsImage::Load (const char* file_name)
{
  DdsImage (file_name).Swap (*this);
}

/*
    Обмен
*/

void DdsImage::Swap (DdsImage& image)
{
  stl::swap (impl, image.impl);
}

namespace media
{

void swap (DdsImage& image1, DdsImage& image2)
{
  image1.Swap (image2);
}

}
