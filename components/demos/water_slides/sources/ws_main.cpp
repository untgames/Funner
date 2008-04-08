#include "shared.h"

int main ()
{
    //инициализация приложения

  MyApplication::Instance ();  

    //запуск приложения
    
  syslib::Application::Run ();
  
    //завершение приложения

  return syslib::Application::GetExitCode ();
}
