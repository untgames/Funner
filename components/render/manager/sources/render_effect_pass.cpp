#include "shared.h"

using namespace render::manager;

/*
    Описание реализации
*/

typedef stl::vector<size_t> TagHashArray;

struct EffectPass::Impl
{
  DeviceManagerPtr             device_manager;           //менеджер устройства отрисовки
  common::StringArray          color_targets;            //целевые буферы цвета
  stl::string                  depth_stencil_target;     //целевой буфер глубины
  render::manager::SortMode    sort_mode;                //режим сортировки
  LowLevelDepthStencilStatePtr depth_stencil_state;      //состояние уровня отсечения
  LowLevelBlendStatePtr        blend_state;              //состояние уровня смешивания цветов
  LowLevelRasterizerStatePtr   rasterizer_state;         //состояние уровня растеризации  
  LowLevelRasterizerStatePtr   rasterizer_scissor_state; //состояние уровня растеризации с включенным тестом отсечения
  LowLevelStateBlockPtr        scissor_off_state_block;  //блок состояний
  LowLevelStateBlockPtr        scissor_on_state_block;   //блок состояний с включенным тестом отсечения
  bool                         state_block_need_update;  //блок состояний требует обновления
  size_t                       clear_flags;              //флаги очистки
  common::StringArray          tags;                     //тэги прохода
  TagHashArray                 tag_hashes;               //хэши тэгов  
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , sort_mode (SortMode_Default)
    , state_block_need_update (true)
    , clear_flags (ClearFlag_ViewportOnly)
  {
    try
    {
      if (!device_manager)
        throw xtl::make_null_argument_exception ("", "device_manager");
        
      render::low_level::StateBlockMask mask;
      
      mask.rs_state               = true;
      mask.os_blend_state         = true;
      mask.os_depth_stencil_state = true;
      
      scissor_on_state_block  = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);
      scissor_off_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::EffectPass::EffectPass");
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
    Целевые буферы отрисовки
*/

void EffectPass::SetColorTargets (const common::StringArray& targets)
{
  impl->color_targets = targets.Clone ();
}

void EffectPass::SetDepthStencilTarget (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::EffectPass::SetDepthStencilTarget", "name");
    
  impl->depth_stencil_target = name;
}

const common::StringArray& EffectPass::ColorTargets ()
{
  return impl->color_targets;
}

const char* EffectPass::DepthStencilTarget ()
{
  return impl->depth_stencil_target.c_str ();
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

void EffectPass::SetRasterizerScissorState (const LowLevelRasterizerStatePtr& state)
{
  impl->rasterizer_scissor_state = state;
  impl->state_block_need_update  = true;
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

const LowLevelRasterizerStatePtr& EffectPass::RasterizerScissorState ()
{
  return impl->rasterizer_scissor_state;
}

/*
    Блок состояний эффекта
*/

LowLevelStateBlockPtr EffectPass::StateBlock (bool scissor_enable)
{
  try
  {
    if (impl->state_block_need_update)
    {      
      render::low_level::IDeviceContext& context = impl->device_manager->ImmediateContext ();
      
      context.OSSetBlendState        (impl->blend_state.get ());
      context.OSSetDepthStencilState (impl->depth_stencil_state.get ());
      context.RSSetState             (impl->rasterizer_state.get ());
      
      impl->scissor_off_state_block->Capture (&context);
      
      context.RSSetState (impl->rasterizer_scissor_state.get ());

      impl->scissor_on_state_block->Capture (&context);
    }    
    
    return scissor_enable ? impl->scissor_on_state_block : impl->scissor_off_state_block;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectPass::StateBlock");
    throw;
  }
}

/*
    Тэги примитивов, которые войдут в проход
*/

void EffectPass::SetTags (const char* tags)
{
  if (!tags)
    throw xtl::make_null_argument_exception ("render::manager::EffectPass::SetTags", "tags");

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
    throw xtl::make_range_exception ("render::manager::EffectPass::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t EffectPass::TagHash (size_t index)
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("render::manager::EffectPass::TagHash", "index", index, impl->tag_hashes.size ());
    
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

void EffectPass::SetSortMode (render::manager::SortMode mode)
{
  switch (mode)
  {
    case SortMode_FrontToBack:
    case SortMode_BackToFront:
    case SortMode_StateSwitch:
      break;
    default:
      throw xtl::make_argument_exception ("render::manager::EffectPass::SetSortMode", "mode", mode);
  }
  
  impl->sort_mode = mode;
}

SortMode EffectPass::SortMode ()
{
  return impl->sort_mode;
}

/*
    Флаги очистки
*/

void EffectPass::SetClearFlags (size_t clear_flags)
{
  impl->clear_flags = clear_flags;
}

size_t EffectPass::ClearFlags ()
{
  return impl->clear_flags;
}
