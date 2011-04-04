#include "shared.h"

using namespace render;

/*
    Описание реализации
*/

typedef stl::vector<size_t> TagHashArray;

struct EffectPass::Impl
{
  DeviceManagerPtr             device_manager;          //менеджер устройства отрисовки
  render::SortMode             sort_mode;               //режим сортировки
  LowLevelDepthStencilStatePtr depth_stencil_state;     //состояние уровня отсечения
  LowLevelBlendStatePtr        blend_state;             //состояние уровня смешивания цветов
  LowLevelRasterizerStatePtr   rasterizer_state;        //состояние уровня растеризации
  LowLevelStateBlockPtr        state_block;             //блок состояний
  bool                         state_block_need_update; //блок состояний требует обновления
  common::StringArray          tags;                    //тэги прохода
  TagHashArray                 tag_hashes;              //хэши тэгов  
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , sort_mode (SortMode_Default)
    , state_block_need_update (true)
  {
    try
    {
      if (!device_manager)
        throw xtl::make_null_argument_exception ("", "device_manager");
        
      render::low_level::StateBlockMask mask;
      
      mask.rs_state               = true;
      mask.os_blend_state         = true;
      mask.os_depth_stencil_state = true;
      
      state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::EffectPass::EffectPass");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

EffectPass::EffectPass (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

EffectPass::~EffectPass ()
{
}

/*
    Состояния уровней устройства отрисовки
*/

void EffectPass::SetBlendState (const LowLevelBlendStatePtr& state)
{
  impl->blend_state             = state;
  impl->state_block_need_update = true;
}

void EffectPass::SetDepthStencilState (const LowLevelDepthStencilStatePtr& state)
{
  impl->depth_stencil_state     = state;
  impl->state_block_need_update = true;
}

void EffectPass::SetRasterizerState (const LowLevelRasterizerStatePtr& state)
{
  impl->rasterizer_state        = state;
  impl->state_block_need_update = true;
}

const LowLevelBlendStatePtr& EffectPass::BlendState ()
{
  return impl->blend_state;
}

const LowLevelDepthStencilStatePtr& EffectPass::DepthStencilState ()
{
  return impl->depth_stencil_state;
}

const LowLevelRasterizerStatePtr& EffectPass::RasterizerState ()
{
  return impl->rasterizer_state;
}

/*
    Блок состояний эффекта
*/

LowLevelStateBlockPtr EffectPass::StateBlock ()
{
  try
  {
    if (!impl->state_block_need_update)
      return impl->state_block;
      
    render::low_level::IDevice& device = impl->device_manager->Device ();
    
    device.OSSetBlendState        (impl->blend_state.get ());
    device.OSSetDepthStencilState (impl->depth_stencil_state.get ());
    device.RSSetState             (impl->rasterizer_state.get ());
    
    impl->state_block->Capture ();
    
    return impl->state_block;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EffectPass::StateBlock");
    throw;
  }
}

/*
    Тэги примитивов, которые войдут в проход
*/

void EffectPass::SetTags (const char* tags)
{
  if (!tags)
    throw xtl::make_null_argument_exception ("render::EffectPass::SetTags", "tags");

  SetTags (common::split (tags, " ")); 
}

void EffectPass::SetTags (const common::StringArray& tags)
{
  TagHashArray new_tag_hashes;
  
  new_tag_hashes.resize (tags.Size ());
  
  for (size_t i=0, count=tags.Size (); i<count; i++)
    new_tag_hashes [i] = common::strhash (tags [i]);
    
  impl->tags = tags;

  impl->tag_hashes.swap (new_tag_hashes);
}

size_t EffectPass::TagsCount ()
{
  return impl->tags.Size ();
}

const char* EffectPass::Tag (size_t index)
{
  if (index >= impl->tags.Size ())
    throw xtl::make_range_exception ("render::EffectPass::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t EffectPass::TagHash (size_t index)
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("render::EffectPass::TagHash", "index", index, impl->tag_hashes.size ());
    
  return impl->tag_hashes [index];
}

const char** EffectPass::Tags ()
{
  if (impl->tags.IsEmpty ())
    return 0;
    
  return impl->tags.Data ();
}

const size_t* EffectPass::TagHashes ()
{
  if (impl->tag_hashes.empty ())
    return 0;
    
  return &impl->tag_hashes [0];
}

/*
    Тип сортировки примитивов
*/

void EffectPass::SetSortMode (render::SortMode mode)
{
  switch (mode)
  {
    case SortMode_FrontToBack:
    case SortMode_BackToFront:
    case SortMode_StateSwitch:
      break;
    default:
      throw xtl::make_argument_exception ("render::EffectPass::SetSortMode", "mode", mode);
  }
  
  impl->sort_mode = mode;
}

SortMode EffectPass::SortMode ()
{
  return impl->sort_mode;
}
