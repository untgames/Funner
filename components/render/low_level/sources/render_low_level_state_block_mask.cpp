#include <memory.h>

#include <stl/functional>

#include <xtl/common_exceptions.h>

#include <common/hash.h>

#include <render/low_level/state_block.h>

using namespace render::low_level;

/*
    Конструктор
*/

StateBlockMask::StateBlockMask ()
{
  Clear ();
}

/*
   Групповая установка
*/

void StateBlockMask::Set (StateBlockGroup group, bool value)
{
  switch (group)
  {
    case StateBlockGroup_InputStage: 
    {
      is_layout       = value;
      is_index_buffer = value;

      for (size_t i = 0; i < DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
        is_vertex_buffers [i] = value;

      break;
    }
    case StateBlockGroup_ShaderStage:
    {
      ss_program                   = value;
      ss_program_parameters_layout = value;

      for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
        ss_constant_buffers [i] = value;

      for (size_t i = 0; i < DEVICE_SAMPLER_SLOTS_COUNT; i++)
        ss_samplers [i] = ss_textures [i] = value;

      break;
    }
    case StateBlockGroup_RasterizerStage:
      rs_state    = value;
      rs_viewport = value;
      rs_scissor  = value;
      break;
    case StateBlockGroup_OutputStage:
      os_blend_state         = value;
      os_depth_stencil_state = value;
      os_render_target_view  = value;
      os_depth_stencil_view  = value;
      break;
    default:
      throw xtl::make_argument_exception ("render::low_level::StateBlockMask::Set", "group", group);
      break;
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
    Хэш
*/

size_t StateBlockMask::Hash () const
{
  return common::crc32 (this, sizeof *this);
}

/*
   Покомопонентные логические операции
*/

namespace
{

inline bool*       begin (StateBlockMask& mask)       { return (bool*)&mask; }
inline bool*       end   (StateBlockMask& mask)       { return &mask.predication + 1; } //change if StateBlockMask changed
inline const bool* begin (const StateBlockMask& mask) { return (const bool*)&mask; }
inline const bool* end   (const StateBlockMask& mask) { return &mask.predication + 1; } //change if StateBlockMask changed

template <class Fn>
StateBlockMask& for_each (StateBlockMask& left, const StateBlockMask& right, Fn fn)
{
  const bool *first  = begin (right),
             *last   = end (right);
  bool       *result = begin (left);

  for (; first != last; first++, result++)
    *result = fn (*result, *first);
    
  return left;
}

struct logical_xor
{
  bool operator () (bool a, bool b) const { return b ? !a : a; }
};

}

StateBlockMask StateBlockMask::operator ~ () const
{
  StateBlockMask ret_value = Uninitialized ();

  const bool *first  = begin (*this),
             *last   = end (*this);
  bool       *result = begin (ret_value);

  while (first != last)
    *result++ = !*first++;

  return ret_value;
}

StateBlockMask& StateBlockMask::operator |= (const StateBlockMask& mask)
{
  return for_each (*this, mask, stl::logical_or<bool> ());
}

StateBlockMask& StateBlockMask::operator &= (const StateBlockMask& mask)
{
  return for_each (*this, mask, stl::logical_and<bool> ());
}

StateBlockMask& StateBlockMask::operator ^= (const StateBlockMask& mask)
{
  return for_each (*this, mask, logical_xor ());
}

StateBlockMask StateBlockMask::operator | (const StateBlockMask& mask) const
{
  return StateBlockMask (*this) |= mask;
}

StateBlockMask StateBlockMask::operator & (const StateBlockMask& mask) const
{
  return StateBlockMask (*this) &= mask;
}

StateBlockMask StateBlockMask::operator ^ (const StateBlockMask& mask) const
{
  return StateBlockMask (*this) ^= mask;
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
  return !(*this == mask);
}
