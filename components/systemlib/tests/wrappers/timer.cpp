#include "shared.h"

void test (Timer& timer)
{
  try
  {
    static size_t count = 0;
    
    count++;
    
    printf ("#%u: elapsed=%u\n", count, timer.ElapsedMilliseconds () / 100 * 100); //точность до 1/10 секунды    
    
    switch (count)
    {
      case 10: timer.SetPeriod (200); break;
      case 20: Application::Exit (0); break;
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of timer_test:\n");  
  
  try
  {
    Timer timer (&test, 100);

    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
