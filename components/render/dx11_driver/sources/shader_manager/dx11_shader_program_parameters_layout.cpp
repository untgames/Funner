#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

ProgramParametersLayout::ProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
  : helpers::ProgramParametersLayout (desc)
  , params (GetParametersCount ())
{
  const ProgramParameter* param = GetParameters ();

  for (size_t i=0, count=GetParametersCount (); i<count; i++, param++)
  {
    ParameterMap::iterator iter = params.find (param->name);

    if (iter != params.end ())
      throw xtl::format_operation_exception ("render::low_level::dx11::ProgramParametersLayout::ProgramParametersLayout", "Duplicate parameter '%s' was found", param->name);

    params.insert_pair (param->name, param);
  }
}

/*
    Поиск параметра
*/

const ProgramParameter* ProgramParametersLayout::FindParameter (const char* name) const
{
  ParameterMap::const_iterator iter = params.find (name);

  if (iter != params.end ())  
    return iter->second;

  return 0;
}
