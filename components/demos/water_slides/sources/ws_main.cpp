#include "shared.h"

int main ()
{
  try
  {
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
