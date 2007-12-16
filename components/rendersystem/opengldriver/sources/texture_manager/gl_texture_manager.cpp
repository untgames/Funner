#include "shared.h"
#include <stdio.h> //!!!!!!!!!!!!!

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Описание реализации TextureManager
*/

struct TextureManager::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager);

  public:
    GLint max_texture_size;
};

TextureManager::Impl::Impl (const ContextManager& context_manager) : ContextObject (context_manager) 
{
  MakeContextCurrent ();
  glEnable (GL_TEXTURE_2D);
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}


/*
   Конструктор / деструктор
*/

TextureManager::TextureManager (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
  {}

TextureManager::~TextureManager ()
{
}
    
/*
   Создание текстуры и сэмплера
*/

ITexture* TextureManager::CreateTexture (const TextureDesc& tex_desc)
{
  if ((int)tex_desc.width > impl->max_texture_size)
    RaiseOutOfRange ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.width", (int)tex_desc.width, 2, impl->max_texture_size);
  else if ((int)tex_desc.height > impl->max_texture_size)
    RaiseOutOfRange ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.height", (int)tex_desc.height, 2, impl->max_texture_size);

  if (tex_desc.generate_mips_enable)
    RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateTexture");  

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      if (tex_desc.width%2)
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.width");
      return new Texture1D (impl->GetContextManager (), tex_desc);
    }
    case TextureDimension_2D:
    {
      if (tex_desc.width%2)
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.width");
      if (tex_desc.height%2)
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.height");
      return new Texture2D (impl->GetContextManager (), tex_desc);
    }
    case TextureDimension_3D: 
    {
      RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateTexture"); 
      break;
    }
    case TextureDimension_Cubemap: RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateTexture"); break;
    default: RaiseInvalidArgument ("render::low_level::opengl::TextureManager::CreateTexture", "tex_desc.dimension", tex_desc.dimension);
  }

  return 0;
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateSamplerState");
  return 0;
}

/*
   Установка текущей текстуры и сэмплера
*/

void TextureManager::SetTexture (size_t sampler_slot, ITexture* texture)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetTexture");
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetSampler");
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetTexture");
  return 0;
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetSampler");
  return 0;
}
