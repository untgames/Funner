#include <ctime>
#include <cstdio>

#include <jni.h>
#include <android/looper.h>
#include <android/native_window_jni.h>

#include <stl/auto_ptr.h>

#include <xtl/type_traits>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/keydefs.h>
#include <syslib/thread.h>
#include <syslib/platform/android.h>

#include <platform/platform.h>
#include <platform/message_queue.h>

namespace syslib
{

namespace android
{

/// получение виртуальной машины
JavaVM* get_vm ();

/// получение окружения текущей нити
JNIEnv& get_env ();

/*
    JNI ссылки
*/

///Стратегия владения локальной JNI ссылкой
template <class T> struct jni_local_ref_strategy
{
  static T*   clone   (T* ptr) { get_env ().NewLocalRef (ptr); return ptr; }
  static void release (T* ptr) { get_env ().DeleteLocalRef (ptr); }  
};

///Стратегия владения глобальной JNI ссылкой
template <class T> struct jni_global_ref_strategy
{
  static T*   clone   (T* ptr) { get_env ().NewGlobalRef (ptr); return ptr; }
  static void release (T* ptr) { get_env ().DeleteGlobalRef (ptr); }  
};

template <class T> class global_ref;

/// Локальная JNI ссылка
template <class T> class local_ref: public xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy>
{
  typedef xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_local_ref_strategy> base;
  public:
    typedef typename base::element_type element_type;

    local_ref () {}
    local_ref (element_type* ptr, bool addref = true) : base (ptr, addref) {}
    local_ref (const local_ref& ref) : base (ref) {}

    template <class T1> local_ref (const local_ref<T1>& ref) : base (ref) {}    
    template <class T1> local_ref (const global_ref<T1>& ref) : base (&*ref) {}    
};

/// Глобальная JNI ссылка
template <class T> class global_ref: public xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_global_ref_strategy>
{
  typedef xtl::intrusive_ptr<typename xtl::type_traits::remove_pointer<T>::type, jni_global_ref_strategy> base;
  public:
    typedef typename base::element_type element_type;

    global_ref () {}
    global_ref (element_type* ptr, bool addref = true) : base (ptr, addref) {}
    global_ref (const global_ref& ref) : base (ref) {}

    template <class T1> global_ref (const global_ref<T1>& ref) : base (ref) {}
    template <class T1> global_ref (const local_ref<T1>& ref) : base (&*ref) {}    
};

/*
    Поиск методов
*/

inline jmethodID find_static_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetStaticMethodID (class_, name, signature);
  
  if (!result)
    throw xtl::format_operation_exception ("JNIEnv::GetStaticMethodID", "Static method '%s %s' not found", name, signature);
    
  return result;
}

inline jmethodID find_method (JNIEnv* env, jclass class_, const char* name, const char* signature)
{
  jmethodID result = env->GetMethodID (class_, name, signature);
  
  if (!result)
    throw xtl::format_operation_exception ("JNIEnv::GetMethodID", "Method '%s %s' not found", name, signature);
    
  return result;
}

/*
    Обёртка для работы со строками
*/

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

/*
    Утилиты
*/

jstring tojstring    (const char* s);
void    check_errors ();

template <class T> T check_errors (T result)
{
  check_errors ();
  
  return result;
}

/*
    Управление запуском
*/

///Контекст запуска приложения
struct ApplicationContext
{
  JavaVM*             vm;          //виртуальная машина
  global_ref<jobject> activity;    //activity, запустившее приложение
  global_ref<jclass>  utils_class; //EngineUtils class
  
  ApplicationContext () : vm (0) {}
};

/// точка входа в приложение
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args);

/// регистрация методов обратного вызова окна
void register_window_callbacks (JNIEnv* env);

/// получение контекста запуска приложения
const ApplicationContext& get_context ();

}

}

