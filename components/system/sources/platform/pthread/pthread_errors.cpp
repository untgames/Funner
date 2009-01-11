#include "shared.h"

namespace syslib
{

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", common::strerror (status), status);
}

}
