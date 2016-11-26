#include "shared.h"

void my_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  try
  {
      //Ї®¤ЇЁбЄ  ­  б®ЎлвЁп Їа®в®Є®«Ёа®ў ­Ёп
    
    common::LogFilter filter ("*", &my_log);
    
      //Ё­ЁжЁ «Ё§ жЁп ЇаЁ«®¦Ґ­Ёп
      
    MyApplication application;
    
    application.SetView (create_test_game_view ());

      //§ ЇгбЄ ЇаЁ«®¦Ґ­Ёп

    syslib::Application::Run ();

      //§ ўҐаиҐ­ЁҐ ЇаЁ«®¦Ґ­Ёп

    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& e)
  {
    printf ("Exception caught: %s\n", e.what ());
    return 0;
  }
}
