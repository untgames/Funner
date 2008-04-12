#include "shared.h"

int main ()
{
  try
  {
    sound::low_level::register_openal_driver ();
      
      //инициализация приложения

    MyApplication::Instance ();  
    
    MyApplication::Instance ().SetView (create_test_game_view ());

      //запуск приложения
      
    syslib::Application::Run ();

      //завершение приложения

    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& e)
  {
    printf ("Exception caught: %s\n", e.what ());
    return 1;
  }
}
