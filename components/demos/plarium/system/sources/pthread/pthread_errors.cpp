#include "shared.h"

namespace plarium
{

namespace system
{

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status)
{
  throw sgi_stl::runtime_error (utility::format ("Operation '%s' failed. Reason: '%s' (function exit with code %d)\n", source, strerror (status), status));
}

}

}
