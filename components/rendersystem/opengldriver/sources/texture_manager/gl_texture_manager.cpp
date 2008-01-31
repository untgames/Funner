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
    has_ext_packed_depth_stencil     = manager.IsSupported (EXT_packed_depth_stencil);  
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

bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}
  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& tex_desc)
{
    //добавить общий код замены нулевых размеров дескриптора текстуры на единичные!!!!

  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture";
  
    //установка текущего контекста

  MakeContextCurrent ();

  TextureManagerExtensions ext (GetContextManager ());    
    
  TextureDesc temp_desc = tex_desc; ////////????????

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      temp_desc.height = 1;
      temp_desc.layers = 1;
      
      bool is_pot = is_power_of_two (tex_desc.width);

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new Texture1D (GetContextManager (), temp_desc);

      if (ext.has_ext_texture_rectangle) ///compressed?????
        return new TextureNPOT (GetContextManager (), temp_desc);

      temp_desc.width = next_higher_power_of_two (tex_desc.width);

      LogPrintf ("Non power of two textures not supported by hardware. Scaled texture created.");
      
      return new TextureEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width, 1);
    }
    case TextureDimension_2D:
    {
      temp_desc.layers = 1;

      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height);

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new Texture2D (GetContextManager (), temp_desc);      

      if (ext.has_ext_texture_rectangle && !is_compressed_format (tex_desc.format))
        return new TextureNPOT (GetContextManager (), temp_desc);

      temp_desc.width  = next_higher_power_of_two (tex_desc.width);
      temp_desc.height = next_higher_power_of_two (tex_desc.height);

      LogPrintf ("Non power of two textures not supported by hardware. Scaled texture created.\n");

      return new TextureEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width, (float)temp_desc.height / (float)tex_desc.height);
    }
    case TextureDimension_3D: 
    {
        //проверка наличия расширений, необходимых для создания трёхмерной текстуры

      if (!ext.has_ext_texture3D) //перенести в Texture3D!!!
        RaiseNotSupported (METHOD_NAME, "3D textures not supported (GL_EXT_texture3D not supported)");

      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height) && is_power_of_two (tex_desc.layers);

      if (!is_pot && !ext.has_arb_texture_non_power_of_two)
      {
        RaiseNotSupported (METHOD_NAME, "Can't create texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                           tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));
      }

      return new Texture3D (GetContextManager (), tex_desc);
    }
    case TextureDimension_Cubemap:
    {
      if (!ext.has_arb_texture_cube_map) //перенести в TextureCubemap!!!
        RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");
        
      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height);

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new TextureCubemap (GetContextManager (), temp_desc);      

      temp_desc.width  = next_higher_power_of_two (tex_desc.width);
      temp_desc.height = next_higher_power_of_two (tex_desc.height);

      LogPrintf ("Non power of two textures not supported by hardware. Scaled  cubemap texture created.\n");

      return new TextureCubemapEmulatedNPOT (GetContextManager (), temp_desc, (float)temp_desc.width / (float)tex_desc.width);
    }
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.dimension", tex_desc.dimension);
      return 0;
  }
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
