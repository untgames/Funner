#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    printf ("drivers (count=%u):\n", DriverManager::DriversCount ());
    
    for (size_t i=0; i<DriverManager::DriversCount (); i++)
      printf ("  driver: name='%s', description='%s'\n", DriverManager::DriverName (i), DriverManager::Driver (i)->GetDescription ());    
    
    SwapChainDesc swap_chain_desc; //не инициализируется, поскольку не будет использован в дальнейшем    
    
    IDevice*    device     = 0;
    ISwapChain* swap_chain = 0;
    
    DriverManager::CreateSwapChainAndDevice ("*_drv", "*", swap_chain_desc, "", swap_chain, device);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
