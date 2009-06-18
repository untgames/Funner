#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#include <platform/platform.h>

namespace syslib
{

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status);

}
