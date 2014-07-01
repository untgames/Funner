#include <stdio.h>
#include <time.h>

#include <xtl/high_precision_time.h>

using namespace xtl;

int main ()
{
  printf ("Results of high_precision_time_resutls:\n");

  high_precision_time_t start_time = high_precision_time ();

  static const size_t DELAY = 2 * CLOCKS_PER_SEC;

  for (size_t start=clock (); clock () - start < DELAY;);

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
