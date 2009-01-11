#include <pthread.h>

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#include <platform/platform.h>

namespace syslib
{

#ifdef _WIN32

void thread_init ();      //функция инициализации библиотеки
void thread_done (void*); //функция деинициализации библиотеки

#else

inline void thread_init () {}
inline void thread_done (void*) {}

#endif

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status);

}
