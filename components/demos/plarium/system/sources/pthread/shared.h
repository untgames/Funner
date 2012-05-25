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

}

}
