const high_precision_time_t NANOSECONDS_PER_ONE_SECOND = 1000000000;

#ifdef _WIN32

extern "C" __declspec (dllimport) int __stdcall QueryPerformanceCounter   (unsigned long long*);
extern "C" __declspec (dllimport) int __stdcall QueryPerformanceFrequency (unsigned long long*);

inline high_precision_time_t high_precision_time ()
{
  high_precision_time_t time;
  
  if (QueryPerformanceCounter (&time))
    return time;

  return 0;
}

inline high_precision_time_t high_precision_time_frequency ()
{
  high_precision_time_t frequency;

  if (QueryPerformanceFrequency (&frequency))
    return frequency;

  return 0;
}

#elif defined (BEAGLEBOARD) || defined (__APPLE__) || defined (MEEGO)

inline high_precision_time_t high_precision_time ()
{
  return clock ();
}

inline high_precision_time_t high_precision_time_frequency ()
{
  return CLOCKS_PER_SEC;
}

#else

inline high_precision_time_t high_precision_time ()
{
  timespec time;
  
  if (clock_gettime (CLOCK_MONOTONIC, &time) != 0)
    return 0;

  return high_precision_time_t (time.tv_nsec) + high_precision_time (time.tv_sec) * NANOSECONDS_PER_ONE_SECOND;
}

inline high_precision_time_t high_precision_time_frequency ()
{
  return NANOSECONDS_PER_ONE_SECOND;
}

#endif

inline void convert_high_precision_time (high_precision_time_t span, size_t& out_seconds, size_t& out_nanoseconds)
{
  high_precision_time_t frequency = high_precision_time_frequency ();

  out_seconds     = static_cast<size_t> (span / frequency);
  out_nanoseconds = static_cast<size_t> ((NANOSECONDS_PER_ONE_SECOND * (span % frequency)) / frequency);
}
