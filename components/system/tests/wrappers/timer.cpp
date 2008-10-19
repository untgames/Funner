#include "shared.h"

size_t last = 0;

void test (Timer& timer)
{
  try
  {
    static size_t count = 0;

    count++;

    if (common::milliseconds () - last < timer.Period () + 20) //точность до 1/50 секунды
    {
      printf ("#%lu: ok\n", count);
    }
    else
    {
      printf ("#%lu: fail (delay=%u msec)\n", count, common::milliseconds () - last);
    }

    switch (count)
    {
      case 10: timer.SetPeriod (200); break;
      case 20: Application::Exit (0); break;
    }

    last = common::milliseconds ();
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
    
    last = common::milliseconds ();

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
