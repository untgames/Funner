#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

StateBlock::StateBlock (Device& device, const StateBlockMask& in_mask)
  : mask (in_mask),
    output_stage_state (device.output_stage.CreateStateBlock ())
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
  output_stage_state->Capture (mask);
}

/*
    Применение настроек устройства
*/

void StateBlock::Apply ()
{
  output_stage_state->Apply (mask);
}
