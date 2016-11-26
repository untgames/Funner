#include "shared.h"

using namespace syslib;
using namespace syslib::android;

extern "C"
{

int main (...);

}

namespace
{

/*
    Константы
*/

const char* THREAD_NAME                             = "system.android.launcher";                                    //имя нити приложения
const char* ENGINE_UTILS_CLASS_NAME                 = "com/untgames/funner/application/EngineUtils";                //имя класса java утилит
const char* ENGINE_SENSOR_EVENT_LISTENER_CLASS_NAME = "com/untgames/funner/application/EngineSensorEventListener";  //имя класса слушателя событий сенсора
const char* APK_FULL_PATH                           = "APK_FULL_PATH";                                              //имя переменной пути к APK-файлу
const char* SEARCH_PATHS                            = "SEARCH_PATHS";                                               //имя переменной списка путей поиска файлов

/// Контекст запуска приложения
ApplicationContext application_context;

/// Защита activity
Mutex current_activity_mutex;

/// Текущее activity
global_ref<jobject> current_activity;

/// Параметры запуска приложения
struct ApplicationStartArgs
{
  common::StringArray args;
  common::PropertyMap env_vars;
  
  ApplicationStartArgs (const char* program_name, const char* in_args, const char* in_env_vars)
  {
    env_vars = common::parse_init_string (in_env_vars);
    
      //разбор параметров запуска
      
    args.Add (program_name);
    args.Add (common::split (in_args));    
  }
};

/// Нить приложения
class ApplicationThread: private ApplicationStartArgs
{
  public:
/// Метод запуска нити приложения
    static void Start (const char* program_name, const char* args, const char* env_vars)
    {
      static stl::auto_ptr<ApplicationThread> instance;
      
      if (instance)
        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Start", "Application thread has already started");
        
      instance = stl::auto_ptr<ApplicationThread> (new ApplicationThread (program_name, args, env_vars));
    }

  private:
/// Конструктор
    ApplicationThread (const char* program_name, const char* in_args, const char* in_env_vars)
      : ApplicationStartArgs (program_name, in_args, in_env_vars)
      , thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
//      thread.SetPriority (syslib::ThreadPriority_High);
    }
    
/// Функция нити
    int ThreadRoutine ()
    {
      try
      {
          //получение окружения
        
        JNIEnv* env = 0;
        
        jint status = get_context ().vm->AttachCurrentThread (&env, 0);
        
        if (status)
          throw xtl::format_operation_exception ("", "JavaVM::AttachCurrentThread failed (status=%d)", status);          

          //передача управления программе
        
        int exit_code = 0;
        
        try
        {
            //формирование окружения

          for (size_t i=0, count=env_vars.Size (); i<count; i++)
          {
            const char* name  = env_vars.PropertyName (i);
            const char* value = env_vars.GetString (i);
            
            if (setenv (name, value, 1))
              throw xtl::format_operation_exception ("", "::setenv failed for '%s=%s'", name, value);
          }

            //предварительные действия

          Preinit ();

            //запуск

          exit_code = main (args.Size (), args.Data (), environ);          
          
//          get_vm ()->DetachCurrentThread ();
        }
        catch (...)
        {
//          get_vm ()->DetachCurrentThread ();          
          throw;
        }
        
        Exit (exit_code);
      }
      catch (std::exception& e)
      {        
        log_error ("%s\n  at syslib::android::ApplicationThread::ThreadRoutine", e.what ());
      }
      catch (...)
      {
        log_error ("unhandled exception\n  at syslib::android::ApplicationThread::ThreadRoutine");
      }
      
      Exit (0);
      
      return 0;
    }
    
/// Посылка сообщения JavaVM о необходимости остановки приложения
    void Exit (int code)
    {
      exit (code);
    }
    
  private:
///Предварительные действия
    void Preinit ()
    {
      try
      {
        const char* apk_path = getenv (APK_FULL_PATH);
        
        if (apk_path)
          common::FileSystem::AddSearchPath (common::format ("/std/%s", apk_path).c_str ());

        const char* search_paths = getenv (SEARCH_PATHS);
        
        if (search_paths)
        {
          common::StringArray paths = common::split (search_paths);
          
          for (size_t i=0, count=paths.Size (); i<count; i++)
            common::FileSystem::AddSearchPath (paths [i]);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::ApplicationThread::Preinit");
        throw;
      }
    }
  
  private:
    syslib::Thread       thread;
    common::StringArray  args;
};

void on_application_notification (const char* notification)
{
  try
  {
    if (!notification)
      throw xtl::make_null_argument_exception ("", "notification");

    local_ref<jobject> activity (get_activity ());
    local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

    if (!activity_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

    jmethodID on_application_notification_method = find_method (&get_env (), activity_class.get (), "onApplicationNotification", "(Ljava/lang/String;)V");

    get_env ().CallVoidMethod (activity.get (), on_application_notification_method, tojstring (notification).get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::on_application_notification");
    throw;
  }
}

}

namespace syslib
{

namespace android
{

/// точка входа в приложение
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args, const char* env_vars)
{
  static const char* METHOD_NAME = "syslib::android::start_application";
  
    //проверка корректности аргументов

  if (!vm)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vm");
    
  if (!activity)
    throw xtl::make_null_argument_exception (METHOD_NAME, "activity");    
    
  if (!program_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "program_name");       
    
  if (!args)
    throw xtl::make_null_argument_exception (METHOD_NAME, "args");

  if (!env_vars)
    throw xtl::make_null_argument_exception (METHOD_NAME, "env_vars"); 
    
    //сохранение контекста запуска приложения

  application_context.vm = vm;
  
  current_activity = activity;
  
  application_context.utils_class                 = local_ref<jclass> (get_env ().FindClass (ENGINE_UTILS_CLASS_NAME), false);
  application_context.sensor_event_listener_class = local_ref<jclass> (get_env ().FindClass (ENGINE_SENSOR_EVENT_LISTENER_CLASS_NAME), false);
  
  if (!application_context.utils_class)
    throw xtl::format_operation_exception ("", "EngineUtils class '%s' not found", ENGINE_UTILS_CLASS_NAME);    
    
  if (!application_context.sensor_event_listener_class)
    throw xtl::format_operation_exception ("", "Can't find EngineSensorEventListener class\n");    

  Application::RegisterNotificationHandler ("*", &on_application_notification);

    //запуск нити приложения
    
  ApplicationThread::Start (program_name, args, env_vars);
}

/// получение контекста запуска приложения
const ApplicationContext& get_context ()
{
  return application_context;
}

/// изменение activity
void set_activity (jobject activity)
{
  Lock lock (current_activity_mutex);

  current_activity = activity;
}

/// получение activity
jobject get_activity ()
{
  Lock lock (current_activity_mutex);

  return current_activity.get ();
}

/// получение виртуальной машины
JavaVM* get_vm ()
{
  return application_context.vm;
}

/// получение окружения текущей нити
JNIEnv& get_env ()
{
  JNIEnv* env = 0;

  jint status = application_context.vm->GetEnv ((void**)&env, JNI_VERSION_1_4);
  
  if (status == JNI_EDETACHED)
    status = application_context.vm->AttachCurrentThread (&env, 0);

  if (status || !env)
    throw xtl::format_operation_exception ("syslib::android::get_env", "JavaVM::GetEnv failed (status=%d)", status);
    
  return *env;
}

}

}
