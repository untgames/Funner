namespace detail
{

/*
    Реализация слота
*/

template <class Signature> class slot_impl: public connection_impl
{
  public:
    typedef xtl::function<Signature> function_type;
  
      //конструкторы / деструктор
    slot_impl () : lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot = this;
      next_slot = this;
    }

    slot_impl (const function_type& in_fn) : fn (in_fn), lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot = next_slot = this;
    }
    
    slot_impl (const function_type& in_fn, slot_impl* in_next_slot) :
      fn (in_fn), prev_slot (in_next_slot->prev_slot), next_slot (in_next_slot), lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot->next_slot = next_slot->prev_slot = this;
    }

    ~slot_impl ()
    {
      next_slot->prev_slot = prev_slot;
      prev_slot->next_slot = next_slot;      
    }

      //функция-обработчик сигнала
    function_type& function () { return fn; }

      //предудущий и следующий слот
    slot_impl* previos () { return prev_slot; }
    slot_impl* next    () { return next_slot; }

      //связывание слотов цепочки
    void connect (slot_impl* new_next_slot)
    {
      if (new_next_slot == next_slot)
        return;

      if (lock_count > 0)
      {
        wait_next_slot = new_next_slot;
        return;
      }
    
        //захватываем слот

      addref ();

        //отсоединяем слот из текущей цепочки
    
      if (next_slot != this) //требуется отсоединение слота из цепочки
      {
        prev_slot->next_slot = next_slot;
        next_slot->prev_slot = prev_slot;

        release (); //отсоединение слота от цепочки сопровождается уменьшением числа ссылок
      }

        //присоединяем слот к новой цепочке

      if (new_next_slot != this)
      {
        next_slot            = new_next_slot;
        prev_slot            = new_next_slot->prev_slot;
        next_slot->prev_slot = prev_slot->next_slot = this;        

        addref ();
      }
      else //disconnect
      {
        next_slot = prev_slot = this;        
      }

        //освобождаем слот

      release ();
    }
    
      //отсоединение
    void disconnect ()
    {
      if (prev_slot == this)
        return;                
        
      connect (this);        
    }
    
      //принудительный разрыв соединения (может быть использован только для поля first сигнала)
    void force_signal_first_slot_disconnect ()
    {
      prev_slot = next_slot = this;
    }

      //проверка соединения
    bool connected () { return prev_slot != this; }

      //заблокирован ли слот
    bool locked () { return lock_count != 0; }

      //блокировка слота от пересоединений
    void lock ()
    {
      lock_count++;
    } 
    
      //снятие блокировки слота от пересоединений
    void unlock ()
    {
      if (!--lock_count && wait_next_slot)
      {
        slot_impl* new_next_slot = wait_next_slot;
        
        wait_next_slot = 0;

        connect (new_next_slot);
      }
    }
    
      //блокировка соединения
    void block ()
    {
      is_blocked = true;
    }
    
      //снятие блокировки соединения
    void unblock ()
    {
      is_blocked = false;
    }
    
      //проверка блокировки соединения
    bool blocked () { return is_blocked; }
    
      //увеличение числа ссылок слотов
    void slot_addref ()
    {
      addref ();
    }
    
      //уменьшение числа ссылок слотов
    void slot_release ()
    {
      if (use_count () == 2 && prev_slot != this)
        disconnect ();

      release ();
    }

  private:
    slot_impl (const slot_impl&); //no impl
    slot_impl& operator = (const slot_impl&); //no impl

  private:
    function_type fn;
    slot_impl     *prev_slot, *next_slot;
    size_t        lock_count;
    slot_impl*    wait_next_slot;
    bool          is_blocked;
};

/*
    Соединение
*/

class slot_connection: public connection
{
  public:
    slot_connection (connection_impl* impl) : connection (impl) {}
};

}

/*
    Конструктор / деструктор
*/

template <class Signature>
inline slot<Signature>::slot ()
  : impl (new detail::slot_impl<Signature> ())
{
}

template <class Signature>
inline slot<Signature>::slot (const function_type& fn)
  : impl (new detail::slot_impl<Signature> (fn))
{
}

template <class Signature>
inline slot<Signature>::slot (const slot& s)
  : impl (s.impl)
{
  impl->slot_addref ();
}

template <class Signature>
inline slot<Signature>::slot (const function_type& fn, slot_impl* next_slot)
  : impl (new slot_impl (fn, next_slot))
{
}

template <class Signature>
inline slot<Signature>::~slot ()
{
  impl->slot_release ();
}

/*
    Присваивание
*/

template <class Signature>
inline slot<Signature>& slot<Signature>::operator = (const slot& s)
{
  slot (s).swap (*this);
  return *this;
}

template <class Signature>
inline slot<Signature>& slot<Signature>::operator = (const function_type& fn)
{
  slot (fn).swap (*this);
  return *this;
}

/*
    Функция-обработчик сигнала
*/

template <class Signature>
inline const typename slot<Signature>::function_type& slot<Signature>::function () const
{
  return impl->function ();
}

/*
    Соединение, связанное со слотом
*/

template <class Signature>
inline xtl::connection slot<Signature>::connection () const
{
  return detail::slot_connection (impl);
}

/*
    Предудущий и следующий слот
*/

template <class Signature>
inline slot<Signature> slot<Signature>::previos () const
{
  return slot (impl->previos ());
}

template <class Signature>
inline slot<Signature> slot<Signature>::next () const
{
  return slot (impl->next ());
}

/*
    Связывание слотов
*/


template <class Signature>
inline void slot<Signature>::disconnect ()
{
  impl->disconnect ();
}

template <class Signature>
inline bool slot<Signature>::connected () const
{
  return impl->connected ();
}

/*
    Обмен
*/

template <class Signature>
inline void slot<Signature>::swap (slot& s)
{
  slot_impl* tmp = s.impl;
  s.impl         = impl;
  impl           = tmp;
}

template <class Signature>
inline void swap (slot<Signature>& s1, slot<Signature>& s2)
{
  s1.swap (s2);
}

/*
    auto_slot
*/

template <class Signature>
inline auto_slot<Signature>::auto_slot (const slot<Signature>& s)
  : base (s)
  {}

template <class Signature>
inline auto_slot<Signature>::auto_slot (const function_type& fn)
  : base (fn)
  {}

template <class Signature>
inline auto_slot<Signature>::~auto_slot ()
{
  base::disconnect ();
}

template <class Signature>
inline auto_slot<Signature>& auto_slot<Signature>::operator = (const slot<Signature>& s)
{
  base::operator = (s);

  return *this;
}
