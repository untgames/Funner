#include "shared.h"

using namespace common;
using namespace medialib;

Image::Image ()
  : impl (new NullImageImpl)
{
  ImageSystemSingleton::Instance ().RegisterImage (*this);
}

Image::Image (stl::auto_ptr <ImageImpl>& source)
{
  ImageSystemSingleton::Instance ().RegisterImage (*this);

  if (source.get())
    impl = source;
  else
    impl.reset(new NullImageImpl ());
}

Image::Image (const Image& source)
  : impl (source.impl->Clone ())
{                  
  ImageSystemSingleton::Instance ().RegisterImage (*this);
}

Image::Image (const Image& source, ImagePixelFormat format)
  : impl (source.impl->Clone ())
{                  
  Convert (format);

  ImageSystemSingleton::Instance ().RegisterImage (*this);
}

Image::Image (const char* file_name)
{
  ImageSystemSingleton::Instance ().RegisterImage (*this);

  Image (new DevILImageImpl ()).Swap (*this);
  CodecLoadFunction* load_func = ImageSystemSingleton::Instance ().GetLoadFunc(GetExtension (file_name));

  if (!load_func)
    Raise <Exception> ("Image::Image", "Can't create image from file '%s'. Format not supported.", file_name);

  if (load_func->in_use)
    RaiseException ("Image::Image", "Load codec called itself.");
  load_func->in_use = true;
  try
  {
    load_func->function (file_name, *this);
  }
  catch (...)
  {
    load_func->in_use = false;
    throw;
  }
  load_func->in_use = false;
}

Image::Image (size_t width,size_t height,size_t depth,ImagePixelFormat format,const void* data)
  : impl (new DevILImageImpl (width,height,depth,format,data))
{
  ImageSystemSingleton::Instance ().RegisterImage (*this);
}

Image::Image (ImageImpl* source)
  : impl (source)
{
  ImageSystemSingleton::Instance ().RegisterImage (*this);
}

Image::~Image ()
{
  ImageSystemSingleton::Instance ().UnregisterImage (*this);
}

Image& Image::operator = (const Image& source)
{
  if (this == &source)
    return *this;

  Image(source.impl->Clone()).Swap (*this);

  return *this; 
}

void Image::Load (const char* file_name)
{  
  CodecLoadFunction* load_func = ImageSystemSingleton::Instance ().GetLoadFunc(GetExtension (file_name));
  Image (new DevILImageImpl ()).Swap (*this);

  if (!load_func)
    Raise <Exception> ("Image::Load", "Can't create image from file '%s'. Format not supported.", file_name);

  if (load_func->in_use)
    RaiseException ("Image::Load", "Load codec called itself.");
  load_func->in_use = true;
  try
  {
    load_func->function (file_name, *this);
  }
  catch (...)
  {
    load_func->in_use = false;
    throw;
  }
  load_func->in_use = false;
}

void Image::DefaultLoader (const char* file_name, Image& img)
{
  swap (img.impl, stl::auto_ptr<ImageImpl> (new DevILImageImpl (file_name)));
}

void Image::DefaultSaver (const char* file_name, const Image& img, ImagePixelFormat format)
{
  img.impl->Save (file_name, format);  
}

const char* Image::Name () const
{
  return impl->Name ();
}

void Image::Rename (const char* new_name)
{
  impl->Rename (new_name);
}

ImagePixelFormat Image::Format () const
{
  return impl->Format();
}

void Image::Convert (ImagePixelFormat new_format)
{
  if (new_format == impl->Format () || new_format == PF_DEFAULT)
    return;

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

void Image::Resize (size_t width,size_t height,size_t depth)
{
  if (width == impl->Width () && height == impl->Height () && depth == impl->Depth ())
    return;

  impl->Resize (width, height, depth);
}

void* Image::Bitmap (size_t z)
{
  if (z >= impl->Depth ())
    RaiseOutOfRange ("Image::Bitmap","z",z,0u,impl->Depth ());  

  return impl->Bitmap (z);
}

const void* Image::Bitmap (size_t z) const
{
  if (z >= impl->Depth ())
    RaiseOutOfRange ("Image::Bitmap","z",z,0u,impl->Depth ());  

  return impl->Bitmap (z);
}

void Image::PutImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format, const void* data)
{
  if (!data)
    RaiseNullArgument ("Image::PutImage", "data");

  if (z >= impl->Depth ())
    RaiseOutOfRange ("Image::PutImage","z",z,0u,impl->Depth ());  

  impl->PutImage (x, y, z, width, height, depth, format, data);
}

void Image::GetImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,void* data) const
{
  if (!data)
    RaiseNullArgument ("Image::GetImage", "data");

  if (z >= impl->Depth ())
    RaiseOutOfRange ("Image::GetImage","z",z,0u,impl->Depth ());  

  impl->GetImage (x, y, z, width, height, depth, format, data);
}

void Image::Save (const char* file_name, ImagePixelFormat recommended_format)
{
  if (!file_name)
    RaiseNullArgument ("Image::Save", "file_name");

  CodecSaveFunction* save_func = ImageSystemSingleton::Instance ().GetSaveFunc(GetExtension (file_name));

  if (!save_func)
    Raise <Exception> ("Image::Save", "Can't save image to file '%s'. Format not supported.", file_name);

  if (save_func->in_use)
    RaiseException ("Image::Save", "Save codec called itself.");
  save_func->in_use = true;
  try
  {
    save_func->function (file_name, *this, recommended_format);
  }
  catch (...)
  {
    save_func->in_use = false;
    throw;
  }
  save_func->in_use = false;
}

void Image::Swap (Image& img)
{
  swap (impl, img.impl);
}

const char* ImageImpl::Name () const
{
  return str_name.c_str ();
}

void ImageImpl::Rename (const char* new_name)
{
  str_name = new_name;
}

namespace medialib
{

void swap (Image& img1,Image& img2)
{
  img1.Swap (img2);
}

}
