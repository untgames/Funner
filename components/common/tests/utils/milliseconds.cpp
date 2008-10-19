#include <cstdio>
#include <ctime>
#include <exception>

#include <common/time.h>

const size_t DELAY = 2000;

using namespace common;

int main ()
{
  try
  {
    printf ("Results of milliseconds_test:\n");
    
    time_t start_time, end_time;
    
    time (&start_time);
    
    for (size_t start = milliseconds (); milliseconds () - start < DELAY;);
    
    time (&end_time);
    
    time_t interval = end_time - start_time;
    
    tm* res = localtime (&interval);
    
    printf ("period: %d\n", res->tm_sec);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
