#ifdef __APPLE__
  #include <sys/time.h>
#endif

#if defined (__GNUC__) && !defined (__APPLE__)
  #include <unistd.h>
#endif

#include <errno.h>
#include <cstdio>
#include <cstring>

#include <pthread.h>

#include <sgi_stl/stdexcept>

#include <system/condition.h>
#include <system/mutex.h>
#include <system/thread.h>
#include <utility/utils.h>

namespace plarium
{

namespace system
{

//raise pthread exception
void pthread_raise_error (const char* source, int status);

}

}
