#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

struct TextureManagerExtensions
{
  bool has_ext_texture_rectangle;        //GL_EXT_texture_rectangle
  bool has_ext_texture3D;                //GL_EXT_texture3D
  bool has_ext_texture3D_extension;      //GL_EXT_texture3D
  bool has_ext_texture_compression_s3tc; //GL_EXT_texture_compression_s3tc
  bool has_ext_packed_depth_stencil;     //GL_EXT_packed_depth_stencil
  bool has_arb_texture_compression;      //GL_ARB_texture_compression
  bool has_arb_texture_cube_map;         //GL_ARB_texture_cubemap
  bool has_arb_texture_non_power_of_two; //GL_ARB_texture_non_power_of_two
  bool has_arb_depth_texture;            //GL_ARB_depth_texture
  
  TextureManagerExtensions (const ContextManager& manager)
  {
    static Extension EXT_texture_rectangle        = "GL_EXT_texture_rectangle",
                     EXT_texture3D                = "GL_EXT_texture3D",
                     EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc",
                     EXT_texture_cube_map         = "GL_EXT_texture_cube_map",
                     EXT_packed_depth_stencil     = "GL_EXT_packed_depth_stencil",
                     EXT_framebuffer_object       = "GL_EXT_framebuffer_object",
                     NV_texture_rectangle         = "GL_NV_texture_rectangle",
                     ARB_texture_cube_map         = "GL_ARB_texture_cube_map",
                     ARB_texture_compression      = "GL_ARB_texture_compression",
                     ARB_texture_non_power_of_two = "GL_ARB_texture_non_power_of_two",
                     ARB_depth_texture            = "GL_ARB_depth_texture",
                     Version_1_2                  = "GL_VERSION_1_2",
                     Version_1_3                  = "GL_VERSION_1_3",
                     Version_1_4                  = "GL_VERSION_1_4",
                     Version_2_0                  = "GL_VERSION_2_0";
      
    has_arb_texture_compression      = manager.IsSupported (ARB_texture_compression);
    has_ext_texture_rectangle        = manager.IsSupported (EXT_texture_rectangle) || manager.IsSupported (NV_texture_rectangle);
    has_ext_texture3D_extension      = manager.IsSupported (EXT_texture3D);
    has_ext_texture3D                = has_ext_texture3D_extension || manager.IsSupported (Version_1_2);
    has_ext_texture_compression_s3tc = (has_arb_texture_compression || manager.IsSupported (Version_1_3)) && 
                                       manager.IsSupported (EXT_texture_compression_s3tc);
    has_ext_packed_depth_stencil     = manager.IsSupported (EXT_packed_depth_stencil) && manager.IsSupported (EXT_framebuffer_object);  
    has_arb_texture_cube_map         = manager.IsSupported (ARB_texture_cube_map) || manager.IsSupported (EXT_texture_cube_map) ||
                                       manager.IsSupported (Version_1_3);
    has_arb_texture_non_power_of_two = manager.IsSupported (ARB_texture_non_power_of_two) || manager.IsSupported (Version_2_0);
    has_arb_depth_texture            = manager.IsSupported (ARB_depth_texture) || manager.IsSupported (Version_1_4);
  }
};

}

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
    GLint max_cube_map_texture_size;   //максимальный размер cubemap текстуры
    GLint max_3d_texture_size;         //максимальный размер 3d текстуры
};


/*
   Конструктор
*/

TextureManager::Impl::Impl (const ContextManager& context_manager) 
  : ContextObject (context_manager), max_cube_map_texture_size (0), max_3d_texture_size (0)
{
  MakeContextCurrent ();

  TextureManagerExtensions ext (context_manager);

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);
  if (ext.has_ext_texture_rectangle)
    glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &max_rectangle_texture_size);
  else
    max_rectangle_texture_size = max_texture_size;
  if (ext.has_arb_texture_cube_map)
    glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &max_cube_map_texture_size);
  if (ext.has_ext_texture3D)
    glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, &max_3d_texture_size);
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   Создание текстуры и сэмплера
*/
  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& tex_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture";

  if (((int)tex_desc.width > max_texture_size) || ((int)tex_desc.height > max_texture_size))
    RaiseNotSupported (METHOD_NAME, "Can't create texture %u*%u. Reason: maximum texture size is %u*%u", 
                       tex_desc.width, tex_desc.height, max_texture_size, max_texture_size);
  
  MakeContextCurrent ();

  TextureManagerExtensions ext (GetContextManager ());

  GLint width;
  TextureDesc temp_desc = tex_desc;            

  if (((tex_desc.format == PixelFormat_D16) || (tex_desc.format == PixelFormat_D24X8)) && !ext.has_arb_depth_texture)
    RaiseNotSupported (METHOD_NAME, "Can't create depth texture. Reason: GL_ARB_depth_texture extension not supported");

  if ((tex_desc.format == PixelFormat_D24S8) && !ext.has_ext_packed_depth_stencil)
    RaiseNotSupported (METHOD_NAME, "Can't create depth-stencil texture. Reason: GL_EXT_packed_depth_stencil extension not supported");

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      temp_desc.height = 1;
      temp_desc.layers = 1;

      if (!ext.has_arb_texture_non_power_of_two)
        if ((tex_desc.width - 1) & tex_desc.width) 
        {
          if (ext.has_ext_texture_rectangle)
            return new TextureNPOT (GetContextManager (), temp_desc);
          else
          {
            temp_desc.width = next_higher_power_of_two (tex_desc.width);

            LogPrintf ("Non power of two textures not supported by hardware. Scaled texture created.");
            return new TextureEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width, 1);
          }
        }

      return new Texture1D (GetContextManager (), temp_desc);
    }
    case TextureDimension_2D:
    {
      temp_desc.layers = 1;
      
      if (!ext.has_arb_texture_non_power_of_two)
      {
        if (((tex_desc.width - 1) & tex_desc.width) || ((tex_desc.height - 1) & tex_desc.height))
          if (ext.has_ext_texture_rectangle && !is_compressed_format (tex_desc.format))
            return new TextureNPOT (GetContextManager (), temp_desc);
          else
          {
            temp_desc.width  = next_higher_power_of_two (tex_desc.width);
            temp_desc.height = next_higher_power_of_two (tex_desc.height);

            LogPrintf ("Non power of two textures not supported by hardware. Scaled texture created.\n");
            return new TextureEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width, (float)temp_desc.height / (float)tex_desc.height);
          }
      }

      return new Texture2D (GetContextManager (), temp_desc);
    }
    case TextureDimension_3D: 
    {
      if (ext.has_ext_texture3D)
      {
        if (is_depth_format (tex_desc.format))
          RaiseNotSupported (METHOD_NAME, "Can't create depth 3d texture. Reason: depth texture may be only 1d or 2d");

        if (tex_desc.width < 1)
          RaiseOutOfRange (METHOD_NAME, "tex_desc.width", (int)tex_desc.width, 1, max_3d_texture_size);
        
        if (tex_desc.height < 1)
          RaiseOutOfRange (METHOD_NAME, "tex_desc.height", (int)tex_desc.height, 1, max_3d_texture_size);
        
        if (tex_desc.layers < 1)
          RaiseOutOfRange (METHOD_NAME, "tex_desc.layers", (int)tex_desc.layers, 1, max_3d_texture_size);
        
        if ((tex_desc.width & 3) && is_compressed_format (tex_desc.format))
          RaiseInvalidArgument (METHOD_NAME, "tex_desc.width", tex_desc.width,
                                "Texture width for compressed image must be a multiple 4");
        
        if ((tex_desc.height & 3) && is_compressed_format (tex_desc.format))
          RaiseInvalidArgument (METHOD_NAME, "tex_desc.height", tex_desc.height,
                                "Texture height for compressed image must be a multiple 4");
        
        if (!ext.has_arb_texture_non_power_of_two)
        {
          if (((tex_desc.width - 1) & tex_desc.width) || ((tex_desc.height - 1) & tex_desc.height))
          {
            temp_desc.width  = next_higher_power_of_two (tex_desc.width);
            temp_desc.height = next_higher_power_of_two (tex_desc.height);

            if (is_compressed_format (tex_desc.format))
            { 
              if (ext.has_ext_texture_compression_s3tc)
              {
                if (ext.has_arb_texture_compression) glCompressedTexImage3DARB (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers, 
                                                                                0, ((temp_desc.width * temp_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
                else                                 glCompressedTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers, 
                                                                                0, ((temp_desc.width * temp_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
              }
              else
              {
                if (ext.has_ext_texture3D_extension) glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, unpack_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers, 
                                                                      0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
                else                                 glTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, unpack_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers,
                                                                      0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);                 
              }
            }
            else
            {
                if (ext.has_ext_texture3D_extension) glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers, 0, 
                                                                      gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
                else                                 glTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), temp_desc.width, temp_desc.height, tex_desc.layers, 0, 
                                                                      gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
            }

            glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_WIDTH, &width);
        
            if (!width)
              Raise <Exception> (METHOD_NAME, "Not enough space to create 3d texture with width = %u, height = %u and layers = %u", 
                                 temp_desc.width, temp_desc.height, temp_desc.layers);

            CheckErrors (METHOD_NAME);
            LogPrintf ("Non power of two textures not supported by hardware. Scaled 3d texture created.\n");
            return new Texture3DEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width, (float)temp_desc.height / (float)tex_desc.height);
          }
        }

        if (is_compressed_format (tex_desc.format))
        {
          if (ext.has_ext_texture_compression_s3tc)
          {
            if (ext.has_arb_texture_compression) glCompressedTexImage3DARB (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers,
                                                                            0, ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
            else                                 glCompressedTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers,
                                                                            0, ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);       //???? Image size may be must multiplied by depth
          }
          else
          {
            if (ext.has_ext_texture3D_extension) glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, unpack_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                                                                  unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
            else                                 glTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, unpack_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                                                                  unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
          }
        }
        else
        {
          if (ext.has_ext_texture3D_extension) glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                                                                gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
          else                                 glTexImage3D    (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                                                                gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
        }

        glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_WIDTH, &width);

        if (!width)
          Raise <Exception> (METHOD_NAME, "Not enough space to create 3d texture with width = %u, height = %u and layers = %u",
                             tex_desc.width, tex_desc.height, tex_desc.layers);

        
        CheckErrors (METHOD_NAME);
        return new Texture3D (GetContextManager (), temp_desc);
      }
      else
        RaiseNotSupported (METHOD_NAME, "3D textuers not supported. No 'EXT_texture3D' extension."); 
    }
    case TextureDimension_Cubemap:
    {
      if (ext.has_arb_texture_cube_map)
      {
        if (!ext.has_arb_texture_non_power_of_two)
        {
          if (((tex_desc.width - 1) & tex_desc.width) || ((tex_desc.height - 1) & tex_desc.height))
          {
            temp_desc.width = temp_desc.height = next_higher_power_of_two (tex_desc.width);

            LogPrintf ("Non power of two textures not supported by hardware. Scaled  cubemap texture created.\n");
            return new TextureCubemapEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width);
          }
        }

        return new TextureCubemap (GetContextManager (), temp_desc);
      }
      else
        RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension."); 
    }
    default: RaiseInvalidArgument (METHOD_NAME, "tex_desc.dimension", tex_desc.dimension);
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
