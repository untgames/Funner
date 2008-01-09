#include "shared.h"

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
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание текстуры и сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*      CreateTexture (const TextureDesc&);    
    ISamplerState* CreateSamplerState (const SamplerDesc&);

  public:
    GLint max_texture_size;
};

/*
   Конструктор
*/

TextureManager::Impl::Impl (const ContextManager& context_manager) : ContextObject (context_manager) 
{
  MakeContextCurrent ();
  glEnable (GL_TEXTURE_2D);
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   Создание текстуры и сэмплера
*/
  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& tex_desc)
{
  if (((int)tex_desc.width > max_texture_size) || ((int)tex_desc.height > max_texture_size))
    RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                       "Can't create texture %u*%u. Reason: maximum texture size is %u*%u", 
                        tex_desc.width, tex_desc.height, max_texture_size, max_texture_size);
  
  MakeContextCurrent ();

  bool has_EXT_texture_compression_s3tc = (GLEW_ARB_texture_compression || GLEW_VERSION_1_3) && GLEW_EXT_texture_compression_s3tc;

  if (!has_EXT_texture_compression_s3tc && is_compressed_format (tex_desc.format))
    RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "DXT texture comression not supported!");

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      if ((tex_desc.width < 2))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", (int)tex_desc.width, 2, max_texture_size);
      if ((tex_desc.width - 1) & tex_desc.width) 
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", tex_desc.width, 
                              "Texture width must be power of 2");
      if (is_compressed_format (tex_desc.format))
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.format");

      GLint width;

      glTexImage1D (GL_PROXY_TEXTURE_1D, 0, gl_internal_format (tex_desc.format), tex_desc.width, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
      
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &width);
      if (!width)
        Raise <Exception> ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                           "Not enough space to create texture with width = %u", tex_desc.width);

      return new Texture1D (GetContextManager (), tex_desc);
    }
    case TextureDimension_2D:
    {
      if ((tex_desc.width < 2))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", (int)tex_desc.width, 2, max_texture_size);
      if ((tex_desc.height < 2))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.height", (int)tex_desc.height, 2, max_texture_size);
      if ((tex_desc.width - 1) & tex_desc.width) 
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", tex_desc.width,
                              "Texture width must be power of 2");
      if ((tex_desc.height - 1) & tex_desc.height) 
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.height", tex_desc.height,
                              "Texture height must be power of 2");
      if ((tex_desc.width & 3) && is_compressed_format (tex_desc.format))
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", tex_desc.width,
                              "Texture width for compressed image must be a multiple 4");
      if ((tex_desc.height & 3) && is_compressed_format (tex_desc.format))
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.height", tex_desc.height,
                              "Texture height for compressed image must be a multiple 4");

      GLint width;

      if (is_compressed_format (tex_desc.format))
        glCompressedTexImage2D (GL_PROXY_TEXTURE_2D, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                                ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
      else
        glTexImage2D (GL_PROXY_TEXTURE_2D, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                      gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
      
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
      if (!width)
        Raise <Exception> ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                           "Not enough space to create texture with width = %u and height = %u", tex_desc.width, tex_desc.height);

      return new Texture2D (GetContextManager (), tex_desc);
    }
    case TextureDimension_3D: 
    {
      RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateTexture"); 
      break;
    }
    case TextureDimension_Cubemap: RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateTexture"); break;
    default: RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.dimension", tex_desc.dimension);
  }

  CheckErrors ("render::low_level::opengl::TextureManager::Impl::CreateTexture");

  return 0;
}

ISamplerState* TextureManager::Impl::CreateSamplerState (const SamplerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateSamplerState");
  return 0;
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
  return impl->CreateTexture (tex_desc);
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc& sampler_desc)
{
  return impl->CreateSamplerState (sampler_desc);
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
