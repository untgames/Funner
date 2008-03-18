#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

StateBlock::StateBlock (Device& device, const StateBlockMask& in_mask)
  : mask (in_mask),
    output_stage_state (device.output_stage.CreateStageState ()),
    input_stage_state (device.input_stage.CreateStageState ()),
    rasterizer_stage_state (device.rasterizer_stage.CreateStageState ()),
    texture_manager_state (device.texture_manager.CreateStageState ()),
    shader_stage_state (device.shader_stage.CreateStageState ()),
    query_manager_state (device.query_manager.CreateStageState ())
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

void StateBlock::Capture ()
{
  try
  {
    output_stage_state->Capture (mask);
    input_stage_state->Capture (mask);
    rasterizer_stage_state->Capture (mask);
    texture_manager_state->Capture (mask);
    shader_stage_state->Capture (mask);
    query_manager_state->Capture (mask);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::StateBlock::Capture");
    throw;
  }
}

/*
    Применение настроек устройства
*/

void StateBlock::Apply ()
{
  try
  {
    output_stage_state->Apply (mask);
    input_stage_state->Apply (mask);
    rasterizer_stage_state->Apply (mask);
    texture_manager_state->Apply (mask);
    shader_stage_state->Apply (mask);
    query_manager_state->Apply (mask);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::StateBlock::Apply");
    throw;
  }
}
