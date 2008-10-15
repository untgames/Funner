#include <common/time.h>

#include <platform/platform.h>

namespace common
{

//Получение времени в миллисекундах от первого вызова milliseconds
size_t milliseconds ()
{
  static size_t start = Platform::GetMilliseconds ();

  return Platform::GetMilliseconds () - start;
}

}
