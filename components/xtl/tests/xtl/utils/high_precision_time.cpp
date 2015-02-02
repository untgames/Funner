#include <stdio.h>

#ifdef __APPLE__
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <xtl/high_precision_time.h>

using namespace xtl;

int main ()
{
  printf ("Results of high_precision_time_resutls:\n");

  high_precision_time_t start_time = high_precision_time ();

#ifdef __APPLE__  //clock is not correct on mac x64
  timeval start;

  gettimeofday (&start, 0);

  size_t start_milliseconds = start.tv_sec * 1000 + start.tv_usec / 1000;

  for (;;)
  {
    timeval current_time;

    gettimeofday (&current_time, 0);

    if (current_time.tv_sec * 1000 + current_time.tv_usec / 1000 - start_milliseconds >= 2000)
      break;
  }
#else
  static const size_t DELAY = 2 * CLOCKS_PER_SEC;

  for (size_t start=clock (); clock () - start < DELAY;);
#endif

  high_precision_time_t end_time = high_precision_time ();

  high_precision_time_t span = end_time - start_time;

  size_t seconds = 0, nanoseconds = 0;

  convert_high_precision_time (span, seconds, nanoseconds);

//  printf ("seconds=%u nanoseconds=%u span=%llu\n", seconds, nanoseconds, span);

  long long int diff = high_precision_time_t (nanoseconds) + high_precision_time_t (seconds) * 1000000000 - 2000000000;

//  printf ("diff=%lld\n", diff);

  static const long long int EPS = 10000000; //10 milliseconds

  if (diff > EPS || diff < -EPS) printf ("test failed. diff is %lld\n", diff);
  else                           printf ("test passed\n");

  return 0;
}
