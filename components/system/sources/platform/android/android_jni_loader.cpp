#include "shared.h"

using namespace syslib::android;

namespace
{

/// Константы
const char* SKELETON_ACTIVITY_CLASS_NAME                    = "com/untgames/android/funner_launcher/SkeletonActivity";
const char* SKELETON_ACTIVITY_START_APPLICATION_METHOD_NAME = "startApplication";

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

void JNICALL startApplication (JNIEnv* env, jobject thiz, jstring jprogram_name, jstring jprogram_args)
{
  if (!env)
  {  
    printf ("Bad JNIEnv passed to SkeletonActivity::startApplication\n");
    return;
  }
  
  if (!jprogram_name || !jprogram_args)
  {
    printf ("Bad arguments passed to SkeletonActivity::startApplication\n");
    return;    
  }
  
  jni_string program_name (env, jprogram_name), program_args (env, jprogram_args);
 
  printf ("Starting program '%s' (args='%s')\n", program_name.get (), program_args.get ());
  fflush (stdout);
  
  try
  {   
    start_application (env, program_name.get (), program_args.get ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception appears\n");
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
  
  JNIEnv* env;
  
  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    printf ("Can't get Java environment\n");
    fflush (stdout);
    
    return -1;
  }
  
  jclass skeletonActivityClass = env->FindClass (SKELETON_ACTIVITY_CLASS_NAME);  
  
  if (!skeletonActivityClass)
  {
    printf ("Can't find SkeletonActivity class\n");
    fflush (stdout);
    
    return -1;
  }
  
  JNINativeMethod method;
  
  memset (&method, 0, sizeof (JNINativeMethod));
  
  method.name      = SKELETON_ACTIVITY_START_APPLICATION_METHOD_NAME;
  method.signature = "(Ljava/lang/String;Ljava/lang/String;)V";
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
