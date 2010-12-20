#include <ctime>
#include <cstdio>

#include <jni.h>

#include <stl/auto_ptr.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/strlib.h>

#include <syslib/thread.h>

#include <platform/platform.h>

namespace syslib
{

namespace android
{

/// точка входа в приложение
void start_application (JavaVM* vm, const char* program_name, const char* args);

/// получение объекта виртуальной машины
JavaVM* get_vm ();

}

}

