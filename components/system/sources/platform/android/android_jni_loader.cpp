#include "shared.h"

using namespace syslib::android;

namespace
{

/// Константы
const char* ENGINE_ACTIVITY_CLASS_NAME                    = "com/untgames/funner/application/EngineActivity";
const char* ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME = "startApplication";

/// Виртуальная машина
JavaVM* java_vm = 0;

jint JNICALL startApplication (JNIEnv* env, jobject thiz, jstring jprogram_name, jstring jwork_dir, jstring jprogram_args, jstring jenv_vars)
{
  if (!env)
  {  
    log_printf ("Bad JNIEnv passed to EngineActivity::startApplication\n");
    return 0;
  }
  
  if (!jprogram_name || !jprogram_args || !jwork_dir)
  {
    log_printf ("Bad arguments passed to EngineActivity::startApplication\n");
    return 0; 
  }
  
  jni_string program_name (env, jprogram_name), program_args (env, jprogram_args), work_dir (env, jwork_dir), env_vars (env, jenv_vars);
 
  log_printf ("Starting program '%s' (args='%s', workdir='%s', env='%s')\n", program_name.get (), program_args.get (), work_dir.get (), env_vars.get ());
  
  try
  {   
    if (chdir (work_dir.get ()))
    {
      log_printf ("Can't set current working dir to '%s'. %s\n", work_dir.get (), strerror (errno));
      
      return 0;
    }
    
    start_application (java_vm, thiz, program_name.get (), program_args.get (), env_vars.get ());
    
    return 1;
  }
  catch (std::exception& e)
  {
    log_printf ("%s\n  at syslib::android::startApplication\n", e.what ());
    
    return 0;
  }
  catch (...)
  {
    log_printf ("unknown exception appears\n  at syslib::android::startApplication\n");
    
    return 0;
  }
}

}

extern "C"
{

__attribute__ ((visibility("default"))) extern JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void* reserved)
{
  if (!vm)
  {
    log_printf ("JavaVM is null\n");

    return -1; 
  }
  
  java_vm = vm;
  
  JNIEnv* env;
  
  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    log_printf ("Can't get Java environment\n");
    
    return -1;
  }
  
  void* libdvm = dlopen ("libdvm.so", RTLD_NOW | RTLD_GLOBAL);

  if (!libdvm)
  {
    log_printf ("libdvm not found\n");
    
    return -1;
  }

  typedef bool (*dvmStdioConverterStartupFn)();
  
  dvmStdioConverterStartupFn dvmStdioConverterStartup = (dvmStdioConverterStartupFn)dlsym (libdvm, "dvmStdioConverterStartup");
  
  if (!dvmStdioConverterStartup)  
  {
    log_printf ("libdvm.dvmStdioConverterStartup not found\n");
    
    return -1;    
  }
  
  if (!dvmStdioConverterStartup ())
  {
    log_printf ("libdvm.dvmStdioConverterStartup () failed\n");
    
    return -1;        
  }
  
  jclass skeletonActivityClass = env->FindClass (ENGINE_ACTIVITY_CLASS_NAME); 
  
  if (!skeletonActivityClass)
  {
    log_printf ("Can't find EngineActivity class\n");
   
    
    return -1;
  }
  
  JNINativeMethod method;
  
  memset (&method, 0, sizeof (JNINativeMethod));
  
  method.name      = ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME;
  method.signature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I";
  method.fnPtr     = (void*)&startApplication;

  jint status = env->RegisterNatives (skeletonActivityClass, &method, 1);

  if (status)
  {
    log_printf ("Can't register natives (status=%d)\n", status);
    
    return -1;
  }
  
  try
  {
    register_window_callbacks (env);
  }
  catch (std::exception& e)
  {
    log_printf ("Can't register window callbacks: %s\n", e.what ());

    return -1;    
  }
  catch (...)
  {
    log_printf ("Can't register window callbacks: unknown exception\n");

    return -1;    
  }

  return JNI_VERSION_1_4;
}

}
