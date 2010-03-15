#include <stl/auto_ptr.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/string_buffer.h>

#include <common/action_queue.h>
#include <common/async.h>

using namespace common;

/*
===================================================================================================
    AsyncResult
===================================================================================================
*/

/*
    Описание реализации асинхронного результата
*/

struct AsyncResult::Impl: public xtl::reference_counter
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
  
  ~Impl ()
  {
    try
    {
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
    action = ActionQueue::PushAction (xtl::bind (&Impl::Perform, this), ActionThread_Current);
  }
  
  void Perform ()
  {
    try
    {
      result    = async_action->Perform ();
      completed = result && result->IsValid ();
    }
    catch (...)
    {
    }
    
    async_action.reset ();
    
    if (callback)
    {
      try
      {
        AsyncResult async_result (this);

        callback (async_result);
      }
      catch (...)
      {
      }

      callback.clear ();
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
  return impl->completed;
}

void AsyncResult::WaitCompleted ()
{
  try
  {
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
