#include "shared.h"

using namespace render;

//TODO: createshared resource: if not resouce - create proxy

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
  return Wrappers::Wrap<Primitive> (impl->PrimitiveManager ().CreatePrimitive ());
}

Primitive RenderManager::CreatePrimitive (const PrimitiveBuffers& buffers)
{
  return Wrappers::Wrap<Primitive> (impl->PrimitiveManager ().CreatePrimitive (Wrappers::Unwrap<PrimitiveBuffersImpl> (buffers)));
}

PrimitiveBuffers RenderManager::CreatePrimitiveBuffers (MeshBufferUsage lines_usage, MeshBufferUsage sprites_usage)
{
  return Wrappers::Wrap<PrimitiveBuffers> (impl->PrimitiveManager ().CreatePrimitiveBuffers (lines_usage, sprites_usage));
}

Entity RenderManager::CreateEntity ()
{
  return Wrappers::Wrap<Entity> (impl->CreateEntity ());
}

Frame RenderManager::CreateFrame ()
{
  return Wrappers::Wrap<Frame> (impl->CreateFrame ());
}

Texture RenderManager::CreateTexture (const media::Image& image, bool generate_mips_enable)
{
  return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (image, generate_mips_enable));
}

Texture RenderManager::CreateTexture (const media::Image& image, TextureDimension dimension, bool generate_mips_enable)
{
  return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (image, dimension, generate_mips_enable));
}

Texture RenderManager::CreateTexture (const media::CompressedImage& image)
{
  return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (image));
}

Texture RenderManager::CreateTexture (const media::CompressedImage& image, TextureDimension dimension)
{
  return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (image, dimension));
}

Texture RenderManager::CreateTexture2D (size_t width, size_t height, PixelFormat format, bool generate_mips_enable)
{
  try
  {
    return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (TextureDimension_2D, width, height, 1, format, generate_mips_enable));
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
    return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (TextureDimension_3D, width, height, depth, format, generate_mips_enable));
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
    return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (TextureDimension_Cubemap, size, size, 6, format, generate_mips_enable));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderManager::CreateTextureCubemap");
    throw;
  }  
}

Material RenderManager::CreateMaterial ()
{
  return Wrappers::Wrap<Material> (impl->MaterialManager ().CreateMaterial ());
}

Texture RenderManager::CreateSharedTexture (const char* name)
{
  static const char* METHOD_NAME = "render::RenderManager::CreateSharedTexture";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  TexturePtr texture = impl->TextureManager ().FindTexture (name);

  if (!texture)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Texture not found");

  return Wrappers::Wrap<Texture> (texture);
}

Material RenderManager::CreateSharedMaterial (const char* name)
{
  static const char* METHOD_NAME = "render::RenderManager::CreateSharedMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  MaterialPtr material = impl->MaterialManager ().FindMaterial (name);
  
  if (!material)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Material not found");

  return Wrappers::Wrap<Material> (material);
}

Primitive RenderManager::CreateSharedPrimitive (const char* name)
{
  static const char* METHOD_NAME = "render::RenderManager::CreateSharedMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  PrimitivePtr primitive = impl->PrimitiveManager ().FindPrimitive (name);

  if (!primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Primitive not found");

  return Wrappers::Wrap<Primitive> (primitive);
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
