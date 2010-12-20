#include <jni.h>
#include <dlfcn.h>
#include <stdio.h>

#include <xtl/common_exceptions.h>

namespace
{

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

/// Обёртка для работы с библиотекой
class jni_shared_library
{
  public:
    jni_shared_library (const char* name)
      : library (0)
    {
      static size_t dlopen_flags = RTLD_NOW | RTLD_GLOBAL;

      library = dlopen (name, dlopen_flags);

      if (!library)
        raise_error ("::dlopen failed");
    }
    
    ~jni_shared_library ()
    {
      dlclose (library);
    }
    
    const void* find_symbol (const char* name) const
    {
      return dlsym (library, name);
    }
    
    const void* get_symbol (const char* name) const
    {
      const void* addr = find_symbol (name);
      
      if (!addr)
        throw xtl::format_operation_exception ("::dlsym", "Symbol '%s' not found", name);
        
      return addr;
    }
    
  private:
    void raise_error (const char* source)
    {
      const char* error = dlerror ();

      if (error)
        throw xtl::format_operation_exception (source, error);

      throw xtl::format_operation_exception (source, "Unknown error");
    }
  
  private:
    void* library;
};

}

extern "C"
{

JNIEXPORT void JNICALL Java_com_untgames_android_funner_1launcher_SkeletonActivity_startApplication
 (JNIEnv* env,
  jobject thiz,
  jstring jprogram_name,
  jstring jprogram_args)
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
    jni_shared_library library (program_name.get ());
    
    typedef void (*start_fn)(JNIEnv* env, const char* args);
    
    start_fn start = (start_fn)library.get_symbol ("android_start_application");
    
    start (env, program_args.get ());
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
