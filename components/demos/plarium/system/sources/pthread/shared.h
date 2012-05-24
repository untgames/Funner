#include <cstdio>
#include <cstring>

#include <pthread.h>

#include <stdexcept>

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

#ifdef _WIN32

void thread_init ();
void thread_done (void*);

#else

inline void thread_init () {}
inline void thread_done (void*) {}

#endif

}

}
