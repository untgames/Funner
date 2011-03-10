#include "shared.h"

using namespace render;

RenderManager::RenderManager ()
  : impl (new RenderManagerImpl)
{
}

RenderManager::RenderManager (RenderManagerImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RenderManager::RenderManager (const RenderManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

RenderManager::~RenderManager ()
{
  release (impl);
}

RenderManager& RenderManager::operator = (const RenderManager& manager)
{
  RenderManager (manager).Swap (*this);
  return *this;
}

const char* RenderManager::Description () const
{
  return impl->Description ();
}

Window RenderManager::CreateWindow (syslib::Window& window, common::PropertyMap& properties)
{
  return Wrappers::Wrap<render::Window> (impl->CreateWindow (window, properties));
}

size_t RenderManager::WindowsCount () const
{
  return impl->WindowsCount ();
}

const Window RenderManager::Window (size_t index) const
{
  return Wrappers::Wrap<render::Window> (impl->Window (index));
}

Window RenderManager::Window (size_t index)
{
  return Wrappers::Wrap<render::Window> (impl->Window (index));
}

RenderTarget RenderManager::CreateRenderBuffer (size_t width, size_t height, PixelFormat format)
{
  return Wrappers::Wrap<RenderTarget> (impl->CreateRenderBuffer (width, height, format));
}

RenderTarget RenderManager::CreateDepthStencilBuffer (size_t width, size_t height)
{
  return Wrappers::Wrap<RenderTarget> (impl->CreateDepthStencilBuffer (width, height));
}
                                       
Primitive RenderManager::CreatePrimitive ()
{
  return Wrappers::Wrap<Primitive> (impl->CreatePrimitive ());
}

Primitive RenderManager::CreatePrimitive (const PrimitiveBuffers& buffers)
{
  return Wrappers::Wrap<Primitive> (impl->CreatePrimitive (Wrappers::Unwrap<PrimitiveBuffersImpl> (buffers)));
}

PrimitiveBuffers RenderManager::CreatePrimitiveBuffers (MeshBufferUsage lines_usage, MeshBufferUsage sprites_usage)
{
  return Wrappers::Wrap<PrimitiveBuffers> (impl->CreatePrimitiveBuffers (lines_usage, sprites_usage));
}

Frame RenderManager::CreateFrame ()
{
  return Wrappers::Wrap<Frame> (impl->CreateFrame ());
}

Texture RenderManager::CreateTexture (const media::Image& image, bool generate_mips_enable)
{
  return Wrappers::Wrap<Texture> (impl->CreateTexture (image, generate_mips_enable));
}

Texture RenderManager::CreateTexture (const media::Image& image, TextureDimension dimension, bool generate_mips_enable)
{
  return Wrappers::Wrap<Texture> (impl->CreateTexture (image, dimension, generate_mips_enable));
}

Texture RenderManager::CreateTexture2D (size_t width, size_t height, PixelFormat format, bool generate_mips_enable)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_2D, width, height, 1, format, generate_mips_enable));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManager::CreateTexture2D");
    throw;
  }
}

Texture RenderManager::CreateTexture3D (size_t width, size_t height, size_t depth, PixelFormat format, bool generate_mips_enable)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_3D, width, height, depth, format, generate_mips_enable));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManager::CreateTexture3D");
    throw;
  }
}

Texture RenderManager::CreateTextureCubemap (size_t size, PixelFormat format, bool generate_mips_enable)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_Cubemap, size, size, 6, format, generate_mips_enable));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManager::CreateTextureCubemap");
    throw;
  }  
}

Material RenderManager::CreateMaterial ()
{
  return Wrappers::Wrap<Material> (impl->CreateMaterial ());
}

Texture RenderManager::FindTexture (const char* name) const
{
  return Wrappers::Wrap<Texture> (impl->FindTexture (name));
}

Material RenderManager::FindMaterial (const char* name) const
{
  return Wrappers::Wrap<Material> (impl->FindMaterial (name));
}

Primitive RenderManager::FindPrimitive (const char* name) const
{
  return Wrappers::Wrap<Primitive> (impl->FindPrimitive (name));
}

void RenderManager::LoadResource (const char* resource_name)
{
  impl->LoadResource (resource_name);
}

void RenderManager::UnloadResource (const char* resource_name)
{
  impl->UnloadResource (resource_name);
}

void RenderManager::LoadResource (const media::rfx::MaterialLibrary& library)
{
  impl->LoadResource (library);
}

void RenderManager::UnloadResource (const media::rfx::MaterialLibrary& library)
{
  impl->UnloadResource (library);
}

void RenderManager::LoadResource (const media::geometry::MeshLibrary& library)
{
  impl->LoadResource (library);
}

void RenderManager::UnloadResource (const media::geometry::MeshLibrary& library)
{
  impl->UnloadResource (library);
}

xtl::connection RenderManager::RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const
{
  return impl->RegisterWindowEventHandler (event, handler);
}

void RenderManager::Swap (RenderManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace render
{

void swap (RenderManager& manager1, RenderManager& manager2)
{
  manager1.Swap (manager2);
}

}
