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
                     
    manager.MakeContextCurrent ();
      
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
    Impl (const ContextManager& context_manager, TextureManager& texture_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание текстуры и сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*      CreateTexture      (const TextureDesc&);
    ISamplerState* CreateSamplerState (const SamplerDesc&);

  private:
    ITexture* CreateTexture1D      (const TextureDesc&);
    ITexture* CreateTexture2D      (const TextureDesc&);
    ITexture* CreateTexture3D      (const TextureDesc&);
    ITexture* CreateTextureCubemap (const TextureDesc&);

  public:
    TextureManager&          texture_manager;             //менеджер текстур
    size_t                   max_texture_size;            //максимальный размер текстуры для устройства
    size_t                   max_rectangle_texture_size;  //максимальный размер текстуры со сторонами не степени 2 для устройства
    size_t                   max_cube_map_texture_size;   //максимальный размер cubemap текстуры
    size_t                   max_3d_texture_size;         //максимальный размер 3d текстуры
    TextureManagerExtensions ext;                         //поддерживаемые расширения
};


/*
   Конструктор
*/

TextureManager::Impl::Impl (const ContextManager& context_manager, TextureManager& in_texture_manager)
  : ContextObject (context_manager),
    texture_manager (in_texture_manager),
    max_rectangle_texture_size (0),
    max_cube_map_texture_size (0),
    max_3d_texture_size (0),
    ext (context_manager) //????????????????MakeContextCurrent после
{
    //установка текущего контекста

  MakeContextCurrent ();

    //запрос максимальных размеров текстуры

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (ext.has_ext_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (ext.has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (ext.has_ext_texture3D)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);

    //проверка ошибок
  
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
    Создание текстуры и сэмплера
*/

namespace
{

bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}

}

ITexture* TextureManager::Impl::CreateTexture1D (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture1D";

  bool is_pot = is_power_of_two (desc.width);

  if (desc.width > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_texture_size);

  if (is_pot || ext.has_arb_texture_non_power_of_two)
    return new Texture1D (GetContextManager (), desc);

  if (ext.has_ext_texture_rectangle)
  {
    if (desc.width > max_rectangle_texture_size)
      RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_rectangle_texture_size);

    return new TextureNPOT (GetContextManager (), desc);
  }

  return new ScaledTexture (texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture2D (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture2D";

  if (desc.width > max_texture_size || desc.height > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 2D texture %ux%u (maximum texture size is %u)", desc.width, desc.height, max_texture_size);

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || ext.has_arb_texture_non_power_of_two)
    return new Texture2D (GetContextManager (), desc);      

  if (ext.has_ext_texture_rectangle && !is_compressed_format (desc.format))
    return new TextureNPOT (GetContextManager (), desc);

  return new ScaledTexture (texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture3D (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture3D";

    //проверка наличия расширений, необходимых для создания трёхмерной текстуры

  if (!ext.has_ext_texture3D) //перенести в Texture3D!!!
    RaiseNotSupported (METHOD_NAME, "3D textures not supported (GL_EXT_texture3D not supported)");
    
  if (desc.width > max_3d_texture_size || desc.height > max_3d_texture_size || desc.layers > max_3d_texture_size)
  {
      //уточнить проверку: если layers = 2^n, width , height - not!!!
    
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u (max_edge_size=%u)", desc.width, desc.height,
                       desc.layers, max_3d_texture_size);
  }

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height) && is_power_of_two (desc.layers);

  if (!is_pot && !ext.has_arb_texture_non_power_of_two)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                       desc.width, desc.height, desc.layers, get_name (desc.format));
  }

  return new Texture3D (GetContextManager (), desc);
}

ITexture* TextureManager::Impl::CreateTextureCubemap (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTextureCubemap";
  
  if (!ext.has_arb_texture_cube_map) //перенести в TextureCubemap!!!
    RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");
    
  if (desc.width > max_cube_map_texture_size || desc.height > max_cube_map_texture_size)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create cubemap texture %ux%u (max_edge_size=%u)", desc.width, desc.height,
                       max_cube_map_texture_size);
  }

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || ext.has_arb_texture_non_power_of_two)
    return new TextureCubemap (GetContextManager (), desc);

  return new ScaledTexture (texture_manager, desc);  
}
  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& desc)
{
    //добавить общий код замены нулевых размеров дескриптора текстуры на единичные!!!!

    //диспетчеризация создания текстур

  switch (desc.dimension)
  {
    case TextureDimension_1D:      return CreateTexture1D (desc);
    case TextureDimension_2D:      return CreateTexture2D (desc);
    case TextureDimension_3D:      return CreateTexture3D (desc);
    case TextureDimension_Cubemap: return CreateTextureCubemap (desc);
    default:
      RaiseInvalidArgument ("render::low_level::opengl::TextureManager::Impl::CreateTexture", "desc.dimension", desc.dimension);
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
  : impl (new Impl (context_manager, *this))
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
