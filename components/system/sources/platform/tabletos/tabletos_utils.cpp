#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

namespace
{

struct TabletOsInitializer
{
  TabletOsInitializer ()
  {
    try
    {
      int status = bps_initialize ();
      
      if (status != BPS_SUCCESS)
        raise_error ("::bps_initialize failed");
    }
    catch (xtl::exception& e)
    {
      e.touch ("syslib::tabletos::TabletOsInitializer");
      throw;
    }
  }
  
  ~TabletOsInitializer ()
  {
  }
};

}

namespace syslib
{

namespace tabletos
{

//инициализация платформы
void platform_initialize ()
{
  typedef common::Singleton<TabletOsInitializer> TabletOsInitializerSingleton;
  
  TabletOsInitializerSingleton::Instance ();
}

//проверка ошибок использования BPS и генерация исключения в случае их наличия
void check_errors (const char* source)
{
  try
  {
    if (errno)
      throw xtl::format_operation_exception ("syslib::check_errors", strerror (errno));
  }
  catch (xtl::exception& e)
  {
    e.touch (source);
    throw;
  }
}

void raise_error (const char* source)
{
  if (errno)
    throw xtl::format_operation_exception (source, strerror (errno));

  throw xtl::format_operation_exception (source, "Invalid operation");
}

}

}
