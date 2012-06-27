#include <sgi_stl/stdexcept>

#include <system/condition.h>
#include <system/mutex.h>
#include <system/thread.h>
#include <utility/utils.h>

#include <process.h>
#include <windows.h>
#include <stdio.h>

namespace plarium
{

namespace system
{

//Check WinAPI usage erros and exception throwing
void check_errors (const char* source);
void raise_error (const char* source);

void thread_init ();
void thread_done (void*);

}

}
