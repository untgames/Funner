#ifndef XTL_HIGH_PRECISION_TIME_HEADER
#define XTL_HIGH_PRECISION_TIME_HEADER

#include <cstddef>

#ifdef __APPLE__
  #ifdef IPHONE
    #include <mach/clock.h>
    #include <mach/mach_host.h>
  #else
    #include <mach/mach_time.h>
  #endif
#elif !defined (_WIN32)
  #include <time.h>
#endif

namespace xtl
{

typedef unsigned long long high_precision_time_t;

//получение времени с высокой точностью
high_precision_time_t high_precision_time ();

//получение частоты таймера
high_precision_time_t high_precision_time_frequency ();

//преобразование к стандартному представлению
void convert_high_precision_time (high_precision_time_t span, size_t& out_seconds, size_t& out_nanoseconds);

#include <xtl/detail/high_precision_time.inl>

}

#endif
