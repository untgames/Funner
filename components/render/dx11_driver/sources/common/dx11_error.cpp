#include "shared.h"

namespace render
{

namespace low_level
{

namespace dx11
{

namespace
{

stl::string get_message (HRESULT result)
{
  return common::format ("%s: %s", DXGetErrorString (result), DXGetErrorDescription (result));
}

}

/*
    Dx11Exception
*/

Dx11Exception::Dx11Exception (const char* source, HRESULT in_result)
  : message_exception_base (source, get_message (in_result).c_str ())
  , hresult (in_result)
{
}

/*
    Проверка ошибок
*/

void check_errors (const char* source, HRESULT result)
{
  if (!FAILED (result))
    return;

  throw Dx11Exception (source, result);
}

void check_errors (HRESULT result)
{
  check_errors ("", result);
}

}

}

}
