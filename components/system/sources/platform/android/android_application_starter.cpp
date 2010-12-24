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

const char* THREAD_NAME = "system.android.launcher"; //им€ нити приложени€

///  онтекст запуска приложени€
ApplicationContext application_context;

/// Ќить приложени€
class ApplicationThread
{
  public:
/// ћетод запуска нити приложени€
    static void Start (const char* program_name, const char* args)
    {
      static stl::auto_ptr<ApplicationThread> instance;
      
      if (instance)
        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Start", "Application thread has already started");
        
      instance = stl::auto_ptr<ApplicationThread> (new ApplicationThread (program_name, args));
    }

  private:
///  онструктор
    ApplicationThread (const char* program_name, const char* in_args)
      : thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
        //разбор параметров запуска
        
      args.Add (program_name);
      args.Add (common::split (in_args));   
    }
    
/// ‘ункци€ нити
    int ThreadRoutine ()
    {
      try
      {
        JNIEnv* env = 0;
        
        jint status = get_context ().vm->AttachCurrentThread (&env, 0);
        
        if (status)
          throw xtl::format_operation_exception ("", "JavaVM::AttachCurrentThread failed (status=%d)", status);
          
        int exit_code = 0;
        
        try
        {
          exit_code = main (args.Size (), args.Data ());
          
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
        printf ("%s\n  at syslib::android::ApplicationThread::ThreadRoutine", e.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unhandled exception\n  at syslib::android::ApplicationThread::ThreadRoutine");
        fflush (stdout);        
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
    syslib::Thread       thread;
    common::StringArray  args;
};

}

namespace syslib
{

namespace android
{

/// точка входа в приложение
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args)
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
    
    //сохранение контекста запуска приложени€

  application_context.vm       = vm;
  application_context.activity = activity;
  
    //запуск нити приложени€
    
  ApplicationThread::Start (program_name, args);
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
