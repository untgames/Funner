#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <common/time.h>

using namespace common;

int main ()
{
  try
  {
    printf ("Results of timer_set_time_test:\n");
    
    Timer parent_timer (false);
    Timer child_timer (parent_timer, false);
    
    parent_timer.Start ();
    child_timer.Start ();

    printf ("Before set time: parent timer time = %.2f, child_timer time = %.2f\n", parent_timer.Time ().cast<float> (), child_timer.Time ().cast<float> ());

    parent_timer.SetTime (Timer::time_t (1));
    child_timer.Update ();

    printf ("After set time: parent timer time = %.2f, child_timer time = %.2f\n", parent_timer.Time ().cast<float> (), child_timer.Time ().cast<float> ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
