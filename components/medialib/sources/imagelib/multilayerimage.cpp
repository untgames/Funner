#include <stl/deque>
#include <stl/algorithm>

#include "shared.h"

using namespace medialib;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Многослойная картинка
///////////////////////////////////////////////////////////////////////////////////////////////////
class MultilayerImageBuilder::Impl : public ImageImpl
{
  public:
    Impl ();
    Impl (const char* file_name);
    Impl (const MultilayerImageBuilder::Impl&,ImagePixelFormat format = PF_DEFAULT);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () const { return layers_width;  }
    size_t Height () const { return layers_height; }
    size_t Depth  () const { return layers.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImagePixelFormat Format () const { return layers_format; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление слоёв
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t InsertLayer  (const Image& source);
    size_t InsertLayer  (const char* file_name);
    size_t InsertLayer  (size_t width,size_t height,ImagePixelFormat format,const void* data);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Захват слоя
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t CaptureLayer (Image& source); //после выполнения операции source сбрасывается

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление слоёв
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveLayer (size_t index) //продумать необходимость!!!
    {
      layers.erase (layers.begin () + index); ///!!!!копирование картинок!!!
    }

    void RemoveAllLayers ()
    {
      layers.clear ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение основных функций
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageImpl* Clone ()
    { 
      return new Impl (*this);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование формата
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (ImagePixelFormat new_format)
    { 
      Impl (*this,new_format).Swap (*this);
    }

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
   void PutImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,const void* data);
   void GetImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Save (const char*,ImagePixelFormat);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Swap (Impl& another)
   {
     layers.swap (another.layers);
     
     stl::swap (layers_width,another.layers_width);
     stl::swap (layers_height,another.layers_height);
     stl::swap (layers_format,another.layers_format);
   }

  private:
    void SaveSixLayerImage (const char* file_name, ImagePixelFormat recommended_format, const char** suffixes, size_t file_name_len);
    void SaveSkybox        (const char* file_name, ImagePixelFormat recommended_format);
    void SaveCubemap       (const char* file_name, ImagePixelFormat recommended_format);

  private:
    typedef stl::deque<Image> LayerList;

  private:
    size_t           layers_width, layers_height;
    ImagePixelFormat layers_format;
    LayerList        layers;
};

namespace
{

const char SKYBOX_EXTENSION_NAME  [] = "skybox";
const char CUBEMAP_EXTENSION_NAME [] = "cubemap";

size_t FormatToBpp (ImagePixelFormat format)
{
  switch (format)
  {
    case PF_RGB8:    return 3;
    case PF_RGB16:   return 6;
    case PF_RGBA8:   return 4;
    case PF_RGBA16:  return 8;
    case PF_BGR8:    return 3;
    case PF_BGRA8:   return 4;
    case PF_L8:      return 1;
    case PF_A8:      return 1;
    case PF_LA8:     return 2;
    default:         RaiseException ("FormatToBpp", "Unknown format parameter received."); return 0;
  }
}

}

/*
    MultilayerImageBuilder::Impl
*/

MultilayerImageBuilder::Impl::Impl ()
  : layers_width (0), layers_height (0), layers_format (PF_DEFAULT)
  { }

MultilayerImageBuilder::Impl::Impl (const MultilayerImageBuilder::Impl& source,ImagePixelFormat format)
  : ImageImpl (source),
    layers_width  (source.layers_width),
    layers_height (source.layers_height),
    layers_format (format),
    layers        (source.layers)
{
  if (format == PF_DEFAULT || format == source.layers_format)
    return;

  for (LayerList::iterator i=layers.begin (),end=layers.end ();i!=end;++i)
    i->Convert (format);
}

size_t MultilayerImageBuilder::Impl::InsertLayer (const Image& source)
{
  Image image (source);

  return CaptureLayer (image);
}

size_t MultilayerImageBuilder::Impl::InsertLayer (const char* file_name)
{
  Image image (file_name);

  return CaptureLayer (image);
}

size_t MultilayerImageBuilder::Impl::InsertLayer (size_t width,size_t height,ImagePixelFormat format,const void* data)
{
  Image image (width,height,1,format,data);

  return CaptureLayer (image);
}

size_t MultilayerImageBuilder::Impl::CaptureLayer (Image& source) //базовая гарантия безопасности
{
  try
  {
    if (layers_format == PF_DEFAULT)
    {
      layers_width  = source.Width  ();
      layers_height = source.Height ();
      layers_format = source.Format ();
    }
    else 
    {      
      source.Resize  (layers_width,layers_height,source.Depth ());
      source.Convert (layers_format);
    }

    layers.push_back ();

    source.Swap (layers.back ());
  }
  catch (...)
  {
    Image ().Swap (source);
    throw;
  }

  return layers.size () - 1;
}

void* MultilayerImageBuilder::Impl::Bitmap (size_t z)
{
  return layers [z].Bitmap ();
}

void MultilayerImageBuilder::Impl::PutImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format, const void* data)
{
  for (size_t i = 0; (i < depth) && ((z + i) < layers.size ()); i++)
    layers [z + i].PutImage (x, y, 0, width, height, 1, format, (char*)data + width * height * i * FormatToBpp (format));
}

void MultilayerImageBuilder::Impl::GetImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,void* data)
{
  for (size_t i = 0; (i < depth) && ((z + i) < layers.size ()); i++)
    layers [z + i].GetImage (x, y, 0, width, height, 1, format, (char*)data + width * height * i * FormatToBpp (format));
}

void MultilayerImageBuilder::Impl::Save (const char* file_name, ImagePixelFormat recommended_format)
{
  if (!_stricmp (CUBEMAP_EXTENSION_NAME, GetExtension (file_name)))
    return SaveCubemap (file_name, recommended_format);
  if (!_stricmp (SKYBOX_EXTENSION_NAME,  GetExtension (file_name)))
    return SaveSkybox  (file_name, recommended_format);

  ImagePixelFormat save_format = recommended_format;
  stl::string f_name (file_name);

  if (_stricmp ("dds", GetExtension (file_name)))
    Raise <Exception> ("MultilayerImageBuilder::Impl::Save", "Can't save multilayer image to %s. Unknown extension.", file_name);

  if (recommended_format == PF_DEFAULT)
    save_format = layers_format;

  Image save_image (layers_width, layers_height, layers.size(), save_format, NULL);

  //Эта проверка предназначена для обхода вылета DevIL при сохранении картинки с 8 битами на компоненту цвета в 16 бит на компоненту!!!
  if ((layers_format != PF_RGB16) && (layers_format != PF_RGBA16) && ((save_format == PF_RGB16) || (save_format == PF_RGBA16)))
    for (size_t i = 0; i < layers.size (); i++)
    {
      Image img (layers[i], save_format);
      save_image.PutImage (0, 0, i, layers_width, layers_height, 1, save_format, img.Bitmap ());      
    }
  else
    for (size_t i = 0; i < layers.size (); i++)
      save_image.PutImage (0, 0, i, layers_width, layers_height, 1, layers_format, layers[i].Bitmap ());

  ImageSystemSingleton::Instance ().GetSaveFunc("dds")->in_use = false;
  save_image.Save (f_name.c_str (), save_format);
}

void MultilayerImageBuilder::Impl::SaveSkybox (const char* file_name, ImagePixelFormat recommended_format)
{
  static const char* suffixes [6] = {"_px", "_nx", "_py", "_ny", "_pz", "_nz"};
  static size_t extension_len = strlen (SKYBOX_EXTENSION_NAME) + 1;

  SaveSixLayerImage (file_name, recommended_format, suffixes, strlen (file_name) - extension_len);
}

void MultilayerImageBuilder::Impl::SaveCubemap (const char* file_name, ImagePixelFormat recommended_format)
{
  static const char* suffixes [6] = {"_up", "_down", "_left", "_right", "_front", "_back"};
  static size_t extension_len = strlen (CUBEMAP_EXTENSION_NAME) + 1;

  SaveSixLayerImage (file_name, recommended_format, suffixes, strlen (file_name) - extension_len);
}

void MultilayerImageBuilder::Impl::SaveSixLayerImage (const char*  file_name, ImagePixelFormat recommended_format,
                                                      const char** suffixes,  size_t           file_name_len)
{
  if (layers.size () < 6)
    Raise <Exception> ("MultilayerImageBuilder::Impl::SaveSixLayerImage", "Can't save image to file '%s', depth '%d' less then 6.", file_name, layers.size ());

  if (!suffixes)
    RaiseNullArgument ("MultilayerImageBuilder::Impl::SaveSixLayerImage", "suffixes");

  static const stl::string::size_type npos = -1;
  stl::string f_name (file_name, file_name_len);

  int dot_index = f_name.find_last_of ('.');

  if (dot_index == npos)
  {
    RaiseException ("MultilayerImageBuilder::Impl::Save", "File hasn't extension.");
  }

  for (size_t i = 0; i < 6; i++)
  {
    stl::string cur_file_name (f_name);
    cur_file_name.insert (dot_index, suffixes[i]);
    layers[i].Save (cur_file_name.c_str (), recommended_format);
  }
}

void MultilayerImageBuilder::Impl::Resize (size_t width,size_t height,size_t new_layers_count)
{
  size_t copied_layers_count = stl::min (new_layers_count,layers.size ());

  LayerList new_layers (layers.begin (),layers.begin ()+copied_layers_count);

  if (width != layers_width || height != layers_height)
  {
    for (LayerList::iterator i=new_layers.begin (),end=new_layers.end ();i!=end;++i)
      i->Resize (width,height,i->Depth ());
  }

  layers_width  = width;
  layers_height = height;

  for (size_t i=copied_layers_count;i<new_layers_count;i++)
  {
    new_layers.push_back ();
    Image (layers_width,layers_height,1,layers_format).Swap (new_layers.back ());
  }

  layers.swap (new_layers);
}

/*
    MultilayerImageBuilder
*/

MultilayerImageBuilder::MultilayerImageBuilder ()
  : impl (new Impl)
  { }

MultilayerImageBuilder::MultilayerImageBuilder (const MultilayerImageBuilder& source)
  : impl (new Impl (*source.impl))
  { }

MultilayerImageBuilder::MultilayerImageBuilder (const MultilayerImageBuilder& source,ImagePixelFormat format)
  : impl (new Impl (*source.impl,format))
  { }

MultilayerImageBuilder::MultilayerImageBuilder (stl::auto_ptr<Impl>& source)
  : impl (source)
  { }

MultilayerImageBuilder::~MultilayerImageBuilder ()
{
}

MultilayerImageBuilder& MultilayerImageBuilder::operator = (const MultilayerImageBuilder& builder)
{
  if (this != &builder)
    MultilayerImageBuilder (builder).Swap (*this);

  return *this; 
}

size_t MultilayerImageBuilder::LayersCount () const
{
  return impl->Impl::Depth ();
}

size_t MultilayerImageBuilder::Width () const
{
  return impl->Impl::Width ();
}

size_t MultilayerImageBuilder::Height () const
{
  return impl->Impl::Height ();
}

ImagePixelFormat MultilayerImageBuilder::Format () const
{
  return impl->Impl::Format ();
}

size_t MultilayerImageBuilder::InsertLayer (const Image& source)
{
  return impl->InsertLayer (source);
}

size_t MultilayerImageBuilder::InsertLayer (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("MultilayerImageBuilder::InsertLayer", "file_name");

  return impl->InsertLayer (file_name);
}

size_t MultilayerImageBuilder::InsertLayer (size_t width,size_t height,ImagePixelFormat format,const void* data)
{
  if (!data)
    RaiseNullArgument ("MultilayerImageBuilder::InsertLayer","data");

  return impl->InsertLayer (width,height,format,data);
}

size_t MultilayerImageBuilder::CaptureLayer (Image& source)
{
  return impl->CaptureLayer (source);
}

void MultilayerImageBuilder::RemoveLayer (size_t index)
{
  if (index >= impl->Depth ())
    RaiseOutOfRange ("MultilayerImageBuilder::RemoveLayer","index",index,0u,impl->Depth ());

  impl->RemoveLayer (index);
}

void MultilayerImageBuilder::RemoveAllLayers ()
{
  impl->RemoveAllLayers ();
}

void MultilayerImageBuilder::Finish (Image& result)
{
  stl::auto_ptr<MultilayerImageBuilder::Impl> ptr (new MultilayerImageBuilder::Impl);

  swap (ptr, impl);

  stl::auto_ptr<ImageImpl> ptr2 (ptr);

  Image (ptr2).Swap (result);
}

void MultilayerImageBuilder::Swap (MultilayerImageBuilder& img_builder)
{
  swap (impl,img_builder.impl);
}

namespace medialib
{

void swap (MultilayerImageBuilder& builder1,MultilayerImageBuilder& builder2)
{
  builder1.Swap (builder2);
}

}
