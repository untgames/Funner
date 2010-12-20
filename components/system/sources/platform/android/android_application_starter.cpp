#include "shared.h"

extern "C"
{

int main (...);

}

namespace
{

struct A
{
  A () { printf ("A::A\n"); fflush (stdout); }
  ~A () { printf ("A::~A\n"); fflush (stdout); }
};

A a;

/*
    Константы
*/

const char* THREAD_NAME = "system.android.launcher"; //имя нити приложения

/// Объект виртуальной машины
JavaVM* java_vm = 0;

/// Нить приложения
class ApplicationThread
{
  public:
/// Метод запуска нити приложения
    static void Start (const char* program_name, const char* args)
    {
      static stl::auto_ptr<ApplicationThread> instance;
      
      if (instance)
        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Start", "Application thread has already started");
        
      instance = stl::auto_ptr<ApplicationThread> (new ApplicationThread (program_name, args));
    }

  private:
/// Конструктор
    ApplicationThread (const char* program_name, const char* in_args)
      : thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
        //разбор параметров запуска
        
      args.Add (program_name);
      args.Add (common::split (in_args));   
    }
    
/// Функция нити
    int ThreadRoutine ()
    {
      try
      {
        int exit_code = main (args.Size (), args.Data ());
        
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
      
      return 0;
    }
    
/// Посылка сообщения JavaVM о необходимости остановки приложения
    void Exit (int code)
    {
//      if (!java_vm)
//        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Exit", "Null JavaVM");        
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
void start_application (JavaVM* vm, const char* program_name, const char* args)
{
  static const char* METHOD_NAME = "syslib::android::start_application";
  
    //проверка корректности аргументов

  if (!vm)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vm");
    
  if (!program_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "program_name");
    
  if (!args)
    throw xtl::make_null_argument_exception (METHOD_NAME, "args");
    
    //сохранение объекта виртуальной машины    

  java_vm = vm;
  
    //запуск нити приложения
    
  ApplicationThread::Start (program_name, args);
}

/// получение объекта виртуальной машины
JavaVM* get_vm ()
{
  return java_vm;
}

}

}
