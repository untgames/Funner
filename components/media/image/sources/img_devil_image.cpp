#include "shared.h"

using namespace media;
using namespace common;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///DevIL картинка
///////////////////////////////////////////////////////////////////////////////////////////////////
class DevILImageImpl: public ImageImpl
{
  public: 
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DevILImageImpl  ();
    DevILImageImpl  (DevILImageImpl&);
    DevILImageImpl  (const char* file_name);
    DevILImageImpl  (size_t width, size_t height, size_t depth, PixelFormat format, const void* data);
    ~DevILImageImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageImpl* Clone ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат / преобразование формата
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format  () { return format; }
    void        Convert (PixelFormat new_format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры картинки / изменение размеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () { return width; }
    size_t Height () { return height; }
    size_t Depth  () { return depth; }
    void   Resize (size_t width, size_t height, size_t depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с образом
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Bitmap   (size_t z);      
    void  PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data);
    void  GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Save (const char* file_name);

  private:
    static void LoadComponent ()
    {
      static ComponentLoader loader ("media.image.DevIL");
    }

  private:
    size_t      il_image;             //идентификатор картинки в DevIL
    size_t      width, height, depth; //кэшируемые размеры картинки
    PixelFormat format;               //кэшируемый формат картинки
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///DevIL исключение
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DevILExceptionTag;

typedef DerivedException<Exception, DevILExceptionTag> DevILException;

//генерация DevIL исключения
void raise_devil_exception (const char* source, const char* format, ...)
{
  va_list list;

  va_start (list, format);

  VRaise<DevILException> (source, format, list);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат пикселей DevIL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DevILPixelFormat
{
  ILenum format, type;
  size_t components;
  
  DevILPixelFormat (ILenum in_format, ILenum in_type, size_t in_components) :
     format (in_format), type (in_type), components (in_components) {}
};

//преобразование PixelFormat -> DevILPixelFormat
DevILPixelFormat get_devil_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:   return DevILPixelFormat (IL_RGB,  IL_UNSIGNED_BYTE, 3);
    case PixelFormat_RGB16:  return DevILPixelFormat (IL_RGB,  IL_UNSIGNED_SHORT, 3);
    case PixelFormat_RGBA8:  return DevILPixelFormat (IL_RGBA, IL_UNSIGNED_BYTE, 4);
    case PixelFormat_RGBA16: return DevILPixelFormat (IL_RGBA, IL_UNSIGNED_SHORT, 4);
    case PixelFormat_BGR8:   return DevILPixelFormat (IL_BGR,  IL_UNSIGNED_BYTE, 3);
    case PixelFormat_BGRA8:  return DevILPixelFormat (IL_BGRA, IL_UNSIGNED_BYTE, 4);
    case PixelFormat_L8:     return DevILPixelFormat (IL_LUMINANCE, IL_UNSIGNED_BYTE, 1);
    case PixelFormat_LA8:    return DevILPixelFormat (IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, 2);
    case PixelFormat_A8:     RaiseNotImplemented ("media::get_devil_format(PixelFormat_A8)"); break;
    default:                 RaiseInvalidArgument ("media::get_devil_format", "format", format); break;
  }
  
  return DevILPixelFormat (0, 0, 0); //для обхода предупреждений компилятора, данная точка недостижима
}

//проверка DevIL ошибок
void check_devil_errors (const char* source, const char* sub_source)
{
  ILenum error = ilGetError ();

  if (error != IL_NO_ERROR)
    raise_devil_exception (source, "Internal DevIL error: %s (after call %s)", iluErrorString (error), sub_source);
}

//получение имени DevIL формата
const char* get_devil_format_name (ILenum format)
{
  switch (format)
  {
    case IL_COLOUR_INDEX:     return "IL_COLOR_INDEX";
    case IL_RGB:              return "IL_RGB";
    case IL_RGBA:             return "IL_RGBA";
    case IL_BGR:              return "IL_BGR";
    case IL_BGRA:             return "IL_BGRA";
    case IL_LUMINANCE:        return "IL_LUMINANCE";
    case IL_LUMINANCE_ALPHA:  return "IL_LUMINANCE_ALPHA";
    default:                  RaiseInvalidArgument ("media::get_devil_format_name", "format", format);
  }
  
  return "";
}

const char* get_devil_type_name (ILenum type)
{
  switch (type)
  {
    case IL_BYTE:           return "IL_BYTE";
    case IL_UNSIGNED_BYTE:  return "IL_UNSIGNED_BYTE";
    case IL_SHORT:          return "IL_SHORT";
    case IL_UNSIGNED_SHORT: return "IL_UNSIGNED_SHORT";
    case IL_INT:            return "IL_INT";
    case IL_UNSIGNED_INT:   return "IL_UNSIGNED_INT";
    case IL_FLOAT:          return "IL_FLOAT";
    case IL_DOUBLE:         return "IL_DOUBLE";
    default:                RaiseInvalidArgument ("media::get_devil_type_name", "type", type);
  }

  return "";
}

/*
    DevILImageImpl
*/

/*
    Конструкторы / деструктор
*/

DevILImageImpl::DevILImageImpl ()
  : width (0), height (0), depth (0), format (PixelFormat_Default)
{
  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl()", "ilGenImages");
}

DevILImageImpl::DevILImageImpl (DevILImageImpl& source)
  : ImageImpl (source), width (source.width), height (source.height), depth (source.depth), format (source.format)
{
  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "ilGenImages");

  try
  {
    ilBindImage        (il_image);
    check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "ilBindImage");
    ilCopyImage        (source.il_image);

    ILenum error = ilGetError ();

    if (error != IL_NO_ERROR)
      raise_devil_exception ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "Can't copy image '%s'. %s.",
                             source.Name (), iluErrorString (error));
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::DevILImageImpl (const char* file_name)
  : width (0), height (0), depth (0), format (PixelFormat_Default)
{
  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const char*)", "ilGenImages");
  
  try
  {
    ilBindImage        (il_image);
    check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const char*)", "ilBindImage");    
    ilLoadImage        (const_cast<ILstring> (file_name));

    ILenum error = ilGetError();

    if (error != IL_NO_ERROR)
      raise_devil_exception ("DevILImageImpl::DevILImageImpl", "Can't create image from file '%s'. %s.", file_name, iluErrorString (error));

    int devil_format = ilGetInteger (IL_IMAGE_FORMAT), devil_type = ilGetInteger (IL_IMAGE_TYPE);    

    switch (devil_format)
    {
      case IL_RGB:
        switch (devil_type)
        {
          case IL_UNSIGNED_BYTE:
            format = PixelFormat_RGB8;
            break;
          case IL_UNSIGNED_SHORT:
            format = PixelFormat_RGB16;
            break;
        }
        break;
      case IL_RGBA:
        switch (devil_type)
        {
          case IL_UNSIGNED_BYTE:
            format = PixelFormat_RGBA8;
            break;
          case IL_UNSIGNED_SHORT:
            format = PixelFormat_RGBA16;
            break;
        }
        break;
      case IL_BGR:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_BGR8;

        break;
      case IL_BGRA:    
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_BGRA8;

        break;
      case IL_LUMINANCE:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_L8;

        break;
      case IL_LUMINANCE_ALPHA:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_LA8;

       break;
    }
    
    if (format == PixelFormat_Default)
      raise_devil_exception ("media::DevILImageImpl::DevILImageImpl(const char*)", "Image '%s' has unsupported format (%s, %s)",
                             file_name, get_devil_format_name (devil_format), get_devil_type_name (devil_type));

    width  = ilGetInteger (IL_IMAGE_WIDTH);
    height = ilGetInteger (IL_IMAGE_HEIGHT);
    depth  = ilGetInteger (IL_IMAGE_DEPTH);
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::DevILImageImpl (size_t in_width, size_t in_height, size_t in_depth, PixelFormat in_format, const void* in_data)
  : width (in_width), height (in_height), depth (in_depth), format (in_format)
{
  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl(size_t,size_t,size_t,PixelFormat,const void*)", "ilGenImages");

  try
  {
    ilBindImage        (il_image);
    check_devil_errors ("media::DevILImageImpl::DevILImageImpl(size_t,size_t,size_t,PixelFormat,const void*)", "ilBindImage");

    DevILPixelFormat devil_format = get_devil_format (format);

    ilTexImage (width, height, depth, devil_format.components, devil_format.format, devil_format.type, const_cast<void*> (in_data));

    ILenum error = ilGetError();

    if (error != IL_NO_ERROR)
      raise_devil_exception ("media::DevILImageImpl::DevILImageImpl(size_t,size_t,size_t,PixelFormat,const void*)",
                             "Can't create image %dx%dx%d with format=%s. %s", width, height, depth, get_format_name (format),
                             iluErrorString (error));
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::~DevILImageImpl ()
{
  ilDeleteImages (1, &il_image);
}

/*
    Копирование
*/

ImageImpl* DevILImageImpl::Clone ()
{
  return new DevILImageImpl (*this);
}

/*
    Формат
*/

void DevILImageImpl::Convert (PixelFormat new_format)
{
  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Convert", "ilBindImage");
  
  DevILPixelFormat devil_format = get_devil_format (new_format);
  
  ilConvertImage (devil_format.format, devil_format.type);

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Convert", "Can't convert image '%s' to format=%s. %s", Name (),
                           get_format_name (new_format), iluErrorString (error));
                           
  format = new_format;
}

/*
    Размеры картинки
*/

void DevILImageImpl::Resize (size_t in_width, size_t in_height, size_t in_depth)
{
  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Resize", "ilBindImage");
  iluScale           (in_width, in_height, in_depth);

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Resize", "Can't resize image '%s' to %ux%ux%u. %s.",
                           Name (), in_width, in_height, in_depth, iluErrorString (error));

  width  = in_width;
  height = in_height;
  depth  = in_depth;
}

/*
    Работа с образом
*/

void* DevILImageImpl::Bitmap (size_t z)
{
  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Bitmap", "ilBindImage");
  
  size_t offset = get_bytes_per_pixel (format) * width * height * z;
  void*  data   = ilGetData () + offset;

  check_devil_errors ("media::DevILImageImpl::Bitmap", "ilGetData");

  return data;
}

void DevILImageImpl::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  if (format == PixelFormat_Default)
    format = this->format;

  ilBindImage (il_image);
  check_devil_errors ("media::DevILImageImpl::PutImage", "ilBindImage");

  DevILPixelFormat devil_format = get_devil_format (format);
  
  ilSetPixels (x, y, z, width, height, depth, devil_format.format, devil_format.type, const_cast<ILvoid*> (data));
  check_devil_errors ("media::DevILImageImpl::PutImage", "ilSetPixels");
}

void DevILImageImpl::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data)
{
  if (format == PixelFormat_Default)
    format = this->format;

  ilBindImage (il_image);
  check_devil_errors ("media::DevILImageImpl::GetImage", "ilBindImage");
  
  DevILPixelFormat devil_format = get_devil_format (format);  
  
  ilCopyPixels (x, y, z, width, height, depth, devil_format.format, devil_format.type, data);
  check_devil_errors ("media::DevILImageImpl::GetImage", "ilCopyPixels");
}

/*
    Сохранение
*/

void DevILImageImpl::Save (const char* file_name)
{
  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Save", "ilBindImage");    
  ilSaveImage        (const_cast<ILstring> (file_name));

  ILenum error = ilGetError ();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Save", "Can't save image '%s' to file '%s'. %s",
                           Name (), file_name, iluErrorString (error));
}

}

namespace media
{

/*
    Создание реализаций картинок
*/

ImageImpl* create_bitmap_image ()
{
  return new DevILImageImpl;
}

ImageImpl* create_bitmap_image (size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  return new DevILImageImpl (width, height, depth, format, data);
}

ImageImpl* create_bitmap_image (const char* file_name)
{
  return new DevILImageImpl (file_name);
}

}
