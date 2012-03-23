#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

jclass    log_class;
jmethodID log_println_method = 0;

int println (int level, const char* message)
{
  if (!log_class)
    return 0;

  if (!log_println_method)
    return 0;

  if (!message)
    return 0;

  try
  {    
    JNIEnv& env = get_env ();    

    return check_errors (env.CallStaticIntMethod (log_class, log_println_method, level, tojstring (message).get ()));
  }
  catch (...)
  {
    return 0;
  }
}

}

namespace syslib
{

namespace android
{

int log_vprintf (int level, const char* format, va_list args)
{
  return println (level, common::vformat (format, args).c_str ());
}

int log_printf (int level, const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return log_vprintf (level, format, args);
}

int log_info (const char* format, ...)
{
  va_list args;

  va_start (args, format);

  return log_vprintf (ANDROID_LOG_INFO, format, args);
}

int log_error (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return log_vprintf (ANDROID_LOG_ERROR, format, args);
}

void register_log_entries (JNIEnv* env, jclass in_log_class)
{
  log_class = (jclass)env->NewGlobalRef (in_log_class);

  if (!log_class)
    return;

  log_println_method = env->GetStaticMethodID (log_class, "printLog", "(ILjava/lang/String;)I");
}

}

}
