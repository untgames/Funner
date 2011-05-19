#include "../shared.h"

using namespace syslib;
using namespace syslib::bada;

namespace
{

class ThreadImpl : public Osp::Base::Runtime::Thread
{
  public:
    ThreadImpl (IThreadCallback* in_callback)
      : callback (in_callback)
      {}

    Osp::Base::Object* Run ()
    {
      callback->Run ();

      return 0;
    }

  private:
    xtl::com_ptr<IThreadCallback> callback;
};

}


struct syslib::thread_handle
{
  ThreadImpl* thread;

  ~thread_handle ()
  {
    delete thread;
  }
};

/*
    Создание / удаление нити
*/

BadaThreadManager::thread_t BadaThreadManager::CreateThread (IThreadCallback* in_callback)
{
  try
  {
      //проверка корректности аргументов, захват объекта обслуживания нити

    if (!in_callback)
      throw xtl::make_null_argument_exception ("", "callback");

    xtl::com_ptr<IThreadCallback> callback (in_callback);

      //создание нити

    stl::auto_ptr<thread_handle> handle (new thread_handle);

    handle->thread = new ThreadImpl (callback.get ());

    result r = handle->thread->Construct ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Thread::Construct", r);

    r = handle->thread->Start ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Thread::Start", r);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::CreateThread");
    throw;
  }
}

void BadaThreadManager::DestroyThread (thread_t thread)
{
  if (!thread)
    return;

  delete thread;
}

/*
    Ожидание завершения нити
*/

void BadaThreadManager::JoinThread (thread_t thread)
{
  try
  {
    if (!thread)
      throw xtl::make_null_argument_exception ("", "thread");

    result r = thread->thread->Join ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Thread::Join", r);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::JoinThread");
    throw;
  }
}

/*
   Получение идентификатора нити
*/

size_t BadaThreadManager::GetThreadId (thread_t thread)
{
  return (size_t)thread->thread;
}

size_t BadaThreadManager::GetCurrentThreadId ()
{
  return (size_t)Osp::Base::Runtime::Thread::GetCurrentThread ();
}
