#ifdef __APPLE__
  #include <mach/clock.h>
  #include <mach/mach.h>
#endif

#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#ifdef _WIN32_WCE
#include <implement.h>
#endif

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#ifdef ANDROID
#include <syslib/platform/android.h>
#endif

#include <shared/platform.h>

namespace syslib
{

struct mutex_handle
{
  pthread_mutex_t mutex;
};

//ЈҐ­Ґа жЁп ЁбЄ«озҐ­Ёп б Є®¤®¬ ®иЁЎЄЁ
void pthread_raise_error (const char* source, int status);

#if defined (_WIN32) || defined (ANDROID)

void thread_init ();      //дг­ЄжЁп Ё­ЁжЁ «Ё§ жЁЁ ЎЁЎ«Ё®вҐЄЁ
void thread_done (void*); //дг­ЄжЁп ¤ҐЁ­ЁжЁ «Ё§ жЁЁ ЎЁЎ«Ё®вҐЄЁ

#else

inline void thread_init () {}
inline void thread_done (void*) {}

#endif

}
