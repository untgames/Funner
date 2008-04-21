#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    IDevice* device = DriverManager::CreateDevice ("*_drv", "*");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
