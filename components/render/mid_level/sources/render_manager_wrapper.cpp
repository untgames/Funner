#include "shared.h"

using namespace render::mid_level;

RenderManager::RenderManager ()
  : impl (new RenderManagerImpl)
{
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
  return Wrappers::Wrap<render::mid_level::Window> (impl->CreateWindow (window, properties));
}

Window RenderManager::CreateWindow (const char* name, syslib::Window& window, common::PropertyMap& properties)
{
  try
  {
    render::mid_level::Window render_window = CreateWindow (window, properties);
  
    render_window.SetId (name);
    
    return render_window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManager::CreateWindow");
    throw;
  }
}

size_t RenderManager::WindowsCount () const
{
  return impl->WindowsCount ();
}

const Window RenderManager::Window (size_t index) const
{
  return Wrappers::Wrap<render::mid_level::Window> (impl->Window (index));
}

Window RenderManager::Window (size_t index)
{
  return Wrappers::Wrap<render::mid_level::Window> (impl->Window (index));
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

Primitive RenderManager::CreatePrimitive (const char* name, ResourceInstanceMode mode)
{
  return Wrappers::Wrap<Primitive> (impl->CreatePrimitive (name, mode));
}

Frame RenderManager::CreateFrame ()
{
  return Wrappers::Wrap<Frame> (impl->CreateFrame ());
}

Texture RenderManager::CreateTexture (const media::Image& image)
{
  return Wrappers::Wrap<Texture> (impl->CreateTexture (image));
}

Texture RenderManager::CreateTexture (const media::Image& image, TextureDimension dimension)
{
  return Wrappers::Wrap<Texture> (impl->CreateTexture (image, dimension));
}

Texture RenderManager::CreateTexture2D (size_t width, size_t height, PixelFormat format)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_2D, width, height, 1, format));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManager::CreateTexture2D");
    throw;
  }
}

Texture RenderManager::CreateTexture3D (size_t width, size_t height, size_t depth, PixelFormat format)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_3D, width, height, depth, format));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManager::CreateTexture3D");
    throw;
  }
}

Texture RenderManager::CreateTextureCubemap (size_t size, PixelFormat format)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->CreateTexture (TextureDimension_Cubemap, size, size, size, format));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManager::CreateTextureCubemap");
    throw;
  }  
}

Material RenderManager::CreateMaterial ()
{
  return Wrappers::Wrap<Material> (impl->CreateMaterial ());
}

ResourceLibrary RenderManager::Load (const char* resource_name)
{
  return Wrappers::Wrap<ResourceLibrary> (impl->Load (resource_name));
}

ResourceLibrary RenderManager::Load (const media::rfx::MaterialLibrary& library)
{
  return Wrappers::Wrap<ResourceLibrary> (impl->Load (library));
}

ResourceLibrary RenderManager::Load (const media::rfx::EffectLibrary& library)
{
  return Wrappers::Wrap<ResourceLibrary> (impl->Load (library));
}

ResourceLibrary RenderManager::Load (const media::geometry::MeshLibrary& library)
{
  return Wrappers::Wrap<ResourceLibrary> (impl->Load (library));
}

ResourceLibrary RenderManager::Load (const media::rfx::ShaderLibrary& library)
{
  return Wrappers::Wrap<ResourceLibrary> (impl->Load (library));
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

namespace mid_level
{

void swap (RenderManager& manager1, RenderManager& manager2)
{
  manager1.Swap (manager2);
}

}

}
