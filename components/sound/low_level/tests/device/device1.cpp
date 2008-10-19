#include "shared.h"

int main ()
{
  printf ("Results of device1_test:\n");
  
  try
  {
    xtl::com_ptr<sound::low_level::IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    DriverManager::CreateDevice ("*_drv", "*");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
