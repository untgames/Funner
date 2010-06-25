namespace detail
{

//базовый интерфейс результата асинхронной операции
class IAsyncResult
{
  public:
    virtual void Release () = 0;
    virtual bool IsValid () = 0;
    
  protected:
    virtual ~IAsyncResult () {}
};

//интерфейс возвращения результата
template <class Ret> class IAsyncResultHolder: public IAsyncResult
{
  public:
    virtual Ret* GetResult () = 0;
};

//возвращение результата операции
template <class Ret> class AsyncResultHolder: public IAsyncResultHolder<Ret>
{
  public:
    static AsyncResultHolder* Create (const xtl::function<Ret ()>& fn)
    {
      return new AsyncResultHolder (fn);
    }
    
    bool IsValid () { return true; }

    Ret* GetResult () { return &result; }
    
    void Release () { delete this; }
    
  private:
    AsyncResultHolder (const xtl::function<Ret ()>& fn) : result (fn ()) {}

  private:
    Ret result;
};

template <> class AsyncResultHolder<void>: public IAsyncResultHolder<void>
{
  public:
    static AsyncResultHolder* Create (const xtl::function<void ()>& fn)
    {
      fn ();
    
      return &xtl::singleton_default<AsyncResultHolder, false>::instance ();
    }
    
    bool IsValid () { return true; }
  
    void* GetResult () { return 0; }
    
    void Release () {}
};

//перегенерация исключения, возникающего при получении результата асинхронной операции
template <class Ret> class AsyncResultException: public IAsyncResultHolder<Ret>
{
  public:
    AsyncResultException () {}
    AsyncResultException (const std::exception& e) : exception (e) {}
    
    bool IsValid () { return false; }

    Ret* GetResult () { throw exception; }
    
    void Release () { delete this; }

  private:
    AsyncException exception;
};

//интерфейс асинхронной операции
class IAsyncAction
{
  public:
    virtual ~IAsyncAction () {}

    virtual IAsyncResult* Perform () = 0;
};

template <class Ret>
class AsyncAction: public IAsyncAction
{
  public:
    AsyncAction (const xtl::function<Ret ()>& in_action) : action (in_action) {}

    IAsyncResult* Perform ()
    {
      try
      {
        return AsyncResultHolder<Ret>::Create (action);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::detail::AsyncAction::Perform");
        return new AsyncResultException<Ret> (e);
      }
      catch (std::exception& e)
      {
        return new AsyncResultException<Ret> (e);
      }
      catch (...)
      {
        return new AsyncResultException<Ret> ();
      }
    }

  private:
    xtl::function<Ret ()> action;
};

}

/*
    Выполнение асинхронной операции
*/

template <class Ret>
inline AsyncResult async_invoke (const xtl::function<Ret ()>& action)
{
  return AsyncResult (new detail::AsyncAction<Ret> (action));
}

template <class Ret>
inline AsyncResult async_invoke (const xtl::function<Ret ()>& action, const xtl::function<void (AsyncResult&)>& callback)
{
  return AsyncResult (new detail::AsyncAction<Ret> (action), callback);
}

/*
    Возвращение результата асинхронной операции
*/

template <class Ret>
typename AsyncResultType<Ret>::Type AsyncResult::Result ()
{
  try
  {
    return *dynamic_cast<detail::IAsyncResultHolder<Ret>&> (*Result ()).GetResult ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::AsyncResult::Result<Ret> ()");
    throw;
  }
}
