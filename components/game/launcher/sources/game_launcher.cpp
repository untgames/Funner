#include "shared.h"

const char* DEFAULT_START_SCRIPT_NAME = "data/scripts/main.lua";

typedef xtl::shared_ptr<TestApplication> TestApplicationPtr;

int main (int argc, char* argv[])
{
  try
  {    
      //инициализация приложения
    
    TestApplicationPtr application;

    if (argc == 1)
      application = TestApplicationPtr (new TestApplication (DEFAULT_START_SCRIPT_NAME));
    else
      application = TestApplicationPtr (new TestApplication (argv[1]));

      //запуск приложения

    return application->Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
