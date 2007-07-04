#include <il/il.h>
#include <il/ilu.h>
#include "shared.h"
#include "devil_system.h"

using namespace medialib;
using namespace common;

namespace
{

const char* CheckError (ILenum error)
{
  while (ilGetError() != IL_NO_ERROR);

  return iluErrorString (error);
}

const char* PixelFormatToString (ImagePixelFormat format)
{
  switch (format)
  {
    case PF_RGB8:    return "PF_RGB8 (DevIL format - IL_RGB, type - IL_UNSIGNED_BYTE)";
    case PF_RGB16:   return "PF_RGB16 (DevIL format - IL_RGB, type - IL_UNSIGNED_SHORT)";
    case PF_RGBA8:   return "PF_RGBA8 (DevIL format - IL_RGBA, type - IL_UNSIGNED_BYTE)";
    case PF_RGBA16:  return "PF_RGBA16 (DevIL format - IL_RGBA, type - IL_UNSIGNED_SHORT)";
    case PF_BGR8:    return "PF_BGR8 (DevIL format - IL_BGR, type - IL_UNSIGNED_BYTE)";
    case PF_BGRA8:   return "PF_BGRA8 (DevIL format - IL_BGRA, type - IL_UNSIGNED_BYTE)";
    case PF_L8:      return "PF_L8 (DevIL format - IL_LUMINANCE, type - IL_UNSIGNED_BYTE)";
    case PF_LA8:     return "PF_LA8 (DevIL format - IL_LUMINANCE_ALPHA, type - IL_UNSIGNED_BYTE)";
    case PF_DEFAULT: return "PF_DEFAULT";
    case PF_A8:      return "PF_A8. There are no such DevIL format.";    
    default:         return "Unknown pixel format";
  }
}

}

DevILImageImpl::DevILImageImpl ()
{
  DevILSystemSingleton::Instance ();
  ilGenImages (1, &name);  
} 

DevILImageImpl::DevILImageImpl (const DevILImageImpl* source)
  : ImageImpl (*source)
{
  ilGenImages (1, &name);  
  ilBindImage (name);
  ilCopyImage (source->name);

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
  {
    ilDeleteImages (1, &name);
    Raise <Exception> ("DevILImageImpl::DevILImageImpl", "Can't copy image '%s'. %s.", source->Name (), CheckError (error));
  }
}

DevILImageImpl::DevILImageImpl (const char* file_name)
{
  DevILSystemSingleton::Instance ();
  ilGetError  ();
  ilGenImages (1,&name);
  ilBindImage (name);  
  
  ilLoadImage (const_cast<ILstring> (file_name));

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
  {
    ilDeleteImages (1,&name);
    Raise <Exception> ("DevILImageImpl::DevILImageImpl", "Can't create image from file %s. %s.", file_name, CheckError (error));
  }
}

DevILImageImpl::DevILImageImpl (size_t width,size_t height,size_t depth,ImagePixelFormat format,const void* data)
{
  DevILSystemSingleton::Instance ();
  ilGenImages (1, &name);
  ilBindImage (name);

  switch (format)
  {
    case PF_RGB8:    ilTexImage (width, height, depth, 3, IL_RGB,             IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_RGB16:   ilTexImage (width, height, depth, 3, IL_RGB,             IL_UNSIGNED_SHORT, const_cast<void*> (data)); break;
    case PF_RGBA8:   ilTexImage (width, height, depth, 4, IL_RGBA,            IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_RGBA16:  ilTexImage (width, height, depth, 4, IL_RGBA,            IL_UNSIGNED_SHORT, const_cast<void*> (data)); break;
    case PF_BGR8:    ilTexImage (width, height, depth, 3, IL_BGR,             IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_BGRA8:   ilTexImage (width, height, depth, 4, IL_BGRA,            IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_L8:      ilTexImage (width, height, depth, 1, IL_LUMINANCE,       IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_LA8:     ilTexImage (width, height, depth, 2, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE,  const_cast<void*> (data)); break;
    case PF_DEFAULT: RaiseException ("DevILImageImpl::DevILImageImpl", "Default pixel format not supported in this constructor."); break;    
    case PF_A8:      RaiseNotImplemented ("DevILImageImpl::DevILImageImpl"); break;    
    default:         RaiseException ("DevILImageImpl::DevILImageImpl", "Unknown convertion parameters received.");
  }
  

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
  {
    ilDeleteImages (1, &name);
    Raise <Exception> ("DevILImageImpl::DevILImageImpl",
                       "Can't create image from memory with width = %d, height = %d, depth = %d, format = %s. %s.",
                        width, height, depth, PixelFormatToString (format), CheckError (error));
  }
}

DevILImageImpl::~DevILImageImpl ()
{
  ilDeleteImages (1, &name);
}

ImagePixelFormat DevILImageImpl::Format () const
{
  ilBindImage (name);

  int format = ilGetInteger (IL_IMAGE_FORMAT), type = ilGetInteger (IL_IMAGE_TYPE);

  switch (format)
  {
    case IL_RGB:             if (type == IL_UNSIGNED_BYTE) return PF_RGB8;  else if (type == IL_UNSIGNED_SHORT) return PF_RGB16;  break;
    case IL_RGBA:            if (type == IL_UNSIGNED_BYTE) return PF_RGBA8; else if (type == IL_UNSIGNED_SHORT) return PF_RGBA16; break;
    case IL_BGR:             if (type == IL_UNSIGNED_BYTE) return PF_BGR8;  break;
    case IL_BGRA:            if (type == IL_UNSIGNED_BYTE) return PF_BGRA8; break;
    case IL_LUMINANCE:       if (type == IL_UNSIGNED_BYTE) return PF_L8;    break;
    case IL_LUMINANCE_ALPHA: if (type == IL_UNSIGNED_BYTE) return PF_LA8;   break;
  }

  RaiseNotSupported ("DevILImageImpl::Format", "Unsupported format used.");
  return PF_DEFAULT;
}

void DevILImageImpl::Convert (ImagePixelFormat new_format)
{
  ilBindImage (name);

  switch (new_format)
  {
    case PF_DEFAULT: break;
    case PF_RGB8:    ilConvertImage (IL_RGB,  IL_UNSIGNED_BYTE);  break;
    case PF_RGB16:   ilConvertImage (IL_RGB,  IL_UNSIGNED_SHORT); break;
    case PF_RGBA8:   ilConvertImage (IL_RGBA, IL_UNSIGNED_BYTE);  break;
    case PF_RGBA16:  ilConvertImage (IL_RGBA, IL_UNSIGNED_SHORT); break;
    case PF_BGR8:    ilConvertImage (IL_BGR,  IL_UNSIGNED_BYTE);  break;
    case PF_BGRA8:   ilConvertImage (IL_BGRA, IL_UNSIGNED_BYTE);  break;
    case PF_L8:      ilConvertImage (IL_LUMINANCE, IL_UNSIGNED_BYTE); break;
    case PF_A8:      RaiseNotImplemented ("DevILImageImpl::Convert"); break;
    case PF_LA8:     ilConvertImage (IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE); break;
    default:         RaiseException ("DevILImageImpl::Convert", "Unknown convertion parameters received.");
  }

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
  {
    ilDeleteImages (1, &name);
    Raise <Exception> ("DevILImageImpl::Convert", "Can't convert image to format %s. %s", PixelFormatToString (new_format), CheckError (error));
  }
}

size_t DevILImageImpl::Width () const
{
  ilBindImage (name);
  return ilGetInteger (IL_IMAGE_WIDTH);
}

size_t DevILImageImpl::Height () const
{
  ilBindImage (name);
  return ilGetInteger (IL_IMAGE_HEIGHT);
}

size_t DevILImageImpl::Depth () const
{
  ilBindImage (name);
  return ilGetInteger (IL_IMAGE_DEPTH);
}

void DevILImageImpl::Resize (size_t width,size_t height,size_t depth)
{
  ilBindImage (name);
  iluScale (width, height, depth);
  
  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    Raise <Exception> ("DevILImageImpl::Resize", "Can't resize image to width %u, height %u, depth %u. %s.", width, height, depth, CheckError (error));
}

void* DevILImageImpl::Bitmap (size_t z)
{
  ilBindImage (name);
  return (ilGetData() + ilGetInteger (IL_IMAGE_BYTES_PER_PIXEL) * ilGetInteger (IL_IMAGE_WIDTH) * ilGetInteger (IL_IMAGE_HEIGHT) * z);
}

void DevILImageImpl::PutImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format, const void* data)
{
  ilBindImage  (name);

  switch (format)
  {
    case PF_DEFAULT: RaiseException ("DevILImageImpl::PutImage", "Default pixel format not supported in this constructor.");
    case PF_RGB8:    ilSetPixels (x, y, z, width, height, depth, IL_RGB, IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data));  break;
    case PF_RGB16:   ilSetPixels (x, y, z, width, height, depth, IL_RGB, IL_UNSIGNED_SHORT, const_cast <ILvoid*> (data)); break;
    case PF_RGBA8:   ilSetPixels (x, y, z, width, height, depth, IL_RGBA,IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data));  break;
    case PF_RGBA16:  ilSetPixels (x, y, z, width, height, depth, IL_RGBA,IL_UNSIGNED_SHORT, const_cast <ILvoid*> (data)); break;
    case PF_BGR8:    ilSetPixels (x, y, z, width, height, depth, IL_BGR, IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data));  break;
    case PF_BGRA8:   ilSetPixels (x, y, z, width, height, depth, IL_BGRA,IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data));  break;
    case PF_L8:      ilSetPixels (x, y, z, width, height, depth, IL_LUMINANCE, IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data)); break;
    case PF_A8:      RaiseNotImplemented ("DevILImageImpl::PutImage"); break;
    case PF_LA8:     ilSetPixels (x, y, z, width, height, depth, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, const_cast <ILvoid*> (data)); break;
    default:         RaiseException ("DevILImageImpl::PutImage", "Unknown convertion parameters received.");
  }

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    Raise <Exception> ("DevILImageImpl::PutImage", "Can't put image with pixel format = %s. %s", PixelFormatToString (format), CheckError (error));
}

void DevILImageImpl::GetImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,void* data)
{
  ilBindImage  (name);

  switch (format)
  {
    case PF_DEFAULT: ilCopyPixels (x, y, z, width, height, depth, ilGetInteger (IL_IMAGE_FORMAT), ilGetInteger (IL_IMAGE_TYPE), data); break;
    case PF_RGB8:    ilCopyPixels (x, y, z, width, height, depth, IL_RGB, IL_UNSIGNED_BYTE, data); break;
    case PF_RGB16:   ilCopyPixels (x, y, z, width, height, depth, IL_RGB, IL_UNSIGNED_SHORT, data); break;
    case PF_RGBA8:   ilCopyPixels (x, y, z, width, height, depth, IL_RGBA,IL_UNSIGNED_BYTE, data);  break;
    case PF_RGBA16:  ilCopyPixels (x, y, z, width, height, depth, IL_RGBA,IL_UNSIGNED_SHORT, data); break;
    case PF_BGR8:    ilCopyPixels (x, y, z, width, height, depth, IL_BGR, IL_UNSIGNED_BYTE, data);  break;
    case PF_BGRA8:   ilCopyPixels (x, y, z, width, height, depth, IL_BGRA,IL_UNSIGNED_BYTE, data);  break;
    case PF_L8:      ilCopyPixels (x, y, z, width, height, depth, IL_LUMINANCE, IL_UNSIGNED_BYTE, data); break;
    case PF_A8:      RaiseNotImplemented ("DevILImageImpl::GetImage"); break;
    case PF_LA8:     ilCopyPixels (x, y, z, width, height, depth, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, data); break;
    default:         RaiseException ("DevILImageImpl::GetImage", "Unknown convertion parameters received.");
  }

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    Raise <Exception> ("DevILImageImpl::GeImage", "Can't get image with pixel format = %s. %s", PixelFormatToString (format), CheckError (error));
}

void DevILImageImpl::Save (const char* file_name, ImagePixelFormat recommended_format)
{
  if (!file_name)
  {
    RaiseException ("DevILImageImpl::Save", "No file name.");
    return;
  }

  ILenum error;

  if ((recommended_format == PF_DEFAULT) || (recommended_format == Format ()))
  {
    ilBindImage (name);

    ilSaveImage ((char*)file_name);

    error = ilGetError ();
    if (error != IL_NO_ERROR)
    {
      Raise <Exception> ("DevILImageImpl::Save", "Can't save image %s. %s", file_name, CheckError (error));
      return;
    }
  }
  else
  {
    size_t new_image;

    ilGenImages (1, &new_image);  
    ilBindImage (new_image);
    ilCopyImage (name);
    
    error = ilGetError();
    if (error != IL_NO_ERROR)
    {
      ilDeleteImages (1, &new_image);
      RaiseException ("DevILImageImpl::Save", CheckError (error));
      return;
    }

    switch (recommended_format)
    {
      case PF_RGB8:    ilConvertImage (IL_RGB,  IL_UNSIGNED_BYTE);  break;
      case PF_RGB16:   ilConvertImage (IL_RGB,  IL_UNSIGNED_SHORT); break;
      case PF_BGR8:    ilConvertImage (IL_BGR,  IL_UNSIGNED_BYTE);  break;
      case PF_RGBA8:   ilConvertImage (IL_RGBA, IL_UNSIGNED_BYTE);  break;
      case PF_RGBA16:  ilConvertImage (IL_RGBA, IL_UNSIGNED_SHORT); break;
      case PF_BGRA8:   ilConvertImage (IL_BGRA, IL_UNSIGNED_BYTE);  break;
      case PF_L8:      ilConvertImage (IL_LUMINANCE, IL_UNSIGNED_BYTE); break;
      case PF_A8:      RaiseNotImplemented ("DevILImageImpl::Save"); break;
      case PF_LA8:     ilConvertImage (IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE); break;
      default:         RaiseException ("DevILImageImpl::Save", "Unknown saving parameters received.");
    }
    
    error = ilGetError();
    if (error != IL_NO_ERROR)
    {
      ilDeleteImages (1, &new_image);
      Raise <Exception> ("DevILImageImpl::Save", "Can't save image to pixel format = %s. %s", PixelFormatToString (recommended_format), CheckError (error));
      return;
    }

    ilSaveImage ((char*)file_name);

    error = ilGetError ();
    if (error != IL_NO_ERROR)
    {
      ilDeleteImages (1, &new_image);
      Raise <Exception> ("DevILImageImpl::Save", "Can't save image %s. %s", file_name, CheckError (error));
      return;
    }
  }
}

ImageImpl* DevILImageImpl::Clone ()
{
  return new DevILImageImpl (this);
}         
