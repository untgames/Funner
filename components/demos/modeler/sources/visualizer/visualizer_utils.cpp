#include "shared.h"

#include <errno.h>

//проверка ошибок
const char* get_spawn_error_name ()
{
  switch (errno)
  {
    case E2BIG:   return "Argument list exceeds 1024 bytes.";
    case EINVAL:  return "mode argument is invalid.";
    case ENOENT:  return "File or path is not found.";
    case ENOEXEC: return "Specified file is not executable or has invalid executable-file format.";
    case ENOMEM:  return "Not enough memory is available to execute the new process.";
    default:      return "Unknown error";
  }
}

namespace math
{

size_t hash (const math::vec3f& vector)
{
  return common::crc32 (&vector, sizeof (vector));
}

}
