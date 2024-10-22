#include "shared.h"

using namespace render::low_level::helpers;

/*
    Количество свойств
*/

size_t DefaultPropertyList::GetSize ()
{
  return 0;
}

/*
    Получение ключа и его значения
*/

const char* DefaultPropertyList::GetKey (size_t index)
{
  throw xtl::make_range_exception ("render::low_level::opengl::DefaultPropertyList::GetKey", "index", index, 0);  
}

const char* DefaultPropertyList::GetValue (size_t index)
{
  throw xtl::make_range_exception ("render::low_level::opengl::DefaultPropertyList::GetValue", "index", index, 0);
}
