#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    DriverManager::RegisterDriver ("test_drv",  xtl::com_ptr<IDriver> (new TestDriver, false).get ());
    
    DriverManager::CreateDevice ("*_drv", "*");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
