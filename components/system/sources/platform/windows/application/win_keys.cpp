#include "../shared.h"

using namespace syslib;
using namespace common;

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t WindowsWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
#ifdef WINCE
  return DefaultWindowManager::GetKeyName (scan_code, buffer_size, buffer);
#else
  static const char* METHOD_NAME = "syslib::WindowsWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;

  return ::GetKeyNameTextA ((unsigned int)scan_code << 16, buffer, (int)buffer_size);
#endif
}
