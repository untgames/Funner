#include "shared.h"

void my_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  try
  {
      //подписка на события протоколирования
    
    common::LogFilter filter ("*", &my_log);
    
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
