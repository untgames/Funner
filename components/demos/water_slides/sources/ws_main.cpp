#include "shared.h"

int main ()
{
    //инициализация приложения

  MyApplication::Instance ();  
  
  MyApplication::Instance ().SetView (create_test_game_view ());

    //запуск приложения
    
  syslib::Application::Run ();
  
    //завершение приложения

  return syslib::Application::GetExitCode ();
}
