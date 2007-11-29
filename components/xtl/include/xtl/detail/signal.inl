namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор слотов для аккумулирования результата распространения сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Slot, class Args> class signal_invoke_iterator
{  
  typedef Slot slot_type;
  public:
    typedef typename slot_type::function_type   function_type;
    typedef typename function_type::result_type value_type;
    typedef stl::bidirectional_iterator_tag     iterator_category;
    
    signal_invoke_iterator (Args& in_args, Slot* first_slot, Slot* in_end) : slot (first_slot), end (in_end), args (&in_args)
    {
      while (slot->slot_type::blocked () && slot != end) slot = slot->next ();

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
      if (this == &i)
        return *this;

      slot_type* old_slot = slot;
      
      slot = i.slot;
      end  = i.end;
      args = i.args;
      
      slot->lock ();
      old_slot->unlock ();

      return *this;
    }

    signal_invoke_iterator& operator ++ ()
    {
      slot_type* old_slot = slot;
    
      do slot = slot->next (); while (slot->slot_type::blocked () && slot != end);
      
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

      do slot = slot->prev (); while (slot->slot_type::blocked () && slot != end);

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

    value_type operator * () const { return apply<value_type> (slot->function (), *args); }

  private:
    Slot* slot;    
    Slot* end;
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
    signal
*/

/*
    Конструкторы / деструкторы
*/

template <class Signature, class Accumulator>
inline signal<Signature, Accumulator>::signal ()
{
  first.lock ();
}

template <class Signature, class Accumulator>
inline signal<Signature, Accumulator>::~signal ()
{
  first.unlock ();
  disconnect_all ();
}

/*
    Установка / разрыв соединения
*/

template <class Signature, class Accumulator>
inline connection signal<Signature, Accumulator>::connect (slot_type& s)
{
  s.impl->connect (&first);
  return s.connection ();
}

template <class Signature, class Accumulator>
inline connection signal<Signature, Accumulator>::connect (const function_type& fn)
{
  slot_type s (fn, &first);

  return s.connection ();
}

template <class Signature, class Accumulator> template <class Fn>
inline void signal<Signature, Accumulator>::disconnect (Fn fn)
{
  for (slot_impl* i=first.next (); i!=&first;)
    if (i->function () == fn)
    {
      slot_impl* tmp = i;
      
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

  for (slot_impl* i=first.previos (), *end=&first; i!=&first;)
  {
    slot_impl* tmp = i;
    
    tmp = tmp->previos ();
    
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
  return !first.connected ();
}

/*
    Количество слотов
*/

template <class Signature, class Accumulator>
inline size_t signal<Signature, Accumulator>::num_slots () const
{
  size_t count = 0;

  for (slot_impl* i=first.next (); i!=&first; i=i->next ())
    count++;
    
  return count;
}

/*
    Распространение сигнала
*/

template <class Signature, class Accumulator> template <class Tuple>
inline typename signal<Signature, Accumulator>::result_type signal<Signature, Accumulator>::invoke (Tuple& args) const
{
  typedef detail::signal_invoke_iterator<slot_impl, Tuple> iterator;

  return Accumulator () (iterator (args, first.next (), &first), iterator (args, &first, &first));
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
