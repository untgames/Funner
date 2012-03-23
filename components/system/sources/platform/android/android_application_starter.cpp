#include "shared.h"

using namespace syslib::android;

extern "C"
{

int main (...);

}

namespace
{

/*
     онстанты
*/

const char* THREAD_NAME             = "system.android.launcher";                     //им€ нити приложени€
const char* ENGINE_UTILS_CLASS_NAME = "com/untgames/funner/application/EngineUtils"; //им€ класса java утилит
const char* APK_FULL_PATH           = "APK_FULL_PATH";                               //им€ переменной пути к APK-файлу
const char* SEARCH_PATHS            = "SEARCH_PATHS";                                //им€ переменной списка путей поиска файлов

///  онтекст запуска приложени€
ApplicationContext application_context;

/// ѕараметры запуска приложени€
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

/// Ќить приложени€
class ApplicationThread: private ApplicationStartArgs
{
  public:
/// ћетод запуска нити приложени€
    static void Start (const char* program_name, const char* args, const char* env_vars)
    {
      static stl::auto_ptr<ApplicationThread> instance;
      
      if (instance)
        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Start", "Application thread has already started");
        
      instance = stl::auto_ptr<ApplicationThread> (new ApplicationThread (program_name, args, env_vars));
    }

  private:
///  онструктор
    ApplicationThread (const char* program_name, const char* in_args, const char* in_env_vars)
      : ApplicationStartArgs (program_name, in_args, in_env_vars)
      , thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
      thread.SetPriority (syslib::ThreadPriority_High);
    }
    
/// ‘ункци€ нити
    int ThreadRoutine ()
    {
      try
      {
          //получение окружени€
        
        JNIEnv* env = 0;
        
        jint status = get_context ().vm->AttachCurrentThread (&env, 0);
        
        if (status)
          throw xtl::format_operation_exception ("", "JavaVM::AttachCurrentThread failed (status=%d)", status);          

          //передача управлени€ программе
        
        int exit_code = 0;
        
        try
        {
            //формирование окружени€

          for (size_t i=0, count=env_vars.Size (); i<count; i++)
          {
            const char* name  = env_vars.PropertyName (i);
            const char* value = env_vars.GetString (i);
            
            if (putenv (common::format ("%s=%s", name, value).c_str ()))
              throw xtl::format_operation_exception ("", "::putenv failed for '%s=%s'", name, value);
          }

            //предварительные действи€

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
        log_info ("%s\n  at syslib::android::ApplicationThread::ThreadRoutine", e.what ());
      }
      catch (...)
      {
        log_info ("unhandled exception\n  at syslib::android::ApplicationThread::ThreadRoutine");
      }
      
      Exit (0);
      
      return 0;
    }
    
/// ѕосылка сообщени€ JavaVM о необходимости остановки приложени€
    void Exit (int code)
    {
      exit (code);
    }
    
  private:
///ѕредварительные действи€
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
    
    //сохранение контекста запуска приложени€

  application_context.vm       = vm;
  application_context.activity = activity;
  
  application_context.utils_class = get_env ().FindClass (ENGINE_UTILS_CLASS_NAME);
  
  if (!application_context.utils_class)
    throw xtl::format_operation_exception ("", "EngineUtils class '%s' not found", ENGINE_UTILS_CLASS_NAME);    
    
    //запуск нити приложени€
    
  ApplicationThread::Start (program_name, args, env_vars);
}

/// получение контекста запуска приложени€
const ApplicationContext& get_context ()
{
  return application_context;
}

/// получение виртуальной машины
JavaVM* get_vm ()
{
  return application_context.vm;
}

/// получение окружени€ текущей нити
JNIEnv& get_env ()
{
  JNIEnv* env = 0;

  jint status = application_context.vm->GetEnv ((void**)&env, JNI_VERSION_1_4);
  
  if (status || !env)
    throw xtl::format_operation_exception ("syslib::android::get_env", "JavaVM::GetEnv failed (status=%d)", status);
    
  return *env;
}

}

}
