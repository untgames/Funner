#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    ContextState
===================================================================================================
*/

ContextState::ContextState (const DeviceManager& manager)
  : render_target_context_state (manager)
  , texture_manager_context_state (manager)
  , input_manager_context_state (manager)
  , shader_manager_context_state (manager)
  , output_manager_context_state (manager)
{
}

/*
===================================================================================================
    StateBlock
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

StateBlock::StateBlock (const DeviceManager& manager, const StateBlockMask& in_mask)
  : DeviceObject (manager)
  , state (manager)
  , mask (in_mask)
{
}

StateBlock::~StateBlock ()
{
}

/*
    Получение маски блока
*/

void StateBlock::GetMask (StateBlockMask& out_mask)
{
  out_mask = mask;
}

/*
    Захват настроек устройства
*/

void StateBlock::Capture (IDeviceContext* in_context)
{
  try
  {
    if (!in_context)
      throw xtl::make_null_argument_exception ("", "context");

    Context* context = cast_object<Context> (*this, in_context, "", "context");

    context->Capture (mask, state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::StateBlock::Capture");
    throw;
  }
}

/*
    Применение настроек устройства
*/

void StateBlock::Apply (IDeviceContext* in_context)
{
  try
  {
    if (!in_context)
      throw xtl::make_null_argument_exception ("", "context");

    Context* context = cast_object<Context> (*this, in_context, "", "context");

    context->Apply (mask, state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::StateBlock::Apply");
    throw;
  }
}
