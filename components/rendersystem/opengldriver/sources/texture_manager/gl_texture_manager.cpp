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
    GLint max_texture_size;            //максимальный размер текстуры для устройства
    GLint max_rectangle_texture_size;  //максимальный размер текстуры со сторонами не степени 2 для устройства
};

/*
   Конструктор
*/

TextureManager::Impl::Impl (const ContextManager& context_manager) : ContextObject (context_manager) 
{
  MakeContextCurrent ();

  bool has_EXT_texture_rectangle = (GLEW_EXT_texture_rectangle || GLEW_NV_texture_rectangle);

  glEnable (GL_TEXTURE_2D);
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);
  if (has_EXT_texture_rectangle)
    glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &max_rectangle_texture_size);
  
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
  bool has_EXT_texture_rectangle        = (GLEW_EXT_texture_rectangle || GLEW_NV_texture_rectangle);
  bool has_ARB_texture_non_power_of_two = (GLEW_ARB_texture_non_power_of_two || GLEW_VERSION_2_0);

  GLint width;
  TextureDesc temp_desc = tex_desc;            

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      temp_desc.height = 1;
      temp_desc.layers = 1;
      if ((tex_desc.width < 1))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", (int)tex_desc.width, 1, max_texture_size);
      if (is_compressed_format (tex_desc.format))
        RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "1D texture can't be compressed.");
      if (!has_ARB_texture_non_power_of_two)
        if ((tex_desc.width - 1) & tex_desc.width) 
          if (has_EXT_texture_rectangle)
          {
            if (tex_desc.generate_mips_enable)
              RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "Mip maps for non power of two textures not supported.");

            glTexImage2D (GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, 1, 0, 
                          gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
            
            glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_WIDTH, &width);
            if (!width)
              Raise <Exception> ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                                 "Not enough space to create texture with width = %u", tex_desc.width);

            CheckErrors ("render::low_level::opengl::TextureManager::Impl::CreateTexture");

            return new TextureNPOT (GetContextManager (), temp_desc);
          }
          else
            RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", tex_desc.width, 
                                  "Texture width must be power of 2");

      glTexImage1D (GL_PROXY_TEXTURE_1D, 0, gl_internal_format (tex_desc.format), tex_desc.width, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
      
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &width);
      if (!width)
        Raise <Exception> ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                           "Not enough space to create texture with width = %u", tex_desc.width);

      CheckErrors ("render::low_level::opengl::TextureManager::Impl::CreateTexture");
      return new Texture1D (GetContextManager (), temp_desc);
    }
    case TextureDimension_2D:
    {
      temp_desc.layers = 1;
      if ((tex_desc.width < 1))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", (int)tex_desc.width, 1, max_texture_size);
      if ((tex_desc.height < 1))
        RaiseOutOfRange ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.height", (int)tex_desc.height, 1, max_texture_size);
      if ((tex_desc.width & 3) && is_compressed_format (tex_desc.format))
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width", tex_desc.width,
                              "Texture width for compressed image must be a multiple 4");
      if ((tex_desc.height & 3) && is_compressed_format (tex_desc.format))
        RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.height", tex_desc.height,
                              "Texture height for compressed image must be a multiple 4");
      if (!has_ARB_texture_non_power_of_two)
      {
        if (((tex_desc.width - 1) & tex_desc.width) || ((tex_desc.height - 1) & tex_desc.height))
          if (has_EXT_texture_rectangle)
          {
            if (is_compressed_format (tex_desc.format))
              RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "Non power of two texture can't be compressed.");
            if (tex_desc.generate_mips_enable)
              RaiseNotSupported ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "Mip maps for non power of two textures not supported.");

            glTexImage2D (GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                          gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
            
            glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_WIDTH, &width);
            if (!width)
              Raise <Exception> ("render::low_level::opengl::TextureManager::Impl::CreateTexture", 
                                 "Not enough space to create texture with width = %u and height = %u", tex_desc.width, tex_desc.height);

            CheckErrors ("render::low_level::opengl::TextureManager::Impl::CreateTexture");
            return new TextureNPOT (GetContextManager (), temp_desc);
          }
          else
            RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.width, tex_desc.height", tex_desc.width,
                                  "Texture width and height must be power of 2");
      }

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

      CheckErrors ("render::low_level::opengl::TextureManager::Impl::CreateTexture");
      return new Texture2D (GetContextManager (), temp_desc);
    }
    case TextureDimension_3D: 
    {
      RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateTexture"); 
      break;
    }
    case TextureDimension_Cubemap: RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateTexture"); break;
    default: RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "tex_desc.dimension", tex_desc.dimension);
  }

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
