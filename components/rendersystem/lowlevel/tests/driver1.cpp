#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    RenderSystem::RegisterDriver ("test_drv", get_pointer (driver));
    
    SwapChainDesc swap_chain_desc; //не инициализируется, поскольку не будет использован в дальнейшем    
    
    xtl::com_ptr<IDevice> device (RenderSystem::CreateDevice ("*_drv", swap_chain_desc), false);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
