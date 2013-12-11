#include "shared.h"

using namespace render::manager;

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

Window RenderManager::CreateWindow (INativeWindow* window, common::PropertyMap& properties)
{
  return Wrappers::Wrap<render::manager::Window> (impl->CreateWindow (window, properties));
}

Window RenderManager::CreateWindow (syslib::Window& window, common::PropertyMap& properties)
{
  try
  {
    xtl::com_ptr<INativeWindow> native_window (make_native_window (window), false);

    return CreateWindow (native_window.get (), properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderManager::CreateWindow(syslib::Window&,common::PropertyMap&)");
    throw;
  }
}

size_t RenderManager::WindowsCount () const
{
  return impl->WindowsCount ();
}

const Window RenderManager::Window (size_t index) const
{
  return Wrappers::Wrap<render::manager::Window> (impl->Window (index));
}

Window RenderManager::Window (size_t index)
{
  return Wrappers::Wrap<render::manager::Window> (impl->Window (index));
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

PrimitiveBuffers RenderManager::CreatePrimitiveBuffers ()
{
  return Wrappers::Wrap<PrimitiveBuffers> (impl->PrimitiveManager ().CreatePrimitiveBuffers ());
}

Entity RenderManager::CreateEntity ()
{
  return Wrappers::Wrap<Entity> (impl->CreateEntity ());
}

Frame RenderManager::CreateFrame ()
{
  return Wrappers::Wrap<Frame> (impl->CreateFrame ());
}

Texture RenderManager::CreateTexture (const char* name)
{
  return Wrappers::Wrap<Texture> (impl->TextureManager ().CreateTexture (name));
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
    e.touch ("render::manager::RenderManager::CreateTexture2D");
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
    e.touch ("render::manager::RenderManager::CreateTexture3D");
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
    e.touch ("render::manager::RenderManager::CreateTextureCubemap");
    throw;
  }  
}

Material RenderManager::CreateMaterial ()
{
  return Wrappers::Wrap<Material> (impl->MaterialManager ().CreateMaterial ());
}

Texture RenderManager::CreateSharedTexture (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderManager::CreateSharedTexture";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  TexturePtr texture = impl->TextureManager ().FindTexture (name);

  if (!texture)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Texture not found");

  return Wrappers::Wrap<Texture> (texture);
}

Material RenderManager::CreateSharedMaterial (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderManager::CreateSharedMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  MaterialPtr material = impl->MaterialManager ().FindMaterial (name);
  
  if (!material)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Material not found");

  return Wrappers::Wrap<Material> (material);
}

Primitive RenderManager::CreateSharedPrimitive (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderManager::CreateSharedMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  PrimitivePtr primitive = impl->PrimitiveManager ().FindPrimitive (name);

  if (!primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Primitive not found");

  return Wrappers::Wrap<Primitive> (primitive);
}

bool RenderManager::HasSharedTexture (const char* name)
{
  return name && impl->TextureManager ().FindTexture (name) != TexturePtr ();
}

bool RenderManager::HasSharedMaterial (const char* name)
{
  return name && impl->MaterialManager ().FindMaterial (name) != MaterialPtr ();
}

bool RenderManager::HasSharedPrimitive (const char* name)
{
  return name && impl->PrimitiveManager ().FindPrimitive (name) != PrimitivePtr ();
}

void RenderManager::ShareTexture (const char* name, const Texture& texture)
{
  try
  {
    impl->TextureManager ().ShareTexture (name, Wrappers::Unwrap<TextureImpl> (texture));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderManager::ShareTexture");
    throw;
  }
}

void RenderManager::ShareMaterial (const char* name, const Material& material)
{
  try
  {
    impl->MaterialManager ().ShareMaterial (name, Wrappers::Unwrap<MaterialImpl> (material));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderManager::ShareMaterial");
    throw;
  }
}

void RenderManager::SharePrimitive (const char* name, const Primitive& primitive)
{
  try
  {
    impl->PrimitiveManager ().SharePrimitive (name, Wrappers::Unwrap<PrimitiveImpl> (primitive));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderManager::SharePrimitive");
    throw;
  }  
}

void RenderManager::UnshareTexture (const char* name)
{
  impl->TextureManager ().UnloadTexture (name);
}

void RenderManager::UnshareMaterial (const char* name)
{
  impl->MaterialManager ().UnshareMaterial (name);
}

void RenderManager::UnsharePrimitive (const char* name)
{
  impl->PrimitiveManager ().UnsharePrimitive (name);
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

void RenderManager::UpdateCache ()
{
  impl->UpdateCache ();
}

void RenderManager::ResetCache ()
{
  impl->ResetCache ();
}

void RenderManager::SetCacheTimeDelay (size_t milliseconds)
{
  impl->CacheManager ().SetTimeDelay (milliseconds);
}

void RenderManager::SetCacheFrameDelay (size_t frames_count)
{
  impl->CacheManager ().SetFrameDelay (frames_count);
}

size_t RenderManager::CacheTimeDelay () const
{
  return impl->CacheManager ().TimeDelay ();
}

size_t RenderManager::CacheFrameDelay () const
{
  return impl->CacheManager ().FrameDelay ();
}

void RenderManager::ChangeSettings (const common::PropertyMap& settings)
{
  impl->EffectManager ().ChangeSettings (settings);
}

common::ParseNode RenderManager::Configuration () const
{
  return impl->EffectManager ().Configuration ();
}

xtl::connection RenderManager::RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const
{
  return impl->RegisterWindowEventHandler (event, handler);
}

xtl::connection RenderManager::RegisterEventHandler (RenderManagerEvent event, const EventHandler& handler) const
{
  return impl->RegisterEventHandler (event, handler);
}

void RenderManager::SetLogLevel (render::manager::LogLevel level)
{
  impl->Settings ().SetLogLevel (level);
}

LogLevel RenderManager::LogLevel () const
{
  return impl->Settings ().LogLevel ();
}

xtl::trackable& RenderManager::GetTrackable () const
{
  return *impl;
}

void RenderManager::Swap (RenderManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace render
{

namespace manager
{

void swap (RenderManager& manager1, RenderManager& manager2)
{
  manager1.Swap (manager2);
}

xtl::trackable& get_trackable (const RenderManager& manager)
{
  return manager.GetTrackable ();
}

}

}
