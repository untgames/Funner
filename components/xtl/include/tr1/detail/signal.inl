namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void addref () { ref_count++; }
    
    void release ()
    {
      if (!--ref_count)
        delete this;
    }
  
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
    void lock ()
    { 
      lock_count++;
    }

    void unlock ()
    {
      if (!--lock_count && wait_disconnect)
        disconnect ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void disconnect ()
    {
      if (!is_connected)
        return;

      if (lock_count)
      {
        wait_disconnect = true;
        return;
      }

      is_connected    = false;
      wait_disconnect = false;

      do_disconnect (); //nothrow      
    }

  protected:
    connection_impl  (bool in_connected=true) 
       : ref_count (1), lock_count (0), is_connected (in_connected), is_blocked (false), wait_disconnect (false) {}
       
    virtual ~connection_impl () {}

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настоящий разрыв соединения реализуется в потомках
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void do_disconnect () = 0;

  private:
    size_t ref_count;
    size_t lock_count;
    bool   is_connected;    
    bool   is_blocked;
    bool   wait_disconnect;
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
    static connection_impl* instance_ptr () {
      static char buffer [sizeof (empty_connection_impl)];
      static connection_impl* empty = new (buffer) empty_connection_impl;

      return empty;
    }

  private:
    empty_connection_impl () : connection_impl (false) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void do_disconnect () {}
};

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4355) //'this' : used in base member initializer list
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слот в обработке сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> class slot: public connection_impl
{
  public:  
    typedef function<Signature>                 function_type;
    typedef typename function_type::result_type result_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
/////////////////////////////////////////////////////////////////////////////////////////////////// 
    slot () : connection_impl (false), prev_slot (this), next_slot (this) {}
    
    template <class Fn> slot (Fn& in_fn, slot* in_next) : fn (in_fn), next_slot (in_next), prev_slot (in_next->prev_slot)
    {
      next_slot->prev_slot = prev_slot->next_slot = this;
    }

    ~slot ()
    {
      prev_slot->next_slot = next_slot;
      next_slot->prev_slot = prev_slot;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение следующего и предыдущего слотов
///////////////////////////////////////////////////////////////////////////////////////////////////
    slot* prev () const { return prev_slot; }
    slot* next () const { return next_slot; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Args> result_type invoke (Args& args) const
    {
      return apply<result_type> (fn, args);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка равенства функций
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> bool equal (Fn& in_fn) const
    {
      return fn == in_fn;
    }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void do_disconnect ()
    {
      prev_slot->next_slot  = next_slot;
      next_slot->prev_slot  = prev_slot;
      next_slot = prev_slot = this;
  
      release ();
    }

  private:
    function_type fn;
    slot          *prev_slot, *next_slot;
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор слотов для аккумулирования результата распространения сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Slot, class Args> class signal_invoke_iterator
{  
  typedef Slot slot_type;
  public:
    typedef typename slot_type::result_type value_type;
    typedef stl::bidirectional_iterator_tag iterator_category;
    
    signal_invoke_iterator (Args& in_args, Slot* first_slot) : slot (first_slot), args (&in_args)
    {   
      while (slot->blocked ()) slot = slot->next ();
      
      slot->lock ();
    }
    
    signal_invoke_iterator (const signal_invoke_iterator& i) : slot (i.slot), args (i.args)
    {
      slot->lock ();
    }
    
    ~signal_invoke_iterator ()
    {
      slot->unlock ();
    }
    
    signal_invoke_iterator& operator = (const signal_invoke_iterator& i)
    {
      if (this != &i)
      {
        slot_type* old_slot = slot;
        
        slot = i.slot;
        args = i.args;
        
        slot->lock ();
        old_slot->unlock ();
      }

      return *this;
    }

    signal_invoke_iterator& operator ++ ()
    {
      slot_type* old_slot = slot;
    
      do slot = slot->next (); while (slot->blocked ());    
      
      slot->lock ();
      old_slot->unlock ();
    
      return *this;
    }        
    
    signal_invoke_iterator operator ++ (int)
    {
      signal_invoke_iterator tmp (*this);
      
      ++*this;
    
      return tmp;
    }
    
    signal_invoke_iterator& operator -- ()
    {
      slot_type* old_slot = slot;    

      do slot = slot->prev (); while (slot->blocked ());

      slot->lock ();
      old_slot->unlock ();

      return *this;
    }
    
    signal_invoke_iterator operator -- (int)
    {
      signal_invoke_iterator tmp (*this);
      
      --*this;
    
      return tmp;
    }    
    
    bool operator == (const signal_invoke_iterator& s) const { return slot == s.slot; }
    bool operator != (const signal_invoke_iterator& s) const { return slot != s.slot; }

    value_type operator * () const { return slot->invoke (*args); }

  private:
    Slot* slot;    
    Args* args;
};

}

/*
    default_signal_accumulator
*/

template <class Ret> struct default_signal_accumulator
{
  typedef Ret result_type;

  template <class InIter> Ret operator () (InIter first, InIter last) const
  {      
    if (first == last)
      return Ret ();
  
    Ret value = *first;
  
    while (++first != last)
      value = *first;
      
    return value;
  }
};

template <> struct default_signal_accumulator<void>
{
  typedef struct unusable {} result_type;

  template <class InIter> result_type operator () (InIter first, InIter last) const
  {
    while (first != last) *first, ++first;

    return result_type ();
  }
};

/*
    connection
*/

inline connection::connection ()
  : impl (detail::empty_connection_impl::instance_ptr ())
{
  impl->addref ();
}

inline connection::connection (detail::connection_impl* in_impl)
  : impl (in_impl)
{
  impl->addref ();
}
  
inline connection::connection (const connection& c)
  : impl (c.impl)
{
  impl->addref ();  
}

inline connection::~connection ()
{
  impl->release ();
}

inline connection& connection::operator = (const connection& c)
{
  connection (c).swap (*this);

  return *this;
}

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

inline void connection::swap (connection& c)
{
  detail::connection_impl* tmp = impl;
  impl                         = c.impl;
  c.impl                       = tmp;
}

inline void swap (connection& a, connection& b)
{
  a.swap (b);
}

/*
    signal
*/

/*
    Конструкторы / деструкторы
*/

template <class Signature, class Accumulator>
inline signal<Signature, Accumulator>::signal ()
  { }

template <class Signature, class Accumulator>
inline signal<Signature, Accumulator>::~signal ()
{
  disconnect_all ();
}

/*
    Установка / разрыв соединения
*/

template <class Signature, class Accumulator> template <class Fn>
inline connection signal<Signature, Accumulator>::connect (Fn fn)
{
  return new slot (fn, &first);
}

template <class Signature, class Accumulator> template <class Fn>
inline void signal<Signature, Accumulator>::disconnect (Fn fn)
{
  for (slot* i=first.next (); i!=&first;)
    if (i->equal (fn))
    {
      slot* tmp = i;
      
      tmp = tmp->next ();
      
      i->disconnect ();
      
      i = tmp;
    }
    else i = i->next ();
}

template <class Signature, class Accumulator>
void signal<Signature, Accumulator>::disconnect_all ()
{
    //данная функция предполагает возможность очистки списка обработчиков во время распространения сигнала

  for (slot* i=first.prev (), *end=&first; i!=&first;)
  {
    slot* tmp = i;
    
    tmp = tmp->prev ();
    
    i->disconnect ();
    
    i = tmp;
  }
}

/*
    Проверка на пустоту
*/

template <class Signature, class Accumulator>
inline bool signal<Signature, Accumulator>::empty () const
{
  return first.next () == &first;
}

/*
    Количество слотов
*/

template <class Signature, class Accumulator>
inline size_t signal<Signature, Accumulator>::num_slots () const
{
  size_t count = 0;

  for (const slot* i=first.next (); i!=&first; i=i->next ())
    count++;
    
  return count;
}

/*
    Обмен
*/

template <class Signature, class Accumulator> template <class Accumulator1>
inline void signal<Signature, Accumulator>::swap (signal<Signature, Accumulator1>& s)
{
  first.swap (s.first);
}

template <class Signature, class Accumulator1, class Accumulator2>
inline void swap (signal<Signature, Accumulator1>& s1, signal<Signature, Accumulator2>& s2)
{
  s1.swap (s2);
}

/*
    Распространение сигнала
*/

template <class Signature, class Accumulator> template <class Tuple>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::invoke (Tuple& args) const
{
  typedef detail::signal_invoke_iterator<slot, Tuple> iterator;

  return Accumulator () (iterator (args, first.next ()), iterator (args, &first));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator () () const
{
  return invoke (tie ());
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg) const
{
  return invoke (tie (arg));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2) const
{
  return invoke (tie (arg1, arg2));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3) const
{
  return invoke (tie (arg1, arg2, arg3));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3, 
  arg4_type arg4) const
{
  return invoke (tie (arg1, arg2, arg3, arg4));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3, 
  arg4_type arg4, 
  arg5_type arg5) const
{
  return invoke (tie (arg1, arg2, arg3, arg4, arg5));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3, 
  arg4_type arg4, 
  arg5_type arg5, 
  arg6_type arg6) const
{
  return invoke (tie (arg1, arg2, arg3, arg4, arg5, arg6));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3, 
  arg4_type arg4, 
  arg5_type arg5, 
  arg6_type arg6, 
  arg7_type arg7) const
{
  return invoke (tie (arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
 (arg1_type arg1, 
  arg2_type arg2, 
  arg3_type arg3, 
  arg4_type arg4, 
  arg5_type arg5, 
  arg6_type arg6, 
  arg7_type arg7, 
  arg8_type arg8) const
{
  return invoke (tie (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

template <class Signature, class Accumulator>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::operator ()
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
  return invoke (tie (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}
