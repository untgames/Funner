#include "shared.h"

using namespace syslib::android;

namespace
{

/// Константы
const char* ENGINE_ACTIVITY_CLASS_NAME                    = "com/untgames/funner/application/EngineActivity";
const char* ENGINE_WINDOW_CLASS_NAME                      = "com/untgames/funner/application/EngineView";
const char* ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME = "startApplication";

/// Виртуальная машина
JavaVM* java_vm = 0;

/// Обёртка для работы со строками
class jni_string
{
  public:
    jni_string (JNIEnv* inEnv, jstring inJavaString)
      : env (inEnv)
      , javaString (inJavaString)
      , string (0)
    {
      string = env->GetStringUTFChars (javaString, 0);            
    }
    
    ~jni_string ()
    {
      if (!string || !env || !javaString)
        return;
        
      env->ReleaseStringUTFChars (javaString, string);
    }
    
    const char* get () const { return string; } 
  
  private:
    JNIEnv*     env;
    jstring     javaString;
    const char* string;
};

jint JNICALL startApplication (JNIEnv* env, jobject thiz, jstring jprogram_name, jstring jprogram_args)
{
  if (!env)
  {  
    printf ("Bad JNIEnv passed to EngineActivity::startApplication\n");
    return 0;
  }
  
  if (!jprogram_name || !jprogram_args)
  {
    printf ("Bad arguments passed to EngineActivity::startApplication\n");
    return 0; 
  }
  
  jni_string program_name (env, jprogram_name), program_args (env, jprogram_args);
 
  printf ("Starting program '%s' (args='%s')\n", program_name.get (), program_args.get ());
  fflush (stdout);
  
  try
  {   
    ApplicationContext context;
    
    memset (&context, 0, sizeof (context));
    
    jclass window_class = env->FindClass (ENGINE_WINDOW_CLASS_NAME);
    
    if (!window_class)
      throw xtl::format_operation_exception ("", "Window class '%s' not found", ENGINE_WINDOW_CLASS_NAME);    
    
    start_application (java_vm, thiz, program_name.get (), program_args.get ());
    
    return 1;
  }
  catch (std::exception& e)
  {
    printf ("%s\n  at syslib::android::startApplication\n", e.what ());
    fflush (stdout);
    
    return 0;
  }
  catch (...)
  {
    printf ("unknown exception appears\n  at syslib::android::startApplication\n");
    fflush (stdout);
    
    return 0;
  }
}

}

extern "C"
{

extern JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void* reserved)
{
  if (!vm)
  {
    printf ("JavaVM is null\n");
    fflush (stdout);

    return -1; 
  }
  
  java_vm = vm;
  
  JNIEnv* env;
  
  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    printf ("Can't get Java environment\n");
    fflush (stdout);
    
    return -1;
  }
  
  jclass skeletonActivityClass = env->FindClass (ENGINE_ACTIVITY_CLASS_NAME); 
  
  if (!skeletonActivityClass)
  {
    printf ("Can't find EngineActivity class\n");
    fflush (stdout);
    
    return -1;
  }
  
  JNINativeMethod method;
  
  memset (&method, 0, sizeof (JNINativeMethod));
  
  method.name      = ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME;
  method.signature = "(Ljava/lang/String;Ljava/lang/String;)I";
  method.fnPtr     = (void*)&startApplication;

  jint status = env->RegisterNatives (skeletonActivityClass, &method, 1);

  if (status)
  {
    printf ("Can't register natives (status=%d)\n", status);
    fflush (stdout);
    
    return -1;
  }

  return JNI_VERSION_1_4;
}

}
