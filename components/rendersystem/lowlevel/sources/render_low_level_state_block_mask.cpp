#include <memory.h>

#include <common/exception.h>

#include <render/low_level/state_block.h>

using namespace render::low_level;

/*
   Групповая установка
*/

void StateBlockMask::Set (StateBlockGroup group, bool value)
{
  switch (group)
  {
    case StateBlockGroup_InputStage: 
      is_input_layout = is_index_buffer = value;
      for (size_t i = 0; i < DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
        is_vertex_buffers [i] = value;
      break;
    case StateBlockGroup_ShaderStage:
      ss_program = ss_program_parameters_layout = value;
      for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
        ss_constant_buffer [i] = value;
      for (size_t i = 0; i < DEVICE_SAMPLER_SLOTS_COUNT; i++)
        ss_samplers [i] = ss_textures [i] = value;
      break;
    case StateBlockGroup_RasterizerStage:
      rs_state = rs_viewport = rs_scissor = value;
      break;
    case StateBlockGroup_OutputStage:
      os_blend_state = os_depth_stencil_state = os_render_target_view = os_depth_stencil_view = value;
      break;
    default:
      common::RaiseInvalidArgument ("render::low_level::StateBlockMask::Set", "group", group);
  }
}

void StateBlockMask::Set (StateBlockGroup group)
{
  Set (group, true);
}

void StateBlockMask::Clear (StateBlockGroup group)
{
  Set (group, false);
}

void StateBlockMask::Set ()
{
  memset (this, 1, sizeof *this);
}

void StateBlockMask::Clear ()
{
  memset (this, 0, sizeof *this);
}

/*
   Покомопонентные логические операции
*/

StateBlockMask StateBlockMask::operator ~ () const
{
  StateBlockMask ret_value (*this);
  
  for (char *i = (char*)&ret_value, *end = (char*)&ret_value + sizeof ret_value; i < end; i++)
    *i = !i;

  return ret_value;
}

StateBlockMask& StateBlockMask::operator |= (const StateBlockMask& right_mask)
{
  for (char *i = (char*)this, *j = (char*)&right_mask, *end = (char*)this + sizeof *this; i < end; i++, j++)
    *i |= *j;

  return *this;
}

StateBlockMask& StateBlockMask::operator &= (const StateBlockMask& right_mask)
{
  for (char *i = (char*)this, *j = (char*)&right_mask, *end = (char*)this + sizeof *this; i < end; i++, j++)
    *i &= *j;

  return *this;
}

StateBlockMask& StateBlockMask::operator ^= (const StateBlockMask& right_mask)
{
  for (char *i = (char*)this, *j = (char*)&right_mask, *end = (char*)this + sizeof *this; i < end; i++, j++)
    *i ^= *j;

  return *this;
}

StateBlockMask StateBlockMask::operator | (const StateBlockMask& right_mask) const
{
  StateBlockMask ret_value (*this);
  
  ret_value |= right_mask;

  return ret_value;
}

StateBlockMask StateBlockMask::operator & (const StateBlockMask& right_mask) const
{
  StateBlockMask ret_value (*this);
    
  ret_value &= right_mask;

  return ret_value;
}

StateBlockMask StateBlockMask::operator ^ (const StateBlockMask& right_mask) const
{
  StateBlockMask ret_value (*this);    

  ret_value ^= right_mask;

  return ret_value;
}

/*
   Сравнение
*/

bool StateBlockMask::operator == (const StateBlockMask& mask) const
{
  return !memcmp (&mask, this, sizeof *this);
}

bool StateBlockMask::operator != (const StateBlockMask& mask) const
{
  return !memcmp (&mask, this, sizeof *this);
}
