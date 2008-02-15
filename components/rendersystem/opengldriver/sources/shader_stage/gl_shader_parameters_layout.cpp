#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор
*/

ShaderParametersLayout::ShaderParametersLayout (const ContextManager& manager)
  : ContextObject (manager)
  {}

/*
   Изменение/получение дескриптора
*/

void ShaderParametersLayout::SetDesc (const ShaderParametersLayoutDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderParametersLayout::SetDesc";

  if (!in_desc.parameters)
    RaiseNullArgument (METHOD_NAME, "in_desc.parameters");

  for (size_t i = 0; i < in_desc.parameters_count; i++)
  {
    if (!in_desc.parameters[i].name)
      Raise <ArgumentNullException> (METHOD_NAME, "Null argument in_desc.parameters[%u].name", i);
  
    if (in_desc.parameters[i].slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      RaiseOutOfRange (METHOD_NAME, "in_desc.parameters.slot", in_desc.parameters[i].slot, (size_t)0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    if (in_desc.parameters[i].type >= ShaderParameterType_Num)
      Raise <ArgumentException> (METHOD_NAME, "Invalid argument value in_desc.parameters[%u].type", i);
  }

  desc = in_desc;
}

void ShaderParametersLayout::GetDesc (ShaderParametersLayoutDesc& target_desc)
{
  target_desc = desc;
}
