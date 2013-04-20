#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

ShaderBuffersSynchronizer::ShaderBuffersSynchronizer (const ProgramParametersLayout& src_layout, const ConstantBufferLayout& dst_layout)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderBuffersSynchronizer::ShaderBuffersSynchronizer");
    throw;
  }
}
