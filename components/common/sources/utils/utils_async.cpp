#include <stl/auto_ptr.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string_buffer.h>

#include <common/action_queue.h>
#include <common/async.h>
#include <common/lockable.h>
#include <common/log.h>

#include <platform/platform.h>

using namespace common;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "common.async_result"; //поток отладочного протоколирования

}

/*
===================================================================================================
    AsyncResult
===================================================================================================
*/

/*
    Описание реализации асинхронного результата
*/

struct AsyncResult::Impl: public xtl::reference_counter, public Lockable
{
  Action                              action;       //операция
  stl::auto_ptr<detail::IAsyncAction> async_action; //реализация операции
  detail::IAsyncResult*               result;       //результат
  CallbackHandler                     callback;     //оповещение об окончании выполнения операции
  bool                                completed;    //выполнена ли операция
  
  Impl ()
    : result (0)
    , completed (false)
  {
  }
  
  Impl (detail::IAsyncAction* action)
    : async_action (action)
    , result (0)
    , completed (false)
  {
    if (!action)
      throw xtl::make_null_argument_exception ("common::AsyncResult::Impl", "action");
      
    AddToQueue ();
  }
  
  Impl (detail::IAsyncAction* action, const CallbackHandler& in_callback)
    : async_action (action)
    , result (0)
    , callback (in_callback)
    , completed (false)
  {
    if (!action)
      throw xtl::make_null_argument_exception ("common::AsyncResult::Impl", "action");
      
    AddToQueue ();
  }
  
  struct ThreadCallback
  {
    CallbackHandler handler;
    ActionThread    thread_type;
    size_t          thread_id;    
    
    ThreadCallback (ActionThread in_thread_type, const CallbackHandler& in_handler)
      : handler (in_handler)
      , thread_type (in_thread_type)
      , thread_id (Platform::GetCurrentThreadId ())
    {
    }
    
    void operator () (AsyncResult& result)
    {
      switch (thread_type)
      {
        case ActionThread_Main:
        case ActionThread_Background:        
          ActionQueue::PushAction (xtl::bind (handler, result), thread_type);
          break;
        case ActionThread_Current:
          ActionQueue::PushAction (xtl::bind (handler, result), thread_id);
          break;
        default:
          throw xtl::format_operation_exception ("common::AsyncResult::ThreadCallback::operator()", "Bad ActionThread %d", thread_type);
      }      
    }
  };
  
  Impl (detail::IAsyncAction* action, ActionThread thread, const CallbackHandler& in_callback)
    : async_action (action)
    , result (0)
    , callback (ThreadCallback (thread, in_callback))
    , completed (false)
  {
    if (!action)
      throw xtl::make_null_argument_exception ("common::AsyncResult::Impl", "action");
      
    AddToQueue ();
  }  
  
  ~Impl ()
  {
    try
    {
      Lock ();
      
      if (result)
        result->Release ();
      
      action.Cancel ();
    }
    catch (...)
    {
    }
  }
  
  void AddToQueue ()
  {
    action = ActionQueue::PushAction (xtl::bind (&Impl::Perform, xtl::intrusive_ptr<Impl> (this)), ActionThread_Background);
  }
  
  void Perform ()
  {
    xtl::intrusive_ptr<Impl> hold_this = this;    
    
    detail::IAsyncResult* tmp_result     = 0;
    bool                  tmp_completed  = false;
    
    try
    {            
      tmp_result    = async_action->Perform ();      
      tmp_completed = tmp_result && tmp_result->IsValid ();
    }
    catch (std::exception& e)
    {
      Log (LOG_NAME).Printf ("%s\n    at common::AsyncResult::Impl::Perform", e.what ());
    }
    catch (...)
    {
      Log (LOG_NAME).Printf ("unknown exception\n    at common::AsyncResult::Impl::Perform");
    }
    
    action = Action ();
    
    CallbackHandler tmp_callback;

    {
      common::Lock lock (*this);
      
      tmp_callback = callback;
      result       = tmp_result;
      completed    = tmp_completed;
      
      async_action.reset ();
      callback.clear ();      
    }            

    if (tmp_callback)
    {
      try
      {
        AsyncResult async_result (this);

        tmp_callback (async_result);
      }
      catch (std::exception& e)
      {
        Log (LOG_NAME).Printf ("%s\n    at common::AsyncResult::Impl::Perform", e.what ());
      }
      catch (...)
      {
        Log (LOG_NAME).Printf ("unknown exception\n    at common::AsyncResult::Impl::Perform"); 
      }      
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

AsyncResult::AsyncResult ()
  : impl (new Impl)
{
}

AsyncResult::AsyncResult (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

AsyncResult::AsyncResult (const AsyncResult& result)
  : impl (result.impl)
{
  addref (impl);
}

AsyncResult::AsyncResult (detail::IAsyncAction* action)
  : impl (new Impl (action))
{
}

AsyncResult::AsyncResult (detail::IAsyncAction* action, const CallbackHandler& callback)
  : impl (new Impl (action, callback))
{
}

AsyncResult::AsyncResult (detail::IAsyncAction* action, ActionThread thread, const CallbackHandler& callback)
  : impl (new Impl (action, thread, callback))
{
}

AsyncResult::~AsyncResult ()
{
  release (impl);
}

AsyncResult& AsyncResult::operator = (const AsyncResult& result)
{
  AsyncResult (result).Swap (*this);
  return *this;
}

/*
    Проверка выполненности операции / ожидание завершения операции / результат
*/

bool AsyncResult::IsCompleted () const
{
  common::Lock lock (*impl);

  return impl->completed;
}

void AsyncResult::WaitCompleted ()
{
  try
  {
    common::Lock lock (*impl);
    
    impl->action.Wait (~0u);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::AsyncResult::WaitCompleted");
    throw;
  }
}

detail::IAsyncResult* AsyncResult::Result ()
{
  try
  {
    common::Lock lock (*impl);
    
    if (!impl->action.IsCompleted ())
      WaitCompleted ();

    if (!impl->result)
      throw xtl::format_operation_exception ("", "Null result");
      
    return impl->result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::AsyncResult::Result()");
    throw;
  }
}

template <>
AsyncResultType<void>::Type AsyncResult::Result<void> ()
{
  try
  {
    Result ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::AsyncResult::Result<void> ()");
    throw;
  }
}

/*
    Обмен
*/

void AsyncResult::Swap (AsyncResult& result)
{
  stl::swap (impl, result.impl);
}

namespace common
{

void swap (AsyncResult& result1, AsyncResult& result2)
{
  result1.Swap (result2);
}

}

/*
===================================================================================================
    AsyncException
===================================================================================================
*/

/*
    Описание реализации исключения асинхронной операции
*/

struct AsyncException::Impl: public xtl::reference_counter
{
  xtl::string_buffer message; //сообщение, возвращаемое исключением
  
  Impl ()
  {
    message.append ("Unknown exception at perform async operation");
  }
  
  Impl (const std::exception& e)
  {
    message.append (e.what ());
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

AsyncException::AsyncException ()
  : impl (new Impl)
{
}

AsyncException::AsyncException (const std::exception& e)
  : impl (new Impl (e))
{
}

AsyncException::AsyncException (const AsyncException& e)
  : impl (e.impl)
{
  addref (impl);
}

AsyncException::~AsyncException () throw ()
{
  release (impl);
}

AsyncException& AsyncException::operator = (const AsyncException& e)
{
  AsyncException (e).Swap (*this);
  return *this;
}

/*
    Переопределение операций xtl::exception
*/

const char* AsyncException::what () const throw ()
{
  return impl->message.data ();
}

void AsyncException::vtouch (const char* format, va_list args) throw ()
{
  if (!format || !*format)
    return;

  impl->message.append         ("\n    at ");
  impl->message.append_vformat (format, args);
}

/*
    Обмен
*/

void AsyncException::Swap (AsyncException& e)
{
  stl::swap (impl, e.impl);
}

namespace common
{

void swap (AsyncException& exception1, AsyncException& exception2)
{
  exception1.Swap (exception2);
}

}
