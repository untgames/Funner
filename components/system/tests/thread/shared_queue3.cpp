#include "shared.h"

const size_t WARMUP_LOOPS_COUNT  = 1000;
const size_t LOOPS_COUNT         = 1000000;
const size_t SPIN_TIME           = 1000;
const size_t MAX_QUEUE_SIZE      = 1;

typedef SharedQueue<xtl::high_precision_time_t> Queue;
typedef stl::vector<xtl::high_precision_time_t> LatencyArray;

Queue        queue (MAX_QUEUE_SIZE, SPIN_TIME);
LatencyArray latencies;

int producer ()
{
  for (size_t i=0; i<WARMUP_LOOPS_COUNT; i++)
    queue.Push (xtl::high_precision_time ());

  for (size_t i=0; i<LOOPS_COUNT; i++)
    queue.Push (xtl::high_precision_time ());
 
  return 0;
}

int consumer ()
{
  for (size_t i=0; i<WARMUP_LOOPS_COUNT; i++)
    queue.Pop ();

  for (size_t i=0; i<LOOPS_COUNT; i++)
  {
    xtl::high_precision_time_t start_time = queue.Pop (), latency = xtl::high_precision_time () - start_time;

    latencies.push_back (latency);
  }

  return 0;
}

size_t get_nanoseconds (xtl::high_precision_time_t time)
{
  size_t seconds = 0, nanoseconds = 0;
  
  xtl::convert_high_precision_time (time, seconds, nanoseconds);

  return seconds * 1000000000 + nanoseconds;
}

int main ()
{
  printf ("Results of shared_queue3:\n");

  try
  {
    latencies.reserve (LOOPS_COUNT);

    Thread consumer_thread (&consumer), producer_thread (&producer);

    consumer_thread.SetAffinity (1 << 1);
    producer_thread.SetAffinity (1 << 3);

    consumer_thread.Join ();
    producer_thread.Join ();

    stl::sort (latencies.begin (), latencies.end ());

    if (!latencies.empty ())
    {
      printf ("min latency:    %uns\n", get_nanoseconds (latencies.front ()));
      printf ("max latency:    %uns\n", get_nanoseconds (latencies.back ()));
      printf ("median latency: %uns\n", get_nanoseconds (latencies [latencies.size () / 2]));
    }
  }
  catch (xtl::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
