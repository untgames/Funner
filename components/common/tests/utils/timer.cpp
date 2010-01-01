#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <common/time.h>

using namespace common;

void dump (const Timer& timer)
{
  printf ("time=%.3f auto_update=%s started=%s", timer.Time (), timer.IsAutoUpdate () ? "true" : "false", 
    timer.IsStarted () ? "true" : "false");
}

template <class T>
void dump (const char* message, const T& value)
{
  printf ("%s: ", message);
  dump (value);
  printf ("\n");
}

size_t current_time = 0;

size_t get_current_time ()
{
  return current_time;
}

int main ()
{
  try
  {
    printf ("Results of timer_test:\n");
    
    Timer timer1;
    
    dump ("default timer", timer1);
    
    Timer timer2 (&get_current_time, 10.0, true);
    
    timer2.Start ();
    
    dump ("timer with source", timer2);
    
    current_time = 12;
    
    dump ("timer with source (after time change)", timer2);    

    timer2.Stop ();
    
    current_time = 14;
    
    dump ("timer with source (after stop & time change)", timer2);
    
    timer2.Start ();        
    
    dump ("timer with source (after resume)", timer2);    
    
    current_time = 17;
    
    dump ("timer with source (after resume & time change)", timer2);
    
    timer2.SetAutoUpdate (false);
    
    current_time = 20;
    
    dump ("timer with source (no auto update)", timer2);
    
    timer2.Update ();
    
    dump ("timer with source (after manual update)", timer2);
    
    timer2.SetAutoUpdate (true);
    
    Timer timer3 (timer2, true);
    
    timer3.Start ();
    
    dump ("child timer", timer3);
    
    current_time = 25;
    
    dump ("child timer (after time change)", timer3);
    
    timer2.Stop ();
    
    current_time = 27;
    
    dump ("child timer (after parent timer stop)", timer3);
    
    timer2.Start ();
    
    current_time = 28;
    
    dump ("child timer (after parent timer resume)", timer3);
    
    timer2.Reset ();
    
    dump ("child timer (after parent timer reset)", timer3);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
