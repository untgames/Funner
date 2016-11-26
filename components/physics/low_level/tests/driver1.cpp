#include "shared.h"

const char* DRIVER_REGISTRATION_NAME = "test_drv";

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    xtl::com_ptr<physics::low_level::IDriver> driver (new TestDriver, false);

    DriverManager::RegisterDriver (DRIVER_REGISTRATION_NAME, get_pointer (driver));
    
    driver = 0;
    
    IDriver* test_driver = DriverManager::FindDriver (DRIVER_REGISTRATION_NAME);

    if (!test_driver)
      throw xtl::format_operation_exception ("", "Can't find registered driver '%s'", DRIVER_REGISTRATION_NAME);

    xtl::com_ptr<physics::low_level::IScene>     scene (test_driver->CreateScene (), false);
    xtl::com_ptr<physics::low_level::IRigidBody> body (scene->CreateRigidBody (0, 0), false);

    body->SetCcdMotionThreshold (10.f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
