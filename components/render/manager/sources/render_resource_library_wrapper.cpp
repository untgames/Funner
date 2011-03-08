#include "shared.h"

using namespace render;

ResourceLibrary::ResourceLibrary ()
  : impl (new ResourceLibraryImpl)
{
}

ResourceLibrary::ResourceLibrary (ResourceLibraryImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

ResourceLibrary::ResourceLibrary (const ResourceLibrary& library)
  : impl (library.impl)
{
  addref (impl);
}

ResourceLibrary::~ResourceLibrary ()
{
  release (impl);
}

ResourceLibrary& ResourceLibrary::operator = (const ResourceLibrary& library)
{
  ResourceLibrary (library).Swap (*this);
  return *this;
}

void ResourceLibrary::Attach (const ResourceLibrary& library)
{
  impl->Attach (Wrappers::Unwrap<ResourceLibraryImpl> (library));
}

void ResourceLibrary::Detach (const ResourceLibrary& library)
{
  impl->Detach (Wrappers::Unwrap<ResourceLibraryImpl> (library));
}

void ResourceLibrary::DetachAllLibraries ()
{
  impl->DetachAllLibraries ();
}

void ResourceLibrary::Swap (ResourceLibrary& library)
{
  stl::swap (impl, library.impl);
}

namespace render
{

void swap (ResourceLibrary& library1, ResourceLibrary& library2)
{
  library1.Swap (library2);
}

}
