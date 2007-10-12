#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

namespace
{

const char* SKYBOX_SUFFIX  = ".skybox";
const char* CUBEMAP_SUFFIX = ".cubemap";
const char* DDS_SUFFIX     = ".dds";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Многослойная картинка
///////////////////////////////////////////////////////////////////////////////////////////////////
class MultilayerImageImpl: public ImageImpl
{
  public:
    MultilayerImageImpl (size_t count, Image* images, LayersCloneMode clone_mode);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () { return layers_width;   }
    size_t Height () { return layers_height;  }
    size_t Depth  () { return layers.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format () { return layers_format; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageImpl* Clone () { return new MultilayerImageImpl (*this); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование формата
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (PixelFormat new_format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize (size_t width, size_t height, size_t depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с образом картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Bitmap (size_t z);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование образа с автоматическим преобразованием формата
///////////////////////////////////////////////////////////////////////////////////////////////////
   void PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data);
   void GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Save (const char*);

  private:
    void SaveSixLayersImage (const char* file_name, const char* suffixes [6]);
    void SaveCubemap        (const char* file_name);
    void SaveSkyBox         (const char* file_name);
    void SaveDDS            (const char* file_name);

    void LoadCubemap (const char* file_name);
    void LoadSkyBox (const char* file_name);

  private:
    typedef stl::vector<Image> ImageArray;

  private:
    size_t      layers_width, layers_height;
    PixelFormat layers_format;
    ImageArray  layers;
};

}

/*
    Конструкторы
*/

MultilayerImageImpl::MultilayerImageImpl (size_t count, Image* images, LayersCloneMode clone_mode)
{
  if (!count)
    RaiseNullArgument ("media::MultilayerImageImpl::MultilayerImageImpl", "count");
    
  if (!images)
    RaiseNullArgument ("media::MultilayerImageImpl::MultilayerImageImpl", "images");

  layers_width  = images [0].Width ();
  layers_height = images [0].Height ();
  layers_format = images [0].Format ();
  
  switch (clone_mode)
  {
    case LayersCloneMode_Copy:
      layers.assign (images, images + count);
      break;
    case LayersCloneMode_Capture:
      layers.resize (count);  //не эффективно!!

      for (size_t i=0; i<count; i++)      
        images [i].Swap (layers [i]);
      break;
    default:
      RaiseInvalidArgument ("media::MultilayerImageImpl::MultilayerImageImpl", "clone_mode", clone_mode);
      break;
  }

  for (size_t i=1; i<count; i++)
  {
    layers [i].Resize (layers_width, layers_height, layers [i].Depth ());
    layers [i].Convert (layers_format);
  }
}

/*
    Работа с образом
*/

void* MultilayerImageImpl::Bitmap (size_t z)
{
  return layers [z].Bitmap ();
}

void MultilayerImageImpl::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  size_t last_z = z + depth;
  
  if (last_z > layers.size ())
    last_z = layers.size ();
    
  const char* source     = (const char*)data;
  size_t      layer_size = width * height * get_bytes_per_pixel (format);

  for (; z<last_z; z++, source += layer_size)
    layers [z].PutImage (x, y, 0, width, height, 1, format, source);
}

void MultilayerImageImpl::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data)
{
  size_t last_z = z + depth;
  
  if (last_z > layers.size ())
    last_z = layers.size ();
    
  char*  destination = (char*)data;
  size_t layer_size  = width * height * get_bytes_per_pixel (format);

  for (; z<last_z; z++, destination += layer_size)
    layers [z].GetImage (x, y, 0, width, height, 1, format, destination);
}

/*
    Изменение размера
*/

void MultilayerImageImpl::Resize (size_t width, size_t height, size_t new_layers_count)
{
  ImageArray new_layers;
  
  new_layers.reserve (new_layers_count);  
  new_layers.assign  (layers.begin (), layers.begin () + (new_layers_count < layers.size () ? new_layers_count : layers.size ()));

  if (width != layers_width || height != layers_height)
  {
    for (ImageArray::iterator i=new_layers.begin (), end=new_layers.end (); i!=end; ++i)
      i->Resize (width, height, i->Depth ());      
  }
  
  if (new_layers.size () < new_layers_count)
    new_layers.resize (new_layers_count, Image (width, height, 1, layers_format));

  layers_width  = width;
  layers_height = height;

  layers.swap (new_layers);
}

/*
    Преобразование формата
*/

void MultilayerImageImpl::Convert (PixelFormat new_format)
{
  ImageArray new_layers = layers;

  for (ImageArray::iterator i=new_layers.begin (), end=new_layers.end (); i!=end; ++i)
    i->Convert (new_format);
    
  layers_format = new_format;
    
  layers.swap (new_layers);
}

/*
    Сохранение
*/

void MultilayerImageImpl::SaveSixLayersImage (const char* file_name, const char* suffixes [6])
{
  if (layers.size () < 6)
    RaiseNotSupported ("media::MultilayerImageImpl::SaveSixLayerImage", "Can't save image '%s', depth=%d<6.", Name (), layers.size ());
    
  string basename1 = common::basename (file_name),
         basename2 = common::basename (basename1),
         suffix    = common::suffix (basename1);
    
  for (size_t i=0; i<6; i++)
    layers [i].Save ((basename2 + suffixes [i] + suffix).c_str ());
}

void MultilayerImageImpl::SaveCubemap (const char* file_name)
{
  static const char* suffixes [6] = {"_px", "_nx", "_py", "_ny", "_pz", "_nz"};  

  SaveSixLayersImage (file_name, suffixes);
}

void MultilayerImageImpl::SaveSkyBox (const char* file_name)
{
  static const char* suffixes [6] = {"_up", "_down", "_left", "_right", "_front", "_back"};

  SaveSixLayersImage (file_name, suffixes);
}

void MultilayerImageImpl::SaveDDS (const char* file_name)
{
  Image save_image (layers_width, layers_height, layers.size(), layers_format);

  for (size_t i=0; i<layers.size (); i++)
    save_image.PutImage (0, 0, i, layers_width, layers_height, 1, layers_format, layers [i].Bitmap ());

  save_image.Save (file_name);
}

void MultilayerImageImpl::Save (const char* file_name)
{
  string suffix = common::suffix (file_name);

  if      (!::strcmp (CUBEMAP_SUFFIX, suffix.c_str ())) SaveCubemap (file_name);
  else if (!::strcmp (SKYBOX_SUFFIX,  suffix.c_str ())) SaveSkyBox  (file_name);
  else if (!::strcmp (DDS_SUFFIX,     suffix.c_str ())) SaveDDS     (file_name);
  else
  {
    RaiseNotSupported ("media::MultilayerImageImpl::Save", "Can't save image '%s' in file '%s. Unknown extension '%s'",
                       Name (), file_name, suffix.c_str ());
  }
}

/*
    Загрузка
*/

void MultilayerImageImpl::LoadCubemap (const char* file_name)
{
//  Image (file_name).Swap (*this);
}

void MultilayerImageImpl::LoadSkyBox (const char* file_name)
{
//  Image (file_name).Swap (*this);
}

namespace media
{
typedef stl::vector<Image> ImageArray;

/*
    Создание реализации
*/

ImageImpl* create_multilayer_image (size_t count, Image* images, LayersCloneMode clone_mode)
{
  return new MultilayerImageImpl (count, images, clone_mode);
}

ImageArray* load_image_array (const char* file_name, size_t count, const char** suffixes)
{
  if (!file_name)
    RaiseNullArgument ("media::load_image_array", "file_name");

  if (!count)
    RaiseNullArgument ("media::load_image_array", "count");

  int suffixes_count = sizeof (suffixes) / sizeof (suffixes [0]);

  if (count != suffixes_count)
    RaiseNullArgument ("media::load_image_array", "suffixes");

  string basename1 = common::basename (file_name),
         basename2 = common::basename (basename1),
         suffix    = common::suffix   (basename1);

  ImageArray* images = new ImageArray (count);

  try
  {
    for (size_t i=0; i<count; i++)
      (*images) [i].Load ((basename2 + suffixes [i] + suffix).c_str ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::load_image_array");
    throw;
  }

  return images;
}

ImageImpl* create_cubemap_image (const char* file_name)
{
  static const char* suffixes [] = {"_px", "_nx", "_py", "_ny", "_pz", "_nz"};  

  ImageArray* images = load_image_array (file_name, 6, suffixes);

  return new MultilayerImageImpl (6, images->begin (), LayersCloneMode_Capture);
}

ImageImpl* create_skybox_image (const char* file_name)
{
  static const char* suffixes [] = {"_up", "_down", "_left", "_right", "_front", "_back"};

  ImageArray* images = load_image_array (file_name, 6, suffixes);

  return new MultilayerImageImpl (6, images->begin (), LayersCloneMode_Capture);
}

}
