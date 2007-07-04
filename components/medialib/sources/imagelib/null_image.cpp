#include "shared.h"

using namespace medialib;
using namespace common;

const char* NullImageImpl::Name () const
{
  Raise ("NullImageImpl::Name");
  return NULL;
}

void NullImageImpl::Rename (const char* new_name)
{
  Raise ("NullImageImpl::Rename");
}

void NullImageImpl::Convert (ImagePixelFormat new_format)
{
  Raise ("NullImageImpl::Convert");
}

size_t NullImageImpl::Width () const
{
  Raise ("NullImageImpl::Width");
  return 0;
}

size_t NullImageImpl::Height () const
{
  Raise ("NullImageImpl::Height");
  return 0;
}

size_t NullImageImpl::Depth () const
{
  Raise ("NullImageImpl::Depth");
  return 0;
}

void NullImageImpl::Resize (size_t width,size_t height,size_t depth)
{
  Raise ("NullImageImpl::Resize");
}

ImagePixelFormat NullImageImpl::Format () const
{
  Raise ("NullImageImpl::Format");
  return PF_DEFAULT;
}

void* NullImageImpl::Bitmap (size_t z)
{
  Raise ("NullImageImpl::Bitmap");
  return NULL;
}

void NullImageImpl::PutImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format, const void* data)
{
  Raise ("NullImageImpl::PutImage");
}

void NullImageImpl::GetImage (size_t x,size_t y,size_t z,size_t width,size_t height,size_t depth,ImagePixelFormat format,void* data)
{
  Raise ("NullImageImpl::Save");
}

void NullImageImpl::Save (const char* file_name, ImagePixelFormat recommended_format)
{
  Raise ("NullImageImpl::Save");
}

ImageImpl* NullImageImpl::Clone ()
{
  return new NullImageImpl ();
}

NullImageImpl::NullImageImpl ()
{
}

NullImageImpl::~NullImageImpl ()
{
}

void NullImageImpl::Raise (const char* source) const
{
  RaiseException (source, "Can't do operations with not initialized image.");
}

