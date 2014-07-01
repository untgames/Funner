#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s ) { e, s },
#define FT_ERROR_START_LIST    {
#define FT_ERROR_END_LIST      };

//ошибки freetype
const struct
{
  int         error_code;    //код ошибки
  const char* error_message; //описание ошибки
} FREETYPE_ERRORS [] =

#include FT_ERRORS_H

}

namespace media
{

namespace freetype
{

//Получение имени ошибки free type
const char* get_free_type_error_name (FT_Error error)
{
  for (size_t i = 0, count = sizeof (FREETYPE_ERRORS) / sizeof (*FREETYPE_ERRORS); i < count; i++)
    if (FREETYPE_ERRORS [i].error_code == error)
      return FREETYPE_ERRORS [i].error_message;

  return "unknown error";
}

//Проверка ошибки free type
void check_free_type_error (FT_Error error, const char* source)
{
  if (!error)
    return;

  throw xtl::format_operation_exception (source, "Free type error '%s' error code %d", get_free_type_error_name (error), error);
}

}

}
