#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

struct ProgramParameterComparator
{
  bool operator () (const ProgramParameter* parameter1, const ProgramParameter* parameter2) const
  {
    return parameter1->slot < parameter2->slot;
  }
};

}

/*
   Конструктор
*/

ProgramParametersLayout::ProgramParametersLayout (const ProgramParametersLayoutDesc& in_desc)
{
  SetDesc (in_desc);
}

/*
   Изменение/получение дескриптора
*/

void ProgramParametersLayout::SetDesc (const ProgramParametersLayoutDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ProgramParametersLayout::SetDesc";

  if (!in_desc.parameters)
    throw xtl::make_null_argument_exception (METHOD_NAME, "in_desc.parameters");

  for (size_t i = 0; i < in_desc.parameters_count; i++)
  {
    if (!in_desc.parameters[i].count)
      throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "Null argument in_desc.parameters[%u].count", i);

    if (!in_desc.parameters[i].name)
      throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "Null argument in_desc.parameters[%u].name", i);
  
    if (in_desc.parameters[i].slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception (METHOD_NAME, "in_desc.parameters.slot", in_desc.parameters[i].slot, (size_t)0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    if (in_desc.parameters[i].type >= ProgramParameterType_Num)
      throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument value in_desc.parameters[%u].type", i);
  }

  stl::vector<const ProgramParameter*> temp_parameters (in_desc.parameters_count);

  for (size_t i = 0; i < in_desc.parameters_count; i++)
    temp_parameters[i] = &in_desc.parameters [i];

  stl::sort (temp_parameters.begin (), temp_parameters.end (), ProgramParameterComparator ());

  parameters.reserve (in_desc.parameters_count);

  parameter_groups.reserve (DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT + 1; //номер текущего слота (для отслеживания изменения группы параметров)
    
  for (size_t i = 0; i < temp_parameters.size (); i++)
  {
    parameters.push_back (*temp_parameters[i]);

      //отслеживание появления новой группы

    if (current_slot != temp_parameters [i]->slot)
    {
      parameter_groups.push_back ();

      ProgramParameterGroup& new_group = parameter_groups.back ();
      
      new_group.slot       = temp_parameters [i]->slot;
      new_group.parameters = &parameters.back ();
      new_group.count      = 0;
      current_slot         = temp_parameters [i]->slot;
    }
  }

  for (stl::vector<ProgramParameterGroup>::iterator iter = parameter_groups.begin (), end = parameter_groups.end ()-1; iter != end; ++iter)
    iter->count = iter [1].parameters - iter [0].parameters;

  parameter_groups.back ().count = parameters.end () - parameter_groups.back ().parameters;    
}

/*
   Получение данных
*/

size_t ProgramParametersLayout::GroupsCount ()
{
  return parameter_groups.size ();
}

ProgramParameterGroup& ProgramParametersLayout::ParametersGroup (size_t index)
{
  if (index >= parameter_groups.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::ProgramParametersLayout::ParametersGroup", "index", index, 0u, parameter_groups.size ());

  return parameter_groups[index];
}
