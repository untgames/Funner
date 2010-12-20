#include <ctime>
#include <cstdio>

#include <jni.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <platform/platform.h>

namespace syslib
{

namespace android
{

void start_application (JNIEnv* env, const char* program_name, const char* args);

}

}

