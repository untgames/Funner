#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const char* LOG_NAME_PREFIX = "common.threads"; //имя потока протоколирования

}

/*
    Описание реализации нити
*/

struct Thread::Impl: public IThreadCallback, public xtl::reference_counter
{
  Function                      thread_function; //функция нити
  ICustomThreadSystem::thread_t handle;          //идентификатор нити
  stl::string                   name;            //имя нити
  int                           exit_code;       //код выхода нити

///Конструкторы
  Impl () : handle (0), exit_code (0) {}  
  
  Impl (const char* in_name, const Function& in_thread_function)
    : thread_function (in_thread_function),
      name (in_name)
  {
    handle = Platform::GetThreadSystem ()->CreateThread (this);
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
      Platform::GetThreadSystem ()->DeleteThread (handle);
    }
    catch (...)
    {
      //подавление всех исключений 
    }
  }
  
///Запуск на выполнение
  void Run ()
  {
    try
    {
      try
      {
        exit_code = thread_function ();
      }
      catch (std::exception& exception)
      {
        Log (common::format ("%s.%s", LOG_NAME_PREFIX, name.c_str ()).c_str ()).Printf ("%s", exception.what ());
      }
      catch (...)
      {
        Log (common::format ("%s.%s", LOG_NAME_PREFIX, name.c_str ()).c_str ()).Printf ("unknown exception");
      }
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

///Подсчёт ссылок
  void AddRef () { addref (this); }

  void Release () { release (this); }
};

/*
    Конструкторы / деструктор / присваивание
*/

Thread::Thread ()
  : impl (new Impl)
{
}

Thread::Thread (const Function& thread_function)
{
  try
  {
    static size_t thread_auto_counter = 0;

    ++thread_auto_counter;

    impl = new Impl (format ("Thread%u", thread_auto_counter).c_str (), thread_function);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::Thread(const Function&, ThreadState)");
    throw;
  }
}

Thread::Thread (const char* name, const Function& thread_function)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl = new Impl (name, thread_function);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::Thread(const char*, const Function&, ThreadState)");
    throw;
  }
}

Thread::Thread (const Thread& thread)
  : impl (thread.impl)
{
  addref (impl);
}

Thread::~Thread ()
{
  release (impl);
}

Thread& Thread::operator = (const Thread& thread)
{
  Thread (thread).Swap (*this);

  return *this;
}

/*
    Имя нити
*/

const char* Thread::Name () const
{
  return impl->name.c_str ();
}

/*
    Отмена нити
*/

void Thread::Cancel ()
{
  if (!impl->handle)
    return;

  Platform::GetThreadSystem ()->CancelThread (impl->handle);
  
  impl->exit_code = THREAD_CANCELED_EXIT_CODE;
}

/*
    Ожидание завершения нити
*/

int Thread::Join ()
{
  if (!impl->handle)
    return 0;    

  Platform::GetThreadSystem ()->JoinThread (impl->handle);

  return impl->exit_code;
}

/*
    Обмен
*/

void Thread::Swap (Thread& thread)
{
  stl::swap (impl, thread.impl);
}

namespace common
{

void swap (Thread& thread1, Thread& thread2)
{
  thread1.Swap (thread2);
}

}
