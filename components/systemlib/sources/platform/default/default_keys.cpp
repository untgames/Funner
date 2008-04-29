#include "shared.h"

using namespace syslib;

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t Platform::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::DefaultPlatform::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    RaiseInvalidArgument (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    RaiseNullArgument (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}
