namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection_impl: public reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка факта наличя соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected () const { return is_connected; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокирование соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void block   ()       { is_blocked = true;  }
    void unblock ()       { is_blocked = false; }
    bool blocked () const { return is_blocked; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокирование разрыва соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool locked () const
    {
      return is_locked;
    }

    void lock ()
    { 
      is_locked       = true;
      wait_disconnect = false;
    }

    void unlock ()
    {
      is_locked = false;

      if (wait_disconnect)
        disconnect ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void disconnect ()
    {
      if (!is_connected)
        return;

      if (is_locked)
      {
        wait_disconnect = true;
        return;
      }

      is_connected = false;

      do_disconnect (); //nothrow      
    }

  protected:
    connection_impl  (bool in_connected=true) 
       : is_connected (in_connected), is_blocked (false), is_locked (false), wait_disconnect (false) {}

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настоящий разрыв соединения реализуется в потомках
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void do_disconnect () = 0;

  private:
    bool is_connected;
    bool is_blocked;
    bool is_locked;
    bool wait_disconnect;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пустое соединение
///////////////////////////////////////////////////////////////////////////////////////////////////
class empty_connection_impl: public connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение единственного экземпляра пустого соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static connection_impl& instance () {
      static char buffer [sizeof (empty_connection_impl)];
      static connection_impl* empty = new (buffer) empty_connection_impl;

      return *empty;
    }
    
  private:
    empty_connection_impl () : connection_impl (false) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение функции удаления объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void on_lost_references () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void do_disconnect () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс обработчика сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> struct signal_invoker: public invoker<Signature,connection_impl>
{
  typedef linked_ptr<signal_invoker*> invoker_pointer;

  virtual signal_invoker* clone (invoker_pointer& next) = 0;
};

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик сигнала делегирующий обработку пользовательскому функциональному объекту
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature,class Fn>
class signal_functor_invoker: public functor_invoker<Signature,Fn,signal_invoker<Signature> >
{
  typedef functor_invoker<Signature,Fn,signal_invoker<Signature> > base;
  public:
    typedef typename base::invoker_pointer invoker_pointer;

    signal_functor_invoker (const Fn& fn,invoker_pointer& next) : base (fn), ptr (this,next) { }

    signal_functor_invoker (const signal_functor_invoker& invoker,invoker_pointer& next)
      : base (invoker), ptr (this,next) { }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void do_disconnect ()
    {
      ptr.disconnect ();
      release ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    signal_invoker<Signature>* clone (invoker_pointer& next)
    {
      return new signal_functor_invoker (*this,next);
    }

  private:
    invoker_pointer ptr;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс-оболочка, не позволяющий разорвать соединение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class InIter> class signal_connection_holder
{
  public:
    signal_connection_holder (InIter& i) : iterator (i)
    {
      iterator->lock ();
    }

    ~signal_connection_holder ()
    {
      InIter j = iterator;

      ++iterator;

      j->unlock ();
    }

    InIter& get_iterator () const { return iterator; }    

  private:
    InIter& iterator;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение итерации вызова обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class InIter,class Tuple>
inline Ret signal_do_invoke (InIter& iterator,const Tuple& args)
{
  using tuples::apply;

  return apply<Ret> (ref (*signal_connection_holder<InIter> (iterator).get_iterator ()),args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс специализирующий распространение сигнала в зависимости от типа возвращаемого значения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class Accum>
struct signal_invoker_helper
{
  template <class InIter,class Tuple>
  static Ret invoke (InIter first,InIter last,const Tuple& args)
  {
    for (;first!=last && first->blocked ();++first);
    
    if (first == last)
      return Ret ();      
    
    Accum accum (signal_do_invoke<Ret> (first,args));

    while (first != last)
      if (!first->blocked ()) accum (signal_do_invoke<Ret> (first,args));
      else                    ++first;

    return accum ();
  }
};

template <class Accum>
struct signal_invoker_helper<void,Accum>
{
  template <class InIter,class Tuple>
  static void invoke (InIter first,InIter last,const Tuple& args)
  {
    while (first != last)
      if (!first->blocked ()) signal_do_invoke<void> (first,args);
      else                    ++first;
  }
};

}

/*
    signal_accumulator
*/

template <class Result,class Fn> template <class T>
inline signal_accumulator<Result,Fn>::signal_accumulator (const T& init_value)
  : result (init_value)
  { }

template <class Result,class Fn> template <class T>
inline void signal_accumulator<Result,Fn>::operator () (const T& value)
{
  result = Fn ()(result,value);
}

/*
    connection
*/

inline connection::connection ()
  : impl (&detail::empty_connection_impl::instance ())
  { }

inline connection::connection (detail::connection_impl& in_impl)
  : impl (&in_impl)
  { }

inline bool connection::connected () const
{
  return impl->connected ();
}

inline void connection::disconnect ()
{
  impl->disconnect ();
}

inline void connection::block ()
{
  impl->block ();
}

inline void connection::unblock ()
{
  impl->unblock ();
}

inline bool connection::blocked () const
{
  return impl->blocked ();
}

inline bool connection::operator == (const connection& c) const
{
  return impl == c.impl;
}

inline bool connection::operator != (const connection& c) const
{
  return !(*this == c);
}

/*
    signal
*/

/*
    Конструкторы / деструкторы
*/

template <class Signature,class Accumulator>
inline signal<Signature,Accumulator>::signal ()
  { }

template <class Signature,class Accumulator>
signal<Signature,Accumulator>::signal (const signal& s)
{
  clone (s);
}

template <class Signature,class Accumulator> template <class Accumulator1>
signal<Signature,Accumulator>::signal (const signal<Signature,Accumulator1>& s)
{
  clone (s);
}

template <class Signature,class Accumulator>
inline signal<Signature,Accumulator>::~signal ()
{
  disconnect_all ();
}

/*
    Копирование обработчиков
*/

template <class Signature,class Accumulator> template <class Accumulator1>
void signal<Signature,Accumulator>::clone (const signal<Signature,Accumulator1>& s)
{
  try
  {
    for (invoker_pointer::iterator i=s.first.next (),end=s.first;i!=end;++i)
      i->clone (first);
  }
  catch (...)
  {
    disconnect_all ();
  }
}

/*
    Присваивание
*/

template <class Signature,class Accumulator>
inline signal<Signature,Accumulator>& signal<Signature,Accumulator>::operator = (const signal& s)
{
  if (this != &s)
    signal (s).swap (*this);

  return *this;
}

template <class Signature,class Accumulator> template <class Accumulator1>
inline signal<Signature,Accumulator>& signal<Signature,Accumulator>::operator = (const signal<Signature,Accumulator1>& s)
{
  if (this != &s)
    signal (s).swap (*this);

  return *this;
}

/*
    Установка / разрыв соединения
*/

template <class Signature,class Accumulator> template <class Fn>
inline typename signal<Signature,Accumulator>::invoker_type* signal<Signature,Accumulator>::connect_internal (const Fn& fn)
{
  return new detail::signal_functor_invoker<Signature,Fn> (fn,first);
}

template <class Signature,class Accumulator> template <class Fn>
inline connection signal<Signature,Accumulator>::connect (Fn fn)
{
  return *connect_internal (fn);
}

template <class Signature,class Accumulator> template <class Ptr,class MemFun>
inline connection signal<Signature,Accumulator>::connect (const Ptr& ptr,MemFun fn)
{
  return *connect_internal (detail::memfun_functor<result_type,Ptr,MemFun> (ptr,fn));
}

template <class Signature,class Accumulator> template <class Fn>
void signal<Signature,Accumulator>::disconnect_internal (const Fn& fn)
{ 
  typedef detail::signal_functor_invoker<Signature,Fn> functor_invoker;

  functor_invoker* casted_invoker;

  for (invoker_pointer::iterator i=first.next (),end=first;i!=end;)
    if ((casted_invoker = dynamic_cast<functor_invoker*> (&*i)) && *casted_invoker == fn)
    {
      invoker_pointer::iterator next = i;

      ++next;

      i->disconnect ();

      i = next;
    }
    else ++i;
}

template <class Signature,class Accumulator> template <class Fn>
inline void signal<Signature,Accumulator>::disconnect (Fn fn)
{
  disconnect_internal (fn);
}

template <class Signature,class Accumulator> template <class Ptr,class MemFun>
inline void signal<Signature,Accumulator>::disconnect (const Ptr& ptr,MemFun fn)
{
  disconnect_internal (detail::memfun_functor<result_type,Ptr,MemFun> (ptr,fn));
}

template <class Signature,class Accumulator>
void signal<Signature,Accumulator>::disconnect_all ()
{
    //данная функция предполагает возможность очистки списка обработчиков во время распространения сигнала

  invoker_pointer::iterator pos = first.prev (), end = first;

  while (pos != end)
  {
    invoker_pointer::iterator prev = pos;

    --prev;

    pos->disconnect ();

    pos = prev;
  }
}

/*
    Проверка на пустоту
*/

template <class Signature,class Accumulator>
inline bool signal<Signature,Accumulator>::empty () const
{
  return !first.connected ();
}

/*
    Обмен
*/

template <class Signature,class Accumulator> template <class Accumulator1>
inline void signal<Signature,Accumulator>::swap (signal<Signature,Accumulator1>& s)
{
  first.swap (s.first);
}

template <class Signature,class Accumulator1,class Accumulator2>
inline void swap (signal<Signature,Accumulator1>& s1,signal<Signature,Accumulator2>& s2)
{
  s1.swap (s2);  
}

/*
    Распространение сигнала
*/

template <class Signature,class Accumulator> template <class Tuple>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::invoke (const Tuple& t) const
{
  return detail::signal_invoker_helper<result_type,accumulator_type>::invoke (first.next ().get_iterator (),first.get_iterator (),t);
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator () () const
{
  return invoke (tie ());
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg) const
{
  return invoke (tie (arg));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2) const
{
  return invoke (tie (arg1,arg2));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3) const
{
  return invoke (tie (arg1,arg2,arg3));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4) const
{
  return invoke (tie (arg1,arg2,arg3,arg4));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5) const
{
  return invoke (tie (arg1,arg2,arg3,arg4,arg5));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6) const
{
  return invoke (tie (arg1,arg2,arg3,arg4,arg5,arg6));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7) const
{
  return invoke (tie (arg1,arg2,arg3,arg4,arg5,arg6,arg7));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7,
  arg8_type arg8) const
{
  return invoke (tie (arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8));
}

template <class Signature,class Accumulator>
inline typename signal<Signature,Accumulator>::result_type signal<Signature,Accumulator>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7,
  arg8_type arg8,
  arg9_type arg9) const
{
  return invoke (tie (arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9));
}
