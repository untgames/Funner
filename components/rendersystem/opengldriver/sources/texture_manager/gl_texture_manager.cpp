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
    
    struct Sampler
    {
      TexturePtr      texture;       //текстура
      SamplerStatePtr sampler_state; //состояние сэмплера
    };
    
    typedef xtl::array<Sampler, DEVICE_SAMPLER_SLOTS_COUNT> SamplerArray;

  public:
    TextureManager& texture_manager;            //менеджер текстур
    size_t          max_texture_size;           //максимальный размер текстуры для устройства
    size_t          max_rectangle_texture_size; //максимальный размер текстуры со сторонами не степени 2 для устройства
    size_t          max_cube_map_texture_size;  //максимальный размер cubemap текстуры
    size_t          max_3d_texture_size;        //максимальный размер 3d текстуры
    size_t          texture_units_count;        //количество слотов мультитекстурирования
    SamplerArray    samplers;                   //массив сэмплеров
    ExtensionsPtr   extensions;                 //поддерживаемые расширения
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
    
      //вынести в TextureExtensions!!!!

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (extensions->has_ext_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (extensions->has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (extensions->has_ext_texture3d)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);
  if (extensions->has_arb_multitexture)      glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);

  if (texture_units_count > DEVICE_SAMPLER_SLOTS_COUNT)
    texture_units_count = DEVICE_SAMPLER_SLOTS_COUNT;

    //проверка ошибок
  
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   Биндинг текстур и сэмплеров
*/

void TextureManager::Impl::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::Bind";
  
    //получение кэш-переменных

  size_t *cache                  = &GetContextDataTable (Stage_TextureManager)[0],
         *current_texture_id     = cache + TextureManagerCache_TextureId0,
         *current_texture_target = cache + TextureManagerCache_TextureTarget0,
         &current_active_slot    = cache [TextureManagerCache_ActiveSlot];
  
    //выбор текущего контекста

  MakeContextCurrent ();  
  
    //получение указателя на функцию выбора активной текстуры
    
  PFNGLACTIVETEXTUREPROC glActiveTexture_fn = 0;

  if (extensions->has_arb_multitexture)
  {
    if (glActiveTexture) glActiveTexture_fn = glActiveTexture;
    else                 glActiveTexture_fn = glActiveTextureARB;
  }

    //установка текстур и сэмплеров
    
  BindableTextureDesc texture_desc;

  for (size_t i = 0; i < texture_units_count; i++)
  {
    BindableTexture* texture        = samplers [i].texture.get ();
    SamplerState*    sampler_state  = samplers [i].sampler_state.get ();
    bool             is_active_slot = texture && sampler_state;          

    if (is_active_slot)
      texture->GetDesc (texture_desc);

    GLenum texture_target      = is_active_slot ? texture_desc.target : 0;
    bool   need_change_sampler = is_active_slot && texture->GetSamplerHash () != sampler_state->GetDescHash (),
           need_bind           = need_change_sampler || is_active_slot && current_texture_id [i] != texture->GetId (),
           need_change_mode    = current_texture_target [i] != texture_target,
           need_set_slot       = need_bind || need_change_mode;

      //установка активного слота

    if (need_set_slot && current_active_slot != i && glActiveTexture_fn)
    {
      glActiveTexture_fn (GL_TEXTURE0 + i);

      current_active_slot = i;
    }
    
      //биндинг текстуры
      
    if (need_bind)
      texture->Bind ();
      
      //установка сэмплера
      
    if (need_change_sampler)
    {
        //проверка случая установки для одной и той же текстуры различных сэмплеров
        
      for (size_t j=0; j<i; j++)
        if (samplers [j].texture == texture && samplers [j].sampler_state && samplers [j].sampler_state != sampler_state)
        {
          RaiseNotSupported (METHOD_NAME, "Sampler slot #%u conflicts with sampler slot #%u. "
                             "Reason: different sampler states for same texture", j, i);
        }

      sampler_state->Bind (texture_target);

      texture->SetSamplerHash (sampler_state->GetDescHash ());
    }
    
      //изменение режима текстурирования для текущего слота

    if (need_change_mode)
    {
      if (current_texture_target [i]) glDisable (current_texture_target [i]);
      if (texture_target)             glEnable  (texture_target);

      current_texture_target [i] = texture_target;
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
  return new SamplerState (GetContextManager (), extensions, in_desc);
}

/*
   Установка текущей текстуры и сэмплера
*/

void TextureManager::Impl::SetTexture (size_t sampler_slot, ITexture* texture)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set texture in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);
  
  samplers [sampler_slot].texture = cast_object<BindableTexture> (*this, texture, METHOD_NAME, "state");
}

void TextureManager::Impl::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set sampler in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  samplers [sampler_slot].sampler_state = cast_object<SamplerState> (*this, state, METHOD_NAME, "state");
}

ITexture* TextureManager::Impl::GetTexture (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get texture from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return samplers [sampler_slot].texture.get ();
}

ISamplerState* TextureManager::Impl::GetSampler (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get sampler from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return samplers [sampler_slot].sampler_state.get ();
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
