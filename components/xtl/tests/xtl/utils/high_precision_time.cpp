#include <stdio.h>
#include <time.h>

#include <xtl/high_precision_time.h>

using namespace xtl;

int main ()
{
  printf ("Results of high_precision_time_resutls:\n");

  high_precision_time_t start_time = high_precision_time ();

  static const size_t DELAY = 2000;

  for (size_t start=clock (); clock () - start < 2000;);

  high_precision_time_t end_time = high_precision_time ();

  high_precision_time_t span = end_time - start_time;

  size_t seconds = 0, nanoseconds = 0;

  convert_high_precision_time (span, seconds, nanoseconds);

//  printf ("seconds=%u nanoseconds=%u\n", seconds, nanoseconds);

  high_precision_time_t diff = high_precision_time_t (nanoseconds) + high_precision_time_t (seconds) * 1000000000 - 2000000000;

//  printf ("diff=%u\n", diff);

  static const high_precision_time_t EPS = 10000000; //10 milliseconds

  if (diff > EPS || diff < -EPS) printf ("test failed. diff is %d\n", int (diff));
  else                           printf ("test passed\n");

  return 0;
}
