#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

enum TextureManagerDataTable
{
  TextureManagerDataTable_Texture0,
  TextureManagerDataTable_Texture1,
  TextureManagerDataTable_Texture2,
  TextureManagerDataTable_Texture3,
  TextureManagerDataTable_Texture4,
  TextureManagerDataTable_Texture5,
  TextureManagerDataTable_Texture6,
  TextureManagerDataTable_Texture7,
  TextureManagerDataTable_Sampler0,
  TextureManagerDataTable_Sampler1,
  TextureManagerDataTable_Sampler2,
  TextureManagerDataTable_Sampler3,
  TextureManagerDataTable_Sampler4,
  TextureManagerDataTable_Sampler5,
  TextureManagerDataTable_Sampler6,
  TextureManagerDataTable_Sampler7,

  TextureManagerDataTable_Num
};

///????
OpenGLTextureTarget get_OpenGLTextureTarget (GLenum tex_target)
{
  switch (tex_target)
  {
    case GL_TEXTURE_1D:            return OpenGLTextureTarget_Texture1D;
    case GL_TEXTURE_2D:            return OpenGLTextureTarget_Texture2D;
    case GL_TEXTURE_RECTANGLE_EXT: return OpenGLTextureTarget_TextureRectangle;
    case GL_TEXTURE_3D:            return OpenGLTextureTarget_Texture3D;
    case GL_TEXTURE_CUBE_MAP_ARB:  return OpenGLTextureTarget_TextureCubemap;
    default: RaiseInvalidArgument ("get_OpenGLTextureTarget", "tex_target"); return OpenGLTextureTarget_Texture2D;
  }
}

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
///Биндинг текстур и сэмплеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание текстуры и сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*       CreateTexture      (const TextureDesc&);
    ISamplerState*  CreateSamplerState (const SamplerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущей текстуры и сэмплера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetTexture (size_t sampler_slot, ITexture* texture);
    void           SetSampler (size_t sampler_slot, ISamplerState* state);
    ITexture*      GetTexture (size_t sampler_slot) const;
    ISamplerState* GetSampler (size_t sampler_slot) const;

  private:
    ITexture* CreateTexture1D      (const TextureDesc&);
    ITexture* CreateTexture2D      (const TextureDesc&);
    ITexture* CreateTexture3D      (const TextureDesc&);
    ITexture* CreateTextureCubemap (const TextureDesc&);

  private:    
    typedef xtl::trackable_ptr<BindableTexture> TexturePtr;
    typedef xtl::trackable_ptr<SamplerState>    SamplerStatePtr;
    
/*    struct Sampler
    {
      TexturePtr      texture; //текстура
      SamplerStatePtr state;   //состояние сэмплера
    };*/

  public:
    TextureManager&          texture_manager;                                //менеджер текстур
    size_t                   max_texture_size;                               //максимальный размер текстуры для устройства
    size_t                   max_rectangle_texture_size;                     //максимальный размер текстуры со сторонами не степени 2 для устройства
    size_t                   max_cube_map_texture_size;                      //максимальный размер cubemap текстуры
    size_t                   max_3d_texture_size;                            //максимальный размер 3d текстуры
    size_t                   texture_units_count;                            //количество слотов мультитекстурирования
    TexturePtr               binded_textures[DEVICE_SAMPLER_SLOTS_COUNT];    //подключенные текстуры
    SamplerStatePtr          binded_samplers[DEVICE_SAMPLER_SLOTS_COUNT];    //подключенные сэмплеры
    GLenum                   binded_tex_targets[DEVICE_SAMPLER_SLOTS_COUNT]; //включенные gl тарджеты//????????!!!!!!!!
    ExtensionsPtr            extensions;                                     //поддерживаемые расширения
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
    texture_units_count (1)
{
    //установка текущего контекста

  MakeContextCurrent ();
  
    //получение поддерживаемых расширений
    
  extensions = ExtensionsPtr (new TextureExtensions (context_manager));

    //запрос максимальных размеров текстур / количества текстурных юнитов

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (extensions->has_ext_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (extensions->has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (extensions->has_ext_texture3d)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);
  if (extensions->has_arb_multitexture)      glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);

  if (texture_units_count > DEVICE_SAMPLER_SLOTS_COUNT)
    texture_units_count = DEVICE_SAMPLER_SLOTS_COUNT;
    
    //сброс таблицы

  memset (binded_tex_targets, 0, sizeof (binded_tex_targets));

    //проверка ошибок
  
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   Биндинг текстур и сэмплеров
*/

void TextureManager::Impl::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::Bind";
  
    //выбор текущего контекста

  MakeContextCurrent ();

    //установка текстур и сэмплеров

  for (size_t i = 0; i < texture_units_count; i++)
  {
    BindableTexture* texture = binded_textures [i].get ();
    SamplerState*    sampler = binded_samplers [i].get ();
    
    if (!texture || !sampler)
    {
      if (extensions->has_arb_multitexture)
      {
        if (glActiveTexture) glActiveTexture    (GL_TEXTURE0 + i);
        else                 glActiveTextureARB (GL_TEXTURE0 + i);
      }

      if (binded_tex_targets [i])
      {
        glDisable (binded_tex_targets [i]);
      
        binded_tex_targets [i] = 0;
      }
      
      continue;
    }

    if (extensions->has_arb_multitexture)
    {
      if (glActiveTexture) glActiveTexture    (GL_TEXTURE0 + i);
      else                 glActiveTextureARB (GL_TEXTURE0 + i);
    }
    
      //переделать!!!!!
      
    RenderTargetTextureDesc tex_desc;
    
    texture->GetDesc (tex_desc);
    
    GLenum tex_target = tex_desc.target;
    size_t tex_id     = tex_desc.id;

    if (binded_tex_targets [i] != tex_target)
    {
      binded_tex_targets [i] = tex_target;

      glEnable (binded_tex_targets [i]);
    }
    
       //доделать!!!!!!!

    if (GetContextData (ContextDataTable_TextureManager, TextureManagerDataTable_Texture0 + i) != texture->GetId ())
    {
      SetContextData (ContextDataTable_TextureManager, TextureManagerDataTable_Texture0 + i, texture->GetId ());

      glBindTexture (tex_target, tex_id);
    }

//    if (GetContextData (ContextDataTable_TextureManager, TextureManagerDataTable_Sampler0 + i) != sampler->GetId ())
    {
//      SetContextData (ContextDataTable_TextureManager, TextureManagerDataTable_Sampler0 + i, sampler->GetId ());

      sampler->Bind (get_OpenGLTextureTarget (tex_target));
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
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

ITexture* TextureManager::Impl::CreateTexture1D (const TextureDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture1D";
  
    //игнорирование неиспользуемых параметров
  
  TextureDesc desc = in_desc;
  
  desc.height = desc.layers = 1;
  
    //проверка возможности создания текстуры

  if (desc.width > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_texture_size);

    //диспетчеризация создания текстуры в зависимости от поддерживаемых расширений

  bool is_pot = is_power_of_two (desc.width);    

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new Texture1D (GetContextManager (), extensions, desc);

  if (extensions->has_ext_texture_rectangle)
  {
    if (desc.width > max_rectangle_texture_size)
      RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_rectangle_texture_size);

    return new TextureNpot (GetContextManager (), extensions, desc);
  }

  return new ScaledTexture (GetContextManager (), texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture2D (const TextureDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture2D";
  
    //игнорирование неиспользуемых параметров
  
  TextureDesc desc = in_desc;
  
  desc.layers = 1;  
  
    //проверка возможности создания текстуры

  if (desc.width > max_texture_size || desc.height > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 2D texture %ux%u (maximum texture size is %u)", desc.width, desc.height, max_texture_size);

    //диспетчеризация создания текстуры в зависимости от поддерживаемых расширений

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new Texture2D (GetContextManager (), extensions, desc);

  if (extensions->has_ext_texture_rectangle && !is_compressed (desc.format) && !desc.generate_mips_enable)
    return new TextureNpot (GetContextManager (), extensions, desc);

  return new ScaledTexture (GetContextManager (), texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture3D (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture3D";

    //проверка возможности создания текстуры

  if (!extensions->has_ext_texture3d)
    RaiseNotSupported (METHOD_NAME, "3D textures not supported (GL_ext_texture_3d not supported)");
    
  if (desc.width > max_3d_texture_size || desc.height > max_3d_texture_size || desc.layers > max_3d_texture_size)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u (max_edge_size=%u)", desc.width, desc.height,
                       desc.layers, max_3d_texture_size);
  }

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height) && is_power_of_two (desc.layers);

  if (!is_pot && !extensions->has_arb_texture_non_power_of_two)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                       desc.width, desc.height, desc.layers, get_name (desc.format));
  }
  
    //создание текстуры

  return new Texture3D (GetContextManager (), extensions, desc);
}

ITexture* TextureManager::Impl::CreateTextureCubemap (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTextureCubemap";
  
    //проверка возможности создания текстуры
  
  if (!extensions->has_arb_texture_cube_map)
    RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");

  if (desc.width > max_cube_map_texture_size || desc.height > max_cube_map_texture_size)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create cubemap texture %ux%u (max_edge_size=%u)", desc.width, desc.height,
                       max_cube_map_texture_size);
  }
  
    //диспетчеризация создания текстуры в зависимости от поддерживаемых расширений

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new TextureCubemap (GetContextManager (), extensions, desc);

  return new ScaledTexture (GetContextManager (), texture_manager, desc);  
}

//диспетчеризация создания текстур  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& desc)
{
  try
  {
    switch (desc.dimension)
    {
      case TextureDimension_1D:      return CreateTexture1D (desc);
      case TextureDimension_2D:      return CreateTexture2D (desc);
      case TextureDimension_3D:      return CreateTexture3D (desc);
      case TextureDimension_Cubemap: return CreateTextureCubemap (desc);
      default:
        RaiseInvalidArgument ("", "desc.dimension", desc.dimension);
        return 0;
    }
  }
  catch (common::Exception& e)
  {
    e.Touch ("render::low_level::opengl::TextureManager::Impl::CreateTexture");

    throw;
  }
}

ISamplerState* TextureManager::Impl::CreateSamplerState (const SamplerDesc& in_desc)
{
  return new SamplerState (GetContextManager (), in_desc);
}

/*
   Установка текущей текстуры и сэмплера
*/

void TextureManager::Impl::SetTexture (size_t sampler_slot, ITexture* texture)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set texture in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);
  
  binded_textures [sampler_slot] = cast_object<BindableTexture> (*this, texture, METHOD_NAME, "state");
}

void TextureManager::Impl::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set sampler in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  binded_samplers [sampler_slot] = cast_object<SamplerState> (*this, state, METHOD_NAME, "state");
}

ITexture* TextureManager::Impl::GetTexture (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get texture from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return binded_textures [sampler_slot].get ();
}

ISamplerState* TextureManager::Impl::GetSampler (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get sampler from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return binded_samplers [sampler_slot].get ();
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
   Биндинг текстур и сэмплеров
*/

void TextureManager::Bind ()
{
  impl->Bind ();
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
  impl->SetTexture (sampler_slot, texture);
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  impl->SetSampler (sampler_slot, state);
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  return impl->GetTexture (sampler_slot);
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  return impl->GetSampler (sampler_slot);
}
