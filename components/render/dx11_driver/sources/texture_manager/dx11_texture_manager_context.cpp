#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    TextureManagerContextState
*/

namespace
{

typedef xtl::com_ptr<Texture>      TexturePtr;
typedef xtl::com_ptr<SamplerState> SamplerStatePtr;

/// Слот текстура - сэмплер
struct SamplerSlot
{
  TexturePtr              texture;
  DxShaderResourceViewPtr texture_view; 
  SamplerStatePtr         sampler;
};

}

struct TextureManagerContextState::Impl: public DeviceObject
{  
  SamplerSlot samplers [DEVICE_SAMPLER_SLOTS_COUNT]; //сэмплеры
  bool        is_dirty;                              //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , is_dirty (true)
  {
  }

/// Десктруктор
  virtual ~Impl () {}

/// Оповещение об изменении состояния
  void UpdateNotify ()
  {
    is_dirty = true;
  }
};

/*
    Конструктор / деструктор
*/

TextureManagerContextState::TextureManagerContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

TextureManagerContextState::TextureManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

TextureManagerContextState::~TextureManagerContextState ()
{
}

/*
    Ссылка на реализацию
*/

TextureManagerContextState::Impl& TextureManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    Установка текущей текстуры и сэмплера
*/

void TextureManagerContextState::SetTexture (size_t sampler_slot, ITexture* in_texture)
{
  try
  {
    Texture* texture = cast_object<Texture> (*impl, in_texture, "", "texture");

    if (sampler_slot >= DEVICE_SAMPLER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "sampler_slot", sampler_slot, DEVICE_SAMPLER_SLOTS_COUNT);

    SamplerSlot& slot = impl->samplers [sampler_slot];

    slot.texture      = texture;
    slot.texture_view = &texture->GetShaderResourceView ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::SetTexture");
    throw;
  }
}

void TextureManagerContextState::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  try
  {
    SamplerState* sampler = cast_object<SamplerState> (*impl, state, "", "state");

    if (sampler_slot >= DEVICE_SAMPLER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "sampler_slot", sampler_slot, DEVICE_SAMPLER_SLOTS_COUNT);

    SamplerSlot& slot = impl->samplers [sampler_slot];

    slot.sampler = sampler;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::SetSampler");
    throw;
  }
}

ITexture* TextureManagerContextState::GetTexture (size_t sampler_slot) const
{
  try
  {
    if (sampler_slot >= DEVICE_SAMPLER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "sampler_slot", sampler_slot, DEVICE_SAMPLER_SLOTS_COUNT);

    return impl->samplers [sampler_slot].texture.get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::GetTexture");
    throw;
  }
}

ISamplerState* TextureManagerContextState::GetSampler (size_t sampler_slot) const
{
  try
  {
    if (sampler_slot >= DEVICE_SAMPLER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "sampler_slot", sampler_slot, DEVICE_SAMPLER_SLOTS_COUNT);

    return impl->samplers [sampler_slot].sampler.get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::GetSampler");
    throw;
  }
}

/*
    Копирование состояния
*/

void TextureManagerContextState::CopyTo (const StateBlockMask& mask, TextureManagerContextState& dst_state) const
{
  try
  {
    bool update_notify = false;

    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      const SamplerSlot& src_slot = impl->samplers [i];
      SamplerSlot&       dst_slot = dst_state.impl->samplers [i];

      if (mask.ss_textures [i] && src_slot.texture != dst_slot.texture)
      {
        dst_slot.texture      = src_slot.texture;
        dst_slot.texture_view = src_slot.texture_view;
        update_notify         = true;
      }

      if (mask.ss_samplers [i] && src_slot.sampler != dst_slot.sampler)
      {
        dst_slot.sampler = src_slot.sampler;
        update_notify    = true;
      }
    }

    if (update_notify)
      dst_state.impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::CopyTo");
    throw;
  }
}

/*
    TextureManagerContext
*/

struct TextureManagerContext::Impl: public TextureManagerContextState::Impl
{
  DxContextPtr context;  //низкоуровневый контекст

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : TextureManagerContextState::Impl (device_manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::TextureManagerContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

TextureManagerContext::TextureManagerContext (const DeviceManager& device_manager, const DxContextPtr& context)
  : TextureManagerContextState (new Impl (device_manager, context))
{
}

TextureManagerContext::~TextureManagerContext ()
{
}

/*
    Ссылка на реализацию
*/

TextureManagerContext::Impl& TextureManagerContext::GetImpl () const
{
  return static_cast<TextureManagerContext::Impl&> (TextureManagerContextState::GetImpl ());
}

/*
    Генерация мип-уровней текстуры (необходимо для текстур в которые ведется рендеринг)
*/

void TextureManagerContext::GenerateMips (ITexture* in_texture)
{
  try
  {
    Impl& impl = GetImpl ();

    Texture* texture = cast_object<Texture> (impl, in_texture, "", "texture");

    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
  
    impl.context->GenerateMips (&texture->GetShaderResourceView ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::GenerateMips");
    throw;
  }
}

/*
    Биндинг
*/

void TextureManagerContext::Bind ()
{
  try
  {
      //проверка флага "грязности"

    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

      //преобразование контекстной информации

    ID3D11SamplerState*       samplers [DEVICE_SAMPLER_SLOTS_COUNT];
    ID3D11ShaderResourceView* texture_views [DEVICE_SAMPLER_SLOTS_COUNT];

    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      SamplerSlot& slot = impl.samplers [i];

      samplers [i]      = slot.sampler ? &slot.sampler->GetHandle () : (ID3D11SamplerState*)0;
      texture_views [i] = slot.texture_view ? slot.texture_view.get () : (ID3D11ShaderResourceView*)0;
    }

      //установка в контекст

    ID3D11DeviceContext& context = *impl.context;

    context.GSSetShaderResources (0, DEVICE_SAMPLER_SLOTS_COUNT, texture_views);
    context.GSSetSamplers        (0, DEVICE_SAMPLER_SLOTS_COUNT, samplers);
    context.HSSetShaderResources (0, DEVICE_SAMPLER_SLOTS_COUNT, texture_views);
    context.HSSetSamplers        (0, DEVICE_SAMPLER_SLOTS_COUNT, samplers);
    context.PSSetShaderResources (0, DEVICE_SAMPLER_SLOTS_COUNT, texture_views);
    context.PSSetSamplers        (0, DEVICE_SAMPLER_SLOTS_COUNT, samplers);
    context.VSSetShaderResources (0, DEVICE_SAMPLER_SLOTS_COUNT, texture_views);
    context.VSSetSamplers        (0, DEVICE_SAMPLER_SLOTS_COUNT, samplers);

      //очистка флага "грязности"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TextureManagerContextState::Bind");
    throw;
  }
}
