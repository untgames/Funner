#include "shared.h"

using namespace common;

namespace syslib
{

namespace android
{

local_ref<jstring> tojstring (const char* s)
{
  if (!s)
    return 0;

  return local_ref<jstring> (get_env ().NewStringUTF (s), false);
}

void check_errors ()
{
  JNIEnv& env = get_env ();
  
  if (!env.ExceptionCheck ())
    return;
    
  local_ref<jthrowable> exception (env.ExceptionOccurred (), false);
  
  if (!exception)
    return;
    
  env.ExceptionClear ();
    
  jmethodID get_stack_trace_method = 0;
  
  const ApplicationContext& context = get_context ();

  if (context.utils_class)
    get_stack_trace_method = env.GetStaticMethodID (context.utils_class.get (), "getStackTrace", "(Ljava/lang/Throwable;)Ljava/lang/String;");
    
  if (!get_stack_trace_method)
    throw xtl::format_operation_exception ("", "JNI exception occurred");  
  
  local_ref<jstring> message = (jstring)env.CallStaticObjectMethod (context.utils_class.get (), get_stack_trace_method, exception.get ());
  
  if (!message)
    throw xtl::format_operation_exception ("", "JNI exception occurred");  

  throw xtl::format_operation_exception ("", jni_string (&env, message.get ()).get ());
}

}

}
